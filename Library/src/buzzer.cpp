#include <Buzzer.h>
#include <config.h>
#include <MqttManager.h>
#include <Arduino.h>

namespace
{
    uint8_t g_pin = 0;
    String latestStatus = "0";
    constexpr uint8_t LEDC_CHANNEL = 0;
    constexpr uint32_t LEDC_FREQ = 2000;   // 2kHz for MH-FMD passive buzzer
    constexpr uint8_t LEDC_RESOLUTION = 8; // 8-bit resolution (0-255)
}

namespace Buzzer
{
    void begin(uint8_t pin, const String &componentTopic)
    {
        g_pin = pin;
        ledcAttach(g_pin, LEDC_FREQ, LEDC_RESOLUTION);
        ledcWrite(g_pin, 0); // Start with buzzer off
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

    void beep(uint16_t duration)
    {
        on();
        delay(duration);
        off();
    }

    void on(uint8_t volume)
    {
        // Clamp volume to 0-100 range
        if (volume > 100)
            volume = 100;
        if (volume == 0)
        {
            off();
            return;
        }
        // Convert volume (0-100) to duty cycle (0-255)
        uint8_t dutyCycle = (volume * 255) / 100;
        ledcWrite(g_pin, dutyCycle);
    }

    void off()
    {
        ledcWrite(g_pin, 0);
    }
}
