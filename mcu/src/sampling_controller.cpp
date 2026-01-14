#include "sampling_controller.h"

SamplingController::SamplingController(Display &disp, ColorSensor &sens,
                                       ColorSampler &samp, Button &btn,
                                       Bluetooth &bluetooth)
    : display(disp), sensor(sens), sampler(samp), button(btn), ble(bluetooth),
      longPressDuration(2000), minSamplesRequired(3), lastButtonState(false),
      longPressHandled(false), lastAvgColor({0, 0, 0}), lastColorName("") {}

void SamplingController::begin() {
  display.showMessage("Ready!", "Press to sample");
  Serial.println("Short press: Add sample | Long press (2s): Finalize");
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

void SamplingController::update() {
  sensor.ensureLedOn();
  bool currentButtonState = button.isPressed();

  if (currentButtonState) {
    if (!lastButtonState) {
      // Read color only when taking a sample
      RGBColor color = sensor.readColor();
      onSampleTaken(color);
    }

    unsigned long duration = button.getPressedDuration();
    if (duration > 0 && !longPressHandled) {
      int progress = min(100, (int)((duration * 100) / longPressDuration));

      // Show progress bar after 200ms of holding
      if (duration > 200) {
        display.showProgress(progress);
      }
    }

    if (button.isPressedFor(longPressDuration) && !longPressHandled) {
      Serial.println("Long press detected! Finalizing...");
      onLongPress();
      longPressHandled = true;
    }
  } else {
    if (lastButtonState && !longPressHandled) {
      if (sampler.getSampleCount() > 0) {
        display.showSamplingMode(sampler.getSampleCount(), lastAvgColor.red,
                                 lastAvgColor.green, lastAvgColor.blue,
                                 lastColorName);
      }
    }
    longPressHandled = false;
  }

  lastButtonState = currentButtonState;
  delay(10); // Small delay to prevent CPU hogging
}
