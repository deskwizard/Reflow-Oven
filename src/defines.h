#ifndef _DEFINES_H_
#define _DEFINES_H_

#include <Arduino.h>
//#include "SerialHelpers.h"

//#define BT_SERIAL_ENABLED
//#define OTA_ENABLED

#ifdef BT_SERIAL_ENABLED
#include "BluetoothSerial.h"
extern BluetoothSerial SerialBT;
#endif

#define OFF false
#define ON true
#define UNIT_C true
#define UNIT_F false

#define MODE_IDLE 0
#define MODE_PID_DWELL 7
#define MODE_PID_PREHEAT 8
#define MODE_PID_RUNNING 9
#define MODE_OTA 66

/********************** Output Pins **********************/

// LCD pins (configured in lib/TFT_eSPI/User_Setup.h)
/*  --- DO NOT UNCOMMENT - FOR REFERENCE ONLY ---
#define TFT_MISO 19
#define TFT_MOSI 23
#define TFT_SCLK 18
#define TFT_CS   21  // Chip select control pin
#define TFT_DC   22  // Data Command control pin
//#define TFT_RST  16  // Reset pin (could connect to RST pin)
#define TFT_RST  -1  // Set TFT_RST to -1 (connect to ESP32 board RST)
*/

// #define PIN_LED 5
#define PIN_PWM 13    // SSR output pin
#define MAX6675_CS 17 // MAX6675 CS pin

// These need to be together since they're
// over the control module cable.
// TODO: Doublecheck HOTA/HOTB pins
#define PIN_FAN 15
#define PIN_HOTA 4
#define PIN_HOTB 2
#define PIN_BUZZ 16

/********************** Keypad **********************/

// Writing on columns, so don't use input only pins for these
// Rows are set as INPUT_PULLDOWN, so we'll need
// external pulldowns for pins 34 and 35.
#define ROWS_COUNT 7
#define COLS_COUNT 2

#define PIN_COL0 12
#define PIN_COL1 27
#define PIN_ROW0 34
#define PIN_ROW1 35
#define PIN_ROW2 33
#define PIN_ROW3 32
#define PIN_ROW4 25
#define PIN_ROW5 14
#define PIN_ROW6 26

/**************** runmodes ***********************/
// #define RUNMODE_IDLE 0
// #define RUNMODE_HEAT 1
// #define RUNMODE_COOL 2
// #define RUNMODE_HOLD 3
// #define RUNMODE_CANCEL 4
// #define RUNMODE_FINISH 5
// #define RUNMODE_ERROR 6

float CtoF(float celsius);
String getSSID();

void initOTA();
void handleOTA();

#endif