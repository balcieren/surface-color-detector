#include "button.h"

Button::Button(uint8_t buttonPin)
    : pin(buttonPin), pressStartTime(0), lastDebounceTime(0),
      wasPressedBefore(false), lastStableState(false), tapCount(0),
      lastTapTime(0) {}

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

void Button::updateTapCount() {
  static bool wasPressed = false;
  bool currentlyPressed = isPressed();

  // Reset tap count if timeout exceeded
  if (tapCount > 0 && millis() - lastTapTime > TAP_TIMEOUT) {
    // Keep the tap count for reading, will be reset after being read
  }

  // Detect button release (end of tap)
  if (wasPressed && !currentlyPressed) {
    // Only count if press was short (not a long press)
    if (millis() - pressStartTime < 500) {
      tapCount++;
      lastTapTime = millis();
    }
  }

  wasPressed = currentlyPressed;
}

int Button::getTapCount() {
  // Return count only after timeout (tapping sequence complete)
  if (tapCount > 0 && millis() - lastTapTime > TAP_TIMEOUT && !isPressed()) {
    return tapCount;
  }
  return 0;
}

void Button::resetTapCount() {
  tapCount = 0;
  lastTapTime = 0;
}
