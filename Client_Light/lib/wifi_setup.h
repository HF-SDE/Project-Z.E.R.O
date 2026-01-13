#pragma once
#ifndef WIFI_SETUP_H
#define WIFI_SETUP_H

#include <WiFi.h>

// ---- PUBLIC API ----
bool wifiConnect(const char *ssid, const char *password, unsigned long timeoutMs);
bool wifiIsConnected();
String wifiGetIp();

void wifiInitStatusLed(uint8_t redPin, uint8_t greenPin, uint8_t bluePin);
void updateWifiStatusLed(bool firstStartup);

#endif
