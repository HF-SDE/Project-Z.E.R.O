#pragma once
#include <Arduino.h>

namespace LightSensor
{
    void begin(uint8_t pin);
    int read(const String &componentTopic);
}
