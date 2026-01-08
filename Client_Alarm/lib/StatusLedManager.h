#ifndef STATUS_LED_MANAGER_H
#define STATUS_LED_MANAGER_H

// Initialize the status LED pins
void statusLedInit(int redPin, int greenPin, int bluePin);

// Update LED based on WiFi and MQTT connection status
void statusLedUpdate(bool wifiConnected, bool mqttConnected, bool firstStartup = false);

#endif
