#pragma once

#include "LedMatrix.hpp"

class LedLevelIndicator {
public :
  LedLevelIndicator(LedMatrix *leds);
  ~LedLevelIndicator();

  void setLevel(float level);

private :
  LedMatrix *mLeds;

  float mLevel = 0.0f;
};
