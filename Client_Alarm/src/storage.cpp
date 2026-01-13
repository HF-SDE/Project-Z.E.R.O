#include "../lib/StorageManager.h"
#include <LittleFS.h>
#include <ArduinoJson.h>

static const char *CONFIG_FILE = "/config.json";
static bool g_initialized = false;

bool storageInit()
{
    if (g_initialized)
        return true;

    if (!LittleFS.begin(true)) // true = format on fail
    {
        Serial.println("[Storage] Failed to mount LittleFS");
        return false;
    }

    g_initialized = true;
    Serial.println("[Storage] LittleFS mounted successfully");
    return true;
}

bool storageSaveConfig(const DeviceConfig &config)
{
    if (!g_initialized && !storageInit())
        return false;

    StaticJsonDocument<512> doc;

    doc["wifiSsid"] = config.wifiSsid;
    doc["wifiPassword"] = config.wifiPassword;
    doc["mqttHost"] = config.mqttHost;
    doc["mqttPort"] = config.mqttPort;
    doc["mqttUser"] = config.mqttUser;
    doc["mqttPassword"] = config.mqttPassword;
    doc["mqttTopic"] = config.mqttTopic;
    doc["deviceId"] = config.deviceId;
    doc["heartbeatInterval"] = config.heartbeatInterval;
    doc["serialFrequency"] = config.serialFrequency;
    doc["status"] = config.status; // Save the status field

    Serial.print("[Storage] Saving heartbeatInterval: ");
    Serial.println(config.heartbeatInterval);

    File file = LittleFS.open(CONFIG_FILE, "w");
    if (!file)
    {
        Serial.println("[Storage] Failed to open config file for writing");
        return false;
    }

    if (serializeJson(doc, file) == 0)
    {
        Serial.println("[Storage] Failed to write config to file");
        file.close();
        return false;
    }

    file.close();

    // Debug: Print what was written to file
    Serial.println("[Storage] JSON written:");
    serializeJsonPretty(doc, Serial);
    Serial.println();

    Serial.println("[Storage] Configuration saved successfully");
    return true;
}

bool storageLoadConfig(DeviceConfig &config)
{
    if (!g_initialized && !storageInit())
        return false;

    if (!LittleFS.exists(CONFIG_FILE))
    {
        Serial.println("[Storage] Config file does not exist");
        return false;
    }

    File file = LittleFS.open(CONFIG_FILE, "r");
    if (!file)
    {
        Serial.println("[Storage] Failed to open config file for reading");
        return false;
    }

    StaticJsonDocument<512> doc;
    DeserializationError err = deserializeJson(doc, file);
    file.close();

    if (err)
    {
        Serial.print("[Storage] Failed to parse config file: ");
        Serial.println(err.c_str());
        return false;
    }

    config.wifiSsid = doc["wifiSsid"] | "";
    config.wifiPassword = doc["wifiPassword"] | "";
    config.mqttHost = doc["mqttHost"] | "";
    config.mqttPort = doc["mqttPort"] | 1883;
    config.mqttUser = doc["mqttUser"] | "";
    config.mqttPassword = doc["mqttPassword"] | "";
    config.mqttTopic = doc["mqttTopic"] | "";
    config.deviceId = doc["deviceId"] | "";
    config.heartbeatInterval = doc["heartbeatInterval"] | 60000;
    config.serialFrequency = doc["serialFrequency"] | 115200;
    config.status = doc["status"] | true; // Load the status field, default to true

    Serial.print("[Storage] Loaded heartbeatInterval from JSON: ");
    Serial.println(config.heartbeatInterval);
    Serial.print("[Storage] Raw JSON heartbeatInterval value: ");
    if (doc.containsKey("heartbeatInterval"))
    {
        Serial.println(doc["heartbeatInterval"].as<int>());
    }
    else
    {
        Serial.println("(key not found, using default)");
    }

    Serial.println("[Storage] Configuration loaded successfully");
    return true;
}

bool storageConfigExists()
{
    if (!g_initialized && !storageInit())
        return false;

    return LittleFS.exists(CONFIG_FILE);
}

bool storageDeleteConfig()
{
    if (!g_initialized && !storageInit())
        return false;

    if (!LittleFS.exists(CONFIG_FILE))
    {
        Serial.println("[Storage] Config file does not exist");
        return true; // Not an error
    }

    if (LittleFS.remove(CONFIG_FILE))
    {
        Serial.println("[Storage] Configuration deleted successfully");
        return true;
    }

    Serial.println("[Storage] Failed to delete configuration");
    return false;
}

bool storageFormat()
{
    if (!g_initialized && !storageInit())
        return false;

    Serial.println("[Storage] Formatting filesystem...");
    if (LittleFS.format())
    {
        Serial.println("[Storage] Filesystem formatted successfully");
        return true;
    }

    Serial.println("[Storage] Failed to format filesystem");
    return false;
}

void printStorageConfig(const DeviceConfig &config)
{
    Serial.println("\n========== Device Configuration ==========");
    Serial.println("WiFi SSID:       " + config.wifiSsid);
    Serial.println("WiFi Password:   " + String(config.wifiPassword.isEmpty() ? "(empty)" : "********"));
    Serial.println("Serial Freq:     " + String(config.serialFrequency) + " bps");
    Serial.println("MQTT Host:       " + config.mqttHost);
    Serial.println("MQTT Port:       " + String(config.mqttPort));
    Serial.println("MQTT User:       " + String(config.mqttUser.isEmpty() ? "(empty)" : config.mqttUser));
    Serial.println("MQTT Password:   " + String(config.mqttPassword.isEmpty() ? "(empty)" : "********"));
    Serial.println("MQTT Topic:      " + config.mqttTopic);
    Serial.println("Device ID:       " + config.deviceId);
    Serial.println("Heartbeat Int.:  " + String(config.heartbeatInterval) + " ms");
    Serial.println("Device Status:   " + String(config.status ? "Active" : "Inactive"));
    Serial.println("=========================================\n");
}
