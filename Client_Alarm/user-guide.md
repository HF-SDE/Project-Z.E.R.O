# Z.E.R.O. Alarm Client - User Guide

## Overview
The Z.E.R.O. Alarm Client is a smart alarm display device that receives alerts and messages wirelessly through your network. When an alarm is triggered, the device displays the alert message on its LCD screen and can activate visual (LED) and audio (buzzer) indicators based on the alarm settings.

## Device Components

### LCD Display
- **20x4 character LCD screen** showing status messages and alarm text
- Displays up to 80 characters (automatically wraps across 4 lines)
- Always shows current device status or active alarm

### Status LEDs (Top of Device)
Three colored LEDs indicate the device's connection status:
- **Blue LED** - Flashing: Device is connecting to WiFi
- **Green LED** - Solid: Device is connected and online  
- **Red LED** - Solid: Connection error (WiFi or MQTT failed)

### Alarm Indicators
- **Red Alarm LED** - Lights up when an alarm is active
- **Buzzer** - Sounds when an alarm with audio is triggered

## Normal Operation

### Power-Up Sequence
1. **Display shows**: "Starting up..."
2. **🟡Yellow LED turns on**: Device connecting to WiFi
3. **🟣LED turns purple**: Indicating MQTT connection in progress - If the stays purple for longer that 5 seconds, it's a indication that the MQTT broker is unreachable.
3. **🟢LED turns green**: Successfully connected and online.
4. **Display shows**: "No alarms. All good."

The device is now ready and monitoring for alarms.

### Standby Mode
When no alarms are active:
- Display shows: "No alarms. All good.".
- Green status LED is lit (solid) - indicating healthy connection.
- All alarm indicators are off.
- Device maintains connection and sends periodic status updates.

## Alarm Notifications

### Receiving an Alarm
When a remote system sends an alarm:

1. **LCD Display** immediately shows the alarm message
   - Message automatically wraps across available lines
   - Remains visible until alarm is cleared

2. **Red Alarm LED** lights up to provide visual indication

3. **Buzzer** may sound (if enabled for this specific alarm)
   - Audio alert can be enabled/disabled by the sending system
   - Buzzer remains active until alarm is cleared

**Example alarm display:**
```
Serverroom Overheat!
Temp: 52°C
```

### Clearing an Alarm
When the remote system clears the alarm:

1. **Display updates** to: "all good fam."
2. **Red Alarm LED** turns off
3. **Buzzer** stops (if it was active)

The device returns to standby mode, ready for the next alert.

## Status Indicators Explained

### Connection Status (Status LEDs)

| Type | Color | Meaning |
|------|------ |---------|
| Solid| Yellow   | Connecting to WiFi (startup) |
| Solid  | Green    | Connected and online (normal) |
| Solid  | Red     | Connection error or offline |
| Solid  | Purple  | WiFi connected, MQTT connecting or unreachable |
| Flashing | Red    | Filesystem malfunction (contact technician) |
| Flashing | Yellow  | Missing config. (contact technician) NOTE: This just means that the config stored on the device was invalid or missing.The device will fallback to use known defaults instead, but it's recommended to repair the configuration. |
| Flashing | Red and yellow    | Failed to use known default fallbacks(contact technician). |


### Display Messages

| Message | Meaning |
|---------|---------|
| "Loading config..." | Device starting up, reading settings |
| "No alarms. All good." | Normal operation, no active alarms |
| "all good fam." | Alarm just cleared, returning to normal |
| "WiFi failed!" | Cannot connect to wireless network |
| "Storage error! Check Serial" | Hardware issue, contact support |
| *Custom text* | Active alarm message from remote system |

## Usage Scenarios

### Example 1: Serverroom overheat alarm
**Remote system sends:**
```json
{
  "message": "Serverroom Overheat! Temp: 52°C",
  "useSound": true
}
```

**Device displays:**
```
Serverroom Overheat!
Temp: 52°C
```
- Red alarm LED lights up
- Buzzer sounds continuously
- This state remains until cleared remotely or the device is restarted.


## Troubleshooting

### Device Not Connecting (Red LED On)
**Symptoms:** Red status LED solid, display shows "WiFi failed!"

**Possible causes:**
- WiFi network is down or out of range
- WiFi credentials have changed
- Network configuration issue

**Action:** Check your WiFi router and network. The device requires reconfiguration if network settings have changed.

### No Alarms Appearing
**Symptoms:** Green LED on, but no alarms displayed when expected

**Possible causes:**
- Remote system not sending to this device
- MQTT connection issue (though green LED suggests connected)
- Incorrect device ID in remote system

**Action:** Verify the remote system is configured to send alarms to this device. Check that green status LED remains solid.

### Display Shows Garbled Text
**Symptoms:** LCD shows random characters or partial text

**Possible causes:**
- Temporary communication glitch
- Display hardware issue

**Action:** Power cycle the device (unplug and replug power). If issue persists, contact support.

### Buzzer Stuck On
**Symptoms:** Buzzer continues sounding after alarm should be cleared

**Possible causes:**
- Clear command not received
- Hardware issue

**Action:** Power cycle the device. Ensure remote system properly sent clear command.

## Technical Specifications

- **Display:** 20x4 character I2C LCD
- **Power:** USB power supply (5V)
- **Connectivity:** WiFi (2.4 GHz), MQTT protocol
- **Response Time:** < 1 second from alarm trigger to display
- **Heartbeat:** Device sends status update every 60 seconds
- **Message Length:** Up to 80 characters (auto-wrapped)

## Safety Notes

⚠️ **Important:**
- This device is a notification display system only
- Do not rely solely on this device for life-safety alarms
- Always follow your organization's safety procedures
- The buzzer is for notification purposes, not fire alarm compliance
- Ensure device has reliable power connection
- Keep device in visible location with clear LCD viewing

## Maintenance

### Regular Checks
- **Weekly:** Verify green status LED is lit
- **Monthly:** Confirm device responds to test alarms
- **As needed:** Clean LCD screen with soft, dry cloth
- **As needed:** Ensure buzzer and LEDs function during tests

## Support

For configuration changes, technical support, or hardware issues:
1. Note the device ID (configured during setup)
2. Document any error messages on the display
3. Observe and note status LED patterns
4. Contact your system administrator

