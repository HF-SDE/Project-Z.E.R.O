#include <Arduino.h>
#include <../lib/SerialManager.h>
#include <../lib/wifi_setup.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <PubSubClient.h>
#include <../lib/DisplayManager.h>
#include <../lib/MqttManager.h>

//---------------------- Wifi config ----------------------
const char *WIFI_SSID = "Case-ZERO_2,4GHz";
const char *WIFI_PASSWORD = "Nogetjegkanhuske";
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

  // mqttInit(
  //     MQTT_HOST,
  //     MQTT_PORT,
  //     nullptr, // user
  //     nullptr, // pass
  //     MQTT_TOPIC);
}

void loop()
{
  updateWifiStatusLed(false);
  // mqttLoop();
  delay(500);
}
