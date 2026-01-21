#include "color_sensor.h"

// Comment out to disable debug output
#define DEBUG_SENSOR

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

void ColorSensor::ensureLedOn() {
  // LED is already on from begin(), no need to toggle
}

RGBColor ColorSensor::readColor() {
  RGBColor color;

  // Read raw frequencies once
  digitalWrite(s2Pin, LOW);
  digitalWrite(s3Pin, LOW);
  delay(20);
  int redFreq = pulseIn(outPin, LOW, 100000);

  digitalWrite(s2Pin, HIGH);
  digitalWrite(s3Pin, HIGH);
  delay(20);
  int greenFreq = pulseIn(outPin, LOW, 100000);

  digitalWrite(s2Pin, LOW);
  digitalWrite(s3Pin, HIGH);
  delay(20);
  int blueFreq = pulseIn(outPin, LOW, 100000);

  // Debug: Print raw frequencies for calibration
#ifdef DEBUG_SENSOR
  Serial.print("Freq R:");
  Serial.print(redFreq);
  Serial.print(" G:");
  Serial.print(greenFreq);
  Serial.print(" B:");
  Serial.println(blueFreq);
#endif

  // Map frequencies to RGB values (0-255)
  // Calibrated: WHITE minFreq, BLACK maxFreq
  color.red = constrain(map(redFreq, 26, 155, 255, 0), 0, 255);
  color.green = constrain(map(greenFreq, 24, 166, 255, 0), 0, 255);
  color.blue = constrain(map(blueFreq, 30, 197, 255, 0), 0, 255);

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

  // YELLOW - check before RED (both R and G are high in yellow)
  if (r > 120 && g > 120 && b < 80 && abs(r - g) < 50) {
    return "YELLOW";
  }

  // ORANGE - check before RED
  if (r > 150 && g > 60 && g < 140 && b < 70) {
    return "ORANGE";
  }

  // RED - lowered threshold for better detection
  if (r > g + 25 && r > b + 25) {
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

  // BROWN
  if (r > 80 && r < 180 && g > 40 && g < 120 && b < 80) {
    return "BROWN";
  }

  // CYAN
  if (g > 150 && b > 150 && r < 100) {
    return "CYAN";
  }

  // MAGENTA / PURPLE
  if (r > 120 && b > 120 && g < 100) {
    if (r > b + 30)
      return "MAGENTA";
    return "PURPLE";
  }

  // PINK
  if (r > 180 && g > 100 && g < 180 && b > 120 && b < 200) {
    return "PINK";
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
