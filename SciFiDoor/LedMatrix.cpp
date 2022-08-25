#include <Arduino.h>
#include "LedMatrix.hpp"

LedMatrix::LedMatrix(int pins[], int width, int height) {
  mPins = pins;
  mWidth = width;
  mHeight = height;
}

LedMatrix::~LedMatrix() {
}

void LedMatrix::init() {
  for (int y = 0; y < mHeight; y++) {
    for (int x = 0; x < mWidth; x++) {
      pinMode(getPin(x, y), OUTPUT);
    }
  }  
}

void LedMatrix::doUpdate(int dt) {
}

int LedMatrix::getWidth() {
  return mWidth;
}

int LedMatrix::getHeight() {
  return mHeight;
}

void LedMatrix::setPin(int x, int y, int state) {
  digitalWrite(getPin(x, y), state);
}

void LedMatrix::setPin(int x, int y, float brightness) {
  analogWrite(getPin(x, y), brightness);
}

int LedMatrix::getPin(int x, int y) {
  return *(mPins + x + (y * mWidth));
}

void LedMatrix::deactivate() {
  for (int y = 0; y < mHeight; y++) {
    for (int x = 0; x < mWidth; x++) {
      digitalWrite(getPin(x, y), LOW);
    }
  }  
}
