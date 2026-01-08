#include "Led.h"

namespace
{
    uint8_t g_pin = 0;
}

namespace Led
{
    void begin(uint8_t pin)
    {
        g_pin = pin;
        pinMode(g_pin, OUTPUT);
        digitalWrite(g_pin, LOW);
    }

    void set(bool on)
    {
        digitalWrite(g_pin, on ? HIGH : LOW);
    }
}
