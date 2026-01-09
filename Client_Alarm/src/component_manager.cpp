#include "../lib/ComponentManager.h"
#include "../lib/MqttManager.h"
#include <ArduinoJson.h>

#define MAX_COMPONENTS 10

static Component *g_components[MAX_COMPONENTS];
static int g_componentCount = 0;
static String g_deviceId = "";

void componentManagerInit(String deviceId)
{
    g_deviceId = deviceId;
    g_componentCount = 0;
    Serial.println("Component Manager initialized");
}

void componentRegister(Component *component)
{
    if (g_componentCount >= MAX_COMPONENTS)
    {
        Serial.println("ERROR: Max components reached!");
        return;
    }

    g_components[g_componentCount++] = component;
    Serial.print("Registered component: ");
    Serial.print(component->config.name);
    Serial.print(" (");
    Serial.print(component->type);
    Serial.println(")");
}

void componentUpdateValue(Component *component, const String &newValue)
{
    if (component->value != newValue)
    {
        component->value = newValue;
        component->stateChanged = true; // Mark component as changed
        Serial.print("Component updated: ");
        Serial.print(component->config.name);
        Serial.print(" = ");
        Serial.println(newValue);

        // Trigger publish of all component states
        componentPublishAll();
    }
}

void componentPublishAll()
{
    if (!mqttIsConnected())
    {
        Serial.println("MQTT not connected, skipping component publish");
        return;
    }

    char topic[128];
    char payload[512];

    for (int i = 0; i < g_componentCount; i++)
    {
        Component *comp = g_components[i];

        if (comp->stateChanged) // Only publish if state has changed
        {
            // Build topic: devices/[id]/components/[component_type]
            snprintf(topic, sizeof(topic), "devices/%d/components/%s", g_deviceId, comp->type);
            Serial.print("Publishing component ");
            Serial.print(comp->config.name);
            Serial.print(" to topic ");
            Serial.println(topic);

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

            Serial.print("Published to ");
            Serial.print(topic);
            Serial.print(": ");
            Serial.println(payload);

            comp->stateChanged = false; // Reset stateChanged flag
        }
    }
}
