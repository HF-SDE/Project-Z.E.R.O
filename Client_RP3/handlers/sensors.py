import math
from grovepi import *
from helpers.config import get_setting

# Dictionary to store sensors
sensor_data = {
    "TEMPERATURE": {
        "value": 0,
        "identifier": "CELSIUS",
        "display_label": lambda value: f"Temperature:    {value}'C"
    },
    "HUMIDITY": {
        "value": 0,
        "identifier": "PERCENTAGE",
        "display_label": lambda value: f"Humidity:       {value}%"
    },
    "LIGHT_LEVEL": {
        "value": 0,
        "identifier": "PERCENTAGE",
        "display_label": lambda value: f"Light Level:    {value}%"
    },
    "SOUND_LEVEL": {
        "value": 0,
        "identifier": "PERCENTAGE",
        "display_label": lambda value: f"Sound Level:    {value}%"
    }
}


def init_sensors():
    """
    This is for initializing sensors setting
    """
    pinMode(get_setting("button_sensor"), "INPUT")
    print("Sensors has ben initialized")

def read_sensors():
    """
    This is for reading the value of the sensors and storing it in the sensors dictionary
    """
    try:
        # Read the sensors and set the values
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
