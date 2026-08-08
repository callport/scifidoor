#pragma once

class Button {
public :
  Button(int inputPin);
  ~Button();

  void init();
  void update(int dt);

  void setLedPin(int ledPin);

  void setLedUnpressed(boolean mode);
  void setLedPressed(boolean mode);
  
  void setLedBlinkPressed(boolean mode);
  void setLedBlinkUnpressed(boolean mode);

  void setDelay(int timeout);

  // Normally open contacts wired to ground read LOW when made, which is what
  // isPressed() assumes.  Normally closed contacts read the other way round,
  // so they need this set.
  void setInverted(boolean inverted);

  boolean isPressed();

  void setOnPressedCallback(void (*onPressed)());
  void setOnReleasedCallback(void (*onReleased)());

private :
  int mInputPin;

  bool mButtonState = false;
  boolean mInverted = false;

  int mLedPin = -1;
  boolean mLedOn = false;

  boolean mLedOnWhenPressed = false;
  boolean mLedOnWhenNotPressed = false;

  boolean mLedBlinkPressed = false;
  boolean mLedBlinkUnpressed = false;
  
  void setLed(boolean on);

  unsigned long mTotalDt = 0;
  int mTimeout = 250;

  void (*mOnPressed)(void) = NULL;
  void (*mOnReleased)(void) = NULL;
};
