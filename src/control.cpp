#include "control.h"

// float setpointLow = 20.0;
extern float setpointHigh;

bool fanState = false;

bool getFanState() {
  return fanState;
}

void initOutputs() {

  pinMode(PIN_PWM, OUTPUT);

  // Set control board output pins
  pinMode(PIN_BUZZ, OUTPUT);
  pinMode(PIN_HOTA, OUTPUT);
  pinMode(PIN_HOTB, OUTPUT);
  pinMode(PIN_FAN, OUTPUT);
}

void setBuzzer(bool state) {
  Serial.print("buzzer: ");
  Serial.println(state);
  digitalWrite(PIN_BUZZ, state);
}

void setFan(bool state) {
  fanState = state;
  Serial.print("fan: ");
  Serial.println(state);
  digitalWrite(PIN_FAN, fanState);
}

void toggleFan() {
  fanState = !fanState;
  setFan(fanState);
}
