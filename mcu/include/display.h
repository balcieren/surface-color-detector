#ifndef DISPLAY_H
#define DISPLAY_H

#include <Adafruit_SSD1306.h>
#include <Arduino.h>

class Display
{
private:
  Adafruit_SSD1306 oled;
  uint8_t sdaPin;
  uint8_t sclPin;
  uint8_t screenWidth;
  uint8_t screenHeight;
  int8_t resetPin;
  uint8_t i2cAddress;

  void prepareDisplay();

public:
  Display(uint8_t width, uint8_t height, uint8_t sda, uint8_t scl, uint8_t address = 0x3C, int8_t reset = -1);
  bool begin();
  void showWelcome();
  void showColorData(int red, int green, int blue, String colorName);
};

#endif
