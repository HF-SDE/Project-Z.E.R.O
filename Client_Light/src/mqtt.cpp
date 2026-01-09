#include "../lib/MqttManager.h"

// -------- INTERNAL STATE (private to this file) --------
static WiFiClient wifiClient;
static PubSubClient mqtt(wifiClient);

static const char *subTopic;
static MqttMessageHandler userHandler = nullptr;

// ------------------------------------------------------

// MQTT callback
static void mqttCallback(char *topic, byte *payload, unsigned int length)
{
    Serial.println("Event received - callback");

    static char msg[256];

    unsigned int n = (length < sizeof(msg) - 1) ? length : sizeof(msg) - 1;
    memcpy(msg, payload, n);
    msg[n] = '\0';

    if (userHandler)
    {
        userHandler(topic, msg);
    }
}

// Internal reconnect
static void mqttReconnect()
{
    while (!mqtt.connected())
    {
        String clientId = "esp32-";
        clientId += String((uint32_t)ESP.getEfuseMac(), HEX);

        if (mqtt.connect(clientId.c_str()))
        {
            mqtt.subscribe(subTopic);
        }
        else
        {
            delay(1000);
        }
    }
}

void mqttSetMessageHandler(MqttMessageHandler handler)
{
    userHandler = handler;
}

// -------- PUBLIC FUNCTIONS --------

void mqttInit(
    const char *host,
    int port,
    const char *user,
    const char *pass,
    const char *subscribeTopic)
{
    subTopic = subscribeTopic;

    mqtt.setServer(host, port);
    mqtt.setCallback(mqttCallback);

    mqttReconnect();
}

void mqttLoop()
{
    if (!mqtt.connected())
    {
        mqttReconnect();
    }
    mqtt.loop();
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