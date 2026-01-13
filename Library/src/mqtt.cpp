
#include <MqttManager.h>
#include <Environment.h>
#include <wifi_setup.h>

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

static int max_retry = 20;
static int retry_count = 0;

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

static String deviceIdHex()
{
#if defined(ESP32)
    uint64_t mac = ESP.getEfuseMac();
    char buf[13];
    snprintf(buf, sizeof(buf), "%012llX", (unsigned long long)(mac & 0xFFFFFFFFFFFFULL));
    return String(buf);
#elif defined(ESP8266)
    return String(ESP.getChipId(), HEX);
#else
    return "UNKNOWN";
#endif
}

// Internal reconnect
static void mqttReconnect()
{
    const int maxAttempts = max_retry;
    int attempt = retry_count;
    if (attempt >= maxAttempts)
    {
        return;
    }

    while (!mqtt.connected())
    {
        String clientId = "esp32-";
        clientId += deviceIdHex();

        if (mqtt.connect(clientId.c_str()))
        {
            mqtt.subscribe(subTopic);
        }
        else
        {
            if (attempt >= maxAttempts)
            {
                Environment::print("MQTT reconnect failed after max attempts");
                retry_count = attempt;
                break;
            }
            attempt++;
            Environment::print("MQTT reconnect failed, retrying...");
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
    if (wifiIsConnected())
    {
        subTopic = subscribeTopic;

        mqtt.setServer(host, port);
        mqtt.setCallback(mqttCallback);

        mqttReconnect();
    }
}

void mqttLoop()
{
    if (!mqtt.connected() && wifiIsConnected())
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