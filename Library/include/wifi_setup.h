#pragma once
#ifndef WIFI_SETUP_H
#define WIFI_SETUP_H

#if defined(ESP8266)
#include <ESP8266WiFi.h>
#elif defined(ESP32)
#include <WiFi.h>
#endif

// ---- PUBLIC API ----
bool wifiConnect(const char *ssid, const char *password, unsigned long timeoutMs);
bool wifiIsConnected();
String wifiGetIp();

void wifiInitStatusLed(uint8_t redPin, uint8_t greenPin, uint8_t bluePin);
void updateWifiStatusLed(bool firstStartup);

#endif
