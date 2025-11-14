#include "display.h"
#include <Wire.h>

Display::Display(uint8_t width, uint8_t height, uint8_t sda, uint8_t scl, uint8_t address, int8_t reset)
    : oled(width, height, &Wire, reset),
      sdaPin(sda), sclPin(scl),
      screenWidth(width), screenHeight(height),
      resetPin(reset), i2cAddress(address)
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
  Wire.begin(sdaPin, sclPin);
  Serial.println("Initializing OLED...");

  if (!oled.begin(SSD1306_SWITCHCAPVCC, i2cAddress))
  {
    Serial.println(F("SSD1306 allocation failed!"));
    Serial.println(F("Check connections:"));
    Serial.print(F("  VCC -> 3.3V, GND -> GND"));
    Serial.print(F("  SDA -> GPIO "));
    Serial.print(sdaPin);
    Serial.print(F(", SCL -> GPIO "));
    Serial.println(sclPin);
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
