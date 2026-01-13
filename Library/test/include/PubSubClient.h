#pragma once
#include <string>
#include <vector>

using byte = unsigned char;
using mqtt_callback_t = void (*)(char *, byte *, unsigned int);

// Minimal WiFiClient placeholder
class WiFiClient
{
};

class PubSubClient
{
public:
    PubSubClient(WiFiClient &) { s_instance = this; }

    void setServer(const char *host, int port)
    {
        (void)host;
        (void)port;
    }
    void setCallback(mqtt_callback_t cb) { s_callback = cb; }

    bool connect(const char *clientId)
    {
        s_connected = true;
        (void)clientId;
        return true;
    }

    bool publish(const char *topic, const char *payload, bool retained = false)
    {
        if (!s_connected)
            return false;
        s_lastTopic = topic;
        s_lastPayload = payload ? payload : "";
        (void)retained;
        return true;
    }

    bool subscribe(const char *topic)
    {
        s_subscribed = true;
        s_subTopic = topic ? topic : "";
        return true;
    }

    bool unsubscribe(const char *topic)
    {
        (void)topic;
        s_subscribed = false;
        return true;
    }

    bool connected() { return s_connected; }
    void loop() { /* no-op for mock */ }

    // Test helper - invoke the stored callback like a broker message
    static void simulateIncoming(const char *topic, const char *payload)
    {
        if (!s_callback)
            return;
        unsigned int len = payload ? (unsigned int)strlen(payload) : 0;
        std::vector<byte> buf(len);
        for (unsigned int i = 0; i < len; ++i)
            buf[i] = static_cast<byte>(payload[i]);
        // Call the callback with mutable topic pointer and payload
        s_callback(const_cast<char *>(topic), buf.data(), len);
    }

    // Inspect published values in tests
    static std::string s_lastTopic;
    static std::string s_lastPayload;

private:
    static inline PubSubClient *s_instance = nullptr;
    static inline bool s_connected = false;
    static inline bool s_subscribed = false;
    static inline std::string s_subTopic;
    static inline mqtt_callback_t s_callback = nullptr;
};
