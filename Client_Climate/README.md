# Client_Climate (ESP8266)

Project-Z.E.R.O climate client that reads temperature + humidity (DHT22), publishes measurements via MQTT, and triggers the alarm client on sensor failures or when a threshold is exceeded.

## Overview

On boot the client fetches configuration via MQTT and writes it to its own filesystem, making it available offline.

In practice:

- The device boots and loads configuration from LittleFS (`/config.json`).
- If no config exists, it writes a default config.
- When online and connected to MQTT, it subscribes to config topics.
- If your broker/backend publishes config as _retained messages_, the device will receive them and persist them to `/config.json`.

## Hardware

- Board: ESP8266 (PlatformIO env: `esp_wroom_02`)
- Sensor: DHT22 (temperature + humidity)
- Filesystem: LittleFS

GPIO mapping (as implemented in code):

- DHT22 data: GPIO2 (`DHTPIN 2`)
- Blue LED: GPIO12
- Yellow LED: GPIO14
- Red LED: GPIO16

## Status LEDs

The client has 3 LEDs that indicate its status:

- Blue:
  - Blinking: Connecting to WiFi
  - Solid: Connected to WiFi
- Yellow:
  - Blinking: Connecting to MQTT
  - Solid: Connected to MQTT
- Red:
  - Solid: Sensor read error

## Alarm behavior

If the client fails to read the sensors, it publishes a message to the alarm client saying the sensors could not be read.

Additionally:

- If the temperature exceeds `max_value`, the alarm client is triggered.
- If the temperature is below (or equal to) `max_value`, a “clear” message is sent.

Note: The alarm client ID is hardcoded in the firmware as `alarmClientId = "000094A4E48E0D84"`.

## Configuration

The client supports these runtime settings (both can be changed live via MQTT):

- `frequency`: how often the client reads sensors and publishes data (milliseconds)
- `max_value` (per sensor): threshold for triggering the alarm client

Configuration is persisted in LittleFS as `/config.json`.

### Default config

If no config exists on the filesystem, the device writes a default config (see [src/config_writer.cpp](src/config_writer.cpp)).

Important: Default WiFi SSID/password and MQTT host/port are hardcoded. In a real setup you should change them, or ensure your broker/backend publishes the correct config as retained messages shortly after the device comes online.

## MQTT

### Base topic

The client operates under a base topic stored in config:

- `mqttTopic` (typically `clients/<deviceId>`)

`deviceId` is derived from the ESP8266 chip ID (hex).

### Published topics

Types (retained):

- `<mqttTopic>/temperature/value_type` → `temperature`
- `<mqttTopic>/humidity/value_type` → `humidity`

Measurements (retained):

- `<mqttTopic>/temperature/value` → e.g. `23.40`
- `<mqttTopic>/humidity/value` → e.g. `45.10`

Alarm topics (not retained):

- `clients/<alarmClientId>/triggers/alarm-trigger`
  - JSON payload: `{ "useSound": false, "message": "..." }`
- `clients/<alarmClientId>/triggers/clear-alarm-trigger`
  - empty payload (`""`)

### Subscribed config topics

The client subscribes to:

- `<mqttTopic>/config`
  - JSON example:
    - `{ "frequency": 10000 }`
- `<mqttTopic>/temperature/config`
  - JSON example:
    - `{ "max_value": 25.0 }`

When a config is received, `g_deviceConfig` is updated and persisted to `/config.json`.

## Build, flash, and monitor (PlatformIO)

Prerequisites:

- VS Code + PlatformIO extension

Commands (run from the `Client_Climate/` folder):

- Build: `pio run`
- Upload: `pio run -t upload`
- Serial monitor: `pio device monitor -b 115200`

Filesystem:

- This project uses LittleFS (see `board_build.filesystem = littlefs` in [platformio.ini](platformio.ini)).

## Troubleshooting

- Blue LED keeps blinking: wrong WiFi credentials, or AP is unavailable.
- Yellow LED keeps blinking: wrong MQTT host/port, broker is down, or network blocks MQTT.
- Red LED is solid: DHT22 read failed (check wiring, pull-up, pin, power, and sensor type).

## Project structure (quick)

- [src/main.cpp](src/main.cpp): boot, LED logic, sensor reads, publishing + alarm logic
- [src/mqtt.cpp](src/mqtt.cpp): MQTT connect + subscribe + publishing type topics
- [src/storage.cpp](src/storage.cpp): LittleFS `/config.json` read/write
- [src/config_writer.cpp](src/config_writer.cpp): default config on first boot
