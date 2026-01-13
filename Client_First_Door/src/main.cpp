#include <Arduino.h>
#include <vector>
#include <Wire.h>
#include <SPI.h>
#include <MFRC522v2.h>
#include <MFRC522DriverSPI.h>
#include <MFRC522DriverPinSimple.h>
#include <MFRC522Debug.h>
#include <Environment.h>
#include <StorageManager.h>
#include <Config.h>
#include <ConfigWriter.h>
#include <MqttManager.h>
#include <Led.h>
#include <button.h>
#include <wifi_setup.h>
#include <DisplayManager.h>
#include <StatusLedManager.h>

// ---------------------- Global Config ----------------------
DeviceConfig config;

// ---------------------- WiFi Config ----------------------
const String WIFI_SSID = "Case-ZERO_2,4GHz";
const String WIFI_PASSWORD = "Nogetjegkanhuske";

// ---------------------- Component Config ----------------------
const String components[9] = {
    "LCD",
    "Overwrite_Button",
    "Blue_Status_LED",
    "Green_Status_LED",
    "Red_Status_LED",
    "White_LED",
    "RFID_Reader",
    "PmodKYPD_Keypad",
    "Buzzer"};

std::vector<ConfigComponents::keys> component_config = {
    {"LCD_Text", 0, "", "text", {"Light Sensor"}},
    {"LCD_Color", 0, "", "color", {"Light Sensor"}},
    {"Overwrite_Button", 0, "boolean", {"Button State"}},
    {"Blue_Status_LED", 0, "rgb_light_state", {"RGB Light State"}},
    {"White_LED", 0, "boolean", {"White LED State"}}};

// ---------------------- Pin Map (per schematic) ----------------------
// I2C bus (PCF8574 -> LCD)
constexpr uint8_t I2C_SDA_PIN = 21; // SDA
constexpr uint8_t I2C_SCL_PIN = 22; // SCL

// MFRC522 RFID reader on VSPI
constexpr uint8_t SPI_MOSI_PIN = 23;   // VSPI MOSI
constexpr uint8_t SPI_MISO_PIN = 19;   // VSPI MISO
constexpr uint8_t SPI_SCK_PIN = 18;    // VSPI SCK
constexpr uint8_t MFRC522_SS_PIN = 5;  // MFRC522 SDA/SS (chip select)
constexpr uint8_t MFRC522_RST_PIN = 4; // MFRC522 RST

// Keypad (PmodKYPD)
// Rows are inputs (input-only GPIOs are used intentionally)
constexpr uint8_t KYPD_ROW1 = 36; // ROW1
constexpr uint8_t KYPD_ROW2 = 39; // ROW2
constexpr uint8_t KYPD_ROW3 = 34; // ROW3
constexpr uint8_t KYPD_ROW4 = 35; // ROW4

// Columns are driven as outputs
constexpr uint8_t KYPD_COL1 = 32; // COL1
constexpr uint8_t KYPD_COL2 = 33; // COL2
constexpr uint8_t KYPD_COL3 = 25; // COL3
constexpr uint8_t KYPD_COL4 = 26; // COL4

// White LED (external, with 220Ω series resistor)
constexpr uint8_t WHITE_LED_PIN = 2; // LED anode via resistor -> GPIO2

// Status indicator (three LEDs with 220Ω resistors)
// Adjust if needed to match your actual wiring colours.
constexpr uint8_t STATUS_LED_RED_PIN = 16;
constexpr uint8_t STATUS_LED_GREEN_PIN = 17;
constexpr uint8_t STATUS_LED_BLUE_PIN = 27;

// MFRC522 and LCD instances
MFRC522DriverPinSimple ss_pin(MFRC522_SS_PIN);
MFRC522DriverSPI driver{ss_pin};
MFRC522 rfid{driver};

// MQTT
const String HOST_DEV = "192.168.1.147";
const String HOST_TEST = "192.168.1.147";
const String HOST_PROD = "192.168.1.5";

// Hysteresis thresholds
constexpr int THRESH_ON = 3100;
constexpr int THRESH_OFF = 3300;

// Debounce
constexpr uint32_t DEBOUNCE_MS = 50;

bool firstWifiStartUp = true;

