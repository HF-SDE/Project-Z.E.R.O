#include "../lib/DisplayManager.h"
#include "../lib/ComponentManager.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// -------- INTERNAL STATE --------
static LiquidCrystal_I2C lcd(0x27, 20, 4);
static uint8_t g_cols = 20;
static uint8_t g_rows = 4;
static bool g_ready = false;
static Component *g_displayComponent = nullptr;
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

    displayShowMessage("Starting up...");
    return true;
}

void displaySetComponent(Component *component)
{
    g_displayComponent = component;
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

    // Clear the entire screen first
    lcd.clear();

    // Split message across all available rows (up to 4 lines)
    // Automatically shifts to next line when running out of columns
    const char *currentPos = msg;

    for (uint8_t row = 0; row < g_rows; row++)
    {
        // Print up to g_cols characters for this row
        lcd.setCursor(0, row);

        // Print characters for this row
        uint8_t i = 0;
        for (; i < g_cols && *currentPos != '\0'; i++)
        {
            lcd.print(*currentPos);
            currentPos++;
        }

        // Stop if we've reached the end of the string
        if (*currentPos == '\0')
            break;
    }

    // Update component state and publish
    if (g_displayComponent != nullptr)
    {
        componentUpdateValue(g_displayComponent, String(msg));
    }
}

void displayOverrideLine(uint8_t lineNumber, const char *msg)
{
    if (!g_ready || msg == nullptr || lineNumber >= g_rows)
        return;

    // Display message on specified line and pad the rest
    printPadded(0, lineNumber, msg, g_cols);
}
