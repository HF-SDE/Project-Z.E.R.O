# Client First Door - RFID Access Control System

An ESP32-based RFID access control system that combines RFID card scanning with PIN code entry for secure door access.

## System Overview

This device implements a two-factor access control system:

1. **RFID Card Scan** - User scans an RFID card
2. **PIN Code Entry** - User enters a correct PIN code via keypad
3. **Access Decision** - System grants or denies access based on validation

## Hardware Components

- **Microcontroller**: ESP32 (ESP-WROVER-KIT)
- **RFID Reader**: MFRC522 (SPI interface)
- **Keypad**: 4x4 Matrix Keypad (PmodKYPD)
- **LCD Display**: 20x4 Character Display with PCF8574 I2C backpack
- **LEDs**:
  - White LED (GPIO 3) - Status indicator
  - RGB Status LEDs (GPIO 4, 0, 2) - System feedback
- **Audio**: Active Buzzer (GPIO 15)
- **Connectivity**: WiFi + MQTT for remote integration

## Access Control State Machine

The system operates using a finite state machine with the following states:

### 1. WAITING_FOR_RFID (Idle State)

**Purpose**: System is ready and waiting for an RFID card scan

**Visual Feedback**:

- **LCD Backlight**: White
- **White LED**: ON (steady)
- **Display Message**: "Scan RFID Card"
- **Audio**: None

**Next State**:

- → WAITING_FOR_CODE (when valid RFID card is detected)

**Notes**: This is the default state after system boot or after completing a transaction

---

### 2. WAITING_FOR_CODE (Code Entry State)

**Purpose**: An RFID card has been accepted; system waits for PIN code entry

**Visual Feedback**:

- **LCD Backlight**: Yellow
- **White LED**: Blinking (500ms toggle)
- **Display Message**: "Enter Code:" + Masked asterisks showing entered digits
- **Audio**: Single short beep (100ms) when RFID is accepted

**Keypad Controls**:

- **0-9**: Enter digit (max 8 digits)
- **A (Enter)**: Submit code for validation
- **C (Clear)**: Clear all entered digits
- **D (Delete)**: Delete last entered digit
- **E-F**: Reserved (Clear and Validate are mapped to C and A)

**Timeout**: 30 seconds - if no valid code is entered, automatically transitions to DENIED state

**Next States**:

- → VALIDATING (when user presses Enter/A key with code)
- → DENIED (on timeout)
- → WAITING_FOR_RFID (if user cancels)

**Notes**: For security, entered digits are displayed as asterisks (\*) on the LCD

---

### 3. VALIDATING (Verification State)

**Purpose**: System is checking if the entered PIN code is correct

**Visual Feedback**:

- **LCD Backlight**: Blue
- **White LED**: OFF
- **Display Message**: "Validating..."
- **Audio**: None

**Duration**: 500ms validation delay

**Verification Logic**:

- Compares entered code with `correctCode` ("1234" default)
- Can be configured via config file

**Next States**:

- → GRANTED (code matches)
- → DENIED (code is incorrect)

**Notes**: The 500ms delay provides visual feedback that the system is processing the request

---

### 4. GRANTED (Access Approved State)

**Purpose**: Access has been successfully granted

**Visual Feedback**:

- **LCD Backlight**: Green
- **White LED**: Blinking (200ms on, 200ms off)
- **Display Message**: "Access Granted!"
- **Audio**: Double beep pattern (150ms beep + 100ms silence + 150ms beep)

**Duration**: 5 seconds - display and LED feedback continue for 5 seconds

**Effects**:

- Buzzer produces encouraging double-beep pattern
- Green backlight provides clear success indication
- White LED blinks rapidly to draw attention to success
- All feedback persists for 5 seconds

**Next State**:

- → WAITING_FOR_RFID (automatic after 5 seconds)

**Notes**: This state provides strong positive feedback to the user

---

### 5. DENIED (Access Denied State)

**Purpose**: Access has been denied (invalid PIN code)

**Visual Feedback**:

- **LCD Backlight**: Red
- **White LED**: OFF
- **Display Message**: "Access Denied"
- **Audio**: Single long beep (300ms)

**Duration**: 2 seconds - display and feedback continue for 2 seconds

**Effects**:

- Buzzer produces single warning beep
- Red backlight clearly indicates failure
- White LED turns off to distinguish from success state

**Next State**:

- → WAITING_FOR_RFID (automatic after 2 seconds)

**Trigger Conditions**:

- Wrong PIN code entered
- Timeout during code entry (30 seconds)

**Notes**: Failed attempts are logged but do not lock the system - user can immediately try again

---

## LED and Audio Behavior Summary

| State            | LCD Backlight | White LED   | Buzzer       | Duration    |
| ---------------- | ------------- | ----------- | ------------ | ----------- |
| WAITING_FOR_RFID | White         | ON (steady) | None         | ∞           |
| WAITING_FOR_CODE | Yellow        | Blinking    | Beep (100ms) | 30s timeout |
| VALIDATING       | Blue          | OFF         | None         | 500ms       |
| GRANTED          | Green         | Blinking    | 2x Beeps     | 5 seconds   |
| DENIED           | Red           | OFF         | 1x Beep      | 2 seconds   |

## Default Configuration

- **Correct PIN Code**: `1234` (can be modified in source code)
- **Code Entry Timeout**: 30 seconds
- **Maximum Code Length**: 8 digits
- **Keypad Poll Rate**: 30ms
- **LCD Update**: Only when state or code changes (optimized for responsiveness)

## Keypad Layout

```
9 7 8 C
6 4 5 B
3 1 2 A
E 0 F D
```

**Key Mapping**:

- `0-9`: Numeric digits
- `A`: Enter/Accept (validates code)
- `C`: Clear all
- `D`: Delete/Backspace (last digit)
- `E`, `B`, `F`: Reserved

## MQTT Integration

The system connects to MQTT for remote monitoring and configuration:

- **Topic Base**: `devices/{deviceId}/`
- **Components**: LCD, Status LEDs, RFID Reader, Keypad, Buzzer, White LED
- **Supports**: Remote monitoring of access attempts, component status updates

## Safety Features

1. **Debouncing**: 20ms debounce on keypad input prevents false triggers
2. **Release Detection**: System waits for complete key release before accepting next press
3. **Masked Input**: PIN code displayed as asterisks for security
4. **Timeout Protection**: 30-second timeout prevents hanging on code entry
5. **No Account Lockout**: Failed attempts don't block future access attempts

## Building and Uploading

```bash
platformio run -e esp-wrover-kit          # Build for ESP-WROVER-KIT
platformio run -e esp-wrover-kit -t upload # Build and upload
platformio device monitor                   # Open serial monitor
```

## Troubleshooting

- **Keypad not registering**: Check pin connections and 30ms poll rate
- **LCD not showing colors**: Verify backlight support in display library
- **RFID not detected**: Ensure antenna is properly connected
- **White LED not blinking**: Verify GPIO 3 connection and PWM capability

## Future Enhancements

- Remote access logging via MQTT
- Multiple RFID card support with different PIN codes
- Temporary access tokens
- Admin mode for configuration changes
- Failed attempt logging and alerts
