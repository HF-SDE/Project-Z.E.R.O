#include <Arduino.h>
#include "Button.h"
#include <vector>
#include "LightSensor.h"
#include "Led.h"
#include <config.h>
#include <../lib/wifi_setup.h>
#include <Wire.h>
#include <PubSubClient.h>
#include <../lib/MqttManager.h>
#include <../lib/ConfigWriter.h>
#include <../lib/StorageManager.h>

// ---------------------- Global Config ----------------------
DeviceConfig config;

//---------------------- Pin config ----------------------
constexpr uint8_t blueWifiLedPin = 27;
constexpr uint8_t greenWifiLedPin = 26;
constexpr uint8_t redWifiLedPin = 25;
constexpr uint8_t LIGHT_SENSOR_PIN = 33;
constexpr uint8_t BUTTON_PIN = 14;
constexpr uint8_t LED_PIN = 32;

// Config component
const String components[4] = {"PhotoResistor", "button", "RGB_Light", "White_LED"};

std::vector<ButtonConfig::keys> component_config = {
    {"PhotoResistor", 0, "light", {"Light Sensor"}},
    {"Button", 0, "boolean", {"Button State"}},
    {"RGB_Light", 0, "rgb_light_state", {"RGB Light State"}},
    {"White_LED", 0, "boolean", {"White LED State"}}};

// MQTT
const String MQTT_HOST = "192.168.1.147";

// Hysteresis thresholds
constexpr int THRESH_ON = 3100;
constexpr int THRESH_OFF = 3300;

// Debounce
constexpr uint32_t DEBOUNCE_MS = 50;

// App state
static bool overwrite = false;
static bool ledState = false;

bool firstWifiStartUp = true;

static void onMqttMessage(const char *topic, const char *payload)
{
  // Handle incoming MQTT messages here

  Serial.print("MQTT Message received on topic: ");
  Serial.println(topic);
  Serial.print("Payload: ");
  Serial.println(payload);
}

static void wifiSetup()
{
  updateWifiStatusLed(firstWifiStartUp);
  firstWifiStartUp = false;

  if (!wifiConnect(config.wifiSsid.c_str(), config.wifiPassword.c_str(), 10000))
  {
    Serial.println("WiFi failed");
    updateWifiStatusLed(false);
    return;
  }

  Serial.print("IP: ");
  Serial.println(wifiGetIp());

  updateWifiStatusLed(false);
}

static void initConfig()
{
  Serial.println("Initializing storage...");
  if (!storageInit())
  {
    Serial.println("Failed to initialize storage!");
    // Blink red LED to indicate error
    for (int i = 0; i < 10; i++)
    {
      digitalWrite(redWifiLedPin, HIGH);
      digitalWrite(greenWifiLedPin, LOW);
      digitalWrite(blueWifiLedPin, LOW);
      delay(200);
      digitalWrite(redWifiLedPin, LOW);
      delay(200);
    }
    return;
  }

  Serial.println("Loading configuration...");
  if (!storageLoadConfig(config))
  {
    Serial.println("Failed to load config! Using defaults and saving...");
    // Blink red LED to indicate config error
    for (int i = 0; i < 10; i++)
    {
      digitalWrite(redWifiLedPin, HIGH);
      digitalWrite(greenWifiLedPin, LOW);
      digitalWrite(blueWifiLedPin, LOW);
      delay(200);
      digitalWrite(redWifiLedPin, LOW);
      delay(200);
    }

    // Write default config and try again
    if (!writeDefaultConfig(MQTT_HOST) || !storageLoadConfig(config))
    {
      Serial.println("Failed to create default config!");
      return;
    }
  }
  storagePrintConfig(config);
}

static bool computeLedState(int lightValue)
{
  if (overwrite)
  {
    ledState = true;
    return ledState;
  }

  if (!ledState && lightValue < THRESH_ON)
  {
    ledState = true;
  }
  else if (ledState && lightValue > THRESH_OFF)
  {
    ledState = false;
  }

  return ledState;
}

static void initComponentConfig()
{
  String basicTopic = "devices/" + config.deviceId + "/";

  for (const auto &p : component_config)
  {
    String componentTopic = basicTopic + String(p.id.c_str());
    String payload = "{\"name\":\"" + String(p.config.name.c_str()) + "\"}";

    bool value_typeExists = retainedMessageExists(String(componentTopic + "/value_type").c_str());
    if (value_typeExists)
    {
      Serial.println(componentTopic + ": Retained message exists");
    }
    else
    {
      Serial.println(componentTopic + ": No retained message on topic");
      mqttPublish(String(componentTopic + "/value_type").c_str(), p.value_type.c_str(), true);
    }

    bool valueExists = retainedMessageExists(String(componentTopic + "/value").c_str());
    if (valueExists)
    {
      Serial.println(componentTopic + ": Retained message exists");
    }
    else
    {
      Serial.println(componentTopic + ": No retained message on topic");
      mqttPublish(String(componentTopic + "/value").c_str(), "1", true);
    }

    bool configExists = retainedMessageExists(String(componentTopic + "/config").c_str());
    if (configExists)
    {
      Serial.println(componentTopic + ": Retained message exists");
    }
    else
    {
      Serial.println(componentTopic + ": No retained message on topic");
      mqttPublish(String(componentTopic + "/config").c_str(), payload.c_str(), true);
    }
  }
}

void setup()
{
  Serial.begin(115200);
  while (!Serial)
  {
    ; // Wait for Serial to be ready
  }

  pinMode(redWifiLedPin, OUTPUT);
  pinMode(blueWifiLedPin, OUTPUT);
  pinMode(greenWifiLedPin, OUTPUT);

  Led::begin(LED_PIN, "devices/" + String(config.deviceId.c_str()) + "/White_LED");
  LightSensor::begin(LIGHT_SENSOR_PIN);

  // For INPUT_PULLUP wiring (button -> GND), press is FALLING.
  Button::begin(BUTTON_PIN, DEBOUNCE_MS, Button::Edge::Falling);

  wifiInitStatusLed(redWifiLedPin, greenWifiLedPin, blueWifiLedPin);

  initConfig();

  wifiSetup();

  mqttInit(
      config.mqttHost.c_str(),
      config.mqttPort,
      config.mqttUser.isEmpty() ? nullptr : config.mqttUser.c_str(),
      config.mqttPassword.isEmpty() ? nullptr : config.mqttPassword.c_str(),
      config.mqttTopic.c_str());

  mqttSetMessageHandler(onMqttMessage);
  initComponentConfig();
}

void loop()
{
  static unsigned long lastHeartbeat = 0;

  updateWifiStatusLed(false);
  mqttLoop();

  unsigned long currentMillis = millis();
  if (currentMillis - lastHeartbeat >= config.heartbeatInterval)
  {
    String statusTopic = "devices/" + config.deviceId + "/status";
    mqttPublish(statusTopic.c_str(), "online", true);
    lastHeartbeat = currentMillis;
  }

  if (Button::consumePress())
  {
    overwrite = !overwrite;
  }

  const int lightValue = LightSensor::read("devices/" + String(config.deviceId.c_str()) + "/PhotoResistor");
  Led::set(computeLedState(lightValue), "devices/" + String(config.deviceId.c_str()) + "/White_LED");

  delay(5);
}