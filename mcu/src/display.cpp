#include "display.h"
#include <Wire.h>

Display::Display(uint8_t width, uint8_t height, uint8_t sda, uint8_t scl,
                 uint8_t address, int8_t reset)
    : oled(width, height, &Wire, reset), sdaPin(sda), sclPin(scl),
      screenWidth(width), screenHeight(height), resetPin(reset),
      i2cAddress(address) {}

void Display::prepareDisplay() {
  oled.clearDisplay();
  oled.setTextSize(1);
  oled.setTextColor(SSD1306_WHITE);
}

bool Display::begin() {
  delay(100);
  Wire.begin(sdaPin, sclPin);
  Serial.println("Initializing OLED...");

  for (int attempt = 1; attempt <= 3; attempt++) {
    if (oled.begin(SSD1306_SWITCHCAPVCC, i2cAddress)) {
      Serial.println("OLED initialized successfully!");
      return true;
    }
    Serial.print("OLED init attempt ");
    Serial.print(attempt);
    Serial.println(" failed, retrying...");
    delay(200);
  }

  Serial.println(F("SSD1306 allocation failed after 3 attempts!"));
  Serial.println(F("Check connections:"));
  Serial.print(F("  VCC -> 3.3V, GND -> GND"));
  Serial.print(F("  SDA -> GPIO "));
  Serial.print(sdaPin);
  Serial.print(F(", SCL -> GPIO "));
  Serial.println(sclPin);
  return false;
}

void Display::showSplash() {
  prepareDisplay();

  // Show WUST text centered (size 3 = 18x24 pixels, "WUST" = 72px wide)
  // Center: (128 - 72) / 2 = 28
  oled.setTextSize(3);
  oled.setCursor(28, 4);
  oled.print(F("WUST"));

  oled.display();
}

void Display::showWelcome() {
  prepareDisplay();
  oled.setCursor(34, 4);
  oled.println(F("Surface"));
  oled.setCursor(16, 18);
  oled.println(F("Color Detector"));
  oled.display();
}

void Display::showColorData(int red, int green, int blue, String colorName) {
  prepareDisplay();

  // Title
  oled.setCursor(0, 0);
  oled.println(F("RGB Color Values:"));

  // RGB values
  oled.setCursor(0, 12);
  oled.print(F("R:"));
  oled.print(red);
  oled.print(F(" G:"));
  oled.print(green);
  oled.print(F(" B:"));
  oled.println(blue);

  // Color name
  oled.setCursor(0, 24);
  oled.print(colorName);

  oled.display();
}

void Display::showSamplingMode(int sampleCount, int red, int green, int blue,
                               String colorName) {
  prepareDisplay();

  // Sampling indicator with count
  oled.setCursor(0, 0);
  oled.print(F("SAMPLING ["));
  oled.print(sampleCount);
  oled.println(F(" samples]"));

  // RGB values
  oled.setCursor(0, 12);
  oled.print(F("R:"));
  oled.print(red);
  oled.print(F(" G:"));
  oled.print(green);
  oled.print(F(" B:"));
  oled.println(blue);

  // Color name
  oled.setCursor(0, 24);
  oled.print(colorName);

  oled.display();
}

void Display::showProgress(int percentage) {
  prepareDisplay();

  oled.setCursor(0, 0);
  oled.println(F("Hold to finalize..."));

  // Progress bar
  int barWidth = screenWidth - 4;
  int filledWidth = (barWidth * percentage) / 100;

  oled.setCursor(0, 14);
  oled.drawRect(2, 14, barWidth, 10, SSD1306_WHITE);
  oled.fillRect(2, 14, filledWidth, 10, SSD1306_WHITE);

  // Percentage text
  oled.setCursor(50, 26);
  oled.print(percentage);
  oled.print(F("%"));

  oled.display();
}

void Display::showMessage(String line1, String line2) {
  prepareDisplay();

  oled.setCursor(0, 8);
  oled.println(line1);

  if (line2.length() > 0) {
    oled.setCursor(0, 20);
    oled.println(line2);
  }

  oled.display();
}
