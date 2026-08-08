#pragma once

// Drives the two speed controller inputs as a ramped pair.  Only one direction
// is ever energized; reversing always passes through rest first so the door
// never slams from full speed one way to full speed the other.
class DoorMotor {
public :
  enum Direction {STOPPED, OPENING, CLOSING};

  DoorMotor(int openPin, int closePin);
  ~DoorMotor();

  void init();
  void update(int dt);

  void open();
  void close();

  // Ramp down to rest.  For a button release, where the door is mid-travel.
  void stop();

  // Drop to rest immediately, skipping the ramp.  For the limit switches,
  // where the door is already at the end of its travel and coasting would
  // drive it into the stop.  The switches also physically interrupt the PWM
  // line, so this is the second of two layers.
  void halt();

  Direction getDirection();
  int getDuty();

  void setRampTime(int ms);
  void setMaxDuty(int duty);

private :
  int mOpenPin;
  int mClosePin;

  Direction mDirection = STOPPED;  // which pin is currently energized
  Direction mRequested = STOPPED;  // which one we are heading towards

  float mDuty = 0.0f;

  int mMaxDuty = 255;
  int mRampTime = 400;

  void writeOutputs();
};
