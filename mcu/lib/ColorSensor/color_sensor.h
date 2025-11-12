#ifndef COLOR_SENSOR_H
#define COLOR_SENSOR_H

#include <Arduino.h>

// TCS230 or TCS3200 pins
#define S0 25
#define S1 26
#define S2 27
#define S3 14
#define LED 32
#define SENSOR_OUT 33

struct RGBColor
{
  int red;
  int green;
  int blue;
};

class ColorSensor
{
private:
  int redFreq;
  int greenFreq;
  int blueFreq;

  // Helper method to read a single color channel
  int readColorChannel(uint8_t s2State, uint8_t s3State, int minFreq, int maxFreq);

public:
  ColorSensor();
  void begin();
  RGBColor readColor();
  String detectColorName(const RGBColor &color);
  void printColorData(const RGBColor &color, const String &colorName);
};

#endif
