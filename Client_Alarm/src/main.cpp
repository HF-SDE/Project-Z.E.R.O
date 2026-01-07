#include <Arduino.h>
#include <../lib/wifiSetup.h>
#include <../lib/SerialManager.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <PubSubClient.h>
#include <../lib/DisplayManager.h>
#include <../lib/MqttManager.h>

//---------------------- Wifi config ----------------------
const char *ssid = "Case-ZERO_2,4GHz";
const char *password = "Nogetjegkanhuske";
const int serialFrequency = 115200;

//---------------------- MQTT config ----------------------
const char *MQTT_HOST = "192.168.1.50"; // broker IP or DNS
const int MQTT_PORT = 1883;
const char *MQTT_TOPIC = "dpit/lcd";
const char *MQTT_USER = ""; // optional
const char *MQTT_PASS = ""; // optional

WiFiClient espClient;
PubSubClient mqtt(espClient);

//---------------------- Pin config ----------------------
const int blueLedPin = 0;
const int greenLedPin = 4;
const int redLedPin = 2;

//---------------------- LCD config ----------------------
int lcdColumns = 20;
int lcdRows = 2;

// vars
//  firstStartUp indicates if it's the first time the device is starting up
bool firstWifiStartUp = true;

void setup()
{
  Wire.begin();
  setupLcd(lcdColumns, lcdRows);
  setupSerial(serialFrequency);

  pinMode(redLedPin, OUTPUT);
  pinMode(blueLedPin, OUTPUT);
  pinMode(greenLedPin, OUTPUT);
  updateWiFiStatusLED(redLedPin, greenLedPin, blueLedPin, firstWifiStartUp);

  Serial.println("Connecting to WiFi...");
  try
  {
    setupWiFi(ssid, password);
    firstWifiStartUp = false;
  }
  catch (const std::runtime_error &e)
  {
    Serial.println(e.what());
    // Handle the error, e.g., retry or halt
  }

  mqttInit(
      MQTT_HOST,
      MQTT_PORT,
      nullptr, // user
      nullptr, // pass
      MQTT_TOPIC);
}

void loop()
{
  updateWiFiStatusLED(redLedPin, greenLedPin, blueLedPin, firstWifiStartUp);

  mqttLoop();

  if (mqttIsConnected())
  {
    Serial.println(mqttGetLastMessage());
  }

  delay(1000);
}
