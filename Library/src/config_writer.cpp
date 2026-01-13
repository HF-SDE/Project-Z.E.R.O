#include <StorageManager.h>
#include <Arduino.h>
#include <Environment.h>

const char *getChipId()
{
    static char chipId[17];
    uint64_t mac = ESP.getEfuseMac();
    snprintf(chipId, sizeof(chipId), "%016llX", mac);
    return chipId;
}

const char *getChipIdEsp8266()
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
[[nodiscard]] bool writeDefaultConfig(const String &mqttHost, const String &wifiSsid, const String &wifiPassword)
{
    Environment::print("[ConfigWriter] Writing default configuration...");

    // Initialize storage
    if (!storageInit())
    {
        Environment::print("[ConfigWriter] Failed to initialize storage");
        return false;
    }

    // Create default configuration
    DeviceConfig config;
    config.wifiSsid = wifiSsid;
    config.wifiPassword = wifiPassword;
    config.serialFrequency = 115200;
    config.mqttHost = mqttHost;
    config.mqttPort = 1883;
    config.mqttUser = "";
    config.mqttPassword = "";
    config.mqttTopic = "devices/" + String(getChipId()) + "/#";
    config.deviceId = getChipId();
    config.heartbeatInterval = 1000 * 60; // 1 minute

    // Save configuration
    if (!storageSaveConfig(config))
    {
        Environment::print("[ConfigWriter] Failed to save configuration");
        return false;
    }

    Environment::print("[ConfigWriter] Configuration written successfully");
    Environment::print("[ConfigWriter] WiFi SSID: " + config.wifiSsid);
    Environment::print("[ConfigWriter] MQTT Host: " + config.mqttHost);
    Environment::print("[ConfigWriter] MQTT Port: " + String(config.mqttPort));
    Environment::print("[ConfigWriter] MQTT Topic: " + config.mqttTopic);
    Environment::print("[ConfigWriter] Device ID: " + config.deviceId);
    Environment::print("[ConfigWriter] Heartbeat Interval: " + String(config.heartbeatInterval) + " ms");
    Environment::print("[ConfigWriter] Serial Frequency: " + String(config.serialFrequency) + " bps");

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
    int serialFrequency)

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
void displayStoredConfig1()
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
    Serial.println("Device ID:      " + config.deviceId);
    Serial.println("Heartbeat Int.: " + String(config.heartbeatInterval) + " ms");
    Serial.println("Serial Freq.:   " + String(config.serialFrequency) + " bps");
    Serial.println("==========================================\n");
}
