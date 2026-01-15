#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <Arduino.h>
#include "Component.h"

// Initialize LCD (address + size + I2C pins optional)
bool displayInit(uint8_t i2cAddress, uint8_t cols, uint8_t rows, int sdaPin = 21, int sclPin = 22);

// Set the component for automatic state tracking
void displaySetComponent(Component *component);

// Show a message (auto-splits across lines)
void displayShowMessage(const char *msg);

// Display message on specific line (0-indexed), clearing rest of line
// Overload 1: Display at column 0 (default)
void displayOverrideLine(uint8_t lineNumber, const char *msg);
// Overload 2: Display at specific column
void displayOverrideLine(uint8_t col, uint8_t lineNumber, const char *msg);

// Optional helpers
void displayClear();

void displaySetColor(const char *colorName);

#endif // DISPLAY_MANAGER_H
