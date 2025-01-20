import os
from dotenv import load_dotenv

base_url = None
token_file_path = None
night_mode_start = None
night_mode_stop = None

button_night_mode_duration = None
page_refresh_interval = None
excluded_pages = None

dht_sensor = None
sound_sensor = None
light_sensor = None
button_sensor = None

def initialize_config():
    dotenv_path = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '.env')
    load_dotenv(dotenv_path)

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
    dht_sensor = int(os.getenv("DHT_SENSOR", 7))
    sound_sensor = int(os.getenv("SOUND_SENSOR", 0))
    light_sensor = int(os.getenv("LIGHT_SENSOR", 1))
    button_sensor = int(os.getenv("BUTTON", 3))

    print("Config loaded")


def get_base_url():
    return base_url
def get_token_file_path():
    return token_file_path
def get_night_mode_start():
    return night_mode_start
def get_night_mode_stop():
    return night_mode_stop
def get_button_night_mode_duration():
    return button_night_mode_duration
def get_page_refresh_interval():
    return page_refresh_interval
def get_excluded_pages():
    return excluded_pages

def get_dht_sensor():
    return dht_sensor
def get_sound_sensor():
    return sound_sensor
def get_light_sensor():
    return light_sensor
def get_button_sensor():
    return button_sensor
