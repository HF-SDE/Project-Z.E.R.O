#include "StatusLedManager.h"
#include "ComponentManager.h"
#include <Arduino.h>

// -------- INTERNAL STATE --------
static int ledRed = -1;
static int ledGreen = -1;
static int ledBlue = -1;
static Component *g_redComponent = nullptr;
static Component *g_greenComponent = nullptr;
static Component *g_blueComponent = nullptr;
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

void statusLedSetComponents(Component *redComponent, Component *greenComponent, Component *blueComponent)
{
    g_redComponent = redComponent;
    g_greenComponent = greenComponent;
    g_blueComponent = blueComponent;
}

void statusLedUpdate(bool wifiConnected, bool mqttConnected, bool firstStartup)
{
    if (firstStartup)
    {
        // Yellow - first startup
        digitalWrite(ledRed, HIGH);
        digitalWrite(ledGreen, HIGH);
        digitalWrite(ledBlue, LOW);

        // Update components
        if (g_redComponent)
            componentUpdateValue(g_redComponent, "true");
        if (g_greenComponent)
            componentUpdateValue(g_greenComponent, "true");
        if (g_blueComponent)
            componentUpdateValue(g_blueComponent, "false");
        return;
    }

    if (wifiConnected && mqttConnected)
    {
        // Green - Both WiFi and MQTT connected
        digitalWrite(ledRed, LOW);
        digitalWrite(ledGreen, HIGH);
        digitalWrite(ledBlue, LOW);

        // Update components
        if (g_redComponent)
            componentUpdateValue(g_redComponent, "false");
        if (g_greenComponent)
            componentUpdateValue(g_greenComponent, "true");
        if (g_blueComponent)
            componentUpdateValue(g_blueComponent, "false");
        return;
    }
    else if (!mqttConnected)
    {
        // Purple - WiFi connected but MQTT offline
        digitalWrite(ledRed, HIGH);
        digitalWrite(ledGreen, LOW);
        digitalWrite(ledBlue, HIGH);

        // Update components
        if (g_redComponent)
            componentUpdateValue(g_redComponent, "true");
        if (g_greenComponent)
            componentUpdateValue(g_greenComponent, "false");
        if (g_blueComponent)
            componentUpdateValue(g_blueComponent, "true");
    }
    else
    {
        // Red - WiFi disconnected
        digitalWrite(ledRed, HIGH);
        digitalWrite(ledGreen, LOW);
        digitalWrite(ledBlue, LOW);

        // Update components
        if (g_redComponent)
            componentUpdateValue(g_redComponent, "true");
        if (g_greenComponent)
            componentUpdateValue(g_greenComponent, "false");
        if (g_blueComponent)
            componentUpdateValue(g_blueComponent, "false");
    }
}
