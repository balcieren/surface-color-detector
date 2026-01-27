#ifndef COLOR_SENSOR_H
#define COLOR_SENSOR_H

#include <Arduino.h>

struct RGBColor {
  int red;
  int green;
  int blue;
};

class ColorSensor {
public:
  // Calibration values (adjust based on your sensor)
  static const int WHITE_RED_FREQ = 26;
  static const int WHITE_GREEN_FREQ = 24;
  static const int WHITE_BLUE_FREQ = 30;
  static const int BLACK_RED_FREQ = 155;
  static const int BLACK_GREEN_FREQ = 166;
  static const int BLACK_BLUE_FREQ = 197;

  ColorSensor(uint8_t s0, uint8_t s1, uint8_t s2, uint8_t s3, uint8_t out,
              uint8_t led);

  void begin();

  // LED Control
  void ensureLedOn();
  void toggleLed();
  void setLed(bool on);
  bool isLedOn();

  // Color Reading
  RGBColor readColor();
  String detectColorName(const RGBColor &color);
  void printColorData(const RGBColor &color, const String &colorName);

private:
  uint8_t s0Pin, s1Pin, s2Pin, s3Pin;
  uint8_t outPin;
  uint8_t ledPin;

  int redFreq, greenFreq, blueFreq;

  unsigned long readFrequency(bool s2State, bool s3State);
};

#endif
