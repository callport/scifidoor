/*
 * SciFiDoor
 * 
 * This is the sketch for my SciFi Door project.  It will have a number of different
 * modes for the different LED displays.
 * 
 * I lost code at some point.  Cannot find "All Systems Nominal" or where the klaxon 
 * only plays X times.
 * 
 * There is also something lost on the door code.  Pins 52 / 53 are unused.  
 * 
 * 
 * BUGS:
 * - Pressing OPEN when the door is open starts the sound again.  It should not.
 * 
 * TODO:
 * - If direction is reversed, ease transition so it doesn't jerk the door.
 * - Verify use of all pins
 * - Power Down Model
 * - Add support for Air Lock Level (Two Inputs Needed for reed sensors)
 * - Add function for Air Lock (Mute?) - (Two Outputs Needed for sound triggers)
 * 
 * TEST:
 * - Open Button (if Open) Produces Error
 * - Open Button (if Open) Does not Play Open Sound
 * - Open Button (while Opening) Continues
 * - Close Button (if Closed) Produces Error
 * - Close Button (if Closed) Does not Play Close Sound
 * - Close Button (while Closing) Continues
 * 
*/

#include "Button.hpp"
#include "Motion.hpp"
#include "LedMatrix.hpp"
#include "LarsonScanner.hpp"
#include "LedLevelIndicator.hpp"
#include "RandomLed.hpp"
#include "Sound.hpp"
#include "Starburst.hpp"

bool mSerialMonitor = true;

/** PIN INVENTORY ***************************/
// (*) Pins defined by board

// 0* - Serial RX
// 1* - Serial TX
// 2 - PWM - Motor                                  [NEW] SOUND TRIGGER - Airlock Engage (Sound 3)
// 3 - BUTTON - Inside Shop Door  [White/Orange] -> [White/Green] - Inside Switch
// 4 - BUTTON - Yellow
// 5 - BUTTON - Red
// 6 - LIMIT SWITCH - Door Closed [White/Brown] -> [White/Brown] - Door Closed - Normally Open Switch
// 7 - LIMIT SWITCH - Door Open   [Orange] -> [White/Blue] - Door Open - Normally Open Switch
// 8 - ???? Motor Harness                           [NEW] PWM - Motor Control Door Close [White/Green] -> [Brown] - Door Closed - Normally Closed Switch -> [Blue] -> [White/Green] -> Speed Controller Input 1
// 9 - ???? Motor Harness                           [NEW] PWM - Motor Control Door Open [Green] -> [Orange] - Door Open - Normally Closed Switch -> [White/Orange] -> [Green] -> Speed Controller Input 2

// 10 -                                             [NEW] SOUND TRIGGER - All Systems Nominal (Sound 2)
// 11 - SWITCH - Blue Mode Switch
// 12 - BUTTON - Green
// 13* - LED - Internal Status
// 14 - MOTION DETECTOR
// 15 -                                             [NEW] SOUND TRIGGER - Airlock Disengage (Sound 5)
// 16 - SOUND TRIGGER - Door (Sound 1)
// 17 - SOUND TRIGGER - Klaxon (Sound 4)
// 18 - SOUND TRIGGER - Hiss (Sound 7)
// 19 - SOUND TRIGGER - Error (Sound 6)

// 20 -
// 21 -
// 22 - LED - Wide Larson Scanner (1)
// 23 - LED - Wide Larson Scanner (2)
// 24 - LED - Wide Larson Scanner (3)
// 25 - LED - Wide Larson Scanner (4)
// 26 - LED - Wide Larson Scanner (5)
// 27 - LED - Wide Larson Scanner (6)
// 28 - LED - Wide Larson Scanner (7)
// 29 - LED - DEFCON Indicator (1)

// 30 - LED - DEFCON Indicator (2)
// 31 - LED - DEFCON Indicator (3)
// 32 - LED - DEFCON Indicator (4)
// 33 - LED - DEFCON Indicator (5)
// 34 - LED - BGW (Middle Row of Matrix) (1)
// 35 - LED - BGW (Middle Row of Matrix) (2)
// 36 - LED - BGW (Middle Row of Matrix) (3)
// 37 - LED - BGW (Middle Row of Matrix) (4)
// 38 - LED - BGW (Middle Row of Matrix) (5)
// 39 - LED - Other (Top Row of Matrix) (1)

// 40 - LED - Other (Top Row of Matrix) (2)
// 41 - LED - Other (Top Row of Matrix) (3)
// 42 - LED - Other (Top Row of Matrix) (4)
// 43 - LED - Other (Top Row of Matrix) (5)
// 44 - LED - Speaker
// 45 - LED - WBW Array (1)
// 46 - LED - WBW Array (2)
// 47 - LED - WBW Array (3)
// 48 - LED - Blue Switch
// 49 - LED - Button (Green)

