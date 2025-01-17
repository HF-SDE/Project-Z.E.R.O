import math
from grovepi import *
from config import dht_sensor, light_sensor, sound_sensor, button_sensor

last_temp = 0
last_hum = 0
last_light = 0
last_sound = 0

def init_sensors():
    pinMode(button_sensor, "INPUT")

def read_sensors():
    global last_temp, last_hum, last_light, last_sound
    try:
        temp, hum = dht(dht_sensor, 1)
        last_temp = temp if not math.isnan(temp) else last_temp
        last_hum = hum if not math.isnan(hum) else last_hum
        last_light = analogRead(light_sensor)
        last_sound = analogRead(sound_sensor)
        return last_temp, last_hum, last_light, last_sound
    except Exception as e:
        print(f"Error reading sensors: {e}")
        return last_temp, last_hum, last_light, last_sound
