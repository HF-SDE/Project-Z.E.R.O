#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <LiquidCrystal_I2C.h>

// Declare the lcd object globally
LiquidCrystal_I2C lcd(0x27, 20, 2);

void setupLcd(const int lcdColumns, const int lcdRows)
{
    lcd.begin(lcdColumns, lcdRows);
    // turn on LCD backlight
    lcd.backlight();
}

void displayOnLcd(const char *msg)
{
    lcd.clear();

    // line 1: first 16 chars
    lcd.setCursor(0, 0);
    for (int i = 0; i < 16 && msg[i] != '\0'; i++)
        lcd.print(msg[i]);

    // line 2: next 16 chars
    lcd.setCursor(0, 1);
    for (int i = 16; i < 32 && msg[i] != '\0'; i++)
        lcd.print(msg[i]);
}

#endif // DISPLAY_MANAGER_H