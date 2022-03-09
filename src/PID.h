#ifndef _PID_H_
#define _PID_H_

#include "defines.h"

void initPIDsTune();
void handlePIDsTune();

void toggleTuning();

float getOutputValue();
float getKp();
float getKi();
float getKd();
float getOutput();
float getSpan();
void printValues();
void printValuesBT();

#endif