#include <Arduino.h>

#include "DoorMotor.hpp"

DoorMotor::DoorMotor(int openPin, int closePin) {
  mOpenPin = openPin;
  mClosePin = closePin;
}

DoorMotor::~DoorMotor() {}

void DoorMotor::init() {
  pinMode(mOpenPin, OUTPUT);
  pinMode(mClosePin, OUTPUT);

  writeOutputs();
}

void DoorMotor::update(int dt) {
  // Sized so a full rest-to-full-speed sweep takes mRampTime.  A reversal
  // costs two sweeps: one down, one back up.
  float step = ((float) mMaxDuty) * dt / mRampTime;

  if ((mRequested == mDirection) && (mRequested != STOPPED)) {
    mDuty += step;

    if (mDuty > mMaxDuty) mDuty = mMaxDuty;

  } else {
    // Either we were asked to stop, or we are reversing and have to reach
    // rest before the other direction can be energized.
    mDuty -= step;

    if (mDuty <= 0.0f) {
      mDuty = 0.0f;
      mDirection = mRequested;
    }
  }

  writeOutputs();
}

void DoorMotor::open() {
  mRequested = OPENING;
}

void DoorMotor::close() {
  mRequested = CLOSING;
}

void DoorMotor::stop() {
  mRequested = STOPPED;
}

void DoorMotor::halt() {
  mRequested = STOPPED;
  mDirection = STOPPED;
  mDuty = 0.0f;

  writeOutputs();
}

DoorMotor::Direction DoorMotor::getDirection() {
  return mDirection;
}

void DoorMotor::setRampTime(int ms) {
  if (ms > 0) mRampTime = ms;
}

void DoorMotor::setMaxDuty(int duty) {
  mMaxDuty = duty;
}

void DoorMotor::writeOutputs() {
  int duty = (int) mDuty;

  if (mDirection == OPENING) analogWrite(mOpenPin, duty);
  else analogWrite(mOpenPin, 0);

  if (mDirection == CLOSING) analogWrite(mClosePin, duty);
  else analogWrite(mClosePin, 0);
}
