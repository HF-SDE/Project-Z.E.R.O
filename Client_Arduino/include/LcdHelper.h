#ifndef LCD_HELPER_H
#define LCD_HELPER_H

#include <Arduino.h>
#include <rgb_lcd.h>

extern rgb_lcd lcd;

void writeToLCD(const String& line1, const String& line2 = "");

#endif