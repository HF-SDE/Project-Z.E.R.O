# Client_Alarm - ESP32 MQTT Alarm Display

ESP32-based alarm client for Project Z.E.R.O. Receives alarm triggers via MQTT and displays messages on a 20x2 LCD with visual (RGB LED) and audio (buzzer) feedback.

## Features

- **WiFi Connectivity** with visual status indicator (RGB LED)
- **MQTT Subscribe** to alarm triggers on wildcard topic
- **20x2 LCD Display** for alarm messages (I2C)
- **Buzzer Control** for audio alerts
- **JSON Message Parsing** for flexible alarm configurations
- **Retained Status Publishing** for device health monitoring

## Hardware Requirements

### Components

- ESP32 Development Board
- 20x4 LCD Display with I2C backpack (address 0x27)
- RGB LED for WiFi status indicator
- Resistors (220 ohm for LEDs)
- Red LED for alarm indicator
- Buzzer
- Jumper wires
- Breadboard

### Pin Connections

| Component | ESP32 Pin |
| --------- | --------- |
| LCD SDA   | GPIO 21   |
| LCD SCL   | GPIO 22   |
| Red LED   | GPIO 2    |
| Green LED | GPIO 4    |
| Blue LED  | GPIO 0    |
| Buzzer    | GPIO 32   |
| Alarm LED | GPIO 26   |

## Software Requirements

- [PlatformIO](https://platformio.org/) (VS Code extension or CLI)
- ESP32 board package
- Dependencies (auto-installed via platformio.ini):
  - WiFi (built-in)
  - LiquidCrystal_I2C ^1.1.4
  - PubSubClient ^2.8.0
  - ArduinoJson ^7.4.2

## Installation

### 1. Configure WiFi/MQTT

Edit [src/main.cpp](src/main.cpp) (lines 11-19):

```cpp
const char *WIFI_SSID = "your-wifi-ssid";
const char *WIFI_PASSWORD = "your-wifi-password";
const char *MQTT_HOST = "192.168.1.143";  // Your MQTT broker IP
```

### 2. Build & Upload

```bash
# Build the project
platformio run

# Upload to ESP32
platformio run --target upload

# Monitor serial output
platformio device monitor
```

Or use PlatformIO IDE buttons: **Build** → **Upload** → **Monitor**

## Usage

### Startup process

1. ESP32 boots and initializes peripherals.
2. Sets up Serial communication at 115200 bps.
3. Inits components: LCD, RGB LED, Buzzer.
4. Reads config from disk (LittleFS).
5. Changes Serial frequency if config differs.
6. Connects to WiFi, shows status via RGB LED.
7. Connects to MQTT broker, starts heartbeat publishing, and subscribes to all topics under its deviceId topic.
8. Registering MQTT message event handlers.
9. Setup loop to keep sending heartbeat and update WiFi status LED.

### MQTT Topics

#### Subscribe (Device listens to):

- `devices/1/triggers/#` - Wildcard for all trigger types
  - `devices/1/triggers/alarm-trigger` - Activate alarm
  - `devices/1/triggers/clear-alarm-trigger` - Clear alarm

#### Publish (Device sends):

- `devices/1/status` - "online" every 60 seconds (retained)

### Alarm Message Format

Send JSON payload to `devices/1/triggers/alarm-trigger`:

```json
{
  "message": "Fire detected in Zone A! Evacuate immediately.",
  "color": "red",
  "useSound": true
}
```

**Fields:**

- `message` (string): Text to display (40 chars max for 20x2 LCD)
- `color` (string): LED color - `"red"`, `"green"`, or `"white"` _(Note: currently forces red)_
- `useSound` (boolean): Enable/disable buzzer

### Clear Alarm

Send any payload to `devices/1/triggers/clear-alarm-trigger`:

```json
{}
```

## Project Structure

```
Client_Alarm/
├── lib/                      # Header files (interfaces)
│   ├── alarm_message.h       # JSON parsing structure
│   ├── DisplayManager.h      # LCD control API
│   ├── MqttManager.h         # MQTT client wrapper
│   └── wifi_setup.h          # WiFi connection API
├── src/                      # Implementation files
│   ├── alarm_message.cpp     # ArduinoJson parsing
│   ├── display.cpp           # LCD I2C driver
│   ├── main.cpp              # Setup/loop orchestration
│   ├── mqtt.cpp              # MQTT reconnect & callbacks
│   └── wifi.cpp              # WiFi connection & status LED
├── platformio.ini            # PlatformIO config
└── README.md                 # This file
```

## Debugging

### Serial Monitor

Connect at **115200 baud** to see:

- WiFi connection status
- IP address assignment
- MQTT connection attempts
- Received topics and payloads
- JSON parsing errors

### LED Status Indicators

- **Blue pulsing**: Connecting to WiFi
- **Green solid**: WiFi connected
- **Red**: WiFi failed (or alarm active)

### Common Issues

**"WiFi failed"**

- Check SSID/password in [main.cpp](src/main.cpp)
- Verify 2.4GHz network (ESP32 doesn't support 5GHz)

**"MQTT not connecting"**

- Verify broker IP is reachable: `ping 192.168.1.143`
- Check broker allows anonymous connections
- Confirm broker port is 1883

**"LCD shows garbage"**

- Verify I2C address with `i2cdetect` (usually 0x27 or 0x3F)
- Check SDA/SCL wiring
- Adjust LCD contrast potentiometer

**"Upload failed"**

- Hold BOOT button on ESP32 during upload
- Check USB cable supports data transfer
- Verify correct COM/tty port in PlatformIO

## Known Limitations

- Display color API not fully implemented (always shows red for alarms)
- Configuration is hardcoded (no web interface or EEPROM storage)
- Single MQTT message handler (both alarm types share one callback)
- No OTA (over-the-air) update support

## License

Part of Project Z.E.R.O. - Educational project for H5 course.

## Contributing

This is an educational project. For questions or improvements, contact the development team.
