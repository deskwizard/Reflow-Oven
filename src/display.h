#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include "mypid.h"
#include "control.h"
#include "defines.h"
#include <SPI.h>
#include <TFT_eSPI.h>

//#define VP_DEBUG

// Are those the right values?
#define DISPLAY_W 127
#define DISPLAY_H 160

// Backgroud fill colours
#ifdef VP_DEBUG
#define TVAL_FILL TFT_BLUE
#define SPVAL_FILL TFT_RED
#define T_UNIT_FILL TFT_NAVY
#define SP_UNIT_FILL TFT_PURPLE
#define IND_PWR_FILL TFT_BROWN
#define IND_STATE_FILL TFT_DARKCYAN
#define TUNE_VAL_FILL TFT_DARKGREY
#define SPR_FAN_FILL TFT_CYAN
#else
#define TVAL_FILL TFT_BLACK
#define SPVAL_FILL TFT_BLACK
#define T_UNIT_FILL TFT_BLACK
#define SP_UNIT_FILL TFT_BLACK
#define IND_PWR_FILL TFT_BLACK
#define IND_STATE_FILL TFT_BLACK
#define TUNE_VAL_FILL TFT_BLACK
#define SPR_FAN_FILL TFT_BLACK
#endif

// These will need renaming to be clearer
#define SP_VP_Y 0 // 100
#define SP_H 24
#define SP_VP_W 40

#define IND_PWR_H 20

#define TVAL_W 110
#define TVAL_H 60
#define TVAL_Y 28

#define T_UNITVP__X TVAL_W + 1
#define T_UNIT_Y TVAL_Y
#define T_UNIT_W 27
#define T_UNIT_H 50

#define GFXFF 1 // GFXFF font is font #1



/*************************** Sprites ***************************/

// Width and height of the fan sprite
#define SPR_FAN_WIDTH  23
#define SPR_FAN_HEIGHT 23

#define SPR_FAN_UPDATE_RATE 100
#define SPR_FAN_ANGLE_PER_FRAME 10

// These are the fan sprite VP positions
#define SPR_FAN_X_POS 104
#define SPR_FAN_Y_POS 0

#define SPR_FAN_BORDER 2 // 2 = 1 pixel on each side, even number obviously

// Offset image by half the border size so it's centered
#define SPR_FAN_X_PIV (SPR_FAN_WIDTH / 2) + (SPR_FAN_BORDER / 2)
#define SPR_FAN_Y_PIV (SPR_FAN_HEIGHT / 2) + (SPR_FAN_BORDER / 2)

/*********************************************************************************/


void initDisplay();
void handleDisplay();
void updateDisplay();

// Update both temp and SP units
void updateUnitDisplay();
void toggleDisplayUnit();

void updateSetpointDisplay();

void updatePowerIndicator();

void updateStateIndicator();
void updatePIDValues();

void displayOTASetup();
void displayOTAIP(String ipaddr);
void displayOTADone();
void displayOTAProgress(uint8_t progress);
void displayOTAStarting(String type);

void updateFanStateDisplay(bool state);
#endif