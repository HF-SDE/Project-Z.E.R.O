#ifndef CONFIG_WRITER_H
#define CONFIG_WRITER_H

#include <Arduino.h>

/**
 * Get unique chip ID from ESP32 MAC address
 * @return Hex string representation of chip ID
 */
const char *getChipId();

/**
 * Write default configuration to device storage
 * Call this function once during initial device setup
 * or when you need to reset to default settings
 * @return true if successful, false otherwise
 */
bool writeDefaultConfig();

#endif // CONFIG_WRITER_H
