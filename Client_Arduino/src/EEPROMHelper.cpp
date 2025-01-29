#include "EEPROMHelper.h"

void saveToEEPROM(int offset, String data) {
    for (int i = 0; i < data.length(); i++) {
        EEPROM.write(offset + i, data[i]);
    }

    EEPROM.write(offset + data.length(), '\0');
}

String readFromEEPROM(int offset) {
  String data = "";
  for (int i = 0; i < EEPROM.length(); i++)
  {
    char c = EEPROM.read(offset + i);
    if (c == '\0')
      break;
    data += c;
  }
  return data;
}

void clearEEPROM() {
    for (int i = 0; i < EEPROM.length(); i++) {
        EEPROM.write(i, 0);
    }
    Serial.println("EEPROM cleared.");
}