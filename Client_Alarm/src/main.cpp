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
#include <../lib/StatusLedManager.h>
#include <../lib/AlarmOutputManager.h>
#include <../lib/Component.h>
#include <../lib/ComponentManager.h>

// ---------------------- Global Config ----------------------
DeviceConfig config;

// ---------------------- Component Definitions ----------------------
// Define all hardware components at compile time
Component displayComponent("display", "text", "Main LCD Display", "20x2 character display");
Component alarmLedComponent("led", "boolean", "Alarm LED", "Red LED indicating alarm state");
Component buzzerComponent("buzzer", "boolean", "Alarm Buzzer", "Audio alarm indicator");
Component statusRedLedComponent("led", "boolean", "Status Red LED", "WiFi/MQTT error indicator");
Component statusGreenLedComponent("led", "boolean", "Status Green LED", "Connection OK indicator");
Component statusBlueLedComponent("led", "boolean", "Status Blue LED", "WiFi connecting indicator");

//---------------------- Pin config ----------------------
const int blueWifiLedPin = 0;
const int greenWifiLedPin = 4;
const int redWifiLedPin = 2;
const int buzzerPin = 32;
const int alarmLedPin = 26;

//---------------------- LCD config ----------------------
int lcdColumns = 20;
int lcdRows = 4;
int lcdI2CAddress = 0x27;
// vars
//  firstStartUp indicates if it's the first time the device is starting up
bool firstWifiStartUp = true;
bool hasPublishedComponentState = false;

static void onMqttAlarmMessage(const char *topic, const char *payload)
{
  // check if the topic matches this event
  if (strstr(topic, "/triggers/alarm-trigger") == nullptr)
  {
    return;
  }
  // if config status is false, ignore alarm triggers
  if (!config.status)
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
  alarmOutputActivate(alarm.useSound);

  Serial.println("Alarm started");
}
static void onMqttClearAlarmMessage(const char *topic, const char *payload)
{
  // check if the topic matches this event
  if (strstr(topic, "/triggers/clear-alarm-trigger") == nullptr)
  {
    return;
  }
  if (!config.status)
  {
    return;
  }
  Serial.println("Event received");

  displayShowMessage("all good fam.");
  alarmOutputDeactivate();
}
static void onMqttConfigMessage(const char *topic, const char *payload)
{
  // check if the topic matches this event (exact match: clients/DEVICE_ID)
  if (topic == nullptr || strcmp(topic, config.mqttTopic.c_str()) != 0)
  {
    return;
  }
  Serial.print("Config event received on topic: ");
  Serial.println(topic);
  Serial.print("Payload: ");
  Serial.println(payload);

  ConfigMessage configMsg;

  if (!parseConfigMessage(payload, configMsg))
  {
    Serial.println("Invalid config JSON");
    return;
  }

  // write new config
  config.heartbeatInterval = configMsg.heartbeatInterval;
  config.status = configMsg.status; // Update status from MQTT config

  storageSaveConfig(config);

  // Apply the new heartbeat interval immediately
  mqttSetHeartbeat(config.deviceId.c_str(), config.heartbeatInterval);

  Serial.println("Configuration updated via MQTT");
  Serial.print("New heartbeat interval: ");
  Serial.print(config.heartbeatInterval);
  Serial.println(" ms");
  Serial.println(config.status ? "Device active" : "Device inactive");

  if (!config.status)
  {
    alarmOutputDeactivate();
    displayShowMessage("Device inactive.");
  }
  else
  {
    displayShowMessage("Active. No alarm.");
  }
}
static void onMqttMessage(const char *topic, const char *payload)
{
  // Dispatcher: each handler checks if the topic matches
  onMqttAlarmMessage(topic, payload);
  onMqttClearAlarmMessage(topic, payload);
  onMqttConfigMessage(topic, payload);
}

