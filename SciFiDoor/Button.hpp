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
  
  void setLedBlink(boolean mode);

  void setDelay(int timeout);

  boolean isPressed();

private :
  int mInputPin;
  
  int mLedPin = -1;
  boolean mLedOn = false;
  boolean mLedBlink = false;
  
  boolean mLedOnWhenPressed = false;
  boolean mLedOnWhenNotPressed = false;

  void setLed(boolean on);

  int mTimeout = 250;

};
