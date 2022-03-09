#include "PID.h"
#include "control.h"
#include "defines.h"
#include "display.h"
#include "keypad.h"

#ifdef BT_SERIAL_ENABLED
BluetoothSerial SerialBT;
#endif

float CtoF(float celsius) { return (celsius * 1.8f) + 32.0f; }

void setup() {

  Serial.begin(115200);
  delay(2000);
  Serial.println("Hello");
  Serial.println();
#ifdef BT_SERIAL_ENABLED
  SerialBT.begin("Reflow Oven"); // Bluetooth device name
  SerialBT.println("Hello");
  SerialBT.println();
#endif
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