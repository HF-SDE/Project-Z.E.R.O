#include "../lib/StatusLedManager.h"
#include <Arduino.h>

// -------- INTERNAL STATE --------
static int ledRed = -1;
static int ledGreen = -1;
static int ledBlue = -1;
// --------------------------------

void statusLedInit(int redPin, int greenPin, int bluePin)
{
    ledRed = redPin;
    ledGreen = greenPin;
    ledBlue = bluePin;

    pinMode(ledRed, OUTPUT);
    pinMode(ledGreen, OUTPUT);
    pinMode(ledBlue, OUTPUT);
}

void statusLedUpdate(bool wifiConnected, bool mqttConnected, bool firstStartup)
{
    if (firstStartup)
    {
        // Yellow - first startup
        digitalWrite(ledRed, HIGH);
        digitalWrite(ledGreen, HIGH);
        digitalWrite(ledBlue, LOW);
        return;
    }

    if (wifiConnected && mqttConnected)
    {
        // Green - Both WiFi and MQTT connected
        digitalWrite(ledRed, LOW);
        digitalWrite(ledGreen, HIGH);
        digitalWrite(ledBlue, LOW);
        return;
    }
    else if (!mqttConnected)
    {
        // Purple - WiFi connected but MQTT offline
        digitalWrite(ledRed, HIGH);
        digitalWrite(ledGreen, LOW);
        digitalWrite(ledBlue, HIGH);
    }
    else
    {
        // Red - WiFi disconnected
        digitalWrite(ledRed, HIGH);
        digitalWrite(ledGreen, LOW);
        digitalWrite(ledBlue, LOW);
    }
}
