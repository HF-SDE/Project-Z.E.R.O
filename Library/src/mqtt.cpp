
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
static String lastTopic;

static int max_retry = 20;
static int retry_count = 0;

// ------------------------------------------------------

// MQTT callback
static void mqttCallback(char *topic, byte *payload, unsigned int length)
{
    mqtt.setBufferSize(2048);
    messageReceived = true;
    Environment::print("Event received - callback");

    lastTopic = String(topic);

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

void onMqttMessage(const char *topic, const char *payload)
{
    // Handle incoming MQTT messages here

    Environment::print("MQTT Message received on topic: ");
    Environment::print(topic);
    Environment::print("Payload: ");
    Environment::print(payload);
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

bool doMqttGetLastPayloadInclude(const char *topic, const char *payload, uint32_t windowMs)
{
    windowMs = 1500;
    messageReceived = false;
    payloadBuf = "";

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
        if (messageReceived && payloadBuf.indexOf(String(payload)) >= 0)
            break;
    }

    mqtt.unsubscribe(topic);

    if (!messageReceived)
    {
        Serial.println("No message received");
        return false;
    }

    // Check if the received payload contains or matches the expected payload
    if (payload == nullptr)
        return true; // If no payload to match, just return that we got a message

    // Check if received payload contains the expected payload
    return payloadBuf.indexOf(String(payload)) >= 0;
}

String mqttWaitForMessage(const char *topic, uint32_t windowMs)
{
    windowMs = (windowMs == 0) ? 1500 : windowMs;
    messageReceived = false;
    payloadBuf = "";

    if (!mqtt.connected())
        return "";

    mqtt.setCallback(mqttCallback);

    // Drain any pending messages from previous subscriptions
    Environment::print("Draining pending messages for topic");
    for (int i = 0; i < 10; i++)
    {
        mqtt.loop();
        delay(1);
    }

    if (!mqtt.subscribe(topic))
        return "";

    // Clear flags again after subscribing to ensure clean state
    messageReceived = false;
    payloadBuf = "";

    uint32_t start = millis();

    while ((millis() - start) < windowMs)
    {
        mqtt.loop();
        delay(2);
        if (messageReceived && lastTopic == String(topic))
            break;
    }

    mqtt.unsubscribe(topic);

    // Drain any remaining messages from this subscription
    for (int i = 0; i < 5; i++)
    {
        mqtt.loop();
        delay(1);
    }

    if (!messageReceived)
        return "";

    Environment::print("MQTT message received: " + payloadBuf);
    Environment::print("Last MQTT topic: " + lastTopic);
    return payloadBuf; // Return the received message payload
}

String mqttGetLastTopic()
{
    Environment::print("Last MQTT topic: " + lastTopic);
    return lastTopic;
}