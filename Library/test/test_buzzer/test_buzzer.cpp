#include <Arduino.h>
#include <unity.h>
#include <Buzzer.h>

// Mock for pinMode and digitalWrite
uint8_t mock_pin = 0;
bool mock_pin_state = false;
uint8_t mock_pin_mode = 0;

void setUp(void)
{
    mock_pin = 0;
    mock_pin_state = false;
    mock_pin_mode = 0;
}

void tearDown(void)
{
}

void test_buzzer_initialization()
{
    // Test that buzzer initializes with pin and sets it to LOW
    Buzzer::begin(13, "clients/test_device/Buzzer");

    // After initialization, pin should be set to OUTPUT mode and LOW state
    TEST_ASSERT_EQUAL(13, mock_pin);
    TEST_ASSERT_FALSE(mock_pin_state);
}

void test_buzzer_set_on()
{
    // Initialize the buzzer
    Buzzer::begin(13, "clients/test_device/Buzzer");

    // Turn the buzzer on
    Buzzer::set(true, "clients/test_device/Buzzer");

    // Pin should be HIGH
    TEST_ASSERT_TRUE(mock_pin_state);
}

void test_buzzer_set_off()
{
    // Initialize the buzzer
    Buzzer::begin(13, "clients/test_device/Buzzer");

    // Turn the buzzer off
    Buzzer::set(false, "clients/test_device/Buzzer");

    // Pin should be LOW
    TEST_ASSERT_FALSE(mock_pin_state);
}

void test_buzzer_toggle()
{
    // Initialize the buzzer
    Buzzer::begin(13, "clients/test_device/Buzzer");

    // Turn on
    Buzzer::set(true, "clients/test_device/Buzzer");
    TEST_ASSERT_TRUE(mock_pin_state);

    // Turn off
    Buzzer::set(false, "clients/test_device/Buzzer");
    TEST_ASSERT_FALSE(mock_pin_state);

    // Turn on again
    Buzzer::set(true, "clients/test_device/Buzzer");
    TEST_ASSERT_TRUE(mock_pin_state);
}

void test_buzzer_beep()
{
    // Initialize the buzzer
    Buzzer::begin(13, "clients/test_device/Buzzer");

    // Perform a beep
    Buzzer::beep(50);

    // After beep, pin should be back to LOW
    TEST_ASSERT_FALSE(mock_pin_state);
}

void setup()
{
    delay(2000); // allow serial port to come up on some boards
    UNITY_BEGIN();
    RUN_TEST(test_buzzer_initialization);
    RUN_TEST(test_buzzer_set_on);
    RUN_TEST(test_buzzer_set_off);
    RUN_TEST(test_buzzer_toggle);
    RUN_TEST(test_buzzer_beep);
    UNITY_END();
}

void loop() {}
