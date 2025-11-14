#include "button.h"

Button::Button(uint8_t buttonPin) : pin(buttonPin), pressStartTime(0), wasPressedBefore(false)
{
}

void Button::begin()
{
  pinMode(pin, INPUT_PULLUP);
}

bool Button::isPressed()
{
  return digitalRead(pin) == LOW;
}

bool Button::isPressedFor(unsigned long durationMs)
{
  bool currentlyPressed = isPressed();

  if (currentlyPressed && !wasPressedBefore)
  {
    pressStartTime = millis();
    wasPressedBefore = true;
    return false;
  }

  if (currentlyPressed && wasPressedBefore)
  {
    unsigned long duration = millis() - pressStartTime;
    return duration >= durationMs;
  }

  if (!currentlyPressed)
  {
    wasPressedBefore = false;
    pressStartTime = 0;
  }

  return false;
}

unsigned long Button::getPressedDuration()
{
  if (isPressed() && wasPressedBefore)
  {
    return millis() - pressStartTime;
  }
  return 0;
}
