#include <Arduino.h>
#include "LedLevelIndicator.hpp"

LedLevelIndicator::LedLevelIndicator(LedMatrix *leds) {
  mLeds = leds;
}

LedLevelIndicator::~LedLevelIndicator() {}

void LedLevelIndicator::setLevel(float level) {
  mLevel = level;

  float interval = 1.0f / mLeds->getWidth();

  for (int i = 0; i < mLeds->getWidth(); i++) {
    if (level > (i * interval)) mLeds->setPin(i, 0, HIGH);
    else mLeds->setPin(i, 0, LOW);
  }
  
}
