#include "Motion.hpp"

namespace Motion {
  int mSensorPin = -1;
  unsigned long mTimeOfLastMotion = 0;

  
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
  
  boolean getMotion() {
    return digitalRead(mSensorPin) == HIGH;
  }
  
  unsigned long getTimeSinceLastMotion() {
    // millis() returns an unsigned long.  Holding it in an unsigned int would
    // truncate it to 16 bits on AVR and wrap every ~65 seconds.
    //
    // Unsigned subtraction already wraps correctly when the clock rolls over
    // at ~49 days, so no special case is needed.
    return millis() - mTimeOfLastMotion;
  }
  
}
