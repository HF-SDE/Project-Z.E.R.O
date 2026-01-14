#include "WiFi.h"
#include "../include/WiFi.h"

WiFiClass WiFi;
int WiFiClass::s_status = 0;
IPAddress WiFiClass::s_localIP = IPAddress("0.0.0.0");
