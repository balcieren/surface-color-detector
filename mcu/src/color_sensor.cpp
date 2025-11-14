#include "color_sensor.h"

ColorSensor::ColorSensor(uint8_t s0, uint8_t s1, uint8_t s2, uint8_t s3, uint8_t out, uint8_t led)
    : s0Pin(s0), s1Pin(s1), s2Pin(s2), s3Pin(s3), outPin(out), ledPin(led),
      redFreq(0), greenFreq(0), blueFreq(0)
{
}

int ColorSensor::readColorChannel(uint8_t s2State, uint8_t s3State, int minFreq, int maxFreq)
{
  digitalWrite(s2Pin, s2State);
  digitalWrite(s3Pin, s3State);
  int freq = pulseIn(outPin, LOW);
  int value = map(freq, minFreq, maxFreq, 255, 0);
  value = constrain(value, 0, 255);
  delay(50);
  return value;
}

void ColorSensor::begin()
{
  pinMode(s0Pin, OUTPUT);
  pinMode(s1Pin, OUTPUT);
  pinMode(s2Pin, OUTPUT);
  pinMode(s3Pin, OUTPUT);
  pinMode(outPin, INPUT);
  pinMode(ledPin, OUTPUT);

  digitalWrite(s0Pin, HIGH);
  digitalWrite(s1Pin, LOW);

  digitalWrite(ledPin, HIGH);
}

RGBColor ColorSensor::readColor()
{
  RGBColor color;

  color.red = readColorChannel(LOW, LOW, 36, 255);
  color.green = readColorChannel(HIGH, HIGH, 35, 255);
  color.blue = readColorChannel(LOW, HIGH, 34, 255);

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
