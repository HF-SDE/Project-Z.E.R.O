#ifndef MQTT_CLIENT_H
#define MQTT_CLIENT_H

#include <WiFi.h>
#include <PubSubClient.h>

// Called once
void mqttInit(
    const char *host,
    int port,
    const char *user,
    const char *pass,
    const char *subscribeTopic);

// Call in loop()
void mqttLoop();

// Optional helpers
bool mqttIsConnected();
const char *mqttGetLastMessage();

typedef void (*MqttMessageHandler)(
    const char *topic,
    const char *payload);

void mqttSetMessageHandler(MqttMessageHandler handler);

bool mqttPublish(const char *topic, const char *payload, bool retained = false);
bool mqttPublish(const char *topic, const uint8_t *payload, bool retained = false);

bool retainedMessageExists(const char *topic, uint32_t windowMs = 150);

void mqttSetComponentValue(const String &componentTopic, const String &status);

#endif
