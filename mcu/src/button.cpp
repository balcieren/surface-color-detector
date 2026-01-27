#include "button.h"

Button::Button(uint8_t buttonPin)
    : pin(buttonPin), pressStartTime(0), lastDebounceTime(0), lastTapTime(0),
      tapCount(0), wasPressedBefore(false), lastStableState(false) {}

void Button::begin() {
  pinMode(pin, INPUT_PULLUP);
  lastStableState = digitalRead(pin) == LOW;
}

// ============================================================================
// Core State
// ============================================================================

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
    return (millis() - pressStartTime) >= durationMs;
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

// ============================================================================
// Edge Detection
// ============================================================================

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

// ============================================================================
// Multi-Tap Detection
// ============================================================================

void Button::updateTapCount() {
  static bool wasPressed = false;
  bool currentlyPressed = isPressed();

  // Detect button release (end of tap)
  if (wasPressed && !currentlyPressed) {
    // Only count short presses as taps
    unsigned long pressDuration = millis() - pressStartTime;
    if (pressDuration < SHORT_PRESS_MAX) {
      tapCount++;
      lastTapTime = millis();
    }
  }

  wasPressed = currentlyPressed;
}

int Button::getTapCount() {
  // Return count only after timeout (tapping sequence complete)
  bool sequenceComplete = (millis() - lastTapTime) > TAP_TIMEOUT;
  if (tapCount > 0 && sequenceComplete && !isPressed()) {
    return tapCount;
  }
  return 0;
}

void Button::resetTapCount() {
  tapCount = 0;
  lastTapTime = 0;
}
