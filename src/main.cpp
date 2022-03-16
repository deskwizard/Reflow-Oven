#include "control.h"
#include "defines.h"
#include "display.h"
#include "keypad.h"
#include "mypid.h"
#include "sensor.h"

void initOTA();
void handleOTA();
void initSerial();
void handleSerial();
void printSerialHelp();

#ifdef BT_SERIAL_ENABLED
BluetoothSerial SerialBT;
#endif

void printSerialHelp();

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

void initSerial() {
  Serial.begin(115200);
  delay(2000);
  Serial.println("Hello");
  Serial.println();
#ifdef BT_SERIAL_ENABLED
  SerialBT.begin("Reflow Oven"); // Bluetooth device name
  SerialBT.println("Hello");
  SerialBT.println();
#endif
}

void handleSerial() {
  if (Serial.available()) {

    unsigned char c = Serial.read();
    float readValueF;
    uint16_t readValueC;

    switch (c) {

    case 'p':
      readValueF = Serial.parseFloat();
      // setKp(readValueF);
      setNearKp(readValueF);
      break;
    case 'i':
      readValueF = Serial.parseFloat();
      // setKi(readValueF);
      setNearKi(readValueF);
      break;
    case 'd':
      readValueF = Serial.parseFloat();
      // setKd(readValueF);
      setNearKd(readValueF);
      break;

    case 'P':
      Serial.print("Kp: ");
      Serial.println(getKp(), 4);
      break;
    case 'I':
      Serial.print("Ki: ");
      Serial.println(getKi(), 4);
      break;
    case 'D':
      Serial.print("Kd: ");
      Serial.println(getKd(), 4);
      break;

    case 'S':
      startPID();
      break;

    case 's':
      stopPID();
      break;

    case 'c':
      readValueC = Serial.parseInt();
      setPreheatDutyCycle(readValueC);
      break;

    case 'v':
      readValueC = Serial.parseInt();
      setPreheatTime(readValueC);
      break;

    case 'w':
      readValueC = Serial.parseInt();
      setDwellTime(readValueC);
      break;

    case 'C':
      Serial.print("Prehead Duty cycle: ");
      Serial.println(getPreheatDutyCycle());
      Serial.print("/");
      Serial.println(MAX_DUTY_CYCLE);
      break;

    case 'V':
      Serial.print("Preheat time: ");
      Serial.println(getPreheatTime());
      break;

    case 'W':
      Serial.print("Dwell time: ");
      Serial.println(getDwellTime());
      break;

    case 'Y':
      Serial.print("Preheat percent: ");
      Serial.println(getPreheatPercent());
      break;

    case 'y':
      readValueC = Serial.parseInt();
      setPreheatPercent(readValueC);
      break;

    case 'h':
      printSerialHelp();
      break;

    case 'f':
      readValueC = Serial.parseInt();
      if (readValueC == 20 || readValueC == 40 || readValueC == 80 ||
          readValueC == 160) {
        setCpuFrequencyMhz(readValueC);
        Serial.print("F_CPU: ");
        Serial.println(readValueC);
      } else {
        Serial.println("Invalid FCPU");
      }
      break;

    } // switch
  }
}

void printSerialHelp() {
  Serial.println("  Command            Action");
  Serial.println("-------------------------------------------------------");
  Serial.println("  S                  Start PID");
  Serial.println("  s                  Stop PID");
  Serial.println();
  Serial.println("  W(int)             Print dwell time");
  Serial.println("  V(int)             Print preheat time");
  Serial.println("  C(int)             Print preheat duty cycle");
  Serial.println("  Y(int)             Print preheat percent");
  Serial.println("  w(int)             Set dwell time");
  Serial.println("  v(int)             Set preheat time");
  Serial.println("  c(int)             Set preheat duty cycle");
  Serial.println("  y(int)             Set preheat percent");
  Serial.println();
  Serial.println("  P                  Print Kp value");
  Serial.println("  I                  Print Ki value");
  Serial.println("  D                  Print Kd value");
  Serial.println("  p(float)           Set Kp");
  Serial.println("  i(float)           Set Ki");
  Serial.println("  d(float)           Set Kd");
  Serial.println();
  Serial.println("  f(20/40/80/160)    Change CPU Frequency to value");
  Serial.println("-------------------------------------------------------");
  Serial.println();
}