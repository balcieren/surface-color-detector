#ifndef SAMPLING_CONTROLLER_H
#define SAMPLING_CONTROLLER_H

#include "ble_service.h"
#include "button.h"
#include "color_sampler.h"
#include "color_sensor.h"
#include "display.h"
#include <Arduino.h>

class SamplingController {
private:
  Display &display;
  ColorSensor &sensor;
  ColorSampler &sampler;
  Button &button;
  Bluetooth &ble;

  // Configuration
  unsigned long longPressDuration;
  int minSamplesRequired;

  // State
  bool lastButtonState;
  bool longPressHandled;
  RGBColor lastAvgColor;
  String lastColorName;

  void onSampleTaken(const RGBColor &color);
  void onLongPress();
  bool canFinalize();

public:
  SamplingController(Display &disp, ColorSensor &sens, ColorSampler &samp,
                     Button &btn, Bluetooth &bluetooth);

  void begin();
  void update();

  void setLongPressDuration(unsigned long ms);
  void setMinSamplesRequired(int count);
};

#endif
