#ifndef CONFIG_MESSAGE_H
#define CONFIG_MESSAGE_H

#include <Arduino.h>

struct ConfigMessage
{
    int heartbeatInterval;
};

bool parseConfigMessage(const char *json, ConfigMessage &out);

#endif