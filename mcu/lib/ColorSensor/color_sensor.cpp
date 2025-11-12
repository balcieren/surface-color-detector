#include "color_sensor.h"

ColorSensor::ColorSensor() : redFreq(0), greenFreq(0), blueFreq(0)
{
}

int ColorSensor::readColorChannel(uint8_t s2State, uint8_t s3State, int minFreq, int maxFreq)
{
  digitalWrite(S2, s2State);
  digitalWrite(S3, s3State);
  int freq = pulseIn(SENSOR_OUT, LOW);
  int value = map(freq, minFreq, maxFreq, 255, 0);
  value = constrain(value, 0, 255);
  delay(50);
  return value;
}

void ColorSensor::begin()
{
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(SENSOR_OUT, INPUT);
  pinMode(LED, OUTPUT);

  // Setting frequency scaling to 20%
  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);

  // Turn on LED
  digitalWrite(LED, HIGH);
}

RGBColor ColorSensor::readColor()
{
  RGBColor color;

  color.red = readColorChannel(LOW, LOW, 36, 255);     // Red filter
  color.green = readColorChannel(HIGH, HIGH, 35, 255); // Green filter
  color.blue = readColorChannel(LOW, HIGH, 34, 255);   // Blue filter

  return color;
}

String ColorSensor::detectColorName(const RGBColor &color)
{
  if (color.red > 200 && color.green < 100 && color.blue < 100)
    return "RED";
  else if (color.green > 150 && color.red < 150 && color.blue < 150)
    return "GREEN";
  else if (color.blue > 200 && color.red < 100 && color.green < 100)
    return "BLUE";
  else if (color.red > 200 && color.green > 200 && color.blue < 100)
    return "YELLOW";
  else if (color.red < 50 && color.green < 50 && color.blue < 50)
    return "BLACK";
  else if (color.red > 200 && color.green > 200 && color.blue > 200)
    return "WHITE";
  else
    return "UNDEFINED";
}

void ColorSensor::printColorData(const RGBColor &color, const String &colorName)
{
  Serial.print("R: ");
  Serial.print(color.red);
  Serial.print(" G: ");
  Serial.print(color.green);
  Serial.print(" B: ");
  Serial.print(color.blue);
  Serial.print(" | Color: ");
  Serial.println(colorName);
}
