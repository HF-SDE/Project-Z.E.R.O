#include <Led.h>
#include <config.h>
#include <MqttManager.h>

namespace
{
    uint8_t g_pin = 0;
    String latestStatus = "0";
}

namespace Led
{
    void begin(uint8_t pin, const String &componentTopic)
    {
        g_pin = pin;
        pinMode(g_pin, OUTPUT);
        digitalWrite(g_pin, LOW);
        latestStatus = "0";
        mqttPublish(String(componentTopic + "/value").c_str(), "0", true);
    }

    void set(bool on, const String &componentTopic)
    {
        digitalWrite(g_pin, on ? HIGH : LOW);
        if (latestStatus != String(on ? "1" : "0"))
        {
            mqttPublish(String(componentTopic + "/value").c_str(), String(on ? "1" : "0").c_str(), true);
            latestStatus = String(on ? "1" : "0");
        }
    }
}
