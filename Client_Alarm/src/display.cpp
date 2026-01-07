#include "../lib/DisplayManager.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// -------- INTERNAL STATE --------
static LiquidCrystal_I2C lcd(0x27, 20, 2);
static uint8_t g_cols = 20;
static uint8_t g_rows = 2;
static bool g_ready = false;
// --------------------------------

bool displayInit(uint8_t i2cAddress, uint8_t cols, uint8_t rows, int sdaPin, int sclPin)
{
    g_cols = cols;
    g_rows = rows;

    Wire.begin(sdaPin, sclPin);

    // Recreate lcd with the chosen address/size.
    // LiquidCrystal_I2C doesn't let us change address after construction in many libs,
    // so we construct with defaults above and rely on setAddress patterns not being portable.
    // The simplest widely-working approach: keep address fixed OR use a library that supports setAddress.
    // If your library supports changing address, we can adapt.
    // For now: assume your address is 0x27 and your constructor matches it.

    // If you NEED dynamic address support, tell me which LiquidCrystal_I2C library you installed.

    lcd.init(); // common for many LiquidCrystal_I2C libs
    lcd.backlight();
    lcd.clear();

    g_ready = true;
    return true;
}

void displayClear()
{
    if (!g_ready)
        return;
    lcd.clear();
}

static void printPadded(uint8_t col, uint8_t row, const char *s, uint8_t len)
{
    lcd.setCursor(col, row);
    // print up to len chars, pad rest with spaces to overwrite old text
    uint8_t i = 0;
    for (; i < len && s[i] != '\0'; i++)
        lcd.print(s[i]);
    for (; i < len; i++)
        lcd.print(' ');
}

void displayShowMessage(const char *msg)
{
    if (!g_ready || msg == nullptr)
        return;

    // Split message into rows of g_cols characters
    // For 20x2: prints first 20 on row 0, next 20 on row 1
    // Also pads with spaces so remnants of old text don't stay.

    // Row 0
    printPadded(0, 0, msg, g_cols);

    // Row 1 (if exists)
    if (g_rows > 1)
    {
        const char *secondLine = msg;
        // advance pointer by g_cols chars, but stop if string ends
        for (uint8_t i = 0; i < g_cols && *secondLine != '\0'; i++)
            secondLine++;
        printPadded(0, 1, secondLine, g_cols);
    }
}

void displaySetColor(const char *colorName)
{
    // For standard single-color backlight LCDs:
    if (!g_ready)
        return;

    // This is a placeholder implementation for standard LCDs
    // If colorName is "off" or "black", turn backlight off
    if (strcmp(colorName, "off") == 0 || strcmp(colorName, "black") == 0)
    {
        lcd.noBacklight();
    }
    else
    {
        // Any other color turns backlight on
        lcd.backlight();
    }
}