#include "LightSensor.h"
#include <../lib/MqttManager.h>

namespace
{
    uint8_t g_pin = 0;
    String latestStatus = "0";
}

namespace LightSensor
{
    void begin(uint8_t pin)
    {
        g_pin = pin;
        analogSetAttenuation(ADC_11db); // ESP32: up to ~3.3V
    }

    int read(const String &componentTopic)
    {
        uint16_t value = analogRead(g_pin);
        if (latestStatus != String(value))
        {
            mqttPublish(String(componentTopic + "/value").c_str(), String(value).c_str(), true);
            latestStatus = String(value);
        }
        return value;
    }
}
