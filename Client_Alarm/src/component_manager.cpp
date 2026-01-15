#include "../lib/ComponentManager.h"
#include "../lib/MqttManager.h"
#include <ArduinoJson.h>

#define MAX_COMPONENTS 10

static Component *g_components[MAX_COMPONENTS];
static int g_componentCount = 0;
static String g_deviceId = "";
static String mqttBaseTopic = "";

void componentManagerInit(String deviceId, String baseMqttTopic)
{
    g_deviceId = deviceId;
    mqttBaseTopic = baseMqttTopic;
    g_componentCount = 0;
}

void componentRegister(Component *component)
{
    if (g_componentCount >= MAX_COMPONENTS)
        return;

    g_components[g_componentCount++] = component;
}

void componentUpdateValue(Component *component, const String &newValue)
{
    if (component->value != newValue)
    {
        component->value = newValue;
        component->stateChanged = true; // Mark component as changed

        // Trigger publish of all component states
        componentPublishAll();
    }
}

void componentPublishAll()
{
    if (!mqttIsConnected())
        return;

    char topic[128];
    char payload[512];

    for (int i = 0; i < g_componentCount; i++)
    {
        Component *comp = g_components[i];

        if (comp->stateChanged) // Only publish if state has changed
        {
            // Build topic: client/[id]/components/[component_type]
            snprintf(topic, sizeof(topic), "%s/components/%s", mqttBaseTopic.c_str(), comp->type);

            // Build JSON payload
            StaticJsonDocument<256> doc;
            doc["value"] = comp->value;
            doc["value_type"] = comp->valueType;

            JsonObject configObj = doc.createNestedObject("config");
            configObj["name"] = comp->config.name;
            if (comp->config.comment != nullptr)
            {
                configObj["comment"] = comp->config.comment;
            }

            // Serialize to string
            serializeJson(doc, payload, sizeof(payload));

            // Publish to MQTT
            mqttPublish(topic, payload, true); // retained = true

            comp->stateChanged = false; // Reset stateChanged flag
        }
    }
}
