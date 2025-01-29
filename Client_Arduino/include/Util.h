#ifndef UTIL_H
#define UTIL_H

#include <SPI.h>

#define BUTTON_PIN 4

#define UUID_OFFSET 0
#define API_KEY_OFFSET 40

String getUuid(String defaultString = "");
String generateUUID();
String getApiKey(String uuid);

#endif