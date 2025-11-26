#include "color_sampler.h"

ColorSampler::ColorSampler()
    : totalRed(0), totalGreen(0), totalBlue(0), sampleCount(0), sampling(false)
{
}

void ColorSampler::addSample(const RGBColor &color)
{
  totalRed += color.red;
  totalGreen += color.green;
  totalBlue += color.blue;
  sampleCount++;
  sampling = true;
}

RGBColor ColorSampler::getAverage()
{
  RGBColor avg = {0, 0, 0};

  if (sampleCount > 0)
  {
    avg.red = totalRed / sampleCount;
    avg.green = totalGreen / sampleCount;
    avg.blue = totalBlue / sampleCount;
  }

  return avg;
}

int ColorSampler::getSampleCount()
{
  return sampleCount;
}

bool ColorSampler::isSampling()
{
  return sampling;
}

void ColorSampler::reset()
{
  totalRed = 0;
  totalGreen = 0;
  totalBlue = 0;
  sampleCount = 0;
  sampling = false;
}

void ColorSampler::printSample(const RGBColor &color)
{
  Serial.print("Sample #");
  Serial.print(sampleCount);
  Serial.print(" - R:");
  Serial.print(color.red);
  Serial.print(" G:");
  Serial.print(color.green);
  Serial.print(" B:");
  Serial.println(color.blue);
}

void ColorSampler::printAverage(const RGBColor &avgColor, const String &colorName)
{
  Serial.println("===== AVERAGE COLOR =====");
  Serial.print("Samples: ");
  Serial.println(sampleCount);
  Serial.print("Avg R:");
  Serial.print(avgColor.red);
  Serial.print(" G:");
  Serial.print(avgColor.green);
  Serial.print(" B:");
  Serial.println(avgColor.blue);
  Serial.print("Color: ");
  Serial.println(colorName);
  Serial.println("=========================");
}
