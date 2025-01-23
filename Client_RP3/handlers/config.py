import os
from dotenv import load_dotenv

settings = {}

def initialize_config():
    global settings

    # Set the path for where the env file is
    dotenv_path = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '.env')

    # Loads the env file
    load_dotenv(dotenv_path)

    # Configuration values
    settings["base_url"] = os.getenv("API_BASE_URL")
    settings["token_file_path"] = os.getenv("TOKE_FILE_PATH", "token")
    settings["night_mode_start"] = os.getenv("NIGHT_MODE_START", "18:00")
    settings["night_mode_stop"] = os.getenv("NIGHT_MODE_STOP", "07:00")
    settings["button_night_mode_duration"] = float(os.getenv("BUTTON_NIGHT_MODE_DURATION", 5))
    settings["page_refresh_interval"] = float(os.getenv("PAGE_REFRESH_INTERVAL", 5))
    settings["excluded_pages"] = [int(x.strip()) for x in os.getenv("EXCLUDED_PAGES", "0,1,2").split(",")]
    settings["wifi_ssid"] = os.getenv("WIFI_SSID", None)
    settings["wifi_password"] = os.getenv("WIFI_PASSWORD", None)



    # Sensors
    settings["dht_sensor"] = int(os.getenv("DHT_SENSOR", 7))
    settings["sound_sensor"] = int(os.getenv("SOUND_SENSOR", 0))
    settings["light_sensor"] = int(os.getenv("LIGHT_SENSOR", 1))
    settings["button_sensor"] = int(os.getenv("BUTTON", 3))

    print("Config loaded")

def get_setting(key):
    """
    This get the value from the environment variable given a key.
    """
    return settings.get(key)

