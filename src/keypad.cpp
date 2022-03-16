#include "keypad.h"

uint8_t adjustStep = 1; // 1, 10, 100

extern float setpointHigh;

// Connect to the rows and columns pins of the kpd
// Use that array in the code so we can have a nested loop
// instead of running the pin loop twice ?
// (premature optimisation...)
// uint8_t colPins[COLS_COUNT] = {PIN_COL0, PIN_COL1};
uint8_t rowPins[ROWS_COUNT] = {PIN_ROW0, PIN_ROW1, PIN_ROW2, PIN_ROW3,
                               PIN_ROW4, PIN_ROW5, PIN_ROW6};

volatile uint16_t allKeys = 0;
uint16_t lastKeys = 0;

hw_timer_t *timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

void initKeypad() {

  for (uint8_t x = 0; x < ROWS_COUNT; x++) {
    pinMode(rowPins[x], INPUT_PULLDOWN);
  }

  pinMode(PIN_COL0, INPUT);
  pinMode(PIN_COL1, INPUT);

  delay(100);

  // Cycle matrix once to initialize
  // the allKeys variable
  getKeys();
  delay(10);
  getKeys();

  // Serial.print("K: ");
  // Serial.println(allKeys);

  if (allKeys == 8) {
    // Key 3 help down
    // Set OTA mode, start OTA, etc...
    setDeviceMode(MODE_OTA);
    Serial.print("OTA key detected");
    Serial.println(allKeys);
  } else {
    initTimer();
  }
}

void handleKeypad() {

  if (lastKeys != allKeys) {
    // Serial.print("Key ");

    for (uint8_t x = 1; x <= 14; x++) { // Start at 1, we don't have a key 0
      if (bitRead(lastKeys, x) != bitRead(allKeys, x)) {
        // Serial.print(x);
        //  Serial.print(": ");
        //   Serial.print(bitRead(allKeys, x));
        reactKeys(x, bitRead(allKeys, x));
        // if (bitRead(allKeys, x) == 1) {
        //   Serial.print(" - Pressed");
        // } else {
        //   Serial.print(" - Released");
        // }
      }
    }
    Serial.println();

    lastKeys = allKeys;
  }
}