// 50 - LED - Button (Red)
// 51 - LED - Button (Yellow)
// 52 - OUTPUT ENABLE - Close Door  ??????          [NEW] MAGNETIC REED SWITCH - Airlock Up
// 53 - OUTPUT ENABLE - Open Door   ??????          [NEW] MAGNETIC REED SWITCH - Airlock Down
// 54 - 

// 55-69???

/** INPUTS **********************************/
int BUTTON_YELLOW = 4;
int BUTTON_RED = 5;
int BUTTON_GREEN = 12;
int SWITCH_BLUE = 9;

int MOTION_DETECTOR = 14;

int LIMIT_SWITCH_DOOR_CLOSE = 6;
int LIMIT_SWITCH_DOOR_OPEN = 7;
int BUTTON_INSIDE_SHOP_DOOR_OPEN = 3;


/** OUTPUTS *********************************/
int MOTOR = 2;

int WIDE_SCANNER_LEDs [] = {22, 23, 24, 25, 26, 27, 28};
int DEF_CON_LEDs [] = {29, 30, 31, 32, 33};
int BGW_LEDs [] = {34, 35, 36, 37, 38};
int OTHER_LEDs [] = {39, 40, 41, 42, 43};
int SPEAKER_LEDs [] = {44};
int WBW_LEDs [] = {45, 46, 47};
int BLUE_RING_LEDs [] = {48};
int BUTTON_LEDs[] = {49, 50, 51};

/** MODES ***********************************/

// 0 - Not Larson
// 1 - Larson Scanner
// 2 - Left to Right Scanner
// 3 - Right to Left Scanner
// x - Starburst?
// 5 - Random Blinky

int WIDE_SCANNER_MODE = 1;  

// 0 - Random Blinky
// 1 - Individual Functions
int LED_MATRIX_MODE = 1;

enum LED_Matrix_Mode {RANDOM_BLINKY, INDIVIDUAL_FUNCTIONS};
LED_Matrix_Mode LEDMatrixMode = RANDOM_BLINKY;

/** HELPERS *********************************/
int WIDE_SCANNER_WIDTH = sizeof(WIDE_SCANNER_LEDs) / sizeof(int);
int DEF_CON_WIDTH = sizeof(DEF_CON_LEDs) / sizeof(int);
int BGW_WIDTH = sizeof(BGW_LEDs) / sizeof(int);
int OTHER_WIDTH = sizeof(OTHER_LEDs) / sizeof(int);
int SPEAKER_WIDTH = sizeof(SPEAKER_LEDs) / sizeof(int);
int WBW_WIDTH = sizeof(WBW_LEDs) / sizeof(int);
int BLUE_RING_WIDTH = sizeof(BLUE_RING_LEDs) / sizeof(int);
int BUTTON_WIDTH = sizeof(BUTTON_LEDs) / sizeof(int);

/** MISCELLANEOUS ***************************/
unsigned long TIMEOUT = ((unsigned long) 10) * 60 * 1000;  // Ten Minutes

boolean ACTIVE_MODE = true;

int SOUND_DOOR = 16;
int SOUND_KLAXON = 17;
int SOUND_HISS = 18;
int SOUND_ERROR = 19;

int CLOSE_DOOR = 52;
int OPEN_DOOR = 53;

LedMatrix mWideScanner(WIDE_SCANNER_LEDs, 7, 1);
LedMatrix mDefCon(DEF_CON_LEDs, 5, 1);
LedMatrix mBlueGreenWhite(BGW_LEDs, 5, 1);
LedMatrix mRedOrangeYellow(OTHER_LEDs, 5, 1);
LedMatrix mWhiteBlueWhite(WBW_LEDs, 3, 1);
LedMatrix mSpeaker(SPEAKER_LEDs, 1, 1);
LedMatrix mBlueRing(BLUE_RING_LEDs, 1, 1);
LedMatrix mButtons(BUTTON_LEDs, 3, 1);

LarsonScanner mLarsonScanner(&mWideScanner);
LarsonScanner mROYScanner(&mRedOrangeYellow);
LarsonScanner mWBWScanner(&mWhiteBlueWhite);

RandomLed mROYRandom(&mRedOrangeYellow);

Starburst mBGWScanner(&mBlueGreenWhite);

LedLevelIndicator mDefConIndicator(&mDefCon);

Button mGreenButton(BUTTON_GREEN);
Button mRedButton(BUTTON_RED);
Button mYellowButton(BUTTON_YELLOW);
Button mBlueSwitch(SWITCH_BLUE);
Button mLimitSwitchDoorClose(LIMIT_SWITCH_DOOR_CLOSE);
Button mLimitSwitchDoorOpen(LIMIT_SWITCH_DOOR_OPEN);
Button mOpenDoor(BUTTON_INSIDE_SHOP_DOOR_OPEN);

