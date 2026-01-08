#include <Arduino.h>
#include <../lib/wifi_setup.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <PubSubClient.h>
#include <../lib/DisplayManager.h>
#include <../lib/MqttManager.h>
#include <../lib/alarm_message.h>

//---------------------- Wifi config ----------------------
const char *WIFI_SSID = "Case-ZERO_2,4GHz";
const char *WIFI_PASSWORD = "Nogetjegkanhuske";
const int serialFrequency = 115200;

//---------------------- MQTT config ----------------------
const char *MQTT_HOST = "192.168.1.143"; // broker IP or DNS
const int MQTT_PORT = 1883;
const char *MQTT_TOPIC = "devices/1/triggers/#";
const char *MQTT_USER = ""; // optional
const char *MQTT_PASS = ""; // optional

WiFiClient espClient;
PubSubClient mqtt(espClient);

//---------------------- Pin config ----------------------
const int blueLedPin = 0;
const int greenLedPin = 4;
const int redLedPin = 2;
const int buzzerPin = 32;

//---------------------- LCD config ----------------------
int lcdColumns = 20;
int lcdRows = 2;
int lcdI2CAddress = 0x27;
// vars
//  firstStartUp indicates if it's the first time the device is starting up
bool firstWifiStartUp = true;

void setup()
{
  Wire.begin();
  displayInit(lcdI2CAddress, lcdColumns, lcdRows);
  Serial.begin(serialFrequency);
  while (!Serial)
  {
    ; // Wait for Serial to be ready
  }
  pinMode(redLedPin, OUTPUT);
  pinMode(blueLedPin, OUTPUT);
  pinMode(greenLedPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);

  Serial.println("Connecting to WiFi...");

  wifiInitStatusLed(redLedPin, greenLedPin, blueLedPin);
  updateWifiStatusLed(firstWifiStartUp);
  firstWifiStartUp = false;

  if (!wifiConnect(WIFI_SSID, WIFI_PASSWORD, 10000))
  {
    Serial.println("WiFi failed");
    updateWifiStatusLed(false);
    return;
  }

  Serial.print("IP: ");
  Serial.println(wifiGetIp());

  updateWifiStatusLed(false);

  mqttInit(
      MQTT_HOST,
      MQTT_PORT,
      nullptr, // user
      nullptr, // pass
      MQTT_TOPIC);
}

static void onMqttAlarmMessage(const char *topic, const char *payload)
{
  Serial.println("Event received");

  if (strcmp(topic, "devices/1/triggers/alarm-trigger") != 0)
    return;

  AlarmMessage alarm;
  Serial.println(payload);

  if (!parseAlarmMessage(payload, alarm))
  {
    Serial.println("Invalid alarm JSON");
    return;
  }

  displayShowMessage(alarm.message);
  // displaySetColor(alarm.color);
  displaySetColor("red");
  if (alarm.useSound)
  {
    digitalWrite(buzzerPin, HIGH);
  }
}
static void onMqttClearAlarmMessage(const char *topic, const char *payload)
{
  Serial.println("Event received");

  if (strcmp(topic, "devices/1/triggers/clear-alarm-trigger") != 0)
    return;

  displayShowMessage("all good fam.");
  // displaySetColor(alarm.color);

  displaySetColor("green");
  digitalWrite(buzzerPin, LOW);
}
void loop()
{
  updateWifiStatusLed(false);
  mqttLoop();
  // displayShowMessage(mqttGetLastMessage());

  delay(500);

  mqttSetMessageHandler(onMqttAlarmMessage);
  mqttSetMessageHandler(onMqttClearAlarmMessage);
}
