#include <Arduino.h>
#include <Button.h>
#include <vector>
#include <LightSensor.h>
#include <Led.h>
#include <Config.h>
#include <wifi_setup.h>
#include <Wire.h>
#include <PubSubClient.h>
#include <MqttManager.h>
#include <ConfigWriter.h>
#include <StorageManager.h>
#include <Environment.h>
#include <time.h>

// ---------------------- Global Config ----------------------
DeviceConfig config;

// ---------------------- Opening Hours Configuration ----------------------
static String openingHours = "08:00-18:00"; // Format: "HH:MM-HH:MM"

//---------------------- Pin config ----------------------
constexpr uint8_t blueWifiLedPin = 27;
constexpr uint8_t greenWifiLedPin = 26;
constexpr uint8_t redWifiLedPin = 25;
constexpr uint8_t LIGHT_SENSOR_PIN = 33;
constexpr uint8_t BUTTON_PIN = 14;
constexpr uint8_t LED_PIN = 32;

// Wifi
const String WIFI_SSID = "Case-ZERO_2,4GHz";
const String WIFI_PASSWORD = "Nogetjegkanhuske";

// Config component
const String components[4] = {"PhotoResistor", "button", "RGB_Light", "White_LED"};

std::vector<ConfigComponents::keys> component_config = {
    {"PhotoResistor", 0, "light", {"Light Sensor"}},
    {"Button", 0, "boolean", {"Button State"}},
    {"RGB_Light", 0, "rgb_light_state", {"RGB Light State"}},
    {"White_LED", 0, "boolean", {"White LED State"}}};

// MQTT
const String HOST_DEV = "192.168.1.147";
const String HOST_TEST = "192.168.1.147";
const String HOST_PROD = "192.168.1.5";

// Hysteresis thresholds
constexpr int THRESH_ON = 3100;
constexpr int THRESH_OFF = 3300;

// Debounce
constexpr uint32_t DEBOUNCE_MS = 50;

// App state
static bool overwrite = false;
static bool ledState = false;

static void initConfig()
{
  Environment::print("Initializing Environment...");
  Environment::configureEnvironment(Environment::EnvironmentMode::DEVELOPMENT, HOST_DEV, HOST_TEST, HOST_PROD);

  Environment::print("Initializing storage...");
  if (!storageInit())
  {
    Environment::print("Failed to initialize storage!");
    // Blink red LED to indicate error
    for (int i = 0; i < 10; i++)
    {
      digitalWrite(redWifiLedPin, HIGH);
      digitalWrite(greenWifiLedPin, LOW);
      digitalWrite(blueWifiLedPin, LOW);
      delay(200);
      digitalWrite(redWifiLedPin, LOW);
      delay(200);
    }
    return;
  }

  Environment::print("Loading configuration...");
  if (!storageLoadConfig(config))
  {
    Environment::print("Failed to load config! Using defaults and saving...");
    // Blink red LED to indicate config error
    for (int i = 0; i < 10; i++)
    {
      digitalWrite(redWifiLedPin, HIGH);
      digitalWrite(greenWifiLedPin, LOW);
      digitalWrite(blueWifiLedPin, LOW);
      delay(200);
      digitalWrite(redWifiLedPin, LOW);
      delay(200);
    }

    // Write default config and try again
    if (!writeDefaultConfig(Environment::getServerIP(), WIFI_SSID, WIFI_PASSWORD) || !storageLoadConfig(config))
    {
      Environment::print("Failed to create default config!");
      return;
    }
  }
  storagePrintConfig(config);
}

static bool isWithinOpeningHours()
{
  if (openingHours == "")
  {
    return true; // No restrictions if not configured
  }

  // Get current time
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo))
  {
    Environment::print("Failed to obtain time");
    return true; // Allow operation if time unavailable
  }

  // Parse opening hours "HH:MM-HH:MM"
  int dashPos = openingHours.indexOf('-');
  if (dashPos < 0)
  {
    return true; // Invalid format, allow operation
  }

  String startTimeStr = openingHours.substring(0, dashPos);
  String endTimeStr = openingHours.substring(dashPos + 1);

  int startHour = startTimeStr.substring(0, 2).toInt();
  int startMin = startTimeStr.substring(3, 5).toInt();
  int endHour = endTimeStr.substring(0, 2).toInt();
  int endMin = endTimeStr.substring(3, 5).toInt();

  int currentMinutes = timeinfo.tm_hour * 60 + timeinfo.tm_min;
  int startMinutes = startHour * 60 + startMin;
  int endMinutes = endHour * 60 + endMin;

  Environment::print("Current time: " + String(timeinfo.tm_hour) + ":" + String(timeinfo.tm_min));
  Environment::print("Opening hours: " + openingHours);

  return (currentMinutes >= startMinutes && currentMinutes <= endMinutes);
}

