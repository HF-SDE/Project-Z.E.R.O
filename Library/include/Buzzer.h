#pragma once
#include <Arduino.h>

namespace Buzzer
{
    /**
     * @brief Initialize the buzzer component
     *
     * Configures the specified GPIO pin as an output for the buzzer,
     * sets it to LOW (off), and publishes the initial state to MQTT.
     *
     * @param pin GPIO pin number connected to the buzzer
     * @param componentTopic MQTT topic base for this buzzer component
     */
    void begin(uint8_t pin, const String &componentTopic);

    /**
     * @brief Set the buzzer state (on/off)
     *
     * Controls the buzzer state and publishes the change to MQTT only if
     * the state has changed from the previous value.
     *
     * @param on true to turn buzzer on, false to turn it off
     * @param componentTopic MQTT topic base for this buzzer component
     */
    void set(bool on, const String &componentTopic);

    /**
     * @brief Generate a short beep sound
     *
     * Turns the buzzer on for the specified duration, then turns it off.
     * This is a blocking call that uses delay().
     *
     * @param duration Duration of the beep in milliseconds (default: 100ms)
     */
    void beep(uint16_t duration = 100);
}
