#ifndef SERIAL_MANAGER_H
#define SERIAL_MANAGER_H

#include <Arduino.h>

// Function to initialize Serial communication
void setupSerial(unsigned long baudRate)
{
    Serial.begin(baudRate);
    while (!Serial)
    {
        ; // Wait for Serial to be ready
    }
    Serial.println("Serial communication initialized");
}

#endif // SERIAL_MANAGER_H