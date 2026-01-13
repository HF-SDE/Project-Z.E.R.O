#ifndef ALARM_OUTPUT_MANAGER_H
#define ALARM_OUTPUT_MANAGER_H

#include "Component.h"

// Initialize alarm LED and buzzer pins
void alarmOutputInit(int alarmLedPin, int buzzerPin);

// Set component pointers for automatic state tracking
void alarmOutputSetComponents(Component *ledComponent, Component *buzzerComponent);

// Activate alarm with LED and optional sound
void alarmOutputActivate(bool useSound);

// Deactivate alarm (turn off LED and buzzer)
void alarmOutputDeactivate();

#endif // ALARM_OUTPUT_MANAGER_H
