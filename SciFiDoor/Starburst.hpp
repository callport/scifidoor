#pragma once

#include "LedMatrix.hpp"

class Starburst {
public :
  enum BurstMode {IN_OUT, OUT_IN, PULSE};

  Starburst(LedMatrix *leds);
  ~Starburst();

  void init();
  void doUpdate(int dt); 
  void setDelay(int timeout);

  void setOffset(int offset);

  void setMode(BurstMode mode);

private :
  LedMatrix *mLeds;

  unsigned long mTotalDt = 0;
  unsigned long mTimeout = 100;
  
  int mMiddleLed = -1;

  int mDir = -1;
  int mCurrentIndex = 0;

  BurstMode mMode = IN_OUT;

  void updateScanner();
};
