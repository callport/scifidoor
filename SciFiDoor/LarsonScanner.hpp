#pragma once

#include "LedMatrix.hpp"

class LarsonScanner {
public :
  enum ScanningMode {BOUNCE, LEFT_TO_RIGHT, RIGHT_TO_LEFT};

  LarsonScanner(LedMatrix *leds);
  ~LarsonScanner();

  void init();
  void doUpdate(int dt); 
  void setDelay(int timeout);

  void setMode(ScanningMode mode);

private :
  LedMatrix *mLeds;

  unsigned long mTotalDt = 0;
  unsigned long mTimeout = 100;
  
  int mDir = 1;
  int mCurrentIndex = 0;

  ScanningMode mMode = BOUNCE;

  void updateScanner();

  
};
