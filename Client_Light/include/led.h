#pragma once
#include <Arduino.h>

namespace Led
{
    void begin(uint8_t pin);
    void set(bool on);
}
