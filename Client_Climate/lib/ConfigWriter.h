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

/**
 * Write custom configuration to device storage
 * @param ssid WiFi SSID
 * @param password WiFi password
 * @param mqttHost MQTT broker host/IP
 * @param mqttPort MQTT broker port
 * @param mqttTopic MQTT topic to subscribe to
 * @param qos MQTT Quality of Service level
 * @param deviceId Device ID for identification
 * @param heartbeatInterval Heartbeat interval in milliseconds
 * @param serialFrequency Serial communication frequency in bps
 * @param status Device active status
 * @param frequency Data publishing frequency in milliseconds
 * @param max_value Maximum threshold value
 * @return true if successful, false otherwise
 */
bool writeCustomConfig(
    const String &ssid,
    const String &password,
    const String &mqttHost,
    uint16_t mqttPort,
    const String &mqttTopic,
    const String &deviceId,
    int heartbeatInterval,
    int serialFrequency,
    bool status,
    int frequency,
    float max_value);

/**
 * Display current configuration from storage
 */
void displayStoredConfig();

#endif // CONFIG_WRITER_H
