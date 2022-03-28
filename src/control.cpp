#include "control.h"
//#include "SerialHelpers.h"

uint8_t deviceMode = MODE_IDLE;
bool fanState = false;

uint8_t selectedHeaters = HEATER_TOP;

bool getFanState() { return fanState; }

uint8_t getDeviceMode() { return deviceMode; }

void setDeviceMode(uint8_t mode) {
  deviceMode = mode;
  Serial.print("mode: ");
  Serial.println(mode);
}

void initOutputs() {

  pinMode(PIN_PWM, OUTPUT);

  // Set control board output pins
  pinMode(PIN_BUZZ, OUTPUT);
  pinMode(PIN_HOTA, OUTPUT);
  pinMode(PIN_HOTB, OUTPUT);
  pinMode(PIN_FAN, OUTPUT);
}

void setBuzzerState(bool state) {
  Serial.print("buzzer: ");
  Serial.println(state);
  digitalWrite(PIN_BUZZ, state);
}

void setFanState(bool state) {
  fanState = state;
  Serial.print("fan: ");
  Serial.println(state);
  digitalWrite(PIN_FAN, fanState);
  updateFanStateDisplay(fanState);
}

void toggleFanState() {
  fanState = !fanState;
  setFanState(fanState);
}

uint8_t getHeatersSelection() { return selectedHeaters; }

void setHeatersState(bool state) {

  if (state) {

    setFanState(ON);

    if (selectedHeaters == HEATER_BOTTOM || selectedHeaters == HEATER_BOTH) {
      digitalWrite(PIN_HOTA, HIGH);
    }

    if (selectedHeaters == HEATER_TOP || selectedHeaters == HEATER_BOTH) {
      digitalWrite(PIN_HOTB, HIGH);
    }

  } else {
    digitalWrite(PIN_HOTA, LOW);
    digitalWrite(PIN_HOTB, LOW);
  }
}

// Toggles the heaters: bottom -> top -> both
void selectHeaters() {

  if (selectedHeaters == HEATER_BOTH) {
    selectedHeaters = HEATER_BOTTOM;
  } else {
    selectedHeaters++;
  }

  updateHeatersSpritesColor();

  Serial.print("heater select: ");
  Serial.println(selectedHeaters);
}
