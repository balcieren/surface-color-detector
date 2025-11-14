#ifndef BUTTON_H
#define BUTTON_H

#include <Arduino.h>

class Button
{
private:
  uint8_t pin;
  unsigned long pressStartTime;
  bool wasPressedBefore;

public:
  Button(uint8_t buttonPin);
  void begin();
  bool isPressed();
  bool isPressedFor(unsigned long durationMs);
  unsigned long getPressedDuration();
};

#endif
