# Tests — Z.E.R.O Library

This document explains how to run and extend the unit and integration tests for the Z.E.R.O Library.

---

## Overview

- Tests are written using the Unity framework (PlatformIO provides Unity integration).
- There are two primary test modes:
  - **Native (host)** — runs tests on your machine without hardware using small Arduino/WiFi/MQTT mocks and a host-friendly storage implementation.
  - **Hardware (device)** — builds and uploads test firmware to an ESP32 (`esp-wrover-kit`) and runs tests on a connected board.

---

## Running tests

- Native (no hardware):

  ```bash
  pio test -e native
  ```

  - Uses `-DUNIT_TEST` to select host-friendly code paths.
  - Tests create a `config.json` file in the repository working directory while running; tests clean it up but you should run them in a safe environment.

- Hardware (requires connected device):

  ```bash
  pio test -e esp-wrover-kit
  ```

  - Uploads a small test firmware to the connected ESP32 and runs Unity tests on the device.
  - **Warning:** storage tests use LittleFS and may delete `/config.json` and format the filesystem — run them only on a test device or after backing up data.

---

## Tests included (current)

- `test/test_environment` — unit tests for `Environment` behavior (mode selection, server IPs).
- `test/test_storage` — device and native tests for configuration storage (save/load/delete/format).
- `test/test_mqtt` — native tests for `MqttManager` behavior using a mocked PubSubClient and WiFi.

---

## Mocks and host support

- `test/include/Arduino.h` + `test/src/ArduinoMock.cpp` — minimal `String` and `Serial` mocks for native tests.
- `test/include/WiFi.h` + `test/src/WiFiMock.cpp` — minimal WiFi mock to control connectivity for tests.
- `test/include/PubSubClient.h` + `test/src/PubSubMock.cpp` — minimal PubSubClient mock for simulating publish/subscribe and incoming messages.
- `src/storage.cpp` contains a `#ifdef UNIT_TEST` branch which provides a filesystem-backed implementation for host tests (uses `std::filesystem` and `fstream`).

---

## Continuous Integration

- A GitHub Actions workflow is included at `.github/workflows/ci.yml` and runs the native tests (`pio test -e native`) on push/PR.

---

## Adding new tests

1. Create a new `test/<name>/` folder with a Unity test source file (see existing tests for examples).
2. Use existing mocks for Arduino, WiFi, and MQTT where appropriate. For hardware-only behavior, add tests under the existing hardware builds or create a new test environment.
3. Add documentation to this README if your tests require special hardware, setup, or destructive actions (formatting LittleFS, erasing flash, etc.).

---

## Safety notes

- Storage tests may erase configuration files or format LittleFS — **do not run them on production devices**.
- Native tests write a `config.json` file into the repository working directory; this will be removed by the test suite but avoid running on important working trees.

---
