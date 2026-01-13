// Denne device er inspirieret af følgende tutorial https://randomnerdtutorials.com/esp8266-nodemcu-mqtt-publish-dht11-dht22-arduino/

#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <DHT.h>
#include <Ticker.h>
#include "../lib/StorageManager.h"
#include "../lib/ConfigWriter.h"
#include "../lib/wifi.h"
#include "../lib/mqtt.h"
// #include <Environment.h>

// ESP8266: Avoid GPIO0/GPIO2/GPIO15 for peripherals (boot strap pins). Use GPIO4 (D2) instead.
#define DHTPIN 2
#define DHTTYPE DHT22 // DHT 22 (AM2302)

#define RED_LED_PIN 16
#define YELLOW_LED_PIN 14
#define BLUE_LED_PIN 12

DHT dht(DHTPIN, DHTTYPE);

float temp = 0.0;
float hum = 0.0;

DeviceConfig g_deviceConfig;

unsigned long previousMillis = 0;

static unsigned long g_lastWifiAttemptMs = 0;
static unsigned long g_lastMqttAttemptMs = 0;
static unsigned long g_lastBlueBlinkMs = 0;
static unsigned long g_lastYellowBlinkMs = 0;
static bool g_blueLedOn = false;
static bool g_yellowLedOn = false;

static void blinkLed(uint8_t pin, unsigned long &lastBlinkMs, bool &ledOn, unsigned long intervalMs)
{
  const unsigned long now = millis();
  if (now - lastBlinkMs >= intervalMs)
  {
    lastBlinkMs = now;
    ledOn = !ledOn;
    digitalWrite(pin, ledOn ? HIGH : LOW);
  }
}

char *getConfigTopic()
{
  String topic = g_deviceConfig.mqttTopic + "/config";
  return strdup(topic.c_str());
}

void onMqttMessage(char *topic, char *payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total)
{
  Serial.println("MQTT Message arrived on topic: " + String(topic));
  Serial.print("Message: ");
  for (size_t i = 0; i < len; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  String config_topic = String(g_deviceConfig.mqttTopic + "/config");

  if (String(topic) == config_topic)
  {
    Serial.println("Incomming configuration message, saving to config...");

    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, payload, len);

    if (err)
    {
      Serial.print("JSON parse failed: ");
      Serial.println(err.c_str());
      return;
    }

    unsigned long frequency = doc["frequency"].as<unsigned long>();

    g_deviceConfig.frequency = frequency;

    storageSaveConfig(g_deviceConfig);
  }
}

void setup()
{
  Serial.begin(115200);

  // Environment::configureEnvironment(Environment::EnvironmentMode::DEVELOPMENT, "prod.ip", "test.ip", "dev.ip");

  storageInit();
  if (!storageLoadConfig(g_deviceConfig))
  {
    writeDefaultConfig();
  }

  Serial.println("Loaded configuration:");
  storagePrintConfig(g_deviceConfig);

  dht.begin();

  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(YELLOW_LED_PIN, OUTPUT);
  pinMode(BLUE_LED_PIN, OUTPUT);

  wifiConnectHandler = WiFi.onStationModeGotIP(onWifiConnect);
  wifiDisconnectHandler = WiFi.onStationModeDisconnected(onWifiDisconnect);

  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
  mqttClient.onSubscribe(onMqttSubscribe);
  mqttClient.onUnsubscribe(onMqttUnsubscribe);
  mqttClient.onPublish(onMqttPublish);
  mqttClient.onMessage(onMqttMessage);

  const char *mqttHost = g_deviceConfig.mqttHost.c_str();
  uint16_t mqttPort = g_deviceConfig.mqttPort;

  mqttClient.setServer(mqttHost, mqttPort);
  // If your broker requires authentication (username and password), set them below
  // mqttClient.setCredentials("REPlACE_WITH_YOUR_USER", "REPLACE_WITH_YOUR_PASSWORD");

  connectToWifi(g_deviceConfig.wifiSsid, g_deviceConfig.wifiPassword);

  Serial.println("Setup complete; entering loop");
}

void loop()
{
  yield();

  const unsigned long now = millis();
  const unsigned long publishIntervalMs = ((unsigned long)g_deviceConfig.frequency < 1000UL) ? 1000UL : (unsigned long)g_deviceConfig.frequency;

  if (!WiFi.isConnected())
  {
    blinkLed(BLUE_LED_PIN, g_lastBlueBlinkMs, g_blueLedOn, 250); // Blink BLUE while connecting to WiFi

    digitalWrite(YELLOW_LED_PIN, LOW);
    g_yellowLedOn = false;

    // Retry WiFi connect every 10 seconds
    if (now - g_lastWifiAttemptMs >= 10000UL)
    {
      g_lastWifiAttemptMs = now;
      connectToWifi(g_deviceConfig.wifiSsid, g_deviceConfig.wifiPassword);
    }

    return;
  }

  digitalWrite(BLUE_LED_PIN, HIGH);
  g_blueLedOn = true;

  if (!mqttClient.connected())
  {

    blinkLed(YELLOW_LED_PIN, g_lastYellowBlinkMs, g_yellowLedOn, 250); // Blink YELLOW while connecting to MQTT

    if (now - g_lastMqttAttemptMs >= 10000UL)
    {
      g_lastMqttAttemptMs = now;
      connectToMqtt();
    }

    return;
  }

  digitalWrite(YELLOW_LED_PIN, HIGH);
  g_yellowLedOn = true;

  if (now - previousMillis < publishIntervalMs)
    return;

  previousMillis = now;

  hum = dht.readHumidity();
  temp = dht.readTemperature();

  if (isnan(hum) || isnan(temp))
  {
    digitalWrite(RED_LED_PIN, HIGH);
    return;
  }

  digitalWrite(RED_LED_PIN, LOW);

  const String tempTopic = g_deviceConfig.mqttTopic + "/temperature/value";
  const String humTopic = g_deviceConfig.mqttTopic + "/humidity/value";

  mqttClient.publish(tempTopic.c_str(), g_deviceConfig.qos, true, String(temp).c_str());
  mqttClient.publish(humTopic.c_str(), g_deviceConfig.qos, true, String(hum).c_str());
}