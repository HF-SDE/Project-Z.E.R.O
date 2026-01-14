#include <Arduino.h>
#include <unity.h>

#include <wifi_setup.h>
#include <MqttManager.h>
#include "PubSubClient.h"
#include "WiFi.h"

#include <thread>
#include <chrono>
#include "../include/WiFi.h"
#include "../include/PubSubClient.h"

void test_connect_and_publish()
{
    // Ensure WiFi reports connected for mqttInit
    WiFiClass::s_status = WL_CONNECTED;

    // Initialize mqtt
    mqttInit("testhost", 1883, "", "", "devices/test/#");

    // Mock connect should have succeeded
    TEST_ASSERT_TRUE(mqttIsConnected());

    // Publish should succeed while connected
    TEST_ASSERT_TRUE(mqttPublish("devices/test/value", "123", true));
}

void test_retained_message_detection()
{
    WiFiClass::s_status = WL_CONNECTED;
    mqttInit("testhost", 1883, "", "", "devices/test/#");

    // Start a thread to simulate an incoming retained message shortly after subscribe
    std::thread t([]()
                  {
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        PubSubClient::simulateIncoming(("devices/test/command"), ("hello")); });

    bool exists = retainedMessageExists("devices/test/command", 1500);
    t.join();

    TEST_ASSERT_TRUE(exists);
}

void setup()
{
    delay(1000);
    UNITY_BEGIN();
    RUN_TEST(test_connect_and_publish);
    RUN_TEST(test_retained_message_detection);
    UNITY_END();
}

void loop() {}
