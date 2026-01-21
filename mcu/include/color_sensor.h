#ifndef COLOR_SENSOR_H
#define COLOR_SENSOR_H

#include <Arduino.h>

struct RGBColor {
  int red;
  int green;
  int blue;
};

class ColorSensor {
private:
  uint8_t s0Pin;
  uint8_t s1Pin;
  uint8_t s2Pin;
  uint8_t s3Pin;
  uint8_t outPin;
  uint8_t ledPin;

  int redFreq;
  int greenFreq;
  int blueFreq;

public:
  ColorSensor(uint8_t s0, uint8_t s1, uint8_t s2, uint8_t s3, uint8_t out,
              uint8_t led);
  void begin();
  void ensureLedOn();
  void setLed(bool state);
  void toggleLed();
  bool isLedOn() const;
  RGBColor readColor();
  String detectColorName(const RGBColor &color);
  void printColorData(const RGBColor &color, const String &colorName);
};

#endif
