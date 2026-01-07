#include "../lib/alarm_message.h"
#include <ArduinoJson.h>

bool parseAlarmMessage(const char *json, AlarmMessage &out)
{
    StaticJsonDocument<256> doc;

    DeserializationError err = deserializeJson(doc, json);
    if (err)
        return false;

    out.message = doc["message"] | "";
    out.color = doc["color"] | "white";
    out.useSound = doc["useSound"] | false;

    return true;
}
