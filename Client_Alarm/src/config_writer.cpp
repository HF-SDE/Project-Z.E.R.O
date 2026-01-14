#include "../lib/StorageManager.h"
#include <Arduino.h>

const char *getChipId()
{
    static char chipId[17];
    uint64_t mac = ESP.getEfuseMac();
    snprintf(chipId, sizeof(chipId), "%016llX", mac);
    return chipId;
}

/**
 * Write default configuration to device storage
 * Call this function once during initial device setup
 * or when you need to reset to default settings
 */
bool writeDefaultConfig()
{
    // Initialize storage
    if (!storageInit())
    {
        return false;
    }

    // Create default configuration
    DeviceConfig config;
    config.wifiSsid = "Case-ZERO_2,4GHz";
    config.wifiPassword = "Nogetjegkanhuske";
    config.serialFrequency = 115200;
    config.mqttHost = "192.168.1.5";
    config.mqttPort = 1883;
    config.mqttUser = "";
    config.mqttPassword = "";
    config.mqttTopic = "devices/" + String(getChipId()) + "";
    config.deviceId = getChipId();
    config.heartbeatInterval = 1000 * 60; // 1 minute
    config.status = true;                 // Device active by default

    // Save configuration
    if (!storageSaveConfig(config))
    {
        return false;
    }

    return true;
}
