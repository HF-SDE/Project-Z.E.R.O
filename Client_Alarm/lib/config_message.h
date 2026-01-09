#ifndef CONFIG_MESSAGE_H
#define CONFIG_MESSAGE_H

#include <Arduino.h>

struct ConfigMessage
{
    int heartbeatInterval;
    bool status; // 0 = inactive, 1 = active
};

bool parseConfigMessage(const char *json, ConfigMessage &out);

#endif