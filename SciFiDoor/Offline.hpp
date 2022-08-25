  /*
  switch (WIDE_SCANNER_MODE) {
    case 0 : 
      doNotLarsonScanner();
      break;
    case 1 : 
      doLarsonScanner();
      break;
    case 2 : 
      doLeftToRightScanner();
      break;
    case 3 : 
      doRightToLeftScanner();
      break;
    case 4 :
      doRandomyScanner();
      break;
  }
  */

/** 
 *  This is an attempt to build a fading Larson
 *  The "spread" can be adjusted to increase the brightness 
 *  of adjacent LEDs.  This also calculates brightness in
 *  between LEDs -- meaning that it is possible that no single
 *  LED will be at full brightness while the sweep adjusts
 *  between whole values.
 *  
 *  It didn't work...yet.
 */
/*void doNotLarsonScanner() {
  int m = 255;
  int a = (WIDE_SCANNER_WIDTH / 2) + (WIDE_SCANNER_WIDTH / 2) * cos(micros() / 1000.0);
  float spread = 0.5;

  for (int i = 0; i < WIDE_SCANNER_WIDTH; i++) {
    int brightness = 0;

    if (i < a) brightness = m * ((spread * i) + 1 - (spread * a));
    else brightness = m * ((-spread * i) + 1 + (spread * a));

    if (brightness < 0) brightness = 0;

    analogWrite(WIDE_SCANNER_LEDs[i], brightness);
  }
}


void doLeftToRightScanner() {
  int m = 255;
  int a = WIDE_SCANNER_WIDTH * (micros() / 1000.0);
  float spread = 0.5;

  for (int i = 0; i < WIDE_SCANNER_WIDTH; i++) {
    int brightness = 0;

    if (i < a) brightness = m * ((spread * i) + 1 - (spread * a));
    else brightness = m * ((-spread * i) + 1 + (spread * a));

    if (brightness < 0) brightness = 0;

    analogWrite(WIDE_SCANNER_LEDs[i], brightness);
  }
}

void doRightToLeftScanner() {
  int m = 255;
  int a = WIDE_SCANNER_WIDTH - WIDE_SCANNER_WIDTH * (micros() / 1000.0);
  float spread = 0.5;

  for (int i = 0; i < WIDE_SCANNER_WIDTH; i++) {
    int brightness = 0;

    if (i < a) brightness = m * ((spread * i) + 1 - (spread * a));
    else brightness = m * ((-spread * i) + 1 + (spread * a));

    if (brightness < 0) brightness = 0;

    analogWrite(WIDE_SCANNER_LEDs[i], brightness);
  }
}

void doRandomyScanner() {
}
*/

/*
void setScale(int leds [],  int len, int percentage) {
  int increment = 100 / len;

  int fullyOn = percentage / increment;

  // Light LEDs that are fully on
  for (int i = 0; i < fullyOn; i++) {
    digitalWrite(leds[i], HIGH);
  }

  // Partially illuminate based on Percentage
  float lastLedBrightness = (percentage - (fullyOn * increment)) / ((float) increment);
  analogWrite(leds[fullyOn], 255 * lastLedBrightness);

  // Turn off all remaining LEDs
  for (int i = fullyOn + 1; i < len; i++) {
    digitalWrite(leds[i], LOW);
  }
}

void setLED(int leds [], int led, bool on) {
  if (on) digitalWrite(leds[led], HIGH);
  else digitalWrite(leds[led], LOW);
}


void updateMatrixScanner() {
  
}

void updateLargeLeds() {
  
}

*/
