#ifndef _MY_PID_
#define _MY_PID_

#include "QuickPID.h"
#include "defines.h"
#include "display.h"

#define PWMFreq 5000 // 5 KHz
#define PWMChannel 0
#define PWMResolution 8
#define MAX_DUTY_CYCLE ((uint16_t)(pow(2, PWMResolution) - 1))

void initPID();
void startPID();
void stopPID();
void handlePID();

void toggleTuning();

float getOutputValue();

float getKp();
float getKi();
float getKd();
float getSpan();

void setKp(float val);
void setKi(float val);
void setKd(float val);

void setNearKp(float val);
void setNearKi(float val);
void setNearKd(float val);

void setConsKp(float val);
void setConsKi(float val);
void setConsKd(float val);

void setPreheatTime(uint8_t minutes);
void setPreheatDutyCycle(uint16_t dutyCycle);
void setDwellTime(uint8_t minutes);

uint8_t getPreheatTime();
uint8_t getPreheatDutyCycle();
uint8_t getDwellTime();

float getSetpointHigh();
void decreaseSetpointHigh(uint8_t step);
void increaseSetpointHigh(uint8_t step);

void printValues();
void printValuesBT();

#endif