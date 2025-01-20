import math
from grovepi import *
from helpers.config import get_dht_sensor, get_light_sensor, get_sound_sensor, get_button_sensor

last_temp = 0
last_hum = 0
last_light = 0
last_sound = 0

def init_sensors():
    pinMode(get_button_sensor(), "INPUT")
    print("Sensors has ben initialized")

def read_sensors():
    global last_temp, last_hum, last_light, last_sound
    try:
        temp, hum = dht(get_dht_sensor(), 1)
        last_temp = temp if not math.isnan(temp) else last_temp
        last_hum = hum if not math.isnan(hum) else last_hum
        last_light = analogRead(get_light_sensor())
        last_sound = analogRead(get_sound_sensor())
        return last_temp, last_hum, last_light, last_sound
    except Exception as e:
        print(f"Error reading sensors: {e}")
        return last_temp, last_hum, last_light, last_sound

def read_button():
    try:
        button_value = digitalRead(get_button_sensor())
        return button_value

    except Exception as e:
        print(f"Error reading button: {e}")
        return 0

def get_last_temp():
    return last_temp
def get_last_hum():
    return last_hum
def get_last_light():
    return last_light
def get_last_sound():
    return last_sound