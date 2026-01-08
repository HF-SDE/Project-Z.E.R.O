#include "../lib/config_message.h"
#include <ArduinoJson.h>

bool parseConfigMessage(const char *json, ConfigMessage &out)
{
    StaticJsonDocument<256> doc;

    DeserializationError err = deserializeJson(doc, json);
    if (err)
        return false;

    out.heartbeatInterval = doc["heartbeatInterval"] | 0;

    return true;
}