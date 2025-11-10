/*********
  ESP32 TCS3200 Color Sensor with OLED Display
*********/
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED Display ayarları
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// TCS230 or TCS3200 pins wiring to ESP32 WROOM
#define S0 25
#define S1 26
#define S2 27
#define S3 14
#define LED 32

#define sensorOut 33
int red_f = 0, green_f = 0, blue_f = 0;

void setup()
{
  Serial.begin(115200);
  delay(1000);
  Serial.println("Starting...");

  // Initialize I2C
  Wire.begin(21, 22); // Explicitly set SDA=21, SCL=22
  Serial.println("I2C initialized");

  // Scan for I2C devices
  Serial.println("Scanning I2C bus...");
  byte count = 0;
  for (byte i = 8; i < 120; i++)
  {
    Wire.beginTransmission(i);
    if (Wire.endTransmission() == 0)
    {
      Serial.print("Found I2C device at address 0x");
      Serial.println(i, HEX);
      count++;
    }
  }
  Serial.print("Found ");
  Serial.print(count);
  Serial.println(" device(s)");

  // Initialize OLED display
  Serial.println("Initializing OLED...");
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
  {
    Serial.println(F("SSD1306 allocation failed!"));
    Serial.println(F("Check connections:"));
    Serial.println(F("  VCC -> 3.3V"));
    Serial.println(F("  GND -> GND"));
    Serial.println(F("  SDA -> GPIO 21"));
    Serial.println(F("  SCL -> GPIO 22"));
    for (;;)
      ; // Don't proceed, loop forever
  }

  Serial.println("OLED initialized successfully!");

  // Clear display and show welcome message
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(F("RGB Color Sensor"));
  display.println(F("TCS3200"));
  display.display();
  delay(2000);

  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(sensorOut, INPUT);
  pinMode(LED, OUTPUT);

  // Setting frequency scaling to 20%
  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);

  // Turn on LED
  digitalWrite(LED, HIGH);
}

void loop()
{
  // Red filter
  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);
  red_f = pulseIn(sensorOut, LOW);
  red_f = map(red_f, 36, 255, 255, 0);
  red_f = constrain(red_f, 0, 255);
  delay(50);

  // Green filter
  digitalWrite(S2, HIGH);
  digitalWrite(S3, HIGH);
  green_f = pulseIn(sensorOut, LOW);
  // Adjust mapping values based on actual sensor readings for green
  green_f = map(green_f, 35, 255, 255, 0);
  green_f = constrain(green_f, 0, 255);
  delay(50);

  // Blue filter
  digitalWrite(S2, LOW);
  digitalWrite(S3, HIGH);
  blue_f = pulseIn(sensorOut, LOW);
  blue_f = map(blue_f, 34, 255, 255, 0);
  blue_f = constrain(blue_f, 0, 255);
  delay(50);

  // Display RGB values on OLED
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  // Title
  display.setCursor(0, 0);
  display.println(F("RGB Color Values:"));

  // RGB values
  display.setCursor(0, 12);
  display.print(F("R:"));
  display.print(red_f);
  display.print(F(" G:"));
  display.print(green_f);
  display.print(F(" B:"));
  display.println(blue_f);

  // Color name
  display.setCursor(0, 24);
  String colorName = "";

  if (red_f > 200 && green_f < 100 && blue_f < 100)
    colorName = "RED";
  else if (green_f > 150 && red_f < 150 && blue_f < 150)
    colorName = "GREEN";
  else if (blue_f > 200 && red_f < 100 && green_f < 100)
    colorName = "BLUE";
  else if (red_f > 200 && green_f > 200 && blue_f < 100)
    colorName = "YELLOW";
  else if (red_f < 50 && green_f < 50 && blue_f < 50)
    colorName = "BLACK";
  else if (red_f > 200 && green_f > 200 && blue_f > 200)
    colorName = "WHITE";
  else
    colorName = "UNDEFINED";

  display.print(colorName);
  display.display();

  // Print RGB values to Serial
  Serial.print("R: ");
  Serial.print(red_f);
  Serial.print(" G: ");
  Serial.print(green_f);
  Serial.print(" B: ");
  Serial.print(blue_f);
  Serial.print(" | Color: ");
  Serial.println(colorName);

  delay(200);
}
