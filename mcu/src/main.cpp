/*********
  ESP32 TCS3200 Color Sensor with OLED Display
*********/
#include <Arduino.h>
#include <Wire.h>
#include "display.h"
#include "color_sensor.h"

Display display(128, 32, 21, 22);
ColorSensor sensor(25, 26, 27, 14, 33, 32);

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

  Serial.println("Setup complete!");
}

void loop()
{
  RGBColor color = sensor.readColor();
  String colorName = sensor.detectColorName(color);

  display.showColorData(color.red, color.green, color.blue, colorName);
  sensor.printColorData(color, colorName);

  delay(200);
}
