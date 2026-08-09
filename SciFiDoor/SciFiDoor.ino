/*
 * SciFiDoor
 * 
 * This is the sketch for my SciFi Door project.  It will have a number of different
 * modes for the different LED displays.
 * 
 * 
 * BUGS:
 *
 * TODO:
 * - Power Down Model
 *
 * TEST:
 * - Open Button (if Open) Produces Error
 * - Open Button (if Open) Does not Play Open Sound
 * - Open Button (while Opening) Continues
 * - Close Button (if Closed) Produces Error
 * - Close Button (if Closed) Does not Play Close Sound
 * - Close Button (while Closing) Continues
 * - Inside Button held while door leaves a limit switch does NOT error
 * - Reversing mid-travel ramps down and back up rather than jerking
 * - Klaxon sounds (DEFCON + 1) times, and Nominal at DEFCON 0
 *
 */
#include "Button.hpp"
#include "DoorMotor.hpp"
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
// 2 - SOUND TRIGGER - Airlock Engage (Sound 3)
// 3 - BUTTON - Inside Shop Door  [White/Orange] -> [White/Green] - Inside Switch
// 4 - BUTTON - Yellow
// 5 - BUTTON - Red
// 6 - LIMIT SWITCH - Door Closed [White/Brown] -> [White/Brown] - Door Closed - Normally CLOSED Switch (measured)
// 7 - LIMIT SWITCH - Door Open   [Orange] -> [White/Blue] - Door Open - Normally CLOSED Switch (measured)
// 8 - PWM - Motor Control Door Close [White/Green] -> [Brown] - Door Closed - Normally Closed Switch -> [Blue] -> [White/Green] -> Speed Controller Input 1
// 9 - PWM - Motor Control Door Open [Green] -> [Orange] - Door Open - Normally Closed Switch -> [White/Orange] -> [Green] -> Speed Controller Input 2
//     Both limit switches sit in series with these lines, so they interrupt the
//     PWM signal in hardware as well as being read on pins 6 and 7.

// 10 - SOUND TRIGGER - All Systems Nominal (Sound 2)
// 11 - SWITCH - Blue Mode Switch
// 12 - BUTTON - Green
// 13* - LED - Internal Status
// 14 - MOTION DETECTOR
// 15 - SOUND TRIGGER - Airlock Disengage (Sound 5)
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
// 52 - MAGNETIC REED SWITCH - Airlock Up
// 53 - MAGNETIC REED SWITCH - Airlock Down
// 54 - 

// 55-69???

/** INPUTS **********************************/
int BUTTON_YELLOW = 4;
int BUTTON_RED = 5;
int BUTTON_GREEN = 12;
int SWITCH_BLUE = 11;

int MOTION_DETECTOR = 14;

int LIMIT_SWITCH_DOOR_CLOSE = 6;
int LIMIT_SWITCH_DOOR_OPEN = 7;
int BUTTON_INSIDE_SHOP_DOOR_OPEN = 3;

int SWITCH_AIRLOCK_UP = 52;
int SWITCH_AIRLOCK_DOWN = 53;


/** OUTPUTS *********************************/
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

int SOUND_NOMINAL = 10;
int SOUND_AIRLOCK_UP = 2;
int SOUND_AIRLOCK_DOWN = 15;

/** TUNING **********************************/

// How long the motor takes to sweep from rest to full speed.  A reversal
// costs two of these: one to wind down, one to wind back up.
const int MOTOR_RAMP_MS = 400;
const int MOTOR_MAX_DUTY = 255;

// Time spent at each DEFCON level.  Drop this to a few seconds to watch the
// whole ladder run on the bench.
const unsigned long DEFCON_INTERVAL_MS = 120000;  // Two Minutes

// DEFCON 0 lights one LED, DEFCON 4 lights all five and holds there forever.
const int DEFCON_MAX = 4;

// Once the ladder is topped out there is nothing left to accumulate.
const unsigned long DEFCON_CEILING_MS = ((unsigned long) DEFCON_MAX) * DEFCON_INTERVAL_MS;

