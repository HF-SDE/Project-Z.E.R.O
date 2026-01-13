#ifndef COMPONENT_MANAGER_H
#define COMPONENT_MANAGER_H

#include "Component.h"

// Initialize the component manager with device ID
void componentManagerInit(String deviceId, String baseMqttTopic);

// Register a component (called at startup)
void componentRegister(Component *component);

// Update a component's value and trigger MQTT publish of all components
void componentUpdateValue(Component *component, const String &newValue);

// Manually trigger publishing all component states
void componentPublishAll();

#endif // COMPONENT_MANAGER_H
