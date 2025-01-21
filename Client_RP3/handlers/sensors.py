import math
from grovepi import *
from helpers.config import get_setting

# Dictionary to store sensor values
sensor_data = {
    "TEMPERATURE": {"value": 0, "identifier": "CELSIUS"},
    "HUMIDITY": {"value": 0, "identifier": "PERCENTAGE"},
    "LIGHT_LEVEL": {"value": 0, "identifier": "PERCENTAGE"},
    "SOUND_LEVEL": {"value": 0, "identifier": "PERCENTAGE"}
}

def init_sensors():
    pinMode(get_setting("button_sensor"), "INPUT")
    print("Sensors has ben initialized")

def read_sensors():
    try:
        temp, hum = dht(get_setting("dht_sensor"), 1)
        sensor_data["TEMPERATURE"]["value"] = temp if not math.isnan(temp) else get_sensor_value("TEMPERATURE")
        sensor_data["HUMIDITY"]["value"]  = hum if not math.isnan(hum) else get_sensor_value("HUMIDITY")
        light = analogRead(get_setting("light_sensor"))
        sound = analogRead(get_setting("sound_sensor"))

        # Update sensor values with calculated percentages
        sensor_data["LIGHT_LEVEL"]["value"] = round((light / 1000) * 100, 2)
        sensor_data["SOUND_LEVEL"]["value"] = round((sound / 1000) * 100, 2)

        return sensor_data
    except Exception as e:
        print(f"Error reading sensors: {e}")
        return sensor_data

def read_button():
    try:
        button_value = digitalRead(get_setting("button_sensor"))
        return button_value

    except Exception as e:
        print(f"Error reading button: {e}")
        return 0

def get_sensor_value(sensor_name):
    """Get the value of a specific sensor by its name."""
    return sensor_data.get(sensor_name, {}).get("value", None)