#pragma once

class LedMatrix {
public :
  LedMatrix(int pins[], int width, int height);
  ~LedMatrix();

  void init();
  void doUpdate(int dt);

  int getWidth();
  int getHeight();

  void setPin(int x, int y, int state);
  void setPin(int x, int y, float brightness);

  int getPin(int x, int y);

  void deactivate();

private :
  int mWidth;
  int mHeight;
  int *mPins;
  
};