// Length of a single klaxon loop.  The klaxon trigger is held low for a whole
// number of these, so this has to match the audio clip or the last repeat
// will be clipped short.
const unsigned long KLAXON_DURATION_MS = 1900;

/** DIAGNOSTICS *****************************/

// Streams the raw state of every input to the serial monitor.  Set false once
// the wiring is confirmed.
bool mDebugInputs = true;
const unsigned long DEBUG_INTERVAL_MS = 500;

// Both limit switches are wired normally closed, so they conduct to ground
// until the door reaches that end of travel and reads 1 only at the limit.
// That is the opposite of what isPressed() assumes, hence the inversion.
//
// Measured on the door, as raw pin reads:
//
//                    pin 6   pin 7
//   fully closed       1       0
//   mid travel         0       0
//   fully open         0       1
const bool LIMIT_SWITCH_CLOSE_INVERTED = true;
const bool LIMIT_SWITCH_OPEN_INVERTED = true;

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
Button mAirlockUp(SWITCH_AIRLOCK_UP);
Button mAirlockDown(SWITCH_AIRLOCK_DOWN);

Sound mSoundDoor(SOUND_DOOR);
Sound mSoundKlaxon(SOUND_KLAXON);
Sound mSoundHiss(SOUND_HISS);
Sound mSoundError(SOUND_ERROR);
Sound mSoundNominal(SOUND_NOMINAL);
Sound mSoundAirlockUp(SOUND_AIRLOCK_UP);
Sound mSoundAirlockDown(SOUND_AIRLOCK_DOWN);

int CLOSE_DOOR = 8;
int OPEN_DOOR = 9;

DoorMotor mDoorMotor(OPEN_DOOR, CLOSE_DOOR);

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

  // setInverted() first: init() seeds the edge detector by reading the pin.
  mLimitSwitchDoorClose.setInverted(LIMIT_SWITCH_CLOSE_INVERTED);
  mLimitSwitchDoorClose.init();
  mLimitSwitchDoorClose.setOnPressedCallback(&stopDoorClose);

  mLimitSwitchDoorOpen.setInverted(LIMIT_SWITCH_OPEN_INVERTED);
  mLimitSwitchDoorOpen.init();
  mLimitSwitchDoorOpen.setOnPressedCallback(&stopDoorOpen);

  // Edge triggered.  Polling this every pass would fire the error sound the
  // moment the door left a limit switch while the button was still held.
  mOpenDoor.init();
  mOpenDoor.setOnPressedCallback(&toggleDoor);

  // Initialize Motor
  mDoorMotor.init();
  mDoorMotor.setRampTime(MOTOR_RAMP_MS);
  mDoorMotor.setMaxDuty(MOTOR_MAX_DUTY);

  mAirlockUp.init();
  mAirlockUp.setOnPressedCallback(&airlockUp);
  mAirlockDown.init();
  mAirlockDown.setOnPressedCallback(&airlockDown);

  // Initialize Sounds
  mSoundDoor.init();
  mSoundDoor.loopMode(true);
  mSoundKlaxon.init();
  mSoundKlaxon.loopMode(true);
  mSoundHiss.init();
  mSoundError.init();
  mSoundNominal.init();
  mSoundAirlockUp.init();
  mSoundAirlockDown.init();

  initLedMatrixMode(LEDMatrixMode);  
  
  mWBWScanner.setDelay(1000);
  mROYRandom.setDelay(800);

  // At the moment, this pin should be unconnected
  randomSeed(analogRead(1));
  
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);

  // This captures the initial state of the sound airlock lever.
  if (mAirlockDown.isPressed()) Sound::muteAll();

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
  // Unsigned subtraction already wraps correctly when millis() rolls over at
  // ~49 days, so no special case is needed.  Read the clock once: calling
  // millis() several times here would let it advance mid-calculation.
  unsigned long now = millis();
  unsigned long dt = now - mLastTime;

  mLastTime = now;

  Motion::doUpdate(dt);

  doAlive();

  animateAndUpdate(dt);

  dumpInputs(dt);

  if (Motion::getMotion()) mSpeaker.setPin(0, 0, HIGH);
  else mSpeaker.setPin(0, 0, LOW);
  
  // This probably isn't necessary...should check to see if time since top of loop was at least 20ms and then delay the delta.
  delay(20);
}

