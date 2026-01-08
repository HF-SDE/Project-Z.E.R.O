#include <Arduino.h>
#include "Button.h"
#include "LightSensor.h"
#include "Led.h"

// Pins
constexpr uint8_t LIGHT_SENSOR_PIN = 36;
constexpr uint8_t BUTTON_PIN = 21;
constexpr uint8_t LED_PIN = 22;

// Hysteresis thresholds
constexpr int THRESH_ON = 3100;
constexpr int THRESH_OFF = 3300;

// Debounce
constexpr uint32_t DEBOUNCE_MS = 50;

// App state
static bool overwrite = false;
static bool ledState = false;

static bool computeLedState(int lightValue)
{
  if (overwrite)
  {
    ledState = true;
    return ledState;
  }

  if (!ledState && lightValue < THRESH_ON)
  {
    ledState = true;
  }
  else if (ledState && lightValue > THRESH_OFF)
  {
    ledState = false;
  }

  return ledState;
}

void setup()
{
  Serial.begin(9600);

  Led::begin(LED_PIN);
  LightSensor::begin(LIGHT_SENSOR_PIN);

  // For INPUT_PULLUP wiring (button -> GND), press is FALLING.
  Button::begin(BUTTON_PIN, DEBOUNCE_MS, Button::Edge::Falling);
}

void loop()
{
  if (Button::consumePress())
  {
    overwrite = !overwrite;
  }

  const int lightValue = LightSensor::read();
  Led::set(computeLedState(lightValue));

  delay(5);
}
