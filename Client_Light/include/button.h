#pragma once
#include <Arduino.h>

namespace Button
{
    enum class Edge
    {
        Rising,
        Falling,
        Change
    };

    void begin(uint8_t pin, uint32_t debounceMs, Edge edge);

    // Returns true once per debounced press event
    bool consumePress();
}
