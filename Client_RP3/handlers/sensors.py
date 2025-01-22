import math
from grovepi import *
from helpers.config import get_setting
import threading


# Dictionary to store sensors
sensor_data = {
    "TEMPERATURE": {
        "value": -9999,
        "identifier": "CELSIUS",
        "display_label": lambda value: f"Temperature:    {value}'C"
    },
    "HUMIDITY": {
        "value": -1,
        "identifier": "PERCENTAGE",
        "display_label": lambda value: f"Humidity:       {value}%"
    },
    "LIGHT_LEVEL": {
        "value": -1,
        "identifier": "PERCENTAGE",
        "display_label": lambda value: f"Light Level:    {value}%"
    },
    "SOUND_LEVEL": {
        "value": -1,
        "identifier": "PERCENTAGE",
        "display_label": lambda value: f"Sound Level:    {value}%"
    }
}


def read_sensors():
    """
    Read values from sensors and update sensor data.
    """
    try:
        dht_sensor_pin = get_setting("dht_sensor")
        light_sensor_pin = get_setting("light_sensor")
        sound_sensor_pin = get_setting("sound_sensor")

        if not all(isinstance(pin, int) for pin in [dht_sensor_pin, light_sensor_pin, sound_sensor_pin]):
            raise ValueError("One or more sensor pins are invalid.")

        # Read DHT sensor
        temp, hum = dht(dht_sensor_pin, 1)
        sensor_data["TEMPERATURE"]["value"] = temp if not math.isnan(temp) else get_sensor_value("TEMPERATURE")
        sensor_data["HUMIDITY"]["value"] = hum if not math.isnan(hum) else get_sensor_value("HUMIDITY")

        # Read light and sound sensors
        light = analogRead(light_sensor_pin)
        sound = analogRead(sound_sensor_pin)
        sensor_data["LIGHT_LEVEL"]["value"] = round((light / 1000) * 100, 2)
        sensor_data["SOUND_LEVEL"]["value"] = round((sound / 1000) * 100, 2)

        return sensor_data
    except Exception as e:
        print(f"Error reading sensors: {e}")
        return sensor_data


def init_sensors():
    """
    This is for initializing sensors setting
    """
    pinMode(get_setting("button_sensor"), "INPUT")
    read_sensors()
    print("Sensors has ben initialized")


def read_button():
    """
    This is for reading the state of the button
    """
    try:
        button_value = digitalRead(get_setting("button_sensor"))
        return button_value

    except Exception as e:
        print(f"Error reading button: {e}")
        return 0

def get_sensor_value(sensor_name):
    """
    Get the value of a specific sensor by its name.
    """
    return sensor_data.get(sensor_name, {}).get("value", None)

def get_display_format():
    """
    This is for getting the display format
    """
    return sensor_data

def get_api_format():
    """
    This is for getting the api format
    """
    return [
        {
            "value": sensor_info["value"],
            "identifier": sensor_info["identifier"],
            "name": sensor_name
        }
        for sensor_name, sensor_info in sensor_data.items()
    ]
def sensor_count():
    """
    This is for getting the number of sensors
    """
    return len(sensor_data)
