#include "Motion.hpp"

namespace Motion {
  int mSensorPin = -1;
  unsigned int mTimeOfLastMotion = 0;

  
  void init(int pin) {
    mSensorPin = pin;
    
    pinMode(mSensorPin, INPUT);
  
    mTimeOfLastMotion = millis();
  }
  
  void doUpdate(int dt) {
    // Ignore dt
    
    int motionState = digitalRead(mSensorPin);
  
    if (motionState == HIGH) {
      mTimeOfLastMotion = millis();
    }
  }
  
  unsigned int getTimeSinceLastMotion() {
    unsigned int currTime = millis();
  
    // First, detect if the millis() timer has rolled over
    // I think this will only happen if the latest time read
    // is a smaller number than the last time (because the clock
    // would have rolled over).
    if (currTime < mTimeOfLastMotion) {
      return currTime + (-1 - mTimeOfLastMotion);
    }
  
    return currTime - mTimeOfLastMotion;
  }
  
}
