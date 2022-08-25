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
  if (isPressed()) {
    setLed(mLedOnWhenPressed);
  } else {
    setLed(mLedOnWhenNotPressed);
  }
}

void Button::setLedPin(int ledPin) {
  mLedPin = ledPin;
  pinMode(ledPin, OUTPUT);
}

void Button::setLed(boolean on) {
  // ADD BLINK LOGIC LATER
  
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

void Button::setLedBlink(boolean mode) {
  
}

void Button::setDelay(int timeout) {
  mTimeout = timeout;
}

boolean Button::isPressed() {
  return (digitalRead(mInputPin) == LOW); // Reverse Logic with Pull-Up Resistor
}
