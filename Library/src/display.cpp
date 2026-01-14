#include "DisplayManager.h"
#include "ComponentManager.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// -------- INTERNAL STATE --------
// Default to LCMI602 (PCF8574 backpack, 16x2)
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

    Wire.end(); // reset any prior driver state
    delay(50);
    // (Re)start I2C every time to guarantee the driver is installed before the first write
    if (!Wire.begin(sdaPin, sclPin, 100000))
    {
        return false; // I2C bus not ready
    }
    Wire.setClock(100000); // keep the bus slow for PCF8574 stability
    delay(100);            // allow driver to fully settle before first transaction

    // Probe the requested address before touching the LCD to avoid invalid-state errors
    Wire.beginTransmission(i2cAddress);
    uint8_t err = Wire.endTransmission();
    if (err != 0)
    {
        g_ready = false;
        return false; // no device responding at 0x27
    }
    delay(50); // additional settle time after successful probe

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

    // Wrap in try-catch equivalent: validate I2C is still alive before heavy operations
    Wire.beginTransmission(0x27);
    if (Wire.endTransmission() != 0)
    {
        g_ready = false; // mark as not ready if device disappeared
        return;
    }

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

    // Check I2C device is still responding
    Wire.beginTransmission(0x27);
    if (Wire.endTransmission() != 0)
    {
        g_ready = false;
        return;
    }

    displayOverrideLine(0, lineNumber, msg);
}

void displayOverrideLine(uint8_t col, uint8_t lineNumber, const char *msg)
{
    if (!g_ready || msg == nullptr || lineNumber >= g_rows)
        return;

    // Check I2C device is still responding
    Wire.beginTransmission(0x27);
    if (Wire.endTransmission() != 0)
    {
        g_ready = false;
        return;
    }

    // Display message on specified line and pad the rest
    printPadded(col, lineNumber, msg, g_cols);
}

void displaySetColor(const char *colorName)
{
    if (!g_ready || colorName == nullptr)
        return;

    // Check I2C device is still responding
    Wire.beginTransmission(0x27);
    if (Wire.endTransmission() != 0)
    {
        g_ready = false;
        return;
    }

    // Simple backlight control - turn on for colors, off for "off"
    if (strcmp(colorName, "off") == 0 || strcmp(colorName, "OFF") == 0)
    {
        lcd.noBacklight();
    }
    else
    {
        // Turn on backlight for all color names (white, red, green, blue, yellow)
        lcd.backlight();
    }
}
