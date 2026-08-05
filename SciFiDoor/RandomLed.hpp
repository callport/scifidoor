#pragma once

#include "LedMatrix.hpp"

class RandomLed {
public :
  RandomLed(LedMatrix *leds);
  ~RandomLed();

  void init();
  void doUpdate(int dt); 
  void setDelay(int timeout);

private :
  LedMatrix *mLeds;

  unsigned long mTotalDt = 0;
  unsigned long mTimeout = 100;
  
  int mCurrentIndex = 0;

  void update();
};
