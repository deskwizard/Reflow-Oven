#ifndef _SENSOR_H
#define _SENSOR_H

#include "defines.h"
#include "mypid.h"
#include <SPI.h>

//#define FAKE_SENSOR
#define FAKE_SENSOR_VAL 123.2f;

#define MAX6675_INVALID -1.0

void initSensor();
float readTempC();
int16_t readSensorValue();
void handleSensor();

void printTempSerial(float tempC);
void printTempSerialBT(float tempC);

float getAverageTemperature();

#endif
