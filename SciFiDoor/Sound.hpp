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

  // How long the trigger is held to ground for a one shot.  Too short and the
  // module ignores it.  Loop mode ignores this and holds until stop().
  void setTriggerHold(int ms);

  static void muteAll();
  static void unmuteAll();
  static bool isMuted();

private :
  int mOutputPin;
  
  // At the old 50ms this was only two or three passes of a 20ms loop, which
  // the module treated as noise more often than not.
  int mTimeout = 150;

  bool mLooping = false;

  unsigned long mTimePlaying;

  static bool mMuted;

};
