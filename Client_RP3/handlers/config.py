import os
from dotenv import load_dotenv

def initialize_config():
    load_dotenv()

    global base_url, token_file_path, night_mode_start, night_mode_stop
    global button_night_mode_duration, page_refresh_interval, excluded_pages
    global dht_sensor, sound_sensor, light_sensor, button_sensor

    # Configuration values
    base_url = os.getenv("API_BASE_URL")
    token_file_path = os.getenv("TOKE_FILE_PATH", "token")
    night_mode_start = os.getenv("NIGHT_MODE_START", "18:00")
    night_mode_stop = os.getenv("NIGHT_MODE_STOP", "07:00")
    button_night_mode_duration = float(os.getenv("BUTTON_NIGHT_MODE_DURATION", 5))
    page_refresh_interval = float(os.getenv("PAGE_REFRESH_INTERVAL", 5))
    excluded_pages = [int(x.strip()) for x in os.getenv("EXCLUDED_PAGES", "0,1,2").split(",")]

    # Inputs
    dht_sensor = float(os.getenv("DHT_SENSOR", 7))
    sound_sensor = float(os.getenv("SOUND_SENSOR", 0))
    light_sensor = float(os.getenv("LIGHT_SENSOR", 1))
    button_sensor = float(os.getenv("BUTTON", 3))
