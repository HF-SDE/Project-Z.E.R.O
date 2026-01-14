#ifndef ALARM_MESSAGE_H
#define ALARM_MESSAGE_H

#include <Arduino.h>

struct AlarmMessage
{
    const char *message;
    bool useSound;
};

bool parseAlarmMessage(const char *json, AlarmMessage &out);

#endif
