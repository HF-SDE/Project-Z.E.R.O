#include "../lib/MqttManager.h"

// -------- INTERNAL STATE (private to this file) --------
static WiFiClient wifiClient;
static PubSubClient mqtt(wifiClient);

static const char *subTopic;

static char lastMessage[MQTT_MAX_MSG_LEN + 1];
// ------------------------------------------------------

// MQTT callback
static void mqttCallback(char *topic, byte *payload, unsigned int length)
{
    unsigned int n = (length < MQTT_MAX_MSG_LEN)
                         ? length
                         : MQTT_MAX_MSG_LEN;

    for (unsigned int i = 0; i < n; i++)
    {
        lastMessage[i] = (char)payload[i];
    }
    lastMessage[n] = '\0';
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

const char *mqttGetLastMessage()
{
    return lastMessage;
}
