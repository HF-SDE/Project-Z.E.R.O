#include <Arduino.h>
#include <unity.h>
#include <Environment.h>

void test_initial_state()
{
    // Before explicit configuration: getServerIP returns empty but mode defaults to DEVELOPMENT
    TEST_ASSERT_EQUAL(Environment::EnvironmentMode::DEVELOPMENT, Environment::getEnvironmentMode());
    TEST_ASSERT_TRUE(Environment::isDevelopment());
    TEST_ASSERT_FALSE(Environment::isProduction());
    TEST_ASSERT_TRUE(Environment::getServerIP().length() == 0);
}

void test_configure_production()
{
    Environment::configureEnvironment(Environment::EnvironmentMode::PRODUCTION, "1.2.3.4", "5.6.7.8", "9.10.11.12");
    TEST_ASSERT_EQUAL(Environment::EnvironmentMode::PRODUCTION, Environment::getEnvironmentMode());
    TEST_ASSERT_TRUE(Environment::isProduction());
    TEST_ASSERT_EQUAL_STRING("1.2.3.4", Environment::getServerIP().c_str());
}

void test_configure_testing()
{
    Environment::configureEnvironment(Environment::EnvironmentMode::TESTING, "1.2.3.4", "5.6.7.8", "9.10.11.12");
    TEST_ASSERT_EQUAL(Environment::EnvironmentMode::TESTING, Environment::getEnvironmentMode());
    TEST_ASSERT_TRUE(Environment::isTesting());
    TEST_ASSERT_EQUAL_STRING("5.6.7.8", Environment::getServerIP().c_str());
}

void setup()
{
    delay(2000); // allow serial port to come up on some boards
    UNITY_BEGIN();
    RUN_TEST(test_initial_state);
    RUN_TEST(test_configure_production);
    RUN_TEST(test_configure_testing);
    UNITY_END();
}

void loop() {}
