#include "SerialHelpers.h"
#include "control.h"
#include "defines.h"
#include "display.h"
#include "keypad.h"
#include "mypid.h"
#include "sensor.h"

float CtoF(float celsius) { return (celsius * 1.8f) + 32.0f; }

void setup() {

  // setCpuFrequencyMhz(160); //  70mA

  initSerial();
  initKeypad();
  initDisplay();

  if (getDeviceMode() == MODE_OTA) {
    // setCpuFrequencyMhz(80);
    initOTA();
  }

  // Does it matter if we start these in OTA mode ?
  initOutputs();
  initSensor();

  initPID();
}

void loop() {

  if (getDeviceMode() != MODE_OTA) {
    handleDisplay();
    handleKeypad();
    handleSensor();
    handlePID();
    handleSerial();
  } else {
    handleOTA();
  }
}