#include "SerialHelpers.h"
#include "mypid.h"

#ifdef BT_SERIAL_ENABLED
BluetoothSerial SerialBT;
#endif

void initSerial() {
  Serial.begin(115200);
  delay(2000);
  SerialPrintln("Hello");
  SerialPrintln();
#ifdef BT_SERIAL_ENABLED
  SerialBT.begin("Reflow Oven"); // Bluetooth device name
  SerialBT.println("Hello");
  SerialBT.println();
#endif
}

void handleSerial() {

  unsigned char c = Serial.read();
  float readValueF;
  uint16_t readValueC;

  if (Serial.available()) {

    c = Serial.read();

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
      SerialPrint("Kp: ");
      SerialPrintln(getKp(), 4);
      break;
    case 'I':
      SerialPrint("Ki: ");
      SerialPrintln(getKi(), 4);
      break;
    case 'D':
      SerialPrint("Kd: ");
      SerialPrintln(getKd(), 4);
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
      SerialPrint("Prehead Duty cycle: ");
      SerialPrintln(getPreheatDutyCycle());
      SerialPrint("/");
      SerialPrintln(MAX_DUTY_CYCLE);
      break;

    case 'V':
      SerialPrint("Preheat time: ");
      SerialPrintln(getPreheatTime());
      break;

    case 'W':
      SerialPrint("Dwell time: ");
      SerialPrintln(getDwellTime());
      break;

    case 'Y':
      SerialPrint("Preheat percent: ");
      SerialPrintln(getPreheatPercent());
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
        SerialPrint("F_CPU: ");
        SerialPrintln(readValueC);
      } else {
        SerialPrintln("Invalid FCPU");
      }
      break;

    } // switch
  }

#ifdef BT_SERIAL_ENABLED

  if (SerialBT.available()) {

    c = SerialBT.read();

    switch (c) {

    case 'p':
      readValueF = SerialBT.parseFloat();
      // setKp(readValueF);
      setNearKp(readValueF);
      break;
    case 'i':
      readValueF = SerialBT.parseFloat();
      // setKi(readValueF);
      setNearKi(readValueF);
      break;
    case 'd':
      readValueF = SerialBT.parseFloat();
      // setKd(readValueF);
      setNearKd(readValueF);
      break;

    case 'P':
      SerialPrint("Kp: ");
      SerialPrintln(getKp(), 4);
      break;
    case 'I':
      SerialPrint("Ki: ");
      SerialPrintln(getKi(), 4);
      break;
    case 'D':
      SerialPrint("Kd: ");
      SerialPrintln(getKd(), 4);
      break;

    case 'S':
      startPID();
      break;

    case 's':
      stopPID();
      break;

    case 'c':
      readValueC = SerialBT.parseInt();
      setPreheatDutyCycle(readValueC);
      break;

    case 'v':
      readValueC = SerialBT.parseInt();
      setPreheatTime(readValueC);
      break;

    case 'w':
      readValueC = SerialBT.parseInt();
      setDwellTime(readValueC);
      break;

    case 'C':
      SerialPrint("Prehead Duty cycle: ");
      SerialPrintln(getPreheatDutyCycle());
      SerialPrint("/");
      SerialPrintln(MAX_DUTY_CYCLE);
      break;

    case 'V':
      SerialPrint("Preheat time: ");
      SerialPrintln(getPreheatTime());
      break;

    case 'W':
      SerialPrint("Dwell time: ");
      SerialPrintln(getDwellTime());
      break;

    case 'Y':
      SerialPrint("Preheat percent: ");
      SerialPrintln(getPreheatPercent());
      break;

    case 'y':
      readValueC = SerialBT.parseInt();
      setPreheatPercent(readValueC);
      break;

    case 'h':
      printSerialHelp();
      break;

    case 'f':
      readValueC = SerialBT.parseInt();
      if (readValueC == 20 || readValueC == 40 || readValueC == 80 ||
          readValueC == 160) {
        setCpuFrequencyMhz(readValueC);
        SerialPrint("F_CPU: ");
        SerialPrintln(readValueC);
      } else {
        SerialPrintln("Invalid FCPU");
      }
      break;

    } // switch
  }
#endif
}

#ifndef BT_SERIAL_ENABLED
void SerialPrintln() { Serial.println(); }
void SerialPrint(bool data) { Serial.print(data); }
void SerialPrintln(bool data) { Serial.println(data); }
void SerialPrint(String data) { Serial.print(data); }
void SerialPrintln(String data) { Serial.println(data); }
void SerialPrint(uint8_t data) { Serial.print(data); }
void SerialPrintln(uint8_t data) { Serial.println(data); }
void SerialPrint(uint16_t data) { Serial.print(data); }
void SerialPrintln(uint16_t data) { Serial.println(data); }
void SerialPrint(float data) { Serial.print(data); }
void SerialPrintln(float data) { Serial.println(data); }
void SerialPrint(float data, unsigned char digits) {
  Serial.print(data, digits);
}
void SerialPrintln(float data, unsigned char digits) {
  Serial.println(data, digits);
}
#else
void SerialPrintln() {
  Serial.println();
  SerialBT.println();
}

void SerialPrint(bool data) {
  Serial.print(data);
  SerialBT.print(data);
}

void SerialPrintln(bool data) {
  Serial.println(data);
  SerialBT.println(data);
}

void SerialPrint(String data) {
  Serial.print(data);
  SerialBT.print(data);
}

void SerialPrintln(String data) {
  Serial.println(data);
  SerialBT.println(data);
}

void SerialPrint(uint8_t data) {
  Serial.print(data);
  SerialBT.print(data);
}

void SerialPrintln(uint8_t data) {
  Serial.println(data);
  SerialBT.println(data);
}

void SerialPrint(uint16_t data) {
  Serial.print(data);
  SerialBT.print(data);
}

void SerialPrintln(uint16_t data) {
  Serial.println(data);
  SerialBT.println(data);
}

void SerialPrint(float data) {
  Serial.print(data);
  SerialBT.print(data);
}

void SerialPrintln(float data) {
  Serial.println(data);
  SerialBT.println(data);
}

void SerialPrint(float data, unsigned char digits) {
  Serial.print(data, digits);
  SerialBT.print(data);
}

void SerialPrintln(float data, unsigned char digits) {
  Serial.println(data, digits);
  SerialBT.println(data, digits);
}
#endif

void printSerialHelp() {
  SerialPrintln("  Command            Action");
  SerialPrintln("-------------------------------------------------------");
  SerialPrintln("  S                  Start PID");
  SerialPrintln("  s                  Stop PID");
  SerialPrintln();
  SerialPrintln("  W(int)             Print dwell time");
  SerialPrintln("  V(int)             Print preheat time");
  SerialPrintln("  C(int)             Print preheat duty cycle");
  SerialPrintln("  Y(int)             Print preheat percent");
  SerialPrintln("  w(int)             Set dwell time");
  SerialPrintln("  v(int)             Set preheat time");
  SerialPrintln("  c(int)             Set preheat duty cycle");
  SerialPrintln("  y(int)             Set preheat percent");
  SerialPrintln();
  SerialPrintln("  P                  Print Kp value");
  SerialPrintln("  I                  Print Ki value");
  SerialPrintln("  D                  Print Kd value");
  SerialPrintln("  p(float)           Set Kp");
  SerialPrintln("  i(float)           Set Ki");
  SerialPrintln("  d(float)           Set Kd");
  SerialPrintln();
  SerialPrintln("  f(20/40/80/160)    Change CPU Frequency to value");
  SerialPrintln("-------------------------------------------------------");
  SerialPrintln();
}