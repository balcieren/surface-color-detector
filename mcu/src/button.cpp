#include "button.h"

Button::Button(uint8_t buttonPin)
    : pin(buttonPin), pressStartTime(0), lastDebounceTime(0),
      wasPressedBefore(false), lastStableState(false) {}

void Button::begin() {
  pinMode(pin, INPUT_PULLUP);
  lastStableState = digitalRead(pin) == LOW;
}

bool Button::isPressed() {
  bool reading = digitalRead(pin) == LOW;

  if (reading != lastStableState) {
    if (millis() - lastDebounceTime > DEBOUNCE_DELAY) {
      lastStableState = reading;
      lastDebounceTime = millis();
    }
  } else {
    lastDebounceTime = millis();
  }

  return lastStableState;
}

bool Button::isPressedFor(unsigned long durationMs) {
  bool currentlyPressed = isPressed();

  if (currentlyPressed && !wasPressedBefore) {
    pressStartTime = millis();
    wasPressedBefore = true;
    return false;
  }

  if (currentlyPressed && wasPressedBefore) {
    unsigned long duration = millis() - pressStartTime;
    return duration >= durationMs;
  }

  if (!currentlyPressed) {
    wasPressedBefore = false;
    pressStartTime = 0;
  }

  return false;
}

unsigned long Button::getPressedDuration() {
  if (isPressed() && wasPressedBefore) {
    return millis() - pressStartTime;
  }
  return 0;
}

bool Button::wasJustPressed() {
  static bool lastState = false;
  bool currentState = isPressed();
  bool justPressed = currentState && !lastState;
  lastState = currentState;
  return justPressed;
}

bool Button::wasJustReleased() {
  static bool lastState = false;
  bool currentState = isPressed();
  bool justReleased = !currentState && lastState;
  lastState = currentState;
  return justReleased;
}