void animateAndUpdate(unsigned long dt) {
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
  mAirlockUp.update(dt);
  mAirlockDown.update(dt);

  mSoundDoor.update(dt);
  mSoundKlaxon.update(dt);
  mSoundHiss.update(dt);
  mSoundError.update(dt);
  mSoundNominal.update(dt);
  mSoundAirlockUp.update(dt);
  mSoundAirlockDown.update(dt);

  mDoorMotor.update(dt);

  updateKlaxon(dt);
}

unsigned long mDefConTime = 0;

// 0 through DEFCON_MAX.  One more LED lights than the level, so DEFCON 0 shows
// a single lit LED and DEFCON 4 shows all five.
int getDefCon() {
  unsigned long level = mDefConTime / DEFCON_INTERVAL_MS;

  if (level > DEFCON_MAX) level = DEFCON_MAX;

  return (int) level;
}

void doDefConAnimation(unsigned long dt) {
  if (mDefConTime < DEFCON_CEILING_MS) {
    mDefConTime += dt;

    if (mDefConTime > DEFCON_CEILING_MS) mDefConTime = DEFCON_CEILING_MS;
  }

  int defCon = getDefCon();

  // Driven off the same integer the klaxon counts with, so the bar can never
  // disagree with the number of times the klaxon sounds.
  mDefConIndicator.setLitCount(defCon + 1);

  float defConLevel = ((float) mDefConTime) / DEFCON_CEILING_MS;

  //                       y =           m             x +    b
  //unsigned long blinkTimeout = (int) (-900 * defConLevel + 1000);
  // 1000 * (.2 / (x - 1.2) + 1.2)
  unsigned long blinkTimeout = (int) (1000 * (.2 / (defConLevel - 1.2) + 1.2));

  // Solid at DEFCON 0, so "not blinking" and "nominal" are the same condition.
  if (defCon == 0) mYellowButton.setLedBlinkUnpressed(false);
  else mYellowButton.setLedBlinkUnpressed(true);

  mYellowButton.setDelay(blinkTimeout);
}

// Raw pin states, so the wiring can be read off the serial monitor rather than
// inferred from behaviour.  0 means the pin is pulled to ground.
unsigned long mDebugTime = 0;

void dumpInputs(unsigned long dt) {
  if (!mDebugInputs) return;
  if (!mSerialMonitor) return;

  mDebugTime += dt;
  if (mDebugTime < DEBUG_INTERVAL_MS) return;
  mDebugTime = 0;

  Serial.print("LIMIT close(6)=");
  Serial.print(digitalRead(LIMIT_SWITCH_DOOR_CLOSE));
  Serial.print(" open(7)=");
  Serial.print(digitalRead(LIMIT_SWITCH_DOOR_OPEN));

  Serial.print(" | BTN grn(12)=");
  Serial.print(digitalRead(BUTTON_GREEN));
  Serial.print(" red(5)=");
  Serial.print(digitalRead(BUTTON_RED));
  Serial.print(" yel(4)=");
  Serial.print(digitalRead(BUTTON_YELLOW));
  Serial.print(" in(3)=");
  Serial.print(digitalRead(BUTTON_INSIDE_SHOP_DOOR_OPEN));
  Serial.print(" blu(11)=");
  Serial.print(digitalRead(SWITCH_BLUE));

  Serial.print(" | AIR up(52)=");
  Serial.print(digitalRead(SWITCH_AIRLOCK_UP));
  Serial.print(" dn(53)=");
  Serial.print(digitalRead(SWITCH_AIRLOCK_DOWN));
  Serial.print(" muted=");
  Serial.print(Sound::isMuted());

  Serial.print(" | MOTOR dir=");
  Serial.print(mDoorMotor.getDirection());
  Serial.print(" duty=");
  Serial.print(mDoorMotor.getDuty());

  Serial.print(" | DEFCON=");
  Serial.print(getDefCon());
  Serial.print(" t=");
  Serial.println(mDefConTime);
}

