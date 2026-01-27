#include "color_sensor.h"

// Comment out to disable debug output
#define DEBUG_SENSOR

static const unsigned long PULSE_TIMEOUT = 100000;
static const unsigned long FILTER_SETTLING_TIME = 20;

ColorSensor::ColorSensor(uint8_t s0, uint8_t s1, uint8_t s2, uint8_t s3,
                         uint8_t out, uint8_t led)
    : s0Pin(s0), s1Pin(s1), s2Pin(s2), s3Pin(s3), outPin(out), ledPin(led),
      redFreq(0), greenFreq(0), blueFreq(0) {}

void ColorSensor::begin() {
  // Configure pins
  pinMode(s0Pin, OUTPUT);
  pinMode(s1Pin, OUTPUT);
  pinMode(s2Pin, OUTPUT);
  pinMode(s3Pin, OUTPUT);
  pinMode(outPin, INPUT);
  pinMode(ledPin, OUTPUT);

  // Set frequency scaling to 20%
  digitalWrite(s0Pin, HIGH);
  digitalWrite(s1Pin, LOW);

  // Turn on sensor LED by default
  digitalWrite(ledPin, HIGH);
}

// ============================================================================
// LED Control
// ============================================================================

void ColorSensor::ensureLedOn() {
  if (digitalRead(ledPin) != HIGH) {
    digitalWrite(ledPin, HIGH);
  }
}

void ColorSensor::toggleLed() { digitalWrite(ledPin, isLedOn() ? LOW : HIGH); }

void ColorSensor::setLed(bool on) { digitalWrite(ledPin, on ? HIGH : LOW); }

bool ColorSensor::isLedOn() { return digitalRead(ledPin) == HIGH; }

// ============================================================================
// Color Reading
// ============================================================================

unsigned long ColorSensor::readFrequency(bool s2State, bool s3State) {
  digitalWrite(s2Pin, s2State ? HIGH : LOW);
  digitalWrite(s3Pin, s3State ? HIGH : LOW);
  delay(FILTER_SETTLING_TIME);
  return pulseIn(outPin, LOW, PULSE_TIMEOUT);
}

RGBColor ColorSensor::readColor() {
  RGBColor color = {0, 0, 0};

  // Read raw frequencies for each channel
  unsigned long rFreq = readFrequency(false, false); // Red
  unsigned long gFreq = readFrequency(true, true);   // Green
  unsigned long bFreq = readFrequency(false, true);  // Blue

  // Check for sensor timeout
  if (rFreq == 0 || gFreq == 0 || bFreq == 0) {
#ifdef DEBUG_SENSOR
    Serial.println("WARNING: Sensor timeout! Check connections.");
#endif
    return color;
  }

#ifdef DEBUG_SENSOR
  Serial.print("Freq R:");
  Serial.print(rFreq);
  Serial.print(" G:");
  Serial.print(gFreq);
  Serial.print(" B:");
  Serial.println(bFreq);
#endif

  // Map frequencies to RGB (0-255) using calibration values
  color.red =
      constrain(map(rFreq, WHITE_RED_FREQ, BLACK_RED_FREQ, 255, 0), 0, 255);
  color.green =
      constrain(map(gFreq, WHITE_GREEN_FREQ, BLACK_GREEN_FREQ, 255, 0), 0, 255);
  color.blue =
      constrain(map(bFreq, WHITE_BLUE_FREQ, BLACK_BLUE_FREQ, 255, 0), 0, 255);

  return color;
}

// ============================================================================
// Color Detection
// ============================================================================

String ColorSensor::detectColorName(const RGBColor &color) {
  const int r = color.red;
  const int g = color.green;
  const int b = color.blue;
  const int brightness = (r + g + b) / 3;

  // Helper lambdas for readability
  auto isGrayish = [&](int tolerance) {
    return abs(r - g) < tolerance && abs(g - b) < tolerance &&
           abs(r - b) < tolerance;
  };

  // Grayscale detection (ordered by brightness)
  if (r < 30 && g < 30 && b < 30)
    return "BLACK";
  if (brightness < 50 && isGrayish(20))
    return "DARK GRAY";
  if (brightness < 120 && isGrayish(25))
    return "GRAY";
  if (brightness < 200 && isGrayish(30))
    return "LIGHT GRAY";
  if (r > 200 && g > 200 && b > 200)
    return "WHITE";

  // Chromatic colors (order matters for proper detection)
  // Yellow - check before RED (both R and G high)
  if (r > 120 && g > 120 && b < 80 && abs(r - g) < 50)
    return "YELLOW";

  // Orange - check before RED
  if (r > 150 && g > 60 && g < 140 && b < 70)
    return "ORANGE";

  // Primary colors
  if (r > g + 25 && r > b + 25) {
    return brightness < 80 ? "DARK RED" : "RED";
  }
  if (g > r + 40 && g > b + 40) {
    return brightness < 80 ? "DARK GREEN" : "GREEN";
  }
  if (b > r + 40 && b > g + 40) {
    return brightness < 80 ? "DARK BLUE" : "BLUE";
  }

  // Secondary colors
  if (r > 80 && r < 180 && g > 40 && g < 120 && b < 80)
    return "BROWN";
  if (g > 150 && b > 150 && r < 100)
    return "CYAN";

  if (r > 120 && b > 120 && g < 100) {
    return r > b + 30 ? "MAGENTA" : "PURPLE";
  }

  if (r > 180 && g > 100 && g < 180 && b > 120 && b < 200)
    return "PINK";

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
