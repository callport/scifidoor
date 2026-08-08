#pragma once

#include "LedMatrix.hpp"

class LedLevelIndicator {
public :
  LedLevelIndicator(LedMatrix *leds);
  ~LedLevelIndicator();

  void setLevel(float level);

  // Light exactly this many LEDs from the left.  Use when the caller already
  // has an integer level, so the display cannot drift out of step with it.
  void setLitCount(int count);

private :
  LedMatrix *mLeds;

  float mLevel = 0.0f;
};
