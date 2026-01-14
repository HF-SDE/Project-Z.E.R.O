#include <Arduino.h>
#include "../lib/StorageManager.h"

const char *getChipId()
{
    static char chipId[9];
    uint32_t id = ESP.getChipId();
    snprintf(chipId, sizeof(chipId), "%08X", id);
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
    config.mqttHost = "192.168.1.5";
    config.mqttPort = 1883;
    config.mqttUser = "";
    config.mqttPassword = "";
    config.mqttTopic = "clients/" + String(getChipId());
    config.qos = 0;
    config.deviceId = getChipId();
    config.heartbeatInterval = 1000 * 60; // 1 minute
    config.status = true;                 // Default to active
    config.frequency = 10 * 1000;         // 10 seconds
    config.max_value = 25.0;              // Default maximum threshold value

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
    Serial.println("[ConfigWriter] Client ID: " + config.deviceId);
    Serial.println("[ConfigWriter] Heartbeat Interval: " + String(config.heartbeatInterval) + " ms");
    Serial.println("[ConfigWriter] Serial Frequency: " + String(config.serialFrequency) + " bps");
    Serial.println("[ConfigWriter] Status: " + String(config.status ? "Active" : "Inactive"));
    Serial.println("[ConfigWriter] Frequency: " + String(config.frequency) + " ms");
    Serial.println("[ConfigWriter] Max Value: " + String(config.max_value));

    return true;
}

/**
 * Write custom configuration to device storage
 * @param ssid WiFi SSID
 * @param password WiFi password
 * @param mqttHost MQTT broker host/IP
 * @param mqttPort MQTT broker port
 * @param mqttTopic MQTT topic to subscribe to
 * @param deviceId Device ID for identification
 * @param heartbeatInterval Heartbeat interval in milliseconds
 * @param serialFrequency Serial communication frequency in bps
 * @param status Device active status
 * @param frequency Data publishing frequency in milliseconds
 * @param max_value Maximum threshold value
 *
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
    float max_value)

{
    Serial.println("[ConfigWriter] Writing custom configuration...");

    if (!storageInit())
    {
        Serial.println("[ConfigWriter] Failed to initialize storage");
        return false;
    }

    DeviceConfig config;
    config.wifiSsid = ssid;
    config.wifiPassword = password;
    config.mqttHost = mqttHost;
    config.mqttPort = mqttPort;
    config.mqttUser = "";
    config.mqttPassword = "";
    config.mqttTopic = mqttTopic;
    config.deviceId = deviceId;
    config.heartbeatInterval = heartbeatInterval;
    config.serialFrequency = serialFrequency;
    config.status = status;
    config.frequency = 10000;
    config.max_value = max_value;

    if (!storageSaveConfig(config))
    {
        Serial.println("[ConfigWriter] Failed to save configuration");
        return false;
    }

    Serial.println("[ConfigWriter] Configuration written successfully");
    return true;
}

/**
 * Display current configuration from storage
 */
void displayStoredConfig()
{
    Serial.println("[ConfigWriter] Reading stored configuration...");

    if (!storageInit())
    {
        Serial.println("[ConfigWriter] Failed to initialize storage");
        return;
    }

    if (!storageConfigExists())
    {
        Serial.println("[ConfigWriter] No configuration found in storage");
        return;
    }

    DeviceConfig config;
    if (!storageLoadConfig(config))
    {
        Serial.println("[ConfigWriter] Failed to load configuration");
        return;
    }

    Serial.println("\n========== Stored Configuration ==========");
    Serial.println("WiFi SSID:      " + config.wifiSsid);
    Serial.println("WiFi Password:  " + String(config.wifiPassword.isEmpty() ? "(empty)" : "********"));
    Serial.println("MQTT Host:      " + config.mqttHost);
    Serial.println("MQTT Port:      " + String(config.mqttPort));
    Serial.println("MQTT User:      " + String(config.mqttUser.isEmpty() ? "(empty)" : config.mqttUser.c_str()));
    Serial.println("MQTT Password:  " + String(config.mqttPassword.isEmpty() ? "(empty)" : "********"));
    Serial.println("MQTT Topic:     " + config.mqttTopic);
    Serial.println("Client ID:      " + config.deviceId);
    Serial.println("Heartbeat Int.: " + String(config.heartbeatInterval) + " ms");
    Serial.println("Serial Freq.:   " + String(config.serialFrequency) + " bps");
    Serial.println("Status:         " + String(config.status ? "Active" : "Inactive"));
    Serial.println("Frequency:      " + String(config.frequency) + " ms");
    Serial.println("Max Value:      " + String(config.max_value));
    Serial.println("==========================================\n");
}
