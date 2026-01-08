#include "LightSensor.h"

namespace
{
    uint8_t g_pin = 0;
}

namespace LightSensor
{
    void begin(uint8_t pin)
    {
        g_pin = pin;
        analogSetAttenuation(ADC_11db); // ESP32: up to ~3.3V
    }

    int read()
    {
        return analogRead(g_pin);
    }
}
