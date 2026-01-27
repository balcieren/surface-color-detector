#ifndef SAMPLING_CONTROLLER_H
#define SAMPLING_CONTROLLER_H

#include "ble_service.h"
#include "button.h"
#include "color_sampler.h"
#include "color_sensor.h"
#include "display.h"
#include <Arduino.h>

class SamplingController {
public:
  // Timing configuration (ms)
  static const unsigned long LONG_PRESS_DURATION = 2000;
  static const unsigned long LED_TOGGLE_DURATION = 5000;
  static const unsigned long AUTO_LED_OFF_TIMEOUT = 120000; // 2 minutes
  static const int MIN_SAMPLES_REQUIRED = 3;

  SamplingController(Display &disp, ColorSensor &sens, ColorSampler &samp,
                     Button &btn, Bluetooth &bluetooth);

  void begin();
  void update();

  // Configuration setters
  void setLongPressDuration(unsigned long ms);
  void setMinSamplesRequired(int count);

private:
  // Dependencies
  Display &display;
  ColorSensor &sensor;
  ColorSampler &sampler;
  Button &button;
  Bluetooth &ble;

  // Configuration
  unsigned long longPressDuration;
  unsigned long ledToggleDuration;
  unsigned long autoLedOffTimeout;
  int minSamplesRequired;

  // State
  bool lastButtonState;
  bool longPressHandled;
  bool ledToggleHandled;
  unsigned long lastActivityTime;
  RGBColor lastAvgColor;
  String lastColorName;

  // Event handlers
  void onSampleTaken(const RGBColor &color);
  void onLongPress();
  void onLedToggle();
  void onTripleTap();

  // Helper methods
  bool canFinalize();
  void checkAutoLedOff();
  void handleWakeUp();
  void showCurrentState();
  void waitForButtonRelease();
  void waitForButtonPress();
  void updateActivity();
};

#endif
