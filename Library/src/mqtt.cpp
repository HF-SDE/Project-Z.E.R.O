#include <MqttManager.h>
#include <Environment.h>

// -------- INTERNAL STATE (private to this file) --------
static WiFiClient wifiClient;
static PubSubClient mqtt(wifiClient);

static const char *subTopic;
static MqttMessageHandler userHandler = nullptr;

static volatile bool messageReceived = false;
static String retainedPayload;
static uint32_t receivedAt = 0;

static volatile bool gotMsg = false;
static String payloadBuf;

// ------------------------------------------------------

// MQTT callback
static void mqttCallback(char *topic, byte *payload, unsigned int length)
{
    mqtt.setBufferSize(2048);
    messageReceived = true;
    Environment::print("Event received - callback");

    payloadBuf = "";
    payloadBuf.reserve(length);
    for (unsigned int i = 0; i < length; i++)
    {
        payloadBuf += (char)payload[i];
    }

    static char msg[256];

    unsigned int n = (length < sizeof(msg) - 1) ? length : sizeof(msg) - 1;
    memcpy(msg, payload, n);
    msg[n] = '\0';

    if (userHandler)
    {
        userHandler(topic, msg);
    }
}

bool retainedMessageExists(const char *topic, uint32_t windowMs)
{
    windowMs = 1500;
    messageReceived = false;
    retainedPayload = "";
    receivedAt = 0;

    if (!mqtt.connected())
        return false;

    mqtt.setCallback(mqttCallback);

    if (!mqtt.subscribe(topic))
        return false;

    uint32_t start = millis();

    while ((millis() - start) < windowMs)
    {
        mqtt.loop();
        delay(2);
        if (messageReceived)
            break;
    }

    mqtt.unsubscribe(topic);

    if (!messageReceived)
        return false;

    return true;
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

void mqttSetComponentValue(const String &componentTopic, const String &status)
{
    String valueTopic = componentTopic + "/value";
    mqttPublish(valueTopic.c_str(), status.c_str(), true);
}