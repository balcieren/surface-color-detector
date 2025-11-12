#include "display.h"
#include <Wire.h>

Display::Display() : oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET)
{
}

void Display::prepareDisplay()
{
  oled.clearDisplay();
  oled.setTextSize(1);
  oled.setTextColor(SSD1306_WHITE);
}

bool Display::begin()
{
  Serial.println("Initializing OLED...");

  if (!oled.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
  {
    Serial.println(F("SSD1306 allocation failed!"));
    Serial.println(F("Check connections:"));
    Serial.println(F("  VCC -> 3.3V"));
    Serial.println(F("  GND -> GND"));
    Serial.println(F("  SDA -> GPIO 21"));
    Serial.println(F("  SCL -> GPIO 22"));
    return false;
  }

  Serial.println("OLED initialized successfully!");
  return true;
}

void Display::showWelcome()
{
  prepareDisplay();
  oled.setCursor(0, 0);
  oled.println(F("RGB Color Sensor"));
  oled.println(F("TCS3200"));
  oled.display();
}

void Display::showColorData(int red, int green, int blue, String colorName)
{
  prepareDisplay();

  // Title
  oled.setCursor(0, 0);
  oled.println(F("RGB Color Values:"));

  // RGB values
  oled.setCursor(0, 12);
  oled.print(F("R:"));
  oled.print(red);
  oled.print(F(" G:"));
  oled.print(green);
  oled.print(F(" B:"));
  oled.println(blue);

  // Color name
  oled.setCursor(0, 24);
  oled.print(colorName);

  oled.display();
}
