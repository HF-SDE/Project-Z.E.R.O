#include <Arduino.h>
#include <StorageManager.h>
#include <ConfigWriter.h>
#include <wifi_setup.h>
#include <MqttManager.h>
#include <Led.h>
#include <LightSensor.h>
#include <Button.h>
#include <Environment.h>

// Pins (adjust to your board)
constexpr uint8_t LED_PIN = 2;    // onboard LED on many ESP32 boards
constexpr uint8_t BUTTON_PIN = 0; // user button
constexpr uint8_t LIGHT_PIN = 34; // ADC pin

// State
bool g_ledState = false;

void onMqttMessage(const char *topic, const char *payload)
{
    // Example handler: toggle LED when receiving "TOGGLE" on a command topic
    if (strcmp(payload, "TOGGLE") == 0)
    {
        g_ledState = !g_ledState;
        Led::set(g_ledState, String("clients/") + getChipId() + "/led1");
        Environment::print(String("MQTT: toggled LED -> ") + String(g_ledState));
    }
}

void setup()
{
    Serial.begin(115200);
    delay(50);

    // Configure environment for helpful logging in examples
    Environment::configureEnvironment(Environment::EnvironmentMode::DEVELOPMENT, "", "", "");

    // Storage & config
    storageInit();
    DeviceConfig cfg;
    if (!storageLoadConfig(cfg))
    {
        // Write a default config once (replace with real values)
        writeDefaultConfig("broker.hivemq.com", "YOUR_WIFI_SSID", "YOUR_WIFI_PASS");
        storageLoadConfig(cfg);
    }

    // Reinitialize Serial using configured serial frequency (if present)
    // Note: calling Serial.begin() again updates the baud rate at runtime.
    int baud = cfg.serialFrequency;
    // Validate baud rate and fall back to a safe default if out of range
    if (baud < 300 || baud > 2000000)
    {
        baud = 115200;
        Environment::print(String("Invalid serial frequency in config; falling back to ") + String(baud) + " bps");
    }
    Serial.begin(baud);
    Environment::print(String("Serial frequency set to ") + String(baud) + " bps");

    // WiFi
    Environment::print("Connecting to WiFi...");
    if (!wifiConnect(cfg.wifiSsid.c_str(), cfg.wifiPassword.c_str(), 15000))
    {
        Environment::print("WiFi connect failed");
    }
    else
    {
        Environment::print(String("WiFi IP: ") + wifiGetIp());
    }

    // MQTT
    mqttSetMessageHandler(onMqttMessage);
    mqttInit(cfg.mqttHost.c_str(), cfg.mqttPort, cfg.mqttUser.c_str(), cfg.mqttPassword.c_str(), cfg.mqttTopic.c_str());

    // Components
    Led::begin(LED_PIN, String("clients/") + cfg.deviceId + "/led1");
    LightSensor::begin(LIGHT_PIN);
    Button::begin(BUTTON_PIN, 50, Button::Edge::Falling);

    Environment::print("Setup complete");
}

unsigned long lastLightMs = 0;

void loop()
{
    mqttLoop();

    // Read sensor every 5s and publish retained value
    if (millis() - lastLightMs >= 5000)
    {
        int lux = LightSensor::read(String("clients/") + getChipId() + "/light1");
        Environment::print(String("Light: ") + String(lux));
        lastLightMs = millis();
    }

    // Button press toggles LED
    if (Button::consumePress())
    {
        g_ledState = !g_ledState;
        Led::set(g_ledState, String("clients/") + getChipId() + "/led1");
        Environment::print(String("Button pressed, LED -> ") + String(g_ledState));
    }

    delay(10);
}
