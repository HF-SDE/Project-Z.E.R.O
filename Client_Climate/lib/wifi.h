#pragma once

#include <Arduino.h>
#include <ESP8266WiFi.h>

extern WiFiEventHandler wifiConnectHandler;
extern WiFiEventHandler wifiDisconnectHandler;

void connectToWifi(String ssid, String password);
void connectToWifi();
void onWifiConnect(const WiFiEventStationModeGotIP &event);
void onWifiDisconnect(const WiFiEventStationModeDisconnected &event);
