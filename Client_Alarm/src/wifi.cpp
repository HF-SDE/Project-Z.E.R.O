#include "../lib/wifi_setup.h"

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