// Milliseconds left in the running klaxon sequence, 0 when idle.
unsigned long mKlaxonRemaining = 0;

void updateKlaxon(unsigned long dt) {
  if (mKlaxonRemaining == 0) return;

  if (dt >= mKlaxonRemaining) {
    mKlaxonRemaining = 0;

    mSoundKlaxon.stop();

    // The alarm has been sounded, so the ladder starts over.
    mDefConTime = 0;

  } else {
    mKlaxonRemaining -= dt;
  }
}

//bool mDoorMoving = false;
bool mDoorClosing = false;
bool mDoorOpening = false;

// The inside shop door button toggles the door.  Called from the button's
// pressed callback, so it fires once per press rather than once per pass.
void toggleDoor() {
  if (mSerialMonitor) Serial.println("toggleDoor()");

  if (mLimitSwitchDoorOpen.isPressed()) closeDoor();
  else if (mLimitSwitchDoorClose.isPressed()) openDoor();
  else mSoundError.trigger();
}

void openDoor() {  
  if (mSerialMonitor) Serial.println("openDoor()");

  if (mLimitSwitchDoorOpen.isPressed()) mSoundError.trigger();
  else {
    mSoundDoor.trigger();
    mDoorMotor.open();
  }
}

void closeDoor() {  
  if (mSerialMonitor) Serial.println("closeDoor()");

  if (mLimitSwitchDoorClose.isPressed()) mSoundError.trigger();
  else {
    mSoundDoor.trigger();
    mDoorMotor.close();
  }
}

// Mid-travel stop, so ease the motor down rather than dropping it.
void stopDoor() {
  if (mSerialMonitor) Serial.println("stopDoor()");
  mDoorMotor.stop();

  mSoundDoor.stop();
  mSoundHiss.trigger();
}

// End of travel.  Ramping down here would coast the door into the stop.
void haltDoor() {
  mDoorMotor.halt();

  mSoundDoor.stop();
  mSoundHiss.trigger();
}

void stopDoorClose() {
  if (mSerialMonitor) Serial.println("stopDoorClose()");
  haltDoor();
}

void stopDoorOpen() {
  if (mSerialMonitor) Serial.println("stopDoorOpen()");
  haltDoor();
}

void startKlaxon() {
  if (mSerialMonitor) Serial.println("startKlaxon()");

  // Already sounding.  Let the running sequence finish.
  if (mKlaxonRemaining > 0) return;

  int defCon = getDefCon();

  if (mSerialMonitor) {
    Serial.print("  DEFCON=");
    Serial.print(defCon);
    Serial.print(" defConTime=");
    Serial.println(mDefConTime);
  }

  if (defCon == 0) {
    if (mSerialMonitor) Serial.println("  -> NOMINAL (pin 10)");

    mSoundNominal.trigger();
    return;
  }

  if (mSerialMonitor) {
    Serial.print("  -> KLAXON x");
    Serial.println(defCon + 1);
  }

  // Held low for a whole number of klaxon loops, one more than the level.
  mKlaxonRemaining = ((unsigned long) (defCon + 1)) * KLAXON_DURATION_MS;

  mSoundKlaxon.trigger();
}

// The sequence runs to completion, so releasing the button does nothing.  It
// stays wired up because the Button class calls it on release.
void stopKlaxon() {
  if (mSerialMonitor) Serial.println("stopKlaxon()");
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

void airlockUp() {  
  if (mSerialMonitor) Serial.println("airlockUp()");
  
  Sound::unmuteAll();
  mSoundAirlockUp.trigger();
}

void airlockDown() {  
  if (mSerialMonitor) Serial.println("airlockDown()");
  mSoundAirlockDown.trigger();
  Sound::muteAll();
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

  // The airlock reed switches are inputs with no LED of their own, so there is
  // nothing to turn off for them here.
}
