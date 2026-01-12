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
    Serial.println("[ConfigWriter] Writing default configuration...");

    // Initialize storage
    if (!storageInit())
    {
        Serial.println("[ConfigWriter] Failed to initialize storage");
        return false;
    }

    // Create default configuration
    DeviceConfig config;
    config.wifiSsid = "Case-ZERO_2,4GHz";
    config.wifiPassword = "Nogetjegkanhuske";
    config.serialFrequency = 115200;
    config.mqttHost = "192.168.1.143";
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
        Serial.println("[ConfigWriter] Failed to save configuration");
        return false;
    }

    Serial.println("[ConfigWriter] Configuration written successfully");
    Serial.println("[ConfigWriter] WiFi SSID: " + config.wifiSsid);
    Serial.println("[ConfigWriter] MQTT Host: " + config.mqttHost);
    Serial.println("[ConfigWriter] MQTT Port: " + String(config.mqttPort));
    Serial.println("[ConfigWriter] MQTT Topic: " + config.mqttTopic);
    Serial.println("[ConfigWriter] Device ID: " + config.deviceId);
    Serial.println("[ConfigWriter] Heartbeat Interval: " + String(config.heartbeatInterval) + " ms");
    Serial.println("[ConfigWriter] Serial Frequency: " + String(config.serialFrequency) + " bps");
    Serial.println("[ConfigWriter] Device Status: " + String(config.status ? "Active" : "Inactive"));

    return true;
}
