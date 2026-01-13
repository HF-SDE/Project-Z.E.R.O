#ifndef STORAGE_MANAGER_H
#define STORAGE_MANAGER_H

#include <Arduino.h>

struct DeviceConfig
{
    String wifiSsid;
    String wifiPassword;
    int serialFrequency;
    String mqttHost;
    uint16_t mqttPort;
    String mqttUser;
    String mqttPassword;
    String mqttTopic;
    String deviceId;
    int heartbeatInterval;
    bool status; // Indicates if the device is active
};

/**
 * Initialize the LittleFS filesystem
 * @return true if successful, false otherwise
 */
bool storageInit();

/**
 * Save configuration to persistent storage
 * @param config The configuration object to save
 * @return true if successful, false otherwise
 */
bool storageSaveConfig(const DeviceConfig &config);

/**
 * Load configuration from persistent storage
 * @param config Reference to config object to populate
 * @return true if successful and config exists, false otherwise
 */
bool storageLoadConfig(DeviceConfig &config);

/**
 * Check if configuration file exists
 * @return true if config exists, false otherwise
 */
bool storageConfigExists();

/**
 * Delete configuration file
 * @return true if successful, false otherwise
 */
bool storageDeleteConfig();

/**
 * Format the LittleFS filesystem (erases all data)
 * @return true if successful, false otherwise
 */
bool storageFormat();

/**
 * Print configuration to Serial
 * @param config The configuration object to print
 */
void printStorageConfig(const DeviceConfig &config);

#endif // STORAGE_MANAGER_H
