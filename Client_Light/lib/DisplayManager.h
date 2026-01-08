#pragma once
#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <Arduino.h>

// Initialize LCD (address + size + I2C pins optional)
bool displayInit(uint8_t i2cAddress, uint8_t cols, uint8_t rows, int sdaPin = 21, int sclPin = 22);

// Show a message (auto-splits across lines)
void displayShowMessage(const char *msg);

// Optional helpers
void displayClear();

void displaySetColor(const char *colorName);

#endif // DISPLAY_MANAGER_H
