/*********
  ESP32 TCS3200 Color Sensor with OLED Display
*********/
#include <Arduino.h>
#include <Wire.h>
#include "display.h"
#include "color_sensor.h"
#include "color_sampler.h"
#include "button.h"
#include "ble_service.h"

Display display(128, 32, 21, 22);
ColorSensor sensor(25, 26, 27, 14, 33, 32);
ColorSampler sampler;
Button button(13);
Bluetooth ble;

void handleSampling(const RGBColor &color)
{
  sampler.addSample(color);

  RGBColor avgColor = sampler.getAverage();
  String avgColorName = sensor.detectColorName(avgColor);

  display.showColorData(avgColor.red, avgColor.green, avgColor.blue, avgColorName);
  sampler.printSample(color);
}

void handleSampleComplete()
{
  RGBColor avgColor = sampler.getAverage();
  String avgColorName = sensor.detectColorName(avgColor);

  sampler.printAverage(avgColor, avgColorName);
  display.showColorData(avgColor.red, avgColor.green, avgColor.blue, avgColorName);

  String bleData = String(avgColor.red) + "," +
                   String(avgColor.green) + "," +
                   String(avgColor.blue) + "," +
                   avgColorName;
  ble.send(bleData);

  delay(3000);

  sampler.reset();
}

void handleNormalMode(const RGBColor &color, const String &colorName)
{
  display.showColorData(color.red, color.green, color.blue, colorName);
  sensor.printColorData(color, colorName);
}

void setup()
{
  Serial.begin(9600);
  delay(1000);
  Serial.println("Starting...");

  if (!display.begin())
  {
    for (;;)
      ;
  }

  display.showWelcome();
  delay(2000);

  sensor.begin();
  button.begin();
  ble.begin("Surface Color Detector");

  Serial.println("Setup complete!");
}

void loop()
{
  RGBColor color = sensor.readColor();

  if (button.isPressed())
    handleSampling(color);
  else if (sampler.isSampling())
    handleSampleComplete();

  delay(100);
}
