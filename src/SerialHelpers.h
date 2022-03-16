#ifndef _SERIAL_HELPERS_H_
#define _SERIAL_HELPERS_H_

#include "defines.h"

void initSerial();
void handleSerial();
void printSerialHelp();
void SerialPrintln();
void SerialPrint(bool data);
void SerialPrintln(bool data);
void SerialPrint(String data);
void SerialPrintln(String data);
void SerialPrint(uint8_t data);
void SerialPrintln(uint8_t data);
void SerialPrint(uint16_t data);
void SerialPrintln(uint16_t data);
void SerialPrint(uint32_t data);
void SerialPrintln(uint32_t data);
void SerialPrint(float data);
void SerialPrintln(float data);
void SerialPrint(float data, uint8_t digits);
void SerialPrintln(float data, uint8_t digits);
void SerialPrint(float data, unsigned char digits);
void SerialPrintln(float data, unsigned char digits);

#endif