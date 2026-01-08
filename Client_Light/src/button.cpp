#include "Button.h"

namespace
{
    volatile bool g_event = false;
    volatile uint32_t g_lastIsrMs = 0;

    uint8_t g_pin = 0;
    uint32_t g_debounceMs = 50;

    void IRAM_ATTR isr()
    {
        const uint32_t now = millis();
        if (now - g_lastIsrMs >= g_debounceMs)
        {
            g_lastIsrMs = now;
            g_event = true;
        }
    }

    int toInterruptMode(Button::Edge edge)
    {
        switch (edge)
        {
        case Button::Edge::Rising:
            return RISING;
        case Button::Edge::Falling:
            return FALLING;
        case Button::Edge::Change:
            return CHANGE;
        }
        return CHANGE;
    }
}

namespace Button
{
    void begin(uint8_t pin, uint32_t debounceMs, Edge edge)
    {
        g_pin = pin;
        g_debounceMs = debounceMs;

        pinMode(g_pin, INPUT_PULLUP);
        attachInterrupt(digitalPinToInterrupt(g_pin), isr, toInterruptMode(edge));
    }

    bool consumePress()
    {
        bool event = false;
        noInterrupts();
        event = g_event;
        g_event = false;
        interrupts();
        return event;
    }
}