Sound mSoundDoor(SOUND_DOOR);
Sound mSoundKlaxon(SOUND_KLAXON);
Sound mSoundHiss(SOUND_HISS);
Sound mSoundError(SOUND_ERROR);

// the setup function runs once when you press reset or power the board
void setup() {
  if (mSerialMonitor) Serial.begin(115200);

  Motion::init(MOTION_DETECTOR);

  mWideScanner.init();
  mDefCon.init();
  mBlueGreenWhite.init();
  mRedOrangeYellow.init();
  mWhiteBlueWhite.init();
  mSpeaker.init();
  mBlueRing.init();
  mButtons.init();

  // Initialize Buttons
  mGreenButton.setLedPin(BUTTON_LEDs[0]);
  mGreenButton.init();
  mGreenButton.setOnPressedCallback(&openDoor);
  mGreenButton.setOnReleasedCallback(&stopDoor);
  mGreenButton.setLedUnpressed(true);
  mGreenButton.setDelay(990);

  mRedButton.setLedPin(BUTTON_LEDs[1]);
  mRedButton.init();
  mRedButton.setOnPressedCallback(&closeDoor);
  mRedButton.setOnReleasedCallback(&stopDoor);
  mRedButton.setLedUnpressed(true);
  mRedButton.setDelay(1000);

  mYellowButton.setLedPin(BUTTON_LEDs[2]);
  mYellowButton.init();
  mYellowButton.setDelay(1000);
  mYellowButton.setLedUnpressed(true);
  mYellowButton.setLedBlinkUnpressed(true);
  mYellowButton.setOnPressedCallback(&startKlaxon);
  mYellowButton.setOnReleasedCallback(&stopKlaxon);  

  mBlueSwitch.init();
  mBlueSwitch.setOnPressedCallback(&blueSwitchOn);
  mBlueSwitch.setOnReleasedCallback(&blueSwitchOff);
  if (mBlueSwitch.isPressed()) blueSwitchOn();
  else blueSwitchOff();

  mLarsonScanner.setDelay(200);

  mLimitSwitchDoorClose.init();
  mLimitSwitchDoorClose.setOnPressedCallback(&stopDoorClose);
  mLimitSwitchDoorOpen.init();
  mLimitSwitchDoorOpen.setOnPressedCallback(&stopDoorOpen);
  mOpenDoor.init();


  // Initialize Sounds
  mSoundDoor.init();
  mSoundDoor.loopMode(true);
  mSoundKlaxon.init();
  mSoundKlaxon.loopMode(true);
  mSoundHiss.init();
  mSoundError.init();

  initLedMatrixMode(LEDMatrixMode);  
  
  mWBWScanner.setDelay(1000);
  mROYRandom.setDelay(800);

  // At the moment, this pin should be unconnected
  randomSeed(analogRead(1));
  
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);

  if (mSerialMonitor) Serial.println("Initialization Complete.");
}

void initLedMatrixMode(LED_Matrix_Mode mode) {
  if (LEDMatrixMode == mode) return;
  
  switch (mode) {
    case RANDOM_BLINKY :
      mBGWScanner.setMode(Starburst::PULSE);
      mBGWScanner.setDelay(250);

      //mROYScanner.setDelay(0);
      //mROYScanner.setOffset(0);
      break;
      
    case INDIVIDUAL_FUNCTIONS :
      mBGWScanner.setDelay(500);
      mBGWScanner.setMode(Starburst::OUT_IN);

      mROYScanner.setDelay(500);
      break;
  }

  LEDMatrixMode = mode;
}

int ALIVE_STATE = HIGH;

void doAlive() {
  digitalWrite(LED_BUILTIN, ALIVE_STATE);
  
  mBlueRing.setPin(0, 0, ALIVE_STATE);

  if (ALIVE_STATE == HIGH) ALIVE_STATE = LOW;
  else ALIVE_STATE = HIGH;
}

unsigned long mLastTime = millis();

void loop() {
  // Calculate the time since the last loop and attempt to account
  // for the clock rolling over.
  unsigned long dt = millis() - mLastTime;

  if (millis() < mLastTime) {
     dt = millis() + (-1 - mLastTime);
  }
  mLastTime = millis();
  
  Motion::doUpdate(dt);

  doAlive();

  processInputs();  

  animateAndUpdate(dt);

  if (Motion::getMotion()) mSpeaker.setPin(0, 0, HIGH);
  else mSpeaker.setPin(0, 0, LOW);
  
  // This probably isn't necessary...should check to see if time since top of loop was at least 20ms and then delay the delta.
  delay(20);
}

