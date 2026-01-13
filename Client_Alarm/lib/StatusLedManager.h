#ifndef STATUS_LED_MANAGER_H
#define STATUS_LED_MANAGER_H

#include "Component.h"

// Initialize the status LED pins
void statusLedInit(int redPin, int greenPin, int bluePin);

// Set the component pointers for automatic state tracking
void statusLedSetComponents(Component *redComponent, Component *greenComponent, Component *blueComponent);

// Update LED based on WiFi and MQTT connection status
void statusLedUpdate(bool wifiConnected, bool mqttConnected, bool firstStartup = false);

#endif
