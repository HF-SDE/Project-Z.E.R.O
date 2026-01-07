#ifndef MQTT_CLIENT_H
#define MQTT_CLIENT_H

#include <WiFi.h>
#include <PubSubClient.h>

// ---- CONFIG ----
#define MQTT_MAX_MSG_LEN 64
// ----------------

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

#endif
