#include "sampling_controller.h"

SamplingController::SamplingController(Display &disp, ColorSensor &sens,
                                       ColorSampler &samp, Button &btn,
                                       Bluetooth &bluetooth)
    : display(disp), sensor(sens), sampler(samp), button(btn), ble(bluetooth),
      longPressDuration(2000), ledToggleDuration(5000),
      autoLedOffTimeout(120000), minSamplesRequired(3), lastButtonState(false),
      longPressHandled(false), ledToggleHandled(false), lastActivityTime(0),
      lastAvgColor({0, 0, 0}), lastColorName("") {}

void SamplingController::begin() {
  lastActivityTime = millis();
  display.showMessage("Ready!", "Press to sample");
  Serial.println("Controls:");
  Serial.println("  Short press: Add sample");
  Serial.println("  2s hold: Finalize and send");
  Serial.println("  5s hold: Toggle LED on/off");
  Serial.println("  Triple tap: Reset samples");
  Serial.print("Minimum samples required: ");
  Serial.println(minSamplesRequired);
}

void SamplingController::setLongPressDuration(unsigned long ms) {
  longPressDuration = ms;
}

void SamplingController::setMinSamplesRequired(int count) {
  minSamplesRequired = count;
}

void SamplingController::onSampleTaken(const RGBColor &color) {
  lastActivityTime = millis();
  sampler.addSample(color);

  RGBColor avgColor = sampler.getAverage();
  String avgColorName = sensor.detectColorName(avgColor);

  lastAvgColor = avgColor;
  lastColorName = avgColorName;

  display.showSamplingMode(sampler.getSampleCount(), avgColor.red,
                           avgColor.green, avgColor.blue, avgColorName);
  sampler.printSample(color);

  Serial.print("Sample #");
  Serial.print(sampler.getSampleCount());
  Serial.println(" added");
}

bool SamplingController::canFinalize() {
  int count = sampler.getSampleCount();

  if (count == 0) {
    Serial.println("No samples to average!");
    display.showMessage("No samples!", "Press button first");
    delay(2000);
    return false;
  }

  if (count < minSamplesRequired) {
    Serial.print("Need at least ");
    Serial.print(minSamplesRequired);
    Serial.println(" samples!");
    display.showMessage("Need more samples!", String(count) + "/" +
                                                  String(minSamplesRequired) +
                                                  " collected");
    delay(2000);
    return false;
  }

  return true;
}

void SamplingController::onLongPress() {
  lastActivityTime = millis();

  if (!canFinalize())
    return;

  RGBColor avgColor = sampler.getAverage();
  String avgColorName = sensor.detectColorName(avgColor);

  sampler.printAverage(avgColor, avgColorName);

  display.showMessage("FINAL: " + avgColorName,
                      "R:" + String(avgColor.red) +
                          " G:" + String(avgColor.green) +
                          " B:" + String(avgColor.blue));

  String bleData = String(avgColor.red) + "," + String(avgColor.green) + "," +
                   String(avgColor.blue) + "," + avgColorName;
  Serial.println("Average Color: " + bleData);
  ble.send(bleData);

  // Wait for button release first
  while (button.isPressed()) {
    delay(50);
  }

  Serial.println("Press button to continue...");

  // Wait for button press (result stays on screen)
  while (!button.isPressed()) {
    delay(50);
  }
  // Wait for button release
  while (button.isPressed()) {
    delay(50);
  }

  sampler.reset();
  display.showMessage("Ready!", "Press to sample");
  Serial.println("Sampler reset. Ready for new samples.");
  delay(300);
}

void SamplingController::onLedToggle() {
  lastActivityTime = millis();
  sensor.toggleLed();

  if (sensor.isLedOn()) {
    display.showMessage("LED ON", "Ready to sample");
    Serial.println("LED turned ON");
  } else {
    display.showMessage("LED OFF", "Power saving mode");
    Serial.println("LED turned OFF - Power saving mode");
  }

  // Wait for button release
  while (button.isPressed()) {
    delay(50);
  }
  delay(1000);

  // Show ready message if LED is on
  if (sensor.isLedOn() && sampler.getSampleCount() == 0) {
    display.showMessage("Ready!", "Press to sample");
  } else if (sensor.isLedOn() && sampler.getSampleCount() > 0) {
    display.showSamplingMode(sampler.getSampleCount(), lastAvgColor.red,
                             lastAvgColor.green, lastAvgColor.blue,
                             lastColorName);
  }
}

void SamplingController::onTripleTap() {
  lastActivityTime = millis();

  if (sampler.getSampleCount() == 0) {
    display.showMessage("Nothing to reset", "");
    Serial.println("Triple tap detected but no samples to reset");
  } else {
    sampler.reset();
    display.showMessage("Samples cleared!", "Starting fresh");
    Serial.println("Triple tap: All samples cleared");
  }

  delay(1500);
  display.showMessage("Ready!", "Press to sample");
}

void SamplingController::checkAutoLedOff() {
  // Only check if LED is currently on
  if (!sensor.isLedOn()) {
    return;
  }

  // Check if inactive for too long
  if (millis() - lastActivityTime > autoLedOffTimeout) {
    sensor.setLed(false);
    display.showMessage("Auto sleep", "Press to wake");
    Serial.println("Auto LED off due to inactivity");
    lastActivityTime = millis(); // Reset to avoid repeated messages
  }
}

void SamplingController::update() {
  // Update tap counter
  button.updateTapCount();

  // Check for triple tap
  int tapCount = button.getTapCount();
  if (tapCount >= 3) {
    onTripleTap();
    button.resetTapCount();
    return;
  }

  // Check auto LED off
  checkAutoLedOff();

  // If LED is off and button pressed, turn it back on (wake up)
  if (!sensor.isLedOn() && button.isPressed()) {
    sensor.setLed(true);
    lastActivityTime = millis();
    display.showMessage("Waking up...", "");
    delay(500);
    display.showMessage("Ready!", "Press to sample");
    // Wait for release to avoid triggering sample
    while (button.isPressed()) {
      delay(50);
    }
    return;
  }

  // Only process sampling if LED is on
  if (!sensor.isLedOn()) {
    delay(100);
    return;
  }

  bool currentButtonState = button.isPressed();

  if (currentButtonState) {
    lastActivityTime = millis();

    if (!lastButtonState) {
      // Read color only when taking a sample
      RGBColor color = sensor.readColor();
      onSampleTaken(color);
    }

    unsigned long duration = button.getPressedDuration();

    // Check for LED toggle (5s) first
    if (duration > 0 && !ledToggleHandled) {
      if (duration >= ledToggleDuration) {
        Serial.println("5s hold detected! Toggling LED...");
        onLedToggle();
        ledToggleHandled = true;
        longPressHandled = true; // Prevent finalize
        return;
      }
    }

    // Check for long press (2s) - show progress
    if (duration > 0 && !longPressHandled && !ledToggleHandled) {
      // Show different progress based on duration
      if (duration < longPressDuration) {
        int progress = min(100, (int)((duration * 100) / longPressDuration));
        if (duration > 200) {
          display.showProgress(progress);
        }
      } else if (duration < ledToggleDuration) {
        // Between 2s and 5s - show LED toggle countdown
        int ledProgress =
            min(100, (int)(((duration - longPressDuration) * 100) /
                           (ledToggleDuration - longPressDuration)));
        display.showMessage("LED Toggle:", String(ledProgress) + "%");
      }
    }

    if (button.isPressedFor(longPressDuration) && !longPressHandled &&
        !ledToggleHandled) {
      Serial.println("2s hold detected! Finalizing...");
      onLongPress();
      longPressHandled = true;
    }
  } else {
    if (lastButtonState && !longPressHandled && !ledToggleHandled) {
      if (sampler.getSampleCount() > 0) {
        display.showSamplingMode(sampler.getSampleCount(), lastAvgColor.red,
                                 lastAvgColor.green, lastAvgColor.blue,
                                 lastColorName);
      }
    }
    longPressHandled = false;
    ledToggleHandled = false;
  }

  lastButtonState = currentButtonState;
  delay(10); // Small delay to prevent CPU hogging
}
