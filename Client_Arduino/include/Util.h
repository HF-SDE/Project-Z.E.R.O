#ifndef UTIL_H
#define UTIL_H

#include <SPI.h>

#define BUTTON_PIN 4

#define UUID_OFFSET 0
#define API_KEY_OFFSET 40

#define DHTPIN A0    
#define DHTTYPE DHT11

#define debug Serial

String getUuid();
String generateUUID();
String getApiKey(String uuid);

#endif