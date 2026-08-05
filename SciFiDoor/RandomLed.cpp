#include <Arduino.h>
#include "RandomLed.hpp"
#include "LedMatrix.hpp"

RandomLed::RandomLed(LedMatrix *leds) {
  mLeds = leds;
  randomSeed(micros());
}

RandomLed::~RandomLed() {}

void RandomLed::init() {
}

void RandomLed::doUpdate(int dt) {
  mTotalDt += dt;

  if (mTotalDt > mTimeout) {
    mTotalDt -= mTimeout;

    update();
  }
}

void RandomLed::setDelay(int timeout) {
  mTimeout = timeout;
}

void RandomLed::update() {
  // Turn Off Last Light
  mLeds->setPin(mCurrentIndex, 0, LOW);

  mCurrentIndex = random(mLeds->getWidth());
  
  // Turn On Next Light
  mLeds->setPin(mCurrentIndex, 0, HIGH);
}
