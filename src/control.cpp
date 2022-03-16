#include "control.h"
//#include "SerialHelpers.h"

uint8_t deviceMode = MODE_IDLE;
bool fanState = false;

bool getFanState() { return fanState; }

uint8_t getDeviceMode() { return deviceMode; }

void setDeviceMode(uint8_t mode) { deviceMode = mode; }

void initOutputs() {

  pinMode(PIN_PWM, OUTPUT);

  // Set control board output pins
  pinMode(PIN_BUZZ, OUTPUT);
  pinMode(PIN_HOTA, OUTPUT);
  pinMode(PIN_HOTB, OUTPUT);
  pinMode(PIN_FAN, OUTPUT);
}

void setBuzzer(bool state) {
  SerialPrint("buzzer: ");
  SerialPrintln(state);
  digitalWrite(PIN_BUZZ, state);
}

void setFan(bool state) {
  fanState = state;
  SerialPrint("fan: ");
  SerialPrintln(state);
  digitalWrite(PIN_FAN, fanState);
  updateFanStateDisplay(fanState);
}

void toggleFan() {
  fanState = !fanState;
  setFan(fanState);
}
