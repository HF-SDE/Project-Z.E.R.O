#ifndef WIFI_SETUP_H
#define WIFI_SETUP_H

#include <WiFi.h>
#include <stdexcept>

// Function to set up WiFi
void setupWiFi(const char *ssid, const char *password)
{
    WiFi.begin(ssid, password);

    unsigned long startAttemptTime = millis();
    const unsigned long timeout = 10000; // 10 seconds timeout

    // Wait for connection
    while (WiFi.status() != WL_CONNECTED)
    {
        if (millis() - startAttemptTime >= timeout)
        {
            throw std::runtime_error("Failed to connect to WiFi: Timeout");
        }
        Serial.print(".");
        delay(100);
    }

    // Connection successful
    Serial.println("WiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
}

// Function to get the IP address
String getIpAddress()
{
    if (WiFi.status() != WL_CONNECTED)
    {
        throw std::runtime_error("WiFi is not connected");
    }
    return WiFi.localIP().toString();
}

// Function to check WiFi status and update RGB LED
void updateWiFiStatusLED(int redPin, int greenPin, int bluePin, bool firstStartUp)
{
    pinMode(redPin, OUTPUT);
    pinMode(greenPin, OUTPUT);
    pinMode(bluePin, OUTPUT);

    if (firstStartUp)
    {
        // Shine yellow on first startup
        digitalWrite(redPin, HIGH);
        digitalWrite(greenPin, HIGH);
        digitalWrite(bluePin, LOW);
    }
    else
    {
        // Check WiFi status every 5 seconds

        if (WiFi.status() == WL_CONNECTED)
        {
            // Turn LED green
            digitalWrite(redPin, LOW);
            digitalWrite(greenPin, HIGH);
            digitalWrite(bluePin, LOW);
        }
        else
        {
            // Turn LED red
            digitalWrite(redPin, HIGH);
            digitalWrite(greenPin, LOW);
            digitalWrite(bluePin, LOW);
        }
    }
}

#endif // WIFI_SETUP_H