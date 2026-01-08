#include <Arduino.h>
#include <../lib/wifi_setup.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <PubSubClient.h>
#include <../lib/DisplayManager.h>
#include <../lib/MqttManager.h>
#include <../lib/alarm_message.h>
#include <../lib/ConfigWriter.h>
#include <../lib/StorageManager.h>
#include <../lib/config_message.h>

// ---------------------- Global Config ----------------------
DeviceConfig config;

//---------------------- Pin config ----------------------
const int blueWifiLedPin = 0;
const int greenWifiLedPin = 4;
const int redWifiLedPin = 2;
const int buzzerPin = 32;
const int alarmLedPin = 26;

//---------------------- LCD config ----------------------
int lcdColumns = 20;
int lcdRows = 2;
int lcdI2CAddress = 0x27;
// vars
//  firstStartUp indicates if it's the first time the device is starting up
bool firstWifiStartUp = true;

static void onMqttAlarmMessage(const char *topic, const char *payload)
{
  // check if the topic matches this event
  if (strstr(topic, "/triggers/alarm-trigger") == nullptr)
  {
    return;
  }
  Serial.println("Event received 1");

  AlarmMessage alarm;
  Serial.println(payload);

  if (!parseAlarmMessage(payload, alarm))
  {
    Serial.println("Invalid alarm JSON");
    return;
  }

  displayShowMessage(alarm.message);
  digitalWrite(alarmLedPin, HIGH);
  if (alarm.useSound)
  {
    digitalWrite(buzzerPin, HIGH);
  }

  Serial.println("Alarm started");
}
static void onMqttClearAlarmMessage(const char *topic, const char *payload)
{
  // check if the topic matches this event
  if (strstr(topic, "/triggers/clear-alarm-trigger") == nullptr)
  {
    return;
  }
  Serial.println("Event received");

  displayShowMessage("all good fam.");
  digitalWrite(alarmLedPin, LOW);

  digitalWrite(buzzerPin, LOW);
}
static void onMqttConfigMessage(const char *topic, const char *payload)
{
  // check if the topic matches this event
  if (strstr(topic, "/config") == nullptr)
  {
    return;
  }
  Serial.println("Event received 1");

  ConfigMessage configMsg;
  Serial.println(payload);

  if (!parseConfigMessage(payload, configMsg))
  {
    Serial.println("Invalid alarm JSON");
    return;
  }

  // write new config
  config.heartbeatInterval = configMsg.heartbeatInterval;
  storageSaveConfig(config);

  Serial.println("Alarm started");
}

static void onMqttMessage(const char *topic, const char *payload)
{
  // Dispatcher: route to the appropriate handler based on topic
  onMqttAlarmMessage(topic, payload);
  onMqttClearAlarmMessage(topic, payload);
  onMqttConfigMessage(topic, payload);
}

void setup()
{
  Wire.begin();
  displayInit(lcdI2CAddress, lcdColumns, lcdRows);
  Serial.begin(115200); // Start with default baud rate
  while (!Serial)
  {
    ; // Wait for Serial to be ready
  }

  pinMode(redWifiLedPin, OUTPUT);
  pinMode(blueWifiLedPin, OUTPUT);
  pinMode(greenWifiLedPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(alarmLedPin, OUTPUT);

  wifiInitStatusLed(redWifiLedPin, greenWifiLedPin, blueWifiLedPin);

  // Initialize storage and load configuration
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
    displayShowMessage("Storage error! Check Serial");
    return;
  }

  // Load configuration
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
    if (!writeDefaultConfig() || !storageLoadConfig(config))
    {
      Serial.println("Failed to create default config!");
      displayShowMessage("Config error! Check Serial");
      return;
    }
  }
  storagePrintConfig(config);

  // Update serial baud rate if different from default
  if (config.serialFrequency != 115200)
  {
    Serial.end();
    Serial.begin(config.serialFrequency);
    delay(100);
  }

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

  mqttInit(
      config.mqttHost.c_str(),
      config.mqttPort,
      config.mqttUser.isEmpty() ? nullptr : config.mqttUser.c_str(),
      config.mqttPassword.isEmpty() ? nullptr : config.mqttPassword.c_str(),
      config.mqttTopic.c_str());

  displayShowMessage("all good fam.");
  mqttSetMessageHandler(onMqttMessage);
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
}
