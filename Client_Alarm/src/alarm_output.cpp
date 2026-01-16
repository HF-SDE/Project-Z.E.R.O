#include "../lib/AlarmOutputManager.h"
#include "../lib/ComponentManager.h"
#include <Arduino.h>

// -------- INTERNAL STATE --------
static int g_alarmLedPin = -1;
static int g_buzzerPin = -1;
static Component *g_ledComponent = nullptr;
static Component *g_buzzerComponent = nullptr;
// --------------------------------

void alarmOutputInit(int alarmLedPin, int buzzerPin)
{
    g_alarmLedPin = alarmLedPin;
    g_buzzerPin = buzzerPin;

    pinMode(g_alarmLedPin, OUTPUT);
    pinMode(g_buzzerPin, OUTPUT);

    // Start with everything off
    digitalWrite(g_alarmLedPin, LOW);
    digitalWrite(g_buzzerPin, LOW);
}

void alarmOutputSetComponents(Component *pLedComponent, Component *pBuzzerComponent)
{
    componentRegister(pLedComponent);
    componentRegister(pBuzzerComponent);
    g_ledComponent = pLedComponent;
    g_buzzerComponent = pBuzzerComponent;
}

void alarmOutputActivate(bool useSound)
{
    digitalWrite(g_alarmLedPin, HIGH);
    if (g_ledComponent)
    {
        componentUpdateValue(g_ledComponent, "true");
    }

    if (useSound)
    {
        digitalWrite(g_buzzerPin, HIGH);
        if (g_buzzerComponent)
        {
            componentUpdateValue(g_buzzerComponent, "true");
        }
    }
    else
    {
        digitalWrite(g_buzzerPin, LOW);
        if (g_buzzerComponent)
        {
            componentUpdateValue(g_buzzerComponent, "false");
        }
    }
}

void alarmOutputDeactivate()
{
    digitalWrite(g_alarmLedPin, LOW);
    digitalWrite(g_buzzerPin, LOW);

    if (g_ledComponent)
    {
        componentUpdateValue(g_ledComponent, "false");
    }
    if (g_buzzerComponent)
    {
        componentUpdateValue(g_buzzerComponent, "false");
    }
}
