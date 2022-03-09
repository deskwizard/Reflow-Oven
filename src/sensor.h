#ifndef _SENSOR_H
#define _SENSOR_H

#include "defines.h"
#include <SPI.h>

#define MAX6675_INVALID -1.0

void initSensor();
float readTempC();
int16_t readSensorValue();
void handleSensor();

#endif
