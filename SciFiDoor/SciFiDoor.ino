/*
 * SciFiDoor
 * 
 * This is the sketch for my SciFi door project.  It will have a number of different
 * modes for the different LED displays.
 * 
 * TODO:
 * - Add Support for Blue Switch LED
 * - Add Support for Button LEDs
 * 
 * - Add Support for Buttons
 *   - for now, just light when pressed
 *   
 * 
 * - Add support for turning on general light stuff
 * 
*/

#include "Motion.hpp"
#include "LedMatrix.hpp"

#include "Button.hpp"
#include "LarsonScanner.hpp"
#include "LedLevelIndicator.hpp"

/** INPUTS **********************************/
int BUTTON_YELLOW = 10;
int BUTTON_RED = 11;
int BUTTON_GREEN = 12;
int SWITCH_BLUE = 13;

int MOTION_DETECTOR = 14;

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
int SOUND_PICARD = 17;
int SOUND_KLAXON = 18;

LedMatrix mWideScanner(WIDE_SCANNER_LEDs, 7, 1);
LedMatrix mDefCon(DEF_CON_LEDs, 5, 1);
LedMatrix mBlueGreenWhite(BGW_LEDs, 5, 1);
LedMatrix mRedOrangeYellow(OTHER_LEDs, 5, 1);
LedMatrix mWhiteBlueWhite(WBW_LEDs, 5, 1);
LedMatrix mSpeaker(SPEAKER_LEDs, 1, 1);
LedMatrix mBlueRing(BLUE_RING_LEDs, 1, 1);
LedMatrix mButtons(BUTTON_LEDs, 3, 1);

LarsonScanner mLarsonScanner(&mWideScanner);
LedLevelIndicator mDefConIndicator(&mDefCon);
Button mGreen(BUTTON_GREEN);

// the setup function runs once when you press reset or power the board
void setup() {
  Motion::init(MOTION_DETECTOR);

  mWideScanner.init();
  mDefCon.init();
  mBlueGreenWhite.init();
  mRedOrangeYellow.init();
  mWhiteBlueWhite.init();
  mSpeaker.init();
  mBlueRing.init();
  mButtons.init();

  mGreen.setLedPin(BUTTON_LEDs[0]);
  mGreen.init();

  // At the moment, this pin should be unconnected
  randomSeed(analogRead(1));
  
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);

  pinMode(SOUND_DOOR, OUTPUT);
  pinMode(SOUND_PICARD, OUTPUT);
  pinMode(SOUND_KLAXON, OUTPUT);
}


int ALIVE_STATE = HIGH;

void doAlive() {
  digitalWrite(LED_BUILTIN, ALIVE_STATE);
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
     mLastTime = millis();
  }
  
  Motion::doUpdate(dt);

  doAlive();

  if (Motion::getTimeSinceLastMotion() > TIMEOUT) {
    if (ACTIVE_MODE) deactivate();
    
  } else {
    if (!ACTIVE_MODE) activate();
    
    doAnimate(dt);

    if (mGreen.isPressed()) {
      digitalWrite(SOUND_DOOR, LOW);
    }

  }
  
  delay(20);
}

void doAnimate(int dt) {
  mLarsonScanner.doUpdate(dt);

  mDefConIndicator.setLevel(millis() / 1000 % 50);

  mGreen.update(dt);

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
