#include <Arduino.h>
#include <unity.h>
#include <StorageManager.h>
#include <Environment.h>

void test_clean_start()
{
    // Ensure storage is initialized and clean
    TEST_ASSERT_TRUE(storageInit());
    storageDeleteConfig();
    TEST_ASSERT_FALSE(storageConfigExists());
}

void test_save_and_load()
{
    DeviceConfig cfg;
    cfg.wifiSsid = "TestSSID";
    cfg.wifiPassword = "TestPass";
    cfg.serialFrequency = 9600;
    cfg.mqttHost = "test.mqtt";
    cfg.mqttPort = 1883;
    cfg.mqttUser = "user";
    cfg.mqttPassword = "pass";
    cfg.mqttTopic = "clients/test/#";
    cfg.deviceId = "TESTDEVICE";
    cfg.heartbeatInterval = 5000;

    TEST_ASSERT_TRUE(storageSaveConfig(cfg));
    TEST_ASSERT_TRUE(storageConfigExists());

    DeviceConfig loaded;
    TEST_ASSERT_TRUE(storageLoadConfig(loaded));

    TEST_ASSERT_EQUAL_STRING(cfg.wifiSsid.c_str(), loaded.wifiSsid.c_str());
    TEST_ASSERT_EQUAL_STRING(cfg.wifiPassword.c_str(), loaded.wifiPassword.c_str());
    TEST_ASSERT_EQUAL(cfg.serialFrequency, loaded.serialFrequency);
    TEST_ASSERT_EQUAL_STRING(cfg.mqttHost.c_str(), loaded.mqttHost.c_str());
    TEST_ASSERT_EQUAL(cfg.mqttPort, loaded.mqttPort);
    TEST_ASSERT_EQUAL_STRING(cfg.mqttUser.c_str(), loaded.mqttUser.c_str());
    TEST_ASSERT_EQUAL_STRING(cfg.mqttPassword.c_str(), loaded.mqttPassword.c_str());
    TEST_ASSERT_EQUAL_STRING(cfg.mqttTopic.c_str(), loaded.mqttTopic.c_str());
    TEST_ASSERT_EQUAL_STRING(cfg.deviceId.c_str(), loaded.deviceId.c_str());
    TEST_ASSERT_EQUAL(cfg.heartbeatInterval, loaded.heartbeatInterval);

    // Print should not crash
    storagePrintConfig(loaded);
}

void test_delete_and_format()
{
    TEST_ASSERT_TRUE(storageDeleteConfig());
    TEST_ASSERT_FALSE(storageConfigExists());
    // Formatting filesystem should succeed
    TEST_ASSERT_TRUE(storageFormat());
}

void setup()
{
    delay(1000);
    UNITY_BEGIN();
    RUN_TEST(test_clean_start);
    RUN_TEST(test_save_and_load);
    RUN_TEST(test_delete_and_format);
    UNITY_END();
}

void loop() {}