static bool computeLedState(int lightValue)
{
  // Check if within opening hours
  if (!isWithinOpeningHours())
  {
    ledState = false;
    return false;
  }

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

static void initComponentConfig()
{
  String basicTopic = "clients/" + config.deviceId + "/";

  for (const auto &p : component_config)
  {
    String componentTopic = basicTopic + String(p.id.c_str());
    String payload = "{\"name\":\"" + String(p.config.name.c_str()) + "\"}";

    bool value_typeExists = retainedMessageExists(String(componentTopic + "/value_type").c_str());
    if (value_typeExists)
    {
      Environment::print(componentTopic + ": Retained message exists");
    }
    else
    {
      Environment::print(componentTopic + ": No retained message on topic");
      mqttPublish(String(componentTopic + "/value_type").c_str(), p.value_type.c_str(), true);
    }

    bool valueExists = retainedMessageExists(String(componentTopic + "/value").c_str());
    if (valueExists)
    {
      Environment::print(componentTopic + ": Retained message exists");
    }
    else
    {
      Environment::print(componentTopic + ": No retained message on topic");
      mqttPublish(String(componentTopic + "/value").c_str(), "1", true);
    }

    bool configExists = retainedMessageExists(String(componentTopic + "/config").c_str());
    if (configExists)
    {
      Environment::print(componentTopic + ": Retained message exists");
    }
    else
    {
      Environment::print(componentTopic + ": No retained message on topic");
      mqttPublish(String(componentTopic + "/config").c_str(), payload.c_str(), true);
    }
  }
}

void setup()
{
  Serial.begin(115200);
  while (!Serial)
  {
    ; // Wait for Serial to be ready
  }

  pinMode(redWifiLedPin, OUTPUT);
  pinMode(blueWifiLedPin, OUTPUT);
  pinMode(greenWifiLedPin, OUTPUT);

  Led::begin(LED_PIN, "clients/" + String(config.deviceId.c_str()) + "/White_LED");
  LightSensor::begin(LIGHT_SENSOR_PIN);

  // For INPUT_PULLUP wiring (button -> GND), press is FALLING.
  Button::begin(BUTTON_PIN, DEBOUNCE_MS, Button::Edge::Falling);

  wifiInitStatusLed(redWifiLedPin, greenWifiLedPin, blueWifiLedPin);

  initConfig();

  wifiSetup(config);

  mqttInit(
      config.mqttHost.c_str(),
      config.mqttPort,
      config.mqttUser.isEmpty() ? nullptr : config.mqttUser.c_str(),
      config.mqttPassword.isEmpty() ? nullptr : config.mqttPassword.c_str(),
      config.mqttTopic.c_str());

  mqttSetMessageHandler(onMqttMessage);
  if (wifiIsConnected() && mqttIsConnected())
  {
    initComponentConfig();

    // Initialize NTP time
    configTime(0, 0, "pool.ntp.org", "time.nist.gov");
    Environment::print("Waiting for NTP time sync...");
    struct tm timeinfo;
    int retries = 0;
    while (!getLocalTime(&timeinfo) && retries < 10)
    {
      delay(500);
      Environment::print(".");
      retries++;
    }
    if (getLocalTime(&timeinfo))
    {
      Environment::print("Time synchronized: " + String(asctime(&timeinfo)));
    }
  }
}

void loop()
{
  static unsigned long lastHeartbeat = 0;

  updateWifiStatusLed(false);
  mqttLoop();

  unsigned long currentMillis = millis();
  if (currentMillis - lastHeartbeat >= config.heartbeatInterval)
  {
    String statusTopic = "clients/" + config.deviceId + "/status";
    mqttPublish(statusTopic.c_str(), "online", true);
    lastHeartbeat = currentMillis;
  }

  if (Button::consumePress())
  {
    overwrite = !overwrite;
  }

  const int lightValue = LightSensor::read("clients/" + String(config.deviceId.c_str()) + "/PhotoResistor");
  Led::set(computeLedState(lightValue), "clients/" + String(config.deviceId.c_str()) + "/White_LED");

  delay(5);
}