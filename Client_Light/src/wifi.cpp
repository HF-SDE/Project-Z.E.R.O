#include "../lib/wifi_setup.h"

// -------- INTERNAL STATE --------
static int ledRed = -1;
static int ledGreen = -1;
static int ledBlue = -1;
// --------------------------------

bool wifiConnect(const char *ssid, const char *password, unsigned long timeoutMs)
{
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    unsigned long start = millis();

    while (WiFi.status() != WL_CONNECTED)
    {
        if (millis() - start >= timeoutMs)
        {
            return false;
        }
        delay(100);
    }

    return true;
}

bool wifiIsConnected()
{
    return WiFi.status() == WL_CONNECTED;
}

String wifiGetIp()
{
    if (!wifiIsConnected())
        return "";
    return WiFi.localIP().toString();
}

void wifiInitStatusLed(uint8_t redPin, uint8_t greenPin, uint8_t bluePin)
{
    ledRed = redPin;
    ledGreen = greenPin;
    ledBlue = bluePin;

    pinMode(ledRed, OUTPUT);
    pinMode(ledGreen, OUTPUT);
    pinMode(ledBlue, OUTPUT);
}

void updateWifiStatusLed(bool firstStartup)
{
    if (ledRed < 0)
        return; // not initialized

    if (firstStartup)
    {
        // Yellow
        digitalWrite(ledRed, HIGH);
        digitalWrite(ledGreen, HIGH);
        digitalWrite(ledBlue, LOW);
        return;
    }

    if (wifiIsConnected())
    {
        // Green
        digitalWrite(ledRed, LOW);
        digitalWrite(ledGreen, HIGH);
        digitalWrite(ledBlue, LOW);
    }
    else
    {
        // Red
        digitalWrite(ledRed, HIGH);
        digitalWrite(ledGreen, LOW);
        digitalWrite(ledBlue, LOW);
    }
}
