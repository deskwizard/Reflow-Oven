#ifndef _MY_PID_
#define _MY_PID_

#include "defines.h"
#include "display.h"
#include "QuickPID.h"

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

// Do these actually work ... ??
float getKp();
float getKi();
float getKd();
float getSpan();
// These do not for sure
void setKp(float val);
void setKi(float val);
void setKd(float val);



void setPreheatTime(uint8_t minutes);
void setPreheatDutyCycle(uint16_t dutyCycle);
void setDwellTime(uint8_t minutes);


void printValues();
void printValuesBT();

#endif