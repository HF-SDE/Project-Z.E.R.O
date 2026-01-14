#pragma once
#include <Arduino.h>

namespace Led
{
    void begin(uint8_t pin, const String &componentTopic);
    void set(bool on, const String &componentTopic);
}
