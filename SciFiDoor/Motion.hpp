#pragma once
#include <Arduino.h>

namespace Motion {
  
  void init(int pin);

  void doUpdate(int dt);

  boolean getMotion();

  unsigned int getTimeSinceLastMotion();
}
