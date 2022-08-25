#include <Arduino.h>
#include "LarsonScanner.hpp"
#include "LedMatrix.hpp"

LarsonScanner::LarsonScanner(LedMatrix *leds) {
  mLeds = leds;
}

LarsonScanner::~LarsonScanner() {}

void LarsonScanner::init() {
}

void LarsonScanner::doUpdate(int dt) {
  mTotalDt += dt;

  if (mTotalDt > mTimeout) {
    mTotalDt -= mTimeout;

    updateScanner();
  }
}

void LarsonScanner::setDelay(int timeout) {
  mTimeout = timeout;
}

void LarsonScanner::setMode(ScanningMode mode) {
  mMode = mode;  
}


void LarsonScanner::updateScanner() {
  // Turn Off Last Light
  mLeds->setPin(mCurrentIndex, 0, LOW);
  
  if (mDir == 1) {
    mCurrentIndex++;

    if (mCurrentIndex >= mLeds->getWidth()) {
      if (mMode == BOUNCE) {
        mCurrentIndex = mLeds->getWidth() - 1;
        mDir = -1;
      } else if (mMode == LEFT_TO_RIGHT) mCurrentIndex = 0;
    }
    
  } else {
    mCurrentIndex--;
    
    if (mCurrentIndex <= 0) {
      if (mMode == BOUNCE) {
        mCurrentIndex = 0;
        mDir = 1;
      } else if (mMode == LEFT_TO_RIGHT) mCurrentIndex = mLeds->getWidth() - 1;
    }
  }
  
  // Turn On Next Light
  mLeds->setPin(mCurrentIndex, 0, HIGH);
}
