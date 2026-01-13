#pragma once

#include <AsyncMqttClient.h>
#include <Ticker.h>
#include "StorageManager.h"

extern DeviceConfig g_deviceConfig;

extern AsyncMqttClient mqttClient;
extern Ticker mqttReconnectTimer;

void connectToMqtt();
void onMqttConnect(bool sessionPresent);
void onMqttDisconnect(AsyncMqttClientDisconnectReason reason);
void onMqttSubscribe(uint16_t packetId, uint8_t qos);
void onMqttUnsubscribe(uint16_t packetId);
void onMqttPublish(uint16_t packetId);