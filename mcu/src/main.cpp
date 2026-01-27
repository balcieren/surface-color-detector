/*********
  ESP32 TCS3200 Color Sensor with OLED Display
*********/
#include "ble_service.h"
#include "button.h"
#include "color_sampler.h"
#include "color_sensor.h"
#include "display.h"
#include "sampling_controller.h"
#include <Arduino.h>
#include <Wire.h>

// Hardware configuration
Display display(128, 32, 21, 22);
ColorSensor sensor(27, 25, 32, 33, 35, 26);
ColorSampler sampler;
Button button(13);
Bluetooth ble;

// Controller
SamplingController controller(display, sensor, sampler, button, ble);

void setup()
{
  Serial.begin(115200);
  delay(1000);
  Serial.println("Starting...");

  if (!display.begin())
  {
    for (;;)
      ;
  }

  display.showSplash();
  delay(2500);

  display.showWelcome();
  delay(1500);

  sensor.begin();
  button.begin();
  ble.begin("Surface Color Detector");

  controller.begin();
  Serial.println("Setup complete!");
}

void loop()
{
  controller.update();
  delay(30);
}
