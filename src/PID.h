#ifndef _PID_H_
#define _PID_H_

void initPIDsTune();
void handlePIDsTune();

void toggleTuning();

float getOutputValue();
float getKp();
float getKi();
float getKd();

#endif