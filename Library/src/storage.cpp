#include "StorageManager.h"
#include <ArduinoJson.h>
#include <Environment.h>

#ifndef UNIT_TEST
#include <LittleFS.h>
static const char *CONFIG_FILE = "/config.json";
static bool g_initialized = false;

bool storageInit()
{
    if (g_initialized)
        return true;
#if defined(ESP32)
    if (!LittleFS.begin(true)) // format on fail
    {
        Environment::print("[Storage] Failed to mount LittleFS");
        return false;
    }
#elif defined(ESP8266)
    if (!LittleFS.begin())
    {
        Environment::print("[Storage] LittleFS mount failed; formatting...");
        LittleFS.format();
        if (!LittleFS.begin())
        {
            Environment::print("[Storage] Failed to mount LittleFS after format");
            return false;
        }
    }
#else
    Environment::print("[Storage] Unsupported platform");
    return false;
#endif

    g_initialized = true;
    Environment::print("[Storage] LittleFS mounted successfully");
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

    File file = LittleFS.open(CONFIG_FILE, "w");
    if (!file)
    {
        Environment::print("[Storage] Failed to open config file for writing");
        return false;
    }

    if (serializeJson(doc, file) == 0)
    {
        Environment::print("[Storage] Failed to write config to file");
        file.close();
        return false;
    }

    file.close();
    Environment::print("[Storage] Configuration saved successfully");
    return true;
}

bool storageLoadConfig(DeviceConfig &config)
{
    if (!g_initialized && !storageInit())
        return false;

    if (!LittleFS.exists(CONFIG_FILE))
    {
        Environment::print("[Storage] Config file does not exist");
        return false;
    }

    File file = LittleFS.open(CONFIG_FILE, "r");
    if (!file)
    {
        Environment::print("[Storage] Failed to open config file for reading");
        return false;
    }

    StaticJsonDocument<512> doc;
    DeserializationError err = deserializeJson(doc, file);
    file.close();

    if (err)
    {
        Environment::print("[Storage] Failed to parse config file: ");
        Environment::print(err.c_str());
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

    Environment::print("[Storage] Configuration loaded successfully");
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
        Environment::print("[Storage] Config file does not exist");
        return true; // Not an error
    }

    if (LittleFS.remove(CONFIG_FILE))
    {
        Environment::print("[Storage] Configuration deleted successfully");
        return true;
    }

    Environment::print("[Storage] Failed to delete configuration");
    return false;
}

bool storageFormat()
{
    if (!g_initialized && !storageInit())
        return false;

    Environment::print("[Storage] Formatting filesystem...");
    if (LittleFS.format())
    {
        Environment::print("[Storage] Filesystem formatted successfully");
        return true;
    }

    Environment::print("[Storage] Failed to format filesystem");
    return false;
}

void storagePrintConfig(const DeviceConfig &config)
{
    Environment::print("\n========== Device Configuration ==========");
    Environment::print("WiFi SSID:       " + config.wifiSsid);
    Environment::print("WiFi Password:   " + String(config.wifiPassword.isEmpty() ? "(empty)" : "********"));
    Environment::print("Serial Freq:     " + String(config.serialFrequency) + " bps");
    Environment::print("MQTT Host:       " + config.mqttHost);
    Environment::print("MQTT Port:       " + String(config.mqttPort));
    Environment::print("MQTT User:       " + String(config.mqttUser.isEmpty() ? "(empty)" : config.mqttUser));
    Environment::print("MQTT Password:   " + String(config.mqttPassword.isEmpty() ? "(empty)" : "********"));
    Environment::print("MQTT Topic:      " + config.mqttTopic);
    Environment::print("Device ID:       " + config.deviceId);
    Environment::print("Heartbeat Int.:  " + String(config.heartbeatInterval) + " ms");
    Environment::print("=========================================\n");
}
#else // UNIT_TEST - host-friendly implementation using std::filesystem and <fstream>

#include <fstream>
#include <filesystem>

static const char *CONFIG_FILE = "config.json"; // local test file

bool storageInit()
{
    // Nothing to mount on host - ensure test directory is writable
    Environment::print("[Storage][UT] storageInit (host)");
    return true;
}

bool storageSaveConfig(const DeviceConfig &config)
{
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

    std::string output;
    if (serializeJson(doc, output) == 0)
    {
        Environment::print("[Storage][UT] Failed to serialize JSON");
        return false;
    }

    std::ofstream ofs(CONFIG_FILE, std::ofstream::trunc);
    if (!ofs)
    {
        Environment::print("[Storage][UT] Failed to open config file for writing");
        return false;
    }

    ofs << output;
    ofs.close();
    Environment::print("[Storage][UT] Configuration saved (host)");
    return true;
}

bool storageLoadConfig(DeviceConfig &config)
{
    if (!std::filesystem::exists(CONFIG_FILE))
    {
        Environment::print("[Storage][UT] Config file does not exist");
        return false;
    }

    std::ifstream ifs(CONFIG_FILE);
    if (!ifs)
    {
        Environment::print("[Storage][UT] Failed to open config file for reading");
        return false;
    }

    std::string content((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
    ifs.close();

    StaticJsonDocument<512> doc;
    DeserializationError err = deserializeJson(doc, content);

    if (err)
    {
        Environment::print(String("[Storage][UT] Failed to parse config file: ") + String(err.c_str()));
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

    Environment::print("[Storage][UT] Configuration loaded (host)");
    return true;
}

bool storageConfigExists()
{
    return std::filesystem::exists(CONFIG_FILE);
}

bool storageDeleteConfig()
{
    if (!std::filesystem::exists(CONFIG_FILE))
    {
        Environment::print("[Storage][UT] Config file does not exist");
        return true; // Not an error
    }

    try
    {
        std::filesystem::remove(CONFIG_FILE);
        Environment::print("[Storage][UT] Configuration deleted (host)");
        return true;
    }
    catch (...)
    {
        Environment::print("[Storage][UT] Failed to delete configuration");
        return false;
    }
}

bool storageFormat()
{
    // For tests, formatting simply removes the config file
    if (std::filesystem::exists(CONFIG_FILE))
    {
        std::filesystem::remove(CONFIG_FILE);
    }
    Environment::print("[Storage][UT] Filesystem formatted (host)");
    return true;
}

void storagePrintConfig(const DeviceConfig &config)
{
    Environment::print("\n========== Device Configuration (host) ==========");
    Environment::print("WiFi SSID:       " + config.wifiSsid);
    Environment::print("WiFi Password:   " + String((config.wifiPassword.size() == 0) ? "(empty)" : "********"));
    Environment::print("Serial Freq:     " + String(config.serialFrequency) + " bps");
    Environment::print("MQTT Host:       " + config.mqttHost);
    Environment::print("MQTT Port:       " + String(config.mqttPort));
    Environment::print("MQTT User:       " + String((config.mqttUser.size() == 0) ? "(empty)" : config.mqttUser));
    Environment::print("MQTT Password:   " + String((config.mqttPassword.size() == 0) ? "(empty)" : "********"));
    Environment::print("MQTT Topic:      " + config.mqttTopic);
    Environment::print("Device ID:       " + config.deviceId);
    Environment::print("Heartbeat Int.:  " + String(config.heartbeatInterval) + " ms");
    Environment::print("=========================================");
}
#endif
