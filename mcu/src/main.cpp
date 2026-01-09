/*********
  ESP32 TCS3200 Color Sensor with OLED Display
*********/
#include "ble_service.h"
#include "button.h"
#include "color_sampler.h"
#include "color_sensor.h"
#include "display.h"
#include <Arduino.h>
#include <Wire.h>

Display display(128, 32, 21, 22);
ColorSensor sensor(27, 14, 32, 33, 35, 26);
ColorSampler sampler;
Button button(13);
// Bluetooth ble;  // BLE: uncomment to enable

void handleSampling(const RGBColor &color) {
  sampler.addSample(color);

  RGBColor avgColor = sampler.getAverage();
  String avgColorName = sensor.detectColorName(avgColor);

  display.showColorData(avgColor.red, avgColor.green, avgColor.blue,
                        avgColorName);
  sampler.printSample(color);
}

void handleSampleComplete() {
  RGBColor avgColor = sampler.getAverage();
  String avgColorName = sensor.detectColorName(avgColor);

  sampler.printAverage(avgColor, avgColorName);
  display.showColorData(avgColor.red, avgColor.green, avgColor.blue,
                        avgColorName);

  String bleData = String(avgColor.red) + "," + String(avgColor.green) + "," +
                   String(avgColor.blue) + "," + avgColorName;
  Serial.println("Average Color: " + bleData);
  // ble.send(bleData);  // BLE: uncomment to enable

  delay(3000);

  sampler.reset();
}

void handleNormalMode(const RGBColor &color, const String &colorName) {
  display.showColorData(color.red, color.green, color.blue, colorName);
  sensor.printColorData(color, colorName);
}

void setup() {
  Serial.begin(9600);
  delay(1000);
  Serial.println("Starting...");

  if (!display.begin()) {
    for (;;)
      ;
  }

  display.showWelcome();
  delay(2000);

  sensor.begin();
  button.begin();
  // ble.begin("Surface Color Detector");  // BLE: uncomment to enable

  Serial.println("Setup complete!");
}

void loop() {
  sensor.ensureLedOn(); // Ensure LED stays on
  RGBColor color = sensor.readColor();

  if (button.isPressed())
    handleSampling(color);
  else if (sampler.isSampling())
    handleSampleComplete();

  delay(100);
}
