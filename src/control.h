#ifndef _CONTROL_H_
#define _CONTROL_H_

#include "defines.h"
#include "display.h"

#define HEATER_BOTTOM 0
#define HEATER_TOP 1
#define HEATER_BOTH 2

// #define MODE_IDLE 0
// #define MODE_RUN 1
// #define MODE_MAINTAIN 2
// #define MODE_HEAT 3
// #define MODE_COOL 4

void initOutputs();

void setBuzzerState(bool state);

void setFanState(bool state);
void toggleFanState();
bool getFanState();

void setDeviceMode(uint8_t mode);
uint8_t getDeviceMode();

void selectHeaters();
void setHeatersState(bool state);
uint8_t getHeatersSelection();

#endif