void animateAndUpdate(int dt) {
  mLarsonScanner.doUpdate(dt);

  mBGWScanner.doUpdate(dt);

  if (LEDMatrixMode == RANDOM_BLINKY) {
    mROYRandom.doUpdate(dt);
  } else {
    mROYScanner.doUpdate(dt);
  }

  mWBWScanner.doUpdate(dt);

  doDefConAnimation(dt);

  mRedButton.update(dt);
  mGreenButton.update(dt);
  mYellowButton.update(dt);
  mBlueSwitch.update(dt);
  mLimitSwitchDoorClose.update(dt);
  mLimitSwitchDoorOpen.update(dt);
  mOpenDoor.update(dt);

  mSoundDoor.update(dt);
  mSoundKlaxon.update(dt);
  mSoundHiss.update(dt);
  mSoundError.update(dt);
}

unsigned long mDefConTime = 0;
unsigned long mDefConTimeout = 600000; // 10 Minutes - compiler does weird math, can't use expression
void doDefConAnimation(int dt) {
  if (mDefConTime < mDefConTimeout) mDefConTime += dt;

  float defConLevel = ((float) mDefConTime) / mDefConTimeout;
  mDefConIndicator.setLevel(defConLevel);

  //                       y =           m             x +    b
  //unsigned long blinkTimeout = (int) (-900 * defConLevel + 1000);
  // 1000 * (.2 / (x - 1.2) + 1.2)
  unsigned long blinkTimeout = (int) (1000 * (.2 / (defConLevel - 1.2) + 1.2));

  if (defConLevel <= 0.2) mYellowButton.setLedBlinkUnpressed(false);
  else mYellowButton.setLedBlinkUnpressed(true);

  mYellowButton.setDelay(blinkTimeout);
}

//bool mDoorMoving = false;
bool mDoorClosing = false;
bool mDoorOpening = false;

void processInputs() {
  if (mOpenDoor.isPressed()) {
    if (mLimitSwitchDoorOpen.isPressed()) closeDoor();
    else if (mLimitSwitchDoorClose.isPressed()) openDoor();
    else mSoundError.trigger();
  }
}

void openDoor() {  
  if (mSerialMonitor) Serial.println("openDoor()");

  if (mLimitSwitchDoorOpen.isPressed()) mSoundError.trigger();
  else {
    mSoundDoor.trigger();
    digitalWrite(OPEN_DOOR, HIGH);  
  }
}

void closeDoor() {  
  if (mSerialMonitor) Serial.println("closeDoor()");

  if (mLimitSwitchDoorClose.isPressed()) mSoundError.trigger();
  else {
    mSoundDoor.trigger();
    digitalWrite(CLOSE_DOOR, HIGH);  
  }
}

void stopDoor() {  
  if (mSerialMonitor) Serial.println("stopDoor()");
  digitalWrite(CLOSE_DOOR, LOW);  
  digitalWrite(OPEN_DOOR, LOW);  

  mSoundDoor.stop();
  mSoundHiss.trigger();
}

void stopDoorClose() {  
  if (mSerialMonitor) Serial.println("stopDoorClose()");
  stopDoor();
}

void stopDoorOpen() {  
  if (mSerialMonitor) Serial.println("stopDoorOpen()");
  stopDoor();
}

void startKlaxon() {
  if (mSerialMonitor) Serial.println("startKlaxon()");
  mSoundKlaxon.trigger();
}

void stopKlaxon() {
  if (mSerialMonitor) Serial.println("stopKlaxon()");
  mSoundKlaxon.stop();
  mDefConTime = 0;
}

void blueSwitchOn() {
  if (mSerialMonitor) Serial.println("blueSwitchOn()");

  mGreenButton.setOnReleasedCallback(NULL);
  mRedButton.setOnReleasedCallback(NULL);

  initLedMatrixMode(RANDOM_BLINKY);
  mGreenButton.setLedBlinkUnpressed(false);
  mRedButton.setLedBlinkUnpressed(false);
}

void blueSwitchOff() {
  if (mSerialMonitor) Serial.println("blueSwitchOff()");

  mGreenButton.setOnReleasedCallback(&stopDoor);
  mRedButton.setOnReleasedCallback(&stopDoor);

  initLedMatrixMode(INDIVIDUAL_FUNCTIONS);

  mGreenButton.setLedBlinkUnpressed(true);
  mRedButton.setLedBlinkUnpressed(true);
}


void activate() {
  ACTIVE_MODE = true;

  // Choose Random Modes
  //WIDE_SCANNER_MODE = random(5);  
  //LED_MATRIX_MODE = random(2);
}

void deactivate() {
  ACTIVE_MODE = false;

  // Turn off lights
  mWideScanner.deactivate();
  mDefCon.deactivate();
  mBlueGreenWhite.deactivate();
  mRedOrangeYellow.deactivate();
  mWhiteBlueWhite.deactivate();
  mSpeaker.deactivate();
  mBlueRing.deactivate();
  mButtons.deactivate();
}
