#include "../lib/StatusLedManager.h"
#include "../lib/ComponentManager.h"
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

void statusLedSetComponents(Component *pRedComponent, Component *pGreenComponent, Component *pBlueComponent)
{
    componentRegister(pRedComponent);
    componentRegister(pGreenComponent);
    componentRegister(pBlueComponent);
    g_redComponent = pRedComponent;
    g_greenComponent = pGreenComponent;
    g_blueComponent = pBlueComponent;
}

void blinkRedLed(int times, int delayMs)
{
    if (ledRed == -1)
        return; // Not initialized

    // Ensure other LEDs are off before blinking
    digitalWrite(ledBlue, LOW);
    digitalWrite(ledGreen, LOW);

    for (int i = 0; i < times; i++)
    {
        digitalWrite(ledRed, HIGH);
        delay(delayMs);
        digitalWrite(ledRed, LOW);
        delay(delayMs);
    }
}
void blinkYellowLed(int times, int delayMs)
{
    if (ledRed == -1 || ledGreen == -1)
        return; // Not initialized

    // Ensure other LEDs are off before blinking
    digitalWrite(ledBlue, LOW);

    for (int i = 0; i < times; i++)
    {
        digitalWrite(ledRed, HIGH);
        digitalWrite(ledGreen, HIGH);
        delay(delayMs);
        digitalWrite(ledRed, LOW);
        digitalWrite(ledGreen, LOW);
        delay(delayMs);
    }
}
void blinkYellowAndRedLed(int times, int delayMs)
{
    if (ledRed == -1 || ledGreen == -1)
        return; // Not initialized

    // Ensure other LEDs are off before blinking
    digitalWrite(ledBlue, LOW);
    digitalWrite(ledRed, HIGH);
    digitalWrite(ledGreen, HIGH);

    for (int i = 0; i < times; i++)
    {
        digitalWrite(ledGreen, HIGH);
        delay(delayMs);
        digitalWrite(ledGreen, LOW);
        delay(delayMs);
    }
}

bool statusLedUpdate(bool wifiConnected, bool mqttConnected, bool firstStartup)
{
    if (ledRed == -1 || ledGreen == -1 || ledBlue == -1)
        return false; // Not initialized
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
        return false;
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
        return true;
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
