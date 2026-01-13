#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <DHT.h>
#include <Ticker.h>
#include "../lib/StorageManager.h"
#include "../lib/ConfigWriter.h"
#include "../lib/wifi.h"
#include "../lib/mqtt.h"

// ESP8266: Avoid GPIO0/GPIO2/GPIO15 for peripherals (boot strap pins). Use GPIO4 (D2) instead.
#define DHTPIN 2      // D2 on many ESP8266 dev boards
#define DHTTYPE DHT22 // DHT 22 (AM2302)

DHT dht(DHTPIN, DHTTYPE);

float temp = 0.0;
float hum = 0.0;

// Understående er fundet fra https://randomnerdtutorials.com/esp8266-nodemcu-mqtt-publish-dht11-dht22-arduino/

char *MQTT_PUB_TEMP = "";
char *MQTT_PUB_HUM = "";

DeviceConfig g_deviceConfig;

unsigned long previousMillis = 0; // Stores last time temperature was published

char *getPublishTopic(String componentType)
{
  String topic = g_deviceConfig.mqttTopic + "/" + componentType + "/value";
  return strdup(topic.c_str());
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

    StaticJsonDocument<256> doc;
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
  Serial.println();
  Serial.println("Booting...");
  Serial.print("Reset reason: ");
  Serial.println(ESP.getResetReason());
  Serial.print("Free heap: ");
  Serial.println(ESP.getFreeHeap());

  storageInit();
  if (!storageLoadConfig(g_deviceConfig))
  {
    writeDefaultConfig();
  }

  Serial.println("Loaded configuration:");
  storagePrintConfig(g_deviceConfig);

  dht.begin();

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
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= g_deviceConfig.frequency)
  {
    previousMillis = currentMillis;

    hum = dht.readHumidity();
    temp = dht.readTemperature();

    if (isnan(hum) || isnan(temp))
    {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }

    if (!WiFi.isConnected())
    {
      Serial.println("Wi-Fi not connected, skipping publish");
      connectToWifi(g_deviceConfig.wifiSsid, g_deviceConfig.wifiPassword);
      return;
    }

    if (!mqttClient.connected())
    {
      Serial.println("MQTT not connected, skipping publish");
      connectToMqtt();
      return;
    }

    char *tempTopic = getPublishTopic("tempature");
    char *humTopic = getPublishTopic("humidity");

    uint16_t packetIdPub1 = mqttClient.publish(tempTopic, g_deviceConfig.qos, true, String(temp).c_str());
    Serial.printf("Publishing on topic %s at QoS 0, packetId: %i ", tempTopic, packetIdPub1);
    Serial.printf("Message: %.2f \n", temp);

    uint16_t packetIdPub2 = mqttClient.publish(humTopic, g_deviceConfig.qos, true, String(hum).c_str());
    Serial.printf("Publishing on topic %s at QoS 0, packetId %i: ", humTopic, packetIdPub2);
    Serial.printf("Message: %.2f \n", hum);
  }
}