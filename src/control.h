#ifndef _CONTROL_H_
#define _CONTROL_H_

#include "defines.h"

// #define MODE_IDLE 0
// #define MODE_RUN 1
// #define MODE_MAINTAIN 2
// #define MODE_HEAT 3
// #define MODE_COOL 4

void initOutputs();
void setBuzzer(bool state);
void setFan(bool state);
void toggleFan();

void initSensor();
void handleSensor();
float readTempC();
int16_t readSensorValue();

#endif
