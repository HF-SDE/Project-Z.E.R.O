#include "LcdHelper.h"

rgb_lcd lcd;

void writeToLCD(const String& line1, const String& line2 = "") {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(line1.substring(0, 16));

    lcd.setCursor(0, 1);

    if (line2 != "") lcd.print(line2.substring(0, 16));
    else lcd.print(line1.substring(16));
}