static void initConfig()
{
  Environment::print("Initializing Environment...");
  Environment::configureEnvironment(Environment::EnvironmentMode::DEVELOPMENT, HOST_DEV, HOST_TEST, HOST_PROD);

  displayInit(0x27, 16, 2);
  statusLedInit(STATUS_LED_RED_PIN, STATUS_LED_GREEN_PIN, STATUS_LED_BLUE_PIN);

  Environment::print("Initializing storage...");
  if (!storageInit())
  {
    Environment::print("Failed to initialize storage!");
    // Blink red LED to indicate error
    for (int i = 0; i < 10; i++)
    {
      digitalWrite(STATUS_LED_RED_PIN, HIGH);
      digitalWrite(STATUS_LED_GREEN_PIN, LOW);
      digitalWrite(STATUS_LED_BLUE_PIN, LOW);
      delay(200);
      digitalWrite(STATUS_LED_RED_PIN, LOW);
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
      digitalWrite(STATUS_LED_RED_PIN, HIGH);
      digitalWrite(STATUS_LED_GREEN_PIN, LOW);
      digitalWrite(STATUS_LED_BLUE_PIN, LOW);
      delay(200);
      digitalWrite(STATUS_LED_RED_PIN, LOW);
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

static void initComponentConfig()
{
  String basicTopic = "devices/" + config.deviceId + "/";

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

static inline void setupKeypadPins()
{
  // Rows sit on input-only pins (34–39) so they cannot use internal pull-ups; add external pull-ups on the board
  pinMode(KYPD_ROW1, INPUT);
  pinMode(KYPD_ROW2, INPUT);
  pinMode(KYPD_ROW3, INPUT);
  pinMode(KYPD_ROW4, INPUT);

  // Columns as outputs; start HIGH (inactive)
  pinMode(KYPD_COL1, OUTPUT);
  pinMode(KYPD_COL2, OUTPUT);
  pinMode(KYPD_COL3, OUTPUT);
  pinMode(KYPD_COL4, OUTPUT);
  digitalWrite(KYPD_COL1, HIGH);
  digitalWrite(KYPD_COL2, HIGH);
  digitalWrite(KYPD_COL3, HIGH);
  digitalWrite(KYPD_COL4, HIGH);
}

void setup()
{
  Serial.begin(115200);
  while (!Serial)
  {
    ; // Wait for Serial to be ready
  }

  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
  SPI.begin(SPI_SCK_PIN, SPI_MISO_PIN, SPI_MOSI_PIN);

  // LEDs
  pinMode(WHITE_LED_PIN, OUTPUT);
  Led::begin(WHITE_LED_PIN, "devices/" + String(config.deviceId.c_str()) + "/White_LED");
  pinMode(STATUS_LED_RED_PIN, OUTPUT);
  pinMode(STATUS_LED_GREEN_PIN, OUTPUT);
  pinMode(STATUS_LED_BLUE_PIN, OUTPUT);
  digitalWrite(STATUS_LED_RED_PIN, LOW);
  digitalWrite(STATUS_LED_GREEN_PIN, LOW);
  digitalWrite(STATUS_LED_BLUE_PIN, LOW);

  wifiInitStatusLed(STATUS_LED_RED_PIN, STATUS_LED_GREEN_PIN, STATUS_LED_BLUE_PIN);

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
  }

  // RFID control pins
  pinMode(MFRC522_RST_PIN, OUTPUT);
  digitalWrite(MFRC522_RST_PIN, HIGH); // keep out of reset

  // Keypad
  setupKeypadPins();

  // Initialize MFRC522 RFID
  rfid.PCD_Init();
  MFRC522Debug::PCD_DumpVersionToSerial(rfid, Serial);
  Serial.println("MFRC522 v2 initialized.");

  // Initialize LCD
  displayOverrideLine(0, "System Initialized");
  displayOverrideLine(1, "System Ready");
  Serial.println("LCD initialized.");

  Serial.println("Pins configured per schematic.");
}

// Keypad scan: returns the key pressed, or 0 if none
static char scanKeypad()
{
  const uint8_t rows[4] = {KYPD_ROW1, KYPD_ROW2, KYPD_ROW3, KYPD_ROW4};
  const uint8_t cols[4] = {KYPD_COL1, KYPD_COL2, KYPD_COL3, KYPD_COL4};
  const char keys[4][4] = {
      {'1', '2', '3', 'A'},
      {'4', '5', '6', 'B'},
      {'7', '8', '9', 'C'},
      {'*', '0', '#', 'D'}};

  for (uint8_t col = 0; col < 4; col++)
  {
    digitalWrite(cols[col], LOW); // Activate column
    delayMicroseconds(100);

    for (uint8_t row = 0; row < 4; row++)
    {
      if (digitalRead(rows[row]) == LOW) // Row pulled low?
      {
        digitalWrite(cols[col], HIGH); // Deactivate
        delayMicroseconds(20);         // Debounce
        if (digitalRead(rows[row]) == LOW)
        { // Confirm press
          return keys[row][col];
        }
      }
    }
    digitalWrite(cols[col], HIGH); // Deactivate column
  }
  return 0;
}

void loop()
{
  static unsigned long lastLedCycle = 0;
  static unsigned long lastKeypadScan = 0;
  unsigned long now = millis();

  // Cycle status LED every 1 second (test)
  if (now - lastLedCycle >= 1000)
  {
    lastLedCycle = now;
    static uint8_t ledState = 0;
    ledState = (ledState + 1) % 3;
    digitalWrite(STATUS_LED_RED_PIN, ledState == 0 ? HIGH : LOW);
    digitalWrite(STATUS_LED_GREEN_PIN, ledState == 1 ? HIGH : LOW);
    digitalWrite(STATUS_LED_BLUE_PIN, ledState == 2 ? HIGH : LOW);
  }

  // Scan keypad every 50ms
  if (now - lastKeypadScan >= 50)
  {
    lastKeypadScan = now;
    char key = scanKeypad();
    if (key)
    {
      Serial.print("Key pressed: ");
      Serial.println(key);
      displayOverrideLine(1, ("Key: " + String(key)).c_str());
    }
  }

  // Check RFID
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial())
  {
    Serial.print("RFID Card UID: ");
    for (byte i = 0; i < rfid.uid.size; i++)
    {
      if (rfid.uid.uidByte[i] < 0x10)
        Serial.print("0");
      Serial.print(rfid.uid.uidByte[i], HEX);
    }
    Serial.println();
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
  }

  delay(5);
}