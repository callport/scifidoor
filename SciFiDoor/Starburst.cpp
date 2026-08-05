#include <Arduino.h>
#include "Starburst.hpp"
#include "LedMatrix.hpp"

Starburst::Starburst(LedMatrix *leds) {
  mLeds = leds;

  // This requires more code to work with even sets of LEDs. 
  // It will work fine with odd sets.
  mMiddleLed = mLeds->getWidth() / 2;

  switch (mMode) {
    case IN_OUT : 
    case PULSE : 
        mDir = 1;  
        mCurrentIndex = mMiddleLed;  
        break;

    case OUT_IN : 
        mDir = 1; 
        mCurrentIndex = 0;  
        break;
  }
}

Starburst::~Starburst() {}

void Starburst::init() {
}

void Starburst::doUpdate(int dt) {
  mTotalDt += dt;

  if (mTotalDt > mTimeout) {
    mTotalDt -= mTimeout;

    updateScanner();
  }
}

void Starburst::setDelay(int timeout) {
  mTimeout = timeout;
}

void Starburst::setMode(BurstMode mode) {
  mMode = mode;  
}

void Starburst::setOffset(int offset) {
  mCurrentIndex = offset;
}

void Starburst::updateScanner() {
  // Turn Off Last Light
  mLeds->setPin(mMiddleLed - mCurrentIndex, 0, LOW);
  mLeds->setPin(mMiddleLed + mCurrentIndex, 0, LOW);
  
  if (mDir == 1) {
    mCurrentIndex--;

    if (mCurrentIndex < 0) {
      if (mMode == PULSE) {
        mCurrentIndex = 1;
        mDir = -1;
      } else mCurrentIndex = mMiddleLed;
    }
    
  } else {
    mCurrentIndex++;
    
    if (mCurrentIndex > mMiddleLed) {
      if (mMode == PULSE) {
        mCurrentIndex = mMiddleLed - 1;
        mDir = 1;
      } else mCurrentIndex = 0;
    }
  }
  
  // Turn On Next Light
  mLeds->setPin(mMiddleLed - mCurrentIndex, 0, HIGH);
  mLeds->setPin(mMiddleLed + mCurrentIndex, 0, HIGH);
}
