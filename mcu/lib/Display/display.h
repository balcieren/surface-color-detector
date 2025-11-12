#ifndef DISPLAY_H
#define DISPLAY_H

#include <Adafruit_SSD1306.h>
#include <Arduino.h>

// OLED Display Settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

class Display
{
private:
  Adafruit_SSD1306 oled;

  // Helper method to setup display for drawing
  void prepareDisplay();

public:
  Display();
  bool begin();
  void showWelcome();
  void showColorData(int red, int green, int blue, String colorName);
};

#endif
