#include "sensor.h"
extern bool running;
const uint16_t sensorReadRate = 300; // --MINIMUM-- is 250ms

// https://makeabilitylab.github.io/physcomp/advancedio/smoothing-input.html#arduino-ewma-implementation
float _ewmaAlpha = 0.1; // the EWMA alpha value (α)
float _ewma = 0.0;      // the EWMA result (Si), initialized to zero

void initSensor() {
  pinMode(MAX6675_CS, OUTPUT);
  digitalWrite(MAX6675_CS, HIGH);
  SPI.begin();
  _ewma = float(readSensorValue() * 0.25); // set EWMA (S1) for index 1
}

float readTempC() {
  return (readSensorValue() * 0.25); // Return bits * resolution (0.25°C)
}

int16_t readSensorValue() {

  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
  digitalWrite(MAX6675_CS, LOW);
  uint16_t data = SPI.transfer16(0x00);
  digitalWrite(MAX6675_CS, HIGH);
  SPI.endTransaction();

  if (data & 0x04) // Thermocouple present bit
    return -1;     // -1 is invalid (sensor starts at 0°C)

  return (data >> 3); // Return bits
}

void handleSensor() {
  //******** WARNING *******/
  // Minimum read time for MAX6675 is 250ms

  uint32_t currentMillis = millis();  // Get snapshot of time
  static uint32_t previousMillis = 0; // Tracks the time since last event fired

  if ((uint32_t)(currentMillis - previousMillis) >= sensorReadRate) {

    float tempC = float(readSensorValue() * 0.25);

    // // Apply the EWMA formula
    // _ewma = (_ewmaAlpha * float(readSensorValue() * 0.25)) +
    //         (1.0 - _ewmaAlpha) * _ewma;

    // Check if reading was successful
    if (tempC != MAX6675_INVALID) {
      // Apply the EWMA formula
      _ewma = (_ewmaAlpha * tempC) + (1.0 - _ewmaAlpha) * _ewma;
      // printTempSerial(tempC);
      // printTempSerialBT(tempC);
    } else {
      Serial.println("Error: Could not read temperature data");
      // Call e-stop here
    }
    previousMillis = currentMillis;
  }
}

void printTempSerial(float tempC) {
  if (!running) {
    Serial.print(tempC);
    Serial.print("°C  (");
    Serial.print(CtoF(_ewma));
    Serial.print("°F)");
  }
  // if (running) {
  //   Serial.print("  -  ");
  //   Serial.print(getOutputValue(), 0);
  //   Serial.print("/");
  //   Serial.print(getSpan(), 0);
  //   Serial.print(" (");
  //   Serial.print((getOutputValue() / getSpan()) * 100, 0);
  //   Serial.print("%)");
  // }

  Serial.println();

  // if (!tuning) {

  // Serial.print("R : ");
  //  Serial.print(tempC);
  //  Serial.print("°C  (");
  //  Serial.print(_ewma);
  //  Serial.print("°C)   ");
  //  Serial.print(CtoF(tempC));
  //  Serial.print("°F  (");
  //  Serial.print(CtoF(_ewma));
  //  Serial.print("°F)   ");
  //  Serial.println();
  //}
}

void printTempSerialBT(float tempC) {
#ifdef BT_SERIAL_ENABLED
  if (!tuning) {
    SerialBT.print("R : ");
    SerialBT.print(tempC);
    SerialBT.print("°C  (");
    SerialBT.print(_ewma);
    SerialBT.print("°C)   ");
    SerialBT.print(CtoF(tempC));
    SerialBT.print("°F  (");
    SerialBT.print(CtoF(_ewma));
    SerialBT.print("°F)   ");
    SerialBT.println();
  }
#endif
}