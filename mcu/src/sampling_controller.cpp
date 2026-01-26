#include "sampling_controller.h"

static const unsigned long BUTTON_WAIT_DELAY = 50;
static const unsigned long PROGRESS_SHOW_DELAY = 200;

SamplingController::SamplingController(Display &disp, ColorSensor &sens,
                                       ColorSampler &samp, Button &btn,
                                       Bluetooth &bluetooth)
    : display(disp), sensor(sens), sampler(samp), button(btn), ble(bluetooth),
      longPressDuration(LONG_PRESS_DURATION),
      ledToggleDuration(LED_TOGGLE_DURATION),
      autoLedOffTimeout(AUTO_LED_OFF_TIMEOUT),
      minSamplesRequired(MIN_SAMPLES_REQUIRED), lastButtonState(false),
      longPressHandled(false), ledToggleHandled(false), lastActivityTime(0),
      lastAvgColor({0, 0, 0}), lastColorName("") {}

// ============================================================================
// Initialization
// ============================================================================

void SamplingController::begin() {
  updateActivity();
  display.showMessage("Ready!", "Press to sample");

  Serial.println("Controls:");
  Serial.println("  Short press: Add sample");
  Serial.println("  2s hold: Finalize and send");
  Serial.println("  5s hold: Toggle LED on/off");
  Serial.println("  Triple tap: Reset samples");
  Serial.print("Min samples: ");
  Serial.println(minSamplesRequired);
}

void SamplingController::setLongPressDuration(unsigned long ms) {
  longPressDuration = ms;
}

void SamplingController::setMinSamplesRequired(int count) {
  minSamplesRequired = count;
}

// ============================================================================
// Helper Methods
// ============================================================================

void SamplingController::updateActivity() { lastActivityTime = millis(); }

void SamplingController::waitForButtonRelease() {
  while (button.isPressed()) {
    delay(BUTTON_WAIT_DELAY);
  }
}

void SamplingController::waitForButtonPress() {
  while (!button.isPressed()) {
    delay(BUTTON_WAIT_DELAY);
  }
}

void SamplingController::showCurrentState() {
  if (sampler.getSampleCount() > 0) {
    display.showSamplingMode(sampler.getSampleCount(), lastAvgColor.red,
                             lastAvgColor.green, lastAvgColor.blue,
                             lastColorName);
  } else {
    display.showMessage("Ready!", "Press to sample");
  }
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

void SamplingController::checkAutoLedOff() {
  if (!sensor.isLedOn())
    return;

  if (millis() - lastActivityTime > autoLedOffTimeout) {
    sensor.setLed(false);
    display.showMessage("Auto sleep", "Press to wake");
    Serial.println("Auto LED off due to inactivity");
    updateActivity(); // Reset to avoid repeated triggers
  }
}

void SamplingController::handleWakeUp() {
  sensor.setLed(true);
  updateActivity();
  display.showMessage("Waking up...", "");
  delay(500);
  display.showMessage("Ready!", "Press to sample");
  waitForButtonRelease();
}

// ============================================================================
// Event Handlers
// ============================================================================

void SamplingController::onSampleTaken(const RGBColor &color) {
  updateActivity();
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

void SamplingController::onLongPress() {
  updateActivity();

  if (!canFinalize())
    return;

  RGBColor avgColor = sampler.getAverage();
  String avgColorName = sensor.detectColorName(avgColor);
  sampler.printAverage(avgColor, avgColorName);

  // Show final result
  display.showMessage("FINAL: " + avgColorName,
                      "R:" + String(avgColor.red) +
                          " G:" + String(avgColor.green) +
                          " B:" + String(avgColor.blue));

  // Send via BLE
  String bleData = String(avgColor.red) + "," + String(avgColor.green) + "," +
                   String(avgColor.blue) + "," + avgColorName;
  Serial.println("Sending: " + bleData);
  ble.send(bleData);

  // Wait for user acknowledgment
  waitForButtonRelease();
  Serial.println("Press button to continue...");
  waitForButtonPress();
  waitForButtonRelease();

  // Reset for next measurement
  sampler.reset();
  display.showMessage("Ready!", "Press to sample");
  Serial.println("Ready for new samples.");
  delay(300);
}

void SamplingController::onLedToggle() {
  updateActivity();
  sensor.toggleLed();

  if (sensor.isLedOn()) {
    display.showMessage("LED ON", "Ready to sample");
    Serial.println("LED ON");
  } else {
    display.showMessage("LED OFF", "Power saving mode");
    Serial.println("LED OFF - Power saving");
  }

  waitForButtonRelease();
  delay(1000);

  if (sensor.isLedOn()) {
    showCurrentState();
  }
}

void SamplingController::onTripleTap() {
  updateActivity();

  if (sampler.getSampleCount() == 0) {
    display.showMessage("Nothing to reset", "");
    Serial.println("Nothing to reset");
  } else {
    int cleared = sampler.getSampleCount();
    sampler.reset();
    display.showMessage("Samples cleared!", String(cleared) + " removed");
    Serial.print("Cleared ");
    Serial.print(cleared);
    Serial.println(" samples");
  }

  delay(1500);
  display.showMessage("Ready!", "Press to sample");
}

// ============================================================================
// Main Update Loop
// ============================================================================

void SamplingController::update() {
  button.updateTapCount();

  // Handle triple tap
  int tapCount = button.getTapCount();
  if (tapCount >= 3) {
    onTripleTap();
    button.resetTapCount();
    return;
  }

  checkAutoLedOff();

  // Handle wake-up from sleep
  if (!sensor.isLedOn() && button.isPressed()) {
    handleWakeUp();
    return;
  }

  // Skip processing if LED is off (sleep mode)
  if (!sensor.isLedOn()) {
    delay(100);
    return;
  }

  bool currentButtonState = button.isPressed();

  if (currentButtonState) {
    updateActivity();

    // Take sample on button press
    if (!lastButtonState) {
      RGBColor color = sensor.readColor();
      onSampleTaken(color);
    }

    unsigned long duration = button.getPressedDuration();

    // Check for LED toggle (5s)
    if (duration >= ledToggleDuration && !ledToggleHandled) {
      Serial.println("5s hold - LED toggle");
      onLedToggle();
      ledToggleHandled = true;
      longPressHandled = true;
      return;
    }

    // Show progress feedback
    if (duration > PROGRESS_SHOW_DELAY && !longPressHandled &&
        !ledToggleHandled) {
      if (duration < longPressDuration) {
        int progress = min(100, (int)((duration * 100) / longPressDuration));
        display.showProgress(progress);
      } else if (duration < ledToggleDuration) {
        int ledProgress =
            min(100, (int)(((duration - longPressDuration) * 100) /
                           (ledToggleDuration - longPressDuration)));
        display.showMessage("LED Toggle:", String(ledProgress) + "%");
      }
    }

    // Check for finalize (2s)
    if (button.isPressedFor(longPressDuration) && !longPressHandled &&
        !ledToggleHandled) {
      Serial.println("2s hold - Finalize");
      onLongPress();
      longPressHandled = true;
    }

  } else {
    // Button released
    if (lastButtonState && !longPressHandled && !ledToggleHandled) {
      showCurrentState();
    }
    longPressHandled = false;
    ledToggleHandled = false;
  }

  lastButtonState = currentButtonState;
  delay(10);
}
