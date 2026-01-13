#include "../lib/config_message.h"
#include <ArduinoJson.h>

bool parseConfigMessage(const char *json, ConfigMessage &out)
{
    StaticJsonDocument<256> doc;

    DeserializationError err = deserializeJson(doc, json);
    if (err)
        return false;

    // Reject if required fields are missing or null
    if (!doc.containsKey("heartbeatInterval") || doc["heartbeatInterval"].isNull())
    {
        return false;
    }
    if (!doc.containsKey("status") || doc["status"].isNull())
    {
        return false;
    }

    out.heartbeatInterval = doc["heartbeatInterval"];
    out.status = doc["status"];

    return true;
}