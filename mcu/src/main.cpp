/*********
  ESP32 TCS3200 Color Sensor with OLED Display
*********/
#include <Arduino.h>
#include <Wire.h>
#include "display.h"
#include "color_sensor.h"

Display display;
ColorSensor sensor;

void setup()
{
  Serial.begin(115120);
  delay(1000);
  Serial.println("Starting...");

  // Initialize I2C
  Wire.begin(21, 22); // SDA=21, SCL=22
  Serial.println("I2C initialized");

  // Initialize OLED display
  if (!display.begin())
  {
    for (;;)
      ; // Don't proceed, loop forever
  }

  // Show welcome message
  display.showWelcome();
  delay(2000);

  // Initialize color sensor
  sensor.begin();

  Serial.println("Setup complete!");
}

void loop()
{
  // Read color from sensor
  RGBColor color = sensor.readColor();
  String colorName = sensor.detectColorName(color);

  // Display on OLED and Serial
  display.showColorData(color.red, color.green, color.blue, colorName);
  sensor.printColorData(color, colorName);

  delay(200);
}
