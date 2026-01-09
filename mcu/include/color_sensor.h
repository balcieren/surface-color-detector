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

  int readColorChannel(uint8_t s2State, uint8_t s3State, int minFreq,
                       int maxFreq);

public:
  ColorSensor(uint8_t s0, uint8_t s1, uint8_t s2, uint8_t s3, uint8_t out,
              uint8_t led);
  void begin();
  void ensureLedOn();
  RGBColor readColor();
  String detectColorName(const RGBColor &color);
  void printColorData(const RGBColor &color, const String &colorName);
};

#endif
