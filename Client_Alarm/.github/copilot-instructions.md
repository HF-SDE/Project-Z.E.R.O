# Project-Z.E.R.O Client Alarm - Copilot Instructions

## Overview
ESP32-based alarm client for the Z.E.R.O. project. Connects to WiFi, subscribes to MQTT broker, displays alarm messages on 20x2 I2C LCD, and controls LEDs/buzzer based on incoming triggers.

## Architecture

### Component Organization
- **lib/** - Header-only interfaces for all managers (DisplayManager, MqttManager, wifi_setup, alarm_message)
- **src/** - Implementation files (*.cpp) that include headers from `../lib/`
- **main.cpp** - Orchestrates WiFi → MQTT → Event handlers. Single `setup()` initializes all systems, `loop()` maintains MQTT connection and publishes heartbeat

### Data Flow
1. WiFi connects with visual status feedback via RGB LED
2. MQTT subscribes to `devices/1/triggers/#` wildcard
3. Two handlers registered:
   - `onMqttAlarmMessage()` - topic: `devices/1/triggers/alarm-trigger` → parses JSON, displays message, activates buzzer
   - `onMqttClearAlarmMessage()` - topic: `devices/1/triggers/clear-alarm-trigger` → resets display/buzzer
4. Loop publishes `devices/1/status: "online"` every 60 seconds with retained flag

### JSON Message Format
```json
{
  "message": "Alert text (40 chars for 20x2 LCD)",
  "color": "red|green|white",
  "useSound": true|false
}
```
Parsed in [alarm_message.cpp](../src/alarm_message.cpp) using ArduinoJson StaticJsonDocument<256>

## Build System (PlatformIO)

### Commands
- **Build**: `platformio run` or PlatformIO IDE "Build" button
- **Upload**: `platformio run --target upload` (auto-detects ESP32 on USB)
- **Monitor**: `platformio device monitor` (115200 baud)
- **Clean**: `platformio run --target clean`

### Configuration ([platformio.ini](../platformio.ini))
- Board: `esp32dev` (generic ESP32)
- Framework: Arduino
- Key dependencies: WiFi (built-in), LiquidCrystal_I2C, PubSubClient (MQTT), ArduinoJson

## Project-Specific Conventions

### Include Patterns
Always use relative paths from src/ to lib/: `#include <../lib/DisplayManager.h>`
Do NOT use standard library-style includes for project headers.

### State Management
Each manager (MQTT, Display, WiFi) maintains internal `static` state in its .cpp file:
- `mqtt.cpp` - `static WiFiClient wifiClient`, `static PubSubClient mqtt`
- `display.cpp` - `static LiquidCrystal_I2C lcd`, `static uint8_t g_cols/g_rows`
- WiFi status LED state in wifi.cpp

Managers expose only high-level APIs in headers - no global state leaks.

### Handler Registration Pattern
```cpp
mqttSetMessageHandler(onMqttAlarmMessage);
mqttSetMessageHandler(onMqttClearAlarmMessage);
```
Both handlers must be registered; MQTT callback in [mqtt.cpp:12-25](../src/mqtt.cpp#L12-L25) invokes them.

### LCD Text Handling
20x2 display splits message at 20 chars automatically ([display.cpp:56-74](../src/display.cpp#L56-L74)).
Always pad with spaces to overwrite remnants of previous messages.

## Hardware Pinout
- **LCD**: I2C (0x27) on GPIO 21 (SDA), 22 (SCL)
- **LEDs**: Red=GPIO2, Green=GPIO4, Blue=GPIO0 (WiFi status)
- **Buzzer**: GPIO32 (HIGH=on for alarms)

## WiFi/MQTT Configuration
Edit [main.cpp:11-19](../src/main.cpp#L11-L19):
```cpp
const char *WIFI_SSID = "Case-ZERO_2,4GHz";
const char *MQTT_HOST = "192.168.1.143";
const char *MQTT_TOPIC = "devices/1/triggers/#";
```
No credentials for MQTT (local broker assumed).

## Debugging
- Serial output at 115200 baud shows connection status, received topics/payloads
- RGB LED states: Blue pulsing = connecting WiFi, Green solid = connected
- Check `monitor_speed = 115200` matches Serial.begin() in [main.cpp:43](../src/main.cpp#L43)

## Known Limitations
- Display color API ([display.cpp:92-106](../src/display.cpp#L92-L106)) only supports on/off - RGB backlight not implemented
- No persistent storage - all config hardcoded
- Alarm clear handler bug: re-registers both handlers on every clear event ([main.cpp:117-118](../src/main.cpp#L117-L118))
