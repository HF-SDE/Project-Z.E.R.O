#include "../lib/MqttManager.h"

// -------- INTERNAL STATE (private to this file) --------
static WiFiClient wifiClient;
static PubSubClient mqtt(wifiClient);

static const char *subTopic;
static MqttMessageHandler userHandler = nullptr;

// Heartbeat state
static String g_heartbeatTopic = "heartbeat";
static String mqttBaseTopic = "";
static unsigned long g_heartbeatInterval = 0;
static unsigned long g_lastHeartbeat = 0;

// ------------------------------------------------------

// MQTT callback
static void mqttCallback(char *topic, byte *payload, unsigned int length)
{

    static char msg[256];

    unsigned int n = (length < sizeof(msg) - 1) ? length : sizeof(msg) - 1;
    memcpy(msg, payload, n);
    msg[n] = '\0';

    if (userHandler)
    {
        userHandler(topic, msg);
    }
}

// Internal reconnect - non-blocking
static void mqttReconnect()
{
    // Only attempt reconnect if not connected
    if (!mqtt.connected())
    {
        String clientId = "esp32-";
        clientId += String((uint32_t)ESP.getEfuseMac(), HEX);

        if (mqtt.connect(clientId.c_str()))
        {
            mqtt.subscribe(subTopic);
        }
    }
}

void mqttSetMessageHandler(MqttMessageHandler handler)
{
    userHandler = handler;
}

void mqttSetHeartbeat(const char *deviceId, unsigned long intervalMs)
{
    g_heartbeatTopic = mqttBaseTopic + "/heartbeat";
    g_heartbeatInterval = intervalMs;
    g_lastHeartbeat = 0;
}

// -------- PUBLIC FUNCTIONS --------

void mqttInit(
    const char *host,
    int port,
    const char *user,
    const char *pass,
    const char *subscribeTopic,
    String baseMqttTopic)
{
    subTopic = subscribeTopic;
    mqttBaseTopic = baseMqttTopic;

    mqtt.setServer(host, port);
    mqtt.setCallback(mqttCallback);

    mqttReconnect();
}

void mqttLoop()
{
    if (!mqtt.connected())
    {
        // Throttle reconnect attempts to avoid spamming
        static unsigned long lastReconnectAttempt = 0;
        unsigned long now = millis();
        if (now - lastReconnectAttempt > 5000) // Try every 5 seconds
        {
            lastReconnectAttempt = now;
            mqttReconnect();
        }
    }
    else
    {
        mqtt.loop();
    }

    // Handle heartbeat if configured
    if (g_heartbeatInterval > 0)
    {
        unsigned long currentMillis = millis();
        if (currentMillis - g_lastHeartbeat >= g_heartbeatInterval)
        {

            mqttPublish(g_heartbeatTopic.c_str(), "online", true);
            g_lastHeartbeat = currentMillis;
        }
    }
}

bool mqttIsConnected()
{
    return mqtt.connected();
}

bool mqttPublish(const char *topic, const char *payload, bool retained)
{
    if (!mqtt.connected())
        return false;
    if (!topic || !payload)
        return false;

    // PubSubClient publish signature supports retained
    return mqtt.publish(topic, payload, retained);
}