/*********
  Rui Santos
  Complete project details at http://randomnerdtutorials.com
*********/
#include <Arduino.h>

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
  Serial.begin(9600);
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
  delay(100);

  // Green filter
  digitalWrite(S2, HIGH);
  digitalWrite(S3, HIGH);
  green_f = pulseIn(sensorOut, LOW);
  // Adjust mapping values based on actual sensor readings for green
  green_f = map(green_f, 35, 255, 255, 0);
  green_f = constrain(green_f, 0, 255);
  delay(100);

  // Blue filter
  digitalWrite(S2, LOW);
  digitalWrite(S3, HIGH);
  blue_f = pulseIn(sensorOut, LOW);
  blue_f = map(blue_f, 34, 255, 255, 0);
  blue_f = constrain(blue_f, 0, 255);
  delay(100);

  // Print RGB values
  Serial.print("R: ");
  Serial.print(red_f);
  Serial.print(" G: ");
  Serial.print(green_f);
  Serial.print(" B: ");
  Serial.print(blue_f);
  Serial.print(" | Color: ");
  // Color detection
  if (red_f > 200 && green_f < 100 && blue_f < 100)
  {
    Serial.println("RED");
  }
  else if (green_f > 150 && red_f < 150 && blue_f < 150)
  {
    Serial.println("GREEN");
  }
  else if (blue_f > 200 && red_f < 100 && green_f < 100)
  {
  }
  else if (blue_f > 200 && red_f < 100 && green_f < 100)
  {
    Serial.println("BLUE");
  }
  else if (red_f > 200 && green_f > 200 && blue_f < 100)
  {
    Serial.println("YELLOW");
  }
  else if (red_f < 50 && green_f < 50 && blue_f < 50)
  {
    Serial.println("BLACK");
  }
  else if (red_f > 200 && green_f > 200 && blue_f > 200)
  {
    Serial.println("WHITE");
  }
  else
  {
    Serial.println("UNDEFINED");
  }

  delay(500);
}
