#ifndef BUTTON_H
#define BUTTON_H

#include <Arduino.h>

class Button {
private:
  uint8_t pin;
  unsigned long pressStartTime;
  unsigned long lastDebounceTime;
  bool wasPressedBefore;
  bool lastStableState;
  static const unsigned long DEBOUNCE_DELAY = 50;

public:
  Button(uint8_t buttonPin);
  void begin();
  bool isPressed();
  bool isPressedFor(unsigned long durationMs);
  unsigned long getPressedDuration();
  bool wasJustPressed();
  bool wasJustReleased();
};

#endif
