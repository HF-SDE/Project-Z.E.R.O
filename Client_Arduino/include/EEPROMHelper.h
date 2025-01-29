#ifndef EEPROM_HELPER_H
#define EEPROM_HELPER_H

#include <EEPROM.h>
#include <SPI.h>

void saveToEEPROM(int offset, String data);
String readFromEEPROM(int offset);
void clearEEPROM();

#endif