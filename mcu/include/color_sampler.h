#ifndef COLOR_SAMPLER_H
#define COLOR_SAMPLER_H

#include <Arduino.h>
#include "color_sensor.h"

class ColorSampler
{
private:
  long totalRed;
  long totalGreen;
  long totalBlue;
  int sampleCount;
  bool sampling;

public:
  ColorSampler();
  void addSample(const RGBColor &color);
  RGBColor getAverage();
  int getSampleCount();
  bool isSampling();
  void reset();
  void printSample(const RGBColor &color);
  void printAverage(const RGBColor &avgColor, const String &colorName);
};

#endif
