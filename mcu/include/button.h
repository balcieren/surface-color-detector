#ifndef BUTTON_H
#define BUTTON_H

#include <Arduino.h>

class Button {
public:
  // Configuration constants
  static const unsigned long DEBOUNCE_DELAY = 50;
  static const unsigned long TAP_TIMEOUT = 400;
  static const unsigned long SHORT_PRESS_MAX = 500;

  Button(uint8_t buttonPin);

  void begin();

  // Core state
  void update();
  bool isPressed();
  bool isPressedFor(unsigned long durationMs);
  unsigned long getPressedDuration();

  // Edge detection
  bool wasJustPressed();
  bool wasJustReleased();

  // Multi-tap detection
  void updateTapCount();
  int getTapCount();
  void resetTapCount();

  unsigned long getLastPressDuration() { return lastPressDuration; }

private:
  uint8_t pin;
  unsigned long pressStartTime;
  unsigned long lastPressDuration;
  unsigned long lastDebounceTime;
  unsigned long lastTapTime;
  int tapCount;
  bool wasPressedBefore;
  bool lastStableState;
};

#endif
