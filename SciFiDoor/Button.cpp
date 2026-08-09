#include <Arduino.h>

#include "Button.hpp"

Button::Button(int inputPin) {
  mInputPin = inputPin;
}

Button::~Button() {}

void Button::init() {
  pinMode(mInputPin, INPUT_PULLUP);

  // Let the pull-up settle before the first read.
  delayMicroseconds(100);

  // Seed the edge detector from where the switch actually is, so one that is
  // already made at power on is not reported as a fresh press.  A door parked
  // on its limit switch would otherwise fire a stop the moment we booted.
  //
  // Call setInverted() before this or the seed is taken the wrong way round.
  mButtonState = isPressed();
}

void Button::update(int dt) {
  mTotalDt += dt;

  if (mTotalDt > mTimeout) {
    mTotalDt -= mTimeout;

    // For Blinking
    mLedOn = !mLedOn;
  }


  if (isPressed()) {
    if (mLedBlinkPressed) setLed(mLedOnWhenPressed && mLedOn);
    else setLed(mLedOnWhenPressed);

    if (!mButtonState && (mOnPressed != NULL)) mOnPressed();
    mButtonState = true;
    
  } else {
    if (mLedBlinkUnpressed) setLed(mLedOnWhenNotPressed && mLedOn);
    else setLed(mLedOnWhenNotPressed);

    if (mButtonState && (mOnReleased != NULL)) mOnReleased();
    mButtonState = false;
  }
}

void Button::setLedPin(int ledPin) {
  mLedPin = ledPin;
  pinMode(ledPin, OUTPUT);
}

void Button::setLed(boolean on) {
  if (mLedPin == -1) return;
  
  if (on) digitalWrite(mLedPin, HIGH);
  else digitalWrite(mLedPin, LOW);
}

void Button::setLedUnpressed(boolean mode) {
  mLedOnWhenNotPressed = mode;
}

void Button::setLedPressed(boolean mode) {
  mLedOnWhenPressed = mode;
}

void Button::setLedBlinkPressed(boolean mode) {
  mLedBlinkPressed = mode;
}

void Button::setLedBlinkUnpressed(boolean mode) {
  mLedBlinkUnpressed = mode;
}

void Button::setDelay(int timeout) {
  mTimeout = timeout;
}

void Button::setInverted(boolean inverted) {
  mInverted = inverted;
}

boolean Button::isPressed() {
  boolean made = (digitalRead(mInputPin) == LOW); // Reverse Logic with Pull-Up Resistor

  if (mInverted) return !made;

  return made;
}

void Button::setOnPressedCallback(void (*onPressed)(void)) {
  mOnPressed = onPressed;
}

void Button::setOnReleasedCallback(void (*onReleased)(void)) {
  mOnReleased = onReleased;
}
