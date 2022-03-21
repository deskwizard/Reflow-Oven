#ifndef _KEYPAD_H_
#define _KEYPAD_H_

#include "control.h"
#include "defines.h"
#include "display.h"
#include "mypid.h"

#define PRESSED 1
#define RELEASED 0

#define KEY_START 13
#define KEY_UNIT 7
#define KEY_FAN 6
#define KEY_UP 11
#define KEY_DOWN 12

// "Public"
void initKeypad();
void handleKeypad();

// "Private"
void initTimer();
void getKeys();
void reactKeys(uint8_t key, uint8_t state);
void printKeyDebug(uint8_t key, uint8_t state);

#endif