#ifndef COMPONENT_H
#define COMPONENT_H

#include <Arduino.h>

// Component configuration metadata
struct ComponentConfig
{
    const char *name;
    const char *comment; // optional, can be nullptr
};

// Represents a hardware component's current state
struct Component
{
    const char *type;      // e.g., "display", "led", "buzzer"
    String value;          // Current value as string
    const char *valueType; // e.g., "text", "boolean", "number"
    ComponentConfig config;
    bool stateChanged = false; // Tracks if the component's state has changed

    // Constructor
    Component(const char *_type, const char *_valueType, const char *_name, const char *_comment = nullptr)
        : type(_type), valueType(_valueType), value(""), config{_name, _comment}
    {
    }
};

#endif // COMPONENT_H
