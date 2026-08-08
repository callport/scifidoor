#pragma once

class Sound {
public :
  Sound(int outputPin);
  ~Sound();

  void init();
  void update(int dt);

  void trigger();
  void loopMode(bool loop);
  void stop();

  static void muteAll();
  static void unmuteAll();

private :
  int mOutputPin;
  
  int mTimeout = 50;

  bool mLooping = false;

  unsigned long mTimePlaying;

  static bool mMuted;

};