void reactKeys(uint8_t key, uint8_t state) {

  printKeyDebug(key, state);

  // return; // for debug.

  // These don't belong here.
  static bool hotAState = false;
  static bool hotBState = false;

  if (key == 1) {
    if (state == PRESSED) {

      if (getDeviceMode() == MODE_IDLE) {
        startPID();
      } else {
        stopPID();
        setDeviceMode(MODE_IDLE);
      }
    }
  }

  if (key == 3) {
    if (state == PRESSED) {
      if (getDisplayMode() == DISP_MODE_TEMP) {
        setDisplayMode(DISP_MODE_MIN);
      } else if (getDisplayMode() == DISP_MODE_MIN) {
        setDisplayMode(DISP_MODE_MAX);
      } else if (getDisplayMode() == DISP_MODE_MAX) {
        setDisplayMode(DISP_MODE_TEMP);
      }
      Serial.print("display mode: ");
      Serial.println(getDisplayMode());
    }
  }

  if (key == 5) {
    Serial.print(F("Fan"));

    if (state == PRESSED) {
      toggleFan();
    }
  }

  // if (key == 6) {
  //   // Serial.print(F("begel"));
  //   if (state == PRESSED) {
  //     // Next digit adjust
  //     if (adjustStep == 100) {
  //       adjustStep = 1;
  //     } else {
  //       adjustStep = adjustStep * 10;
  //     }
  //   }
  // }

  if (key == 7) {
    // Serial.print(F("begel"));
    if (state == PRESSED) {
      toggleDisplayUnit();
    }
  }

  if (key == 8) {
    if (state == PRESSED) {
      Serial.print(F("HOTA: "));
      hotAState = !hotAState;
      Serial.println(hotAState);
      digitalWrite(PIN_HOTA, hotAState);
    }
  }
  if (key == 9) {
    if (state == PRESSED) {
      Serial.print(F("HOTB: "));
      hotBState = !hotBState;
      Serial.println(hotBState);
      digitalWrite(PIN_HOTB, hotBState);
    }
  }
  if (key == 10) {
    Serial.print(F("Degel"));

    if (state == PRESSED) {
      setBuzzer(HIGH);
    } else if (state == RELEASED) {
      setBuzzer(LOW);
    }
  }

  if (key == 11) {
    if (state == PRESSED) {
      // if (getDisplayUnit() == UNIT_C) {
      //   setpointHigh = setpointHigh + adjustStep;
      // } else {
      //   setpointHigh = setpointHigh + (adjustStep * 0.555555556f);
      // }
      increaseSetpointHigh(adjustStep);
      Serial.print("SPH: ");
      Serial.println(getSetpointHigh());
      updateSetpointDisplay();
      /*
      if (displayMode == MODE_LSET) {
        if (displayUnit == UNIT_C) {
          setpointLow = setpointLow + adjustStep;
        } else {
          setpointLow = setpointLow + (adjustStep * 0.555555556f);
        }
        Serial.print("LSET: ");
        Serial.println(setpointLow);
        // setLowAlarm(setpointLow);
        // //updateDisplay();
      } else if (displayMode == MODE_HSET) {
        if (displayUnit == UNIT_C) {
          setpointHigh = setpointHigh + adjustStep;
        } else {
          setpointHigh = setpointHigh + (adjustStep * 0.555555556f);
        }
        // setHighAlarm(setpointHigh);
        Serial.print("HSET: ");
        Serial.println(setpointHigh);
        // //updateDisplay();
      }
      //updateDisplay();
      */
    }
  }

  if (key == 12) {
    // Serial.print(F("Down"));
    if (state == PRESSED) {
      // if (getDisplayUnit() == UNIT_C) {
      //   setpointHigh = setpointHigh - adjustStep;
      // } else {
      //   setpointHigh = setpointHigh - (adjustStep * 0.555555556f);
      // }
      decreaseSetpointHigh(adjustStep);
      Serial.print("SPH: ");
      Serial.println(getSetpointHigh());
      updateSetpointDisplay();
      /*
      if (displayMode == MODE_LSET) {
        if (displayUnit == UNIT_C) {
          setpointLow = setpointLow - adjustStep;
        } else {
          setpointLow = setpointLow - (adjustStep * 0.555555556f);
        }
        Serial.print("LSET: ");
        Serial.println(setpointLow);
        // setLowAlarm(setpointLow);
        // //updateDisplay();
      } else if (displayMode == MODE_HSET) {
        if (displayUnit == UNIT_C) {
          setpointHigh = setpointHigh - adjustStep;
        } else {
          setpointHigh = setpointHigh - (adjustStep * 0.555555556f);
        }
        // setHighAlarm(setpointHigh);
        Serial.print("HSET: ");
        Serial.println(setpointHigh);
        // //updateDisplay();
      }
      // updateDisplay();
      */
    }
    /*************************************/
  }

  if (key == 14) {
    Serial.print(F("Start/Stop"));
    if (state == PRESSED) {
      /*
            if (displayMode == MODE_RUN) {
              lastIdleOrRun = displayMode;
              displayMode++;
            }

            else if (displayMode == MODE_IDLE) {
              lastIdleOrRun = displayMode;
              displayMode = MODE_LSET;
            } else {
              displayMode++;
            }

            // deviceMode++;
            if (displayMode > MODE_LAST) {
              displayMode = lastIdleOrRun;
            }
            // setDisplayMode();

            // updateDisplay();
            Serial.print(F("Mode change to: "));
            Serial.println(displayMode);

            */
    }
  }

  Serial.println();
}

void getKeys() {
  static bool cycle = 0;

  bool pinRead;

  if (cycle == 0) {
    pinMode(PIN_COL0, OUTPUT);
    digitalWrite(PIN_COL0, HIGH);
    for (uint8_t x = 0; x < ROWS_COUNT; x++) {
      pinRead = digitalRead(rowPins[x]);
      bitWrite(allKeys, x, pinRead);
      // Serial.print(pinRead);
      // Serial.print(" ");
    }
    pinMode(PIN_COL0, INPUT);
  } else {
    pinMode(PIN_COL1, OUTPUT);
    digitalWrite(PIN_COL1, HIGH);
    for (uint8_t x = 0; x < ROWS_COUNT; x++) {
      pinRead = digitalRead(rowPins[x]);
      bitWrite(allKeys, x + 7, pinRead);
      // Serial.print(pinRead);
      // Serial.print(" ");
    }
    pinMode(PIN_COL1, INPUT);
  }
  cycle = !cycle;
}

void printKeyDebug(uint8_t key, uint8_t state) {

  // Serial debug for keys
  Serial.print("Key ");
  Serial.print(key);

  if (state == PRESSED) {
    Serial.println(" was Pressed");
  } else if (state == RELEASED) {
    Serial.println(" was Released");
  } else {
    Serial.println("Woops, something went wrong");
  }
}

void printKeyDebugBT(uint8_t key, uint8_t state) {
#ifdef BT_SERIAL_ENABLED
  // Serial debug for keys
  SerialBT.print("Key ");
  SerialBT.print(key);

  if (state == PRESSED) {
    SerialBT.println(" was Pressed");
  } else if (state == RELEASED) {
    SerialBT.println(" was Released");
  } else {
    SerialBT.println("Woops, something went wrong");
  }
#endif
}

void IRAM_ATTR onTimer() {
  portENTER_CRITICAL_ISR(&timerMux);
  getKeys();
  portEXIT_CRITICAL_ISR(&timerMux);
}

void initTimer() {
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 10000, true); // In microseconds
  timerAlarmEnable(timer);
}