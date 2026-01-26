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

  // Tap counting
  int tapCount;
  unsigned long lastTapTime;
  static const unsigned long TAP_TIMEOUT = 400; // ms between taps

public:
  Button(uint8_t buttonPin);
  void begin();
  bool isPressed();
  bool isPressedFor(unsigned long durationMs);
  unsigned long getPressedDuration();
  bool wasJustPressed();
  bool wasJustReleased();

  // New tap detection
  int getTapCount();
  void resetTapCount();
  void updateTapCount();
};

#endif