void setup()
{
  Serial.begin(115200); // Start with default baud rate
  while (!Serial)
  {
    ; // Wait for Serial to be ready
  }

  displayInit(lcdI2CAddress, lcdColumns, lcdRows);

  // Initialize status LED and alarm output managers
  statusLedInit(redWifiLedPin, greenWifiLedPin, blueWifiLedPin);
  alarmOutputInit(alarmLedPin, buzzerPin);

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
  displayOverrideLine(1, "Loading config...");
  if (!storageLoadConfig(config))
  {
    Serial.println("Failed to load config! Using defaults and saving...");
    // Blink red LED to indicate config error
    displayOverrideLine(1, "Loading failed!");
    displayOverrideLine(2, "Trying fallback...");

    for (int i = 0; i < 10; i++)
    {
      digitalWrite(redWifiLedPin, HIGH);
      digitalWrite(greenWifiLedPin, HIGH);
      digitalWrite(blueWifiLedPin, LOW);
      delay(200);
      digitalWrite(redWifiLedPin, LOW);
      digitalWrite(greenWifiLedPin, LOW);
      delay(200);
    }

    // Write default config and try again
    if (!writeDefaultConfig() || !storageLoadConfig(config))
    {
      Serial.println("Failed to create default config!");
      displayShowMessage("Config error! Check Serial");

      while (true)
      {
        // blink red and yellow
        digitalWrite(redWifiLedPin, HIGH);
        digitalWrite(greenWifiLedPin, HIGH);
        digitalWrite(blueWifiLedPin, LOW);
        delay(200);
        digitalWrite(redWifiLedPin, LOW);
        digitalWrite(greenWifiLedPin, LOW);
        delay(200);
      }
      return;
    }
  }
  displayOverrideLine(1, "Loading wifi...");
  displayOverrideLine(2, "");

  printStorageConfig(config);

  // If the default serial frequency is different from the device config, reinitialize Serial
  if (config.serialFrequency != 115200)
  {
    Serial.end();
    Serial.begin(config.serialFrequency);
    delay(100);
  }

  // Initialize Component Manager
  componentManagerInit(config.deviceId, config.mqttTopic);

  // Register all components
  componentRegister(&displayComponent);
  componentRegister(&alarmLedComponent);
  componentRegister(&buzzerComponent);
  componentRegister(&statusRedLedComponent);
  componentRegister(&statusGreenLedComponent);
  componentRegister(&statusBlueLedComponent);

  // Set component pointers in managers so they auto-update
  displaySetComponent(&displayComponent);
  alarmOutputSetComponents(&alarmLedComponent, &buzzerComponent);
  statusLedSetComponents(&statusRedLedComponent, &statusGreenLedComponent, &statusBlueLedComponent);

  statusLedUpdate(false, false, firstWifiStartUp);
  firstWifiStartUp = false;

  if (!wifiConnect(config.wifiSsid.c_str(), config.wifiPassword.c_str(), 10000))
  {
    Serial.println("WiFi failed");
    statusLedUpdate(false, false, false);
    displayShowMessage("WiFi failed!");
    return;
  }

  Serial.print("IP: ");
  Serial.println(wifiGetIp());

  statusLedUpdate(true, false, false);
  displayOverrideLine(1, "Connecting mqtt...");

  String mqttTopicWithWildcard = config.mqttTopic + "/#";
  mqttInit(
      config.mqttHost.c_str(),
      config.mqttPort,
      config.mqttUser.isEmpty() ? nullptr : config.mqttUser.c_str(),
      config.mqttPassword.isEmpty() ? nullptr : config.mqttPassword.c_str(),
      mqttTopicWithWildcard.c_str(),
      config.mqttTopic);

  mqttSetHeartbeat(config.deviceId.c_str(), config.heartbeatInterval);

  mqttSetMessageHandler(onMqttMessage); // This will define the handlers that run on specific topics

  if (config.status)
    displayShowMessage("No alarms. All good.");
  else
    displayShowMessage("Device inactive.");
}

void loop()
{
  if (!hasPublishedComponentState)
  {
    componentPublishAll();
    hasPublishedComponentState = true;
  }
  statusLedUpdate(wifiIsConnected(), mqttIsConnected(), false);

  delay(500); // Small delay to avoid busy loop
  mqttLoop();
}
