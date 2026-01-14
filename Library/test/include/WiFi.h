#pragma once
#include <string>

static const int WL_CONNECTED = 3;

struct IPAddress
{
    std::string s;
    IPAddress() : s("0.0.0.0") {}
    IPAddress(const std::string &v) : s(v) {}
    std::string toString() const { return s; }
};

class WiFiClass
{
public:
    enum Mode
    {
        WIFI_STA
    };

    void mode(Mode) {}

    void begin(const char *ssid, const char *pass)
    {
        s_status = WL_CONNECTED;
        s_localIP = IPAddress("192.168.1.100");
        (void)ssid;
        (void)pass;
    }

    int status() const { return s_status; }
    IPAddress localIP() const { return s_localIP; }

    // Test helpers
    static void setStatus(int st) { s_status = st; }

    static int s_status;
    static IPAddress s_localIP;
};

extern WiFiClass WiFi;
