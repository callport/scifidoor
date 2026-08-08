#include <Arduino.h>

#include "Sound.hpp"

bool Sound::mMuted = false;

Sound::Sound(int outputPin) {
  mOutputPin = outputPin;
}

Sound::~Sound() {}

void Sound::init() {
  pinMode(mOutputPin, OUTPUT);
  digitalWrite(mOutputPin, HIGH);
}

void Sound::update(int dt) {
  mTimePlaying += dt;

  if ((mTimePlaying > mTimeout) && !mLooping) digitalWrite(mOutputPin, HIGH);
}

void Sound::trigger() {
  if (mMuted) return;
  
  mTimePlaying = 0;

  digitalWrite(mOutputPin, LOW);
}

void Sound::stop() {
  digitalWrite(mOutputPin, HIGH);
}

void Sound::loopMode(bool loop) {
/*  if (loop) {
    if (!mLooping) trigger();
  }
*/
  mLooping = loop;
}

void Sound::muteAll() {
  mMuted = true;
}

void Sound::unmuteAll() {
  mMuted = false;
}
