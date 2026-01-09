#include "color_sensor.h"

ColorSensor::ColorSensor(uint8_t s0, uint8_t s1, uint8_t s2, uint8_t s3,
                         uint8_t out, uint8_t led)
    : s0Pin(s0), s1Pin(s1), s2Pin(s2), s3Pin(s3), outPin(out), ledPin(led),
      redFreq(0), greenFreq(0), blueFreq(0) {}

void ColorSensor::begin() {
  pinMode(s0Pin, OUTPUT);
  pinMode(s1Pin, OUTPUT);
  pinMode(s2Pin, OUTPUT);
  pinMode(s3Pin, OUTPUT);
  pinMode(outPin, INPUT);
  pinMode(ledPin, OUTPUT);

  // Set frequency scaling to 20%
  digitalWrite(s0Pin, HIGH);
  digitalWrite(s1Pin, LOW);

  // Turn on sensor LED
  digitalWrite(ledPin, HIGH);
}

void ColorSensor::ensureLedOn() { digitalWrite(ledPin, HIGH); }

int ColorSensor::readColorChannel(uint8_t s2State, uint8_t s3State, int minFreq,
                                  int maxFreq) {
  digitalWrite(s2Pin, s2State);
  digitalWrite(s3Pin, s3State);
  digitalWrite(ledPin, HIGH); // Keep LED stable during read
  delay(20);                  // Reduced from 100ms for faster readings

  int freq = pulseIn(outPin, LOW);
  int mapped = map(freq, minFreq, maxFreq, 255, 0);
  return constrain(mapped, 0, 255);
}

RGBColor ColorSensor::readColor() {
  RGBColor color;

  // Red channel (S2=LOW, S3=LOW)
  color.red = readColorChannel(LOW, LOW, 25, 72);

  // Green channel (S2=HIGH, S3=HIGH)
  color.green = readColorChannel(HIGH, HIGH, 30, 90);

  // Blue channel (S2=LOW, S3=HIGH)
  color.blue = readColorChannel(LOW, HIGH, 25, 70);

  return color;
}

String ColorSensor::detectColorName(const RGBColor &color) {
  int r = color.red;
  int g = color.green;
  int b = color.blue;

  int brightness = (r + g + b) / 3;

  // BLACK
  if (r < 30 && g < 30 && b < 30) {
    return "BLACK";
  }

  // DARK GRAY
  if (brightness < 50 && abs(r - g) < 20 && abs(g - b) < 20 &&
      abs(r - b) < 20) {
    return "DARK GRAY";
  }

  // GRAY
  if (brightness < 120 && abs(r - g) < 25 && abs(g - b) < 25 &&
      abs(r - b) < 25) {
    return "GRAY";
  }

  // LIGHT GRAY
  if (brightness < 200 && abs(r - g) < 30 && abs(g - b) < 30 &&
      abs(r - b) < 30) {
    return "LIGHT GRAY";
  }

  // WHITE
  if (r > 200 && g > 200 && b > 200) {
    return "WHITE";
  }

  // RED
  if (r > g + 40 && r > b + 40) {
    if (brightness < 80)
      return "DARK RED";
    return "RED";
  }

  // GREEN
  if (g > r + 40 && g > b + 40) {
    if (brightness < 80)
      return "DARK GREEN";
    return "GREEN";
  }

  // BLUE
  if (b > r + 40 && b > g + 40) {
    if (brightness < 80)
      return "DARK BLUE";
    return "BLUE";
  }

  // YELLOW
  if (r > 150 && g > 150 && b < 100) {
    return "YELLOW";
  }

  // ORANGE
  if (r > 180 && g > 80 && g < 160 && b < 80) {
    return "ORANGE";
  }

  // BROWN
  if (r > 80 && r < 180 && g > 40 && g < 120 && b < 80) {
    return "BROWN";
  }

  // CYAN
  if (g > 150 && b > 150 && r < 100) {
    return "CYAN";
  }

  return "UNKNOWN";
}

void ColorSensor::printColorData(const RGBColor &color,
                                 const String &colorName) {
  Serial.print("R:");
  Serial.print(color.red);
  Serial.print(" G:");
  Serial.print(color.green);
  Serial.print(" B:");
  Serial.print(color.blue);
  Serial.print(" - ");
  Serial.println(colorName);
}
