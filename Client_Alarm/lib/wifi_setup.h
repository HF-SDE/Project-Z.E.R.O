#ifndef WIFI_SETUP_H
#define WIFI_SETUP_H

#include <WiFi.h>

// ---- PUBLIC API ----
bool wifiConnect(const char *ssid, const char *password, unsigned long timeoutMs);
bool wifiIsConnected();
String wifiGetIp();

#endif
