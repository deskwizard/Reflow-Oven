#ifndef _KEYPAD_H_
#define _KEYPAD_H_

#include "PID.h"
#include "control.h"
#include "defines.h"
#include "display.h"

#define PRESSED 1
#define RELEASED 0

// "Public"
void initKeypad();
void handleKeypad();

// "Private"
void initTimer();
void getKeys();
void reactKeys(uint8_t key, uint8_t state);
void printKeyDebug(uint8_t key, uint8_t state);

#endif