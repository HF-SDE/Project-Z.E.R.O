#include "../lib/wifi.h"
#include "../lib/mqtt.h"
#include "../lib/StorageManager.h"
#include "../lib/ConfigWriter.h"
#include <Ticker.h>

Ticker wifiReconnectTimer;
WiFiEventHandler wifiConnectHandler;
WiFiEventHandler wifiDisconnectHandler;

void connectToWifi(String ssid, String password)
{
  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(ssid, password);
}

void connectToWifi()
{
  Serial.println("Connecting to Wi-Fi (saved credentials)...");
  WiFi.begin();
}

void onWifiConnect(const WiFiEventStationModeGotIP &event)
{
  Serial.println("Connected to Wi-Fi.");
  connectToMqtt();
}

void onWifiDisconnect(const WiFiEventStationModeDisconnected &event)
{
  Serial.println("Disconnected from Wi-Fi.");
  mqttReconnectTimer.detach(); // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
  wifiReconnectTimer.once(2, []() { connectToWifi(); });
}