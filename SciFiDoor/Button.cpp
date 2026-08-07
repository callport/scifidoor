#include <Arduino.h>

#include "Button.hpp"

Button::Button(int inputPin) {
  mInputPin = inputPin;
}

Button::~Button() {}

void Button::init() {
  pinMode(mInputPin, INPUT_PULLUP);
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

boolean Button::isPressed() {
  return (digitalRead(mInputPin) == LOW); // Reverse Logic with Pull-Up Resistor
}

void Button::setOnPressedCallback(void (*onPressed)(void)) {
  mOnPressed = onPressed;
}

void Button::setOnReleasedCallback(void (*onReleased)(void)) {
  mOnReleased = onReleased;
}
