#include "defines.h"
#include "display.h"
#include "keypad.h"
#include "PID.h"
#include "control.h"

float CtoF(float celsius) { return (celsius * 1.8f) + 32.0f; }

void setup() {

  Serial.begin(115200);
  delay(2000);
  Serial.println("Hello");
  Serial.println();

  initDisplay();
  initOutputs();
  initSensor();
  initKeypad();
}

void loop() {
  handleDisplay();
  handleKeypad();
  handleSensor();
  handlePIDsTune();
}