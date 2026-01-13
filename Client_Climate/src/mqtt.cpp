#include "../lib/mqtt.h"
#include "../lib/wifi.h"

AsyncMqttClient mqttClient;
Ticker mqttReconnectTimer;

bool devMode = false;

void connectToMqtt()
{
  Serial.println("Connecting to MQTT...");
  mqttClient.connect();
}

void onMqttConnect(bool sessionPresent)
{
  Serial.println("Connected to MQTT.");

  String tempTypeTopic = String(g_deviceConfig.mqttTopic + "/temperature/value_type");
  String humTypeTopic = String(g_deviceConfig.mqttTopic + "/humidity/value_type");

  uint16_t packetIdPubType1 = mqttClient.publish(tempTypeTopic.c_str(), 1, true, "temperature");
  uint16_t packetIdPubType2 = mqttClient.publish(humTypeTopic.c_str(), 1, true, "humidity");

  String configTopic = String(g_deviceConfig.mqttTopic + "/config");
  uint16_t packetIdSub = mqttClient.subscribe(configTopic.c_str(), g_deviceConfig.qos);

  if (!devMode)
    return;

  Serial.print("Publishing temperature & humidity type at QoS 1, packetId: ");
  Serial.print(packetIdPubType1);
  Serial.print(" & ");
  Serial.println(packetIdPubType2);

  Serial.print("Subscribing to topic: ");
  Serial.print(configTopic);
  Serial.print(" at QoS ");
  Serial.print(g_deviceConfig.qos);
  Serial.print(", packetId: ");
  Serial.println(packetIdSub);
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason)
{
  Serial.println("Disconnected from MQTT.");

  if (WiFi.isConnected())
  {
    mqttReconnectTimer.once(2, connectToMqtt);
  }
}

void onMqttSubscribe(uint16_t packetId, uint8_t qos)
{
  if (!devMode)
    return;

  Serial.println("Subscribe acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
  Serial.print("  qos: ");
  Serial.println(qos);
}

void onMqttUnsubscribe(uint16_t packetId)
{
  if (!devMode)
    return;

  Serial.println("Unsubscribe acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
}

void onMqttPublish(uint16_t packetId)
{
  if (!devMode)
    return;

  Serial.print("Publish acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
}