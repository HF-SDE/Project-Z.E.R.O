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
        return false;

    g_initialized = true;

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

    File file = LittleFS.open(CONFIG_FILE, "w");
    if (!file)
        return false;

    if (serializeJson(doc, file) == 0)
    {
        file.close();
        return false;
    }

    file.close();

    serializeJsonPretty(doc, Serial);

    return true;
}

bool storageLoadConfig(DeviceConfig &config)
{
    if (!g_initialized && !storageInit())
        return false;

    if (!LittleFS.exists(CONFIG_FILE))
    {
        return false;
    }

    File file = LittleFS.open(CONFIG_FILE, "r");
    if (!file)
        return false;

    StaticJsonDocument<512> doc;
    DeserializationError err = deserializeJson(doc, file);
    file.close();

    if (err)
        return false;

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
        return true; // Not an error

    if (LittleFS.remove(CONFIG_FILE))
        return true;

    return false;
}

bool storageFormat()
{
    if (!g_initialized && !storageInit())
        return false;

    if (LittleFS.format())
        return true;

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
