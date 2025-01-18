import time
from grovepi import *
from grove_rgb_lcd import *
import socket
import math
import re, uuid
import requests
import os
import datetime
from dotenv import load_dotenv

load_dotenv()

# Settings
base_url = os.getenv("API_BASE_URL")

token_file_path = os.getenv("TOKE_FILE_PATH", "token")

night_mode_start = os.getenv("NIGHT_MODE_START", "18:00")
night_mode_stop = os.getenv("NIGHT_MODE_STOP", "07:00")
button_night_mode_duration = float(os.getenv("BUTTON_NIGHT_MODE_DURATION", 5))  # Duration in seconds

page_refresh_interval = float(os.getenv("PAGE_REFRESH_INTERVAL", 5))  # Refresh every 5 seconds

# Get the value from the environment variable
excluded_pages_str = os.getenv("EXCLUDED_PAGES", "0,1,2")

# Convert the string to a list of integers
excluded_pages = [int(x.strip()) for x in excluded_pages_str.split(",")]




# Inputs
DHT_SENSOR = float(os.getenv("DHT_SENSOR", 7))
SOUND_SENSOR = float(os.getenv("SOUND_SENSOR", 0))
LIGHT_SENSOR = float(os.getenv("LIGHT_SENSOR", 1))
BUTTON = float(os.getenv("BUTTON", 3))

# Last known sensor values
last_temp = 0
last_hum = 0
last_light = 0
last_sound = 0


# Last device information
device_ip = ""
device_mac = ""
device_id = ""

server_device_info = None

last_sensor_time = time.time()

last_button_state = 0
last_button_time = time.time()

screen_color = [0, 0, 0]

last_refresh_time = time.time()

token =  ""


def refresh_display_color():
    try: 
        current_time = datetime.datetime.now()
        night_start = datetime.datetime.strptime(night_mode_start, "%H:%M").time()
        night_stop = datetime.datetime.strptime(night_mode_stop, "%H:%M").time()

        # Calculate the time elapsed since the button was last pressed
        time_elapsed_since_button = time.time() - last_button_time
            
        if (night_start <= current_time.time() <= night_stop) and (time_elapsed_since_button > button_night_mode_duration):
            setRGB(0, 0, 0)
        else:
            setRGB(screen_color[0], screen_color[1], screen_color[2])
    except Exception as e:
        print(e)


def get_device_id():
    global device_id
    try:
        with open("/etc/machine-id", "r") as f:
            device_id = f.read().strip()
            return device_id
    except FileNotFoundError:
        try:
            with open("/var/lib/dbus/machine-id", "r") as f:
                device_id = f.read().strip()
                return device_id
        except Exception as e:
            return f"Error: {e}"


def get_device_info():
    """
    Makes a request to the API to get device information.
    """
    global server_device_info

    try:
        url = f"{base_url}/device"
        headers = {
            "device-id": device_id,
        }

        if token:
            headers["x-api-key"] = token

        response = requests.get(url, timeout=5, headers=headers)

        if response.status_code == 200:
            server_device_info = response.json()
            return server_device_info
        else:
            print(f"Error: Received status code {response.status_code} from API.")
            return None
    except requests.RequestException as e:
        print(f"Error making API request: {e}")
        return None

def init_token():
    """
    Initializes the token by checking the file, or fetching and saving it.
    """
    global token
    get_device_info()

    token_from_api_call = None

    if (
            server_device_info and
            "data" in server_device_info and
            isinstance(server_device_info["data"], list) and
            len(server_device_info["data"]) > 0 and
            "token" in server_device_info["data"][0]
    ):
        # Set the token from the API response
        token_from_api_call = server_device_info["data"][0]["token"]

    if os.path.exists(token_file_path) and token_from_api_call is None:
        # Token file exists and the server has not sent a new one, read the token
        print("From file")
        try:
            with open(token_file_path, "r") as f:
                token = f.read().strip()
                print(f"Token loaded from file: {token}")
        except Exception as e:
            print(f"Error reading token file: {e}")
            raise Exception("Error reading token file")
            token = ""
    elif token_from_api_call is not None:
        # Token file does not exist in a file or the server has sent a new token, fetch token from API
        print("From api")
        try:
            token = token_from_api_call
            with open(token_file_path, "w") as f:
                f.write(token_from_api_call)
                print(f"Token saved to file: {token_file_path}")
        except Exception as e:
            print(f"Error writing token file: {e}")
            raise Exception("Error writing token file")
            token = ""
    else:
        raise Exception("No token from \nAPI or in file")
            
def controll_api_access():
    global screen_color
    try:
        response = requests.get(base_url + "/health", timeout=5)
        if response.status_code != 200:
            screen_color = [255, 0, 0]
            refresh_display_color()
        else:
            screen_color = [0, 64, 0]
            refresh_display_color()

        
        return response.status_code == 200
        
    except Exception as e:
        screen_color = [255, 0, 0]
        refresh_display_color()
        print("URL" + str(base_url))
        raise Exception("The API is not \nresponding")


def init():
    controll_api_access()

    pinMode(BUTTON, "INPUT")

    get_device_id()

    init_token()
    
    


def read_sensors():
    global last_temp, last_hum, last_light, last_sound, screen_color

    try:
        # Read DHT sensor
        temp, hum = dht(DHT_SENSOR, 1)
        if math.isnan(temp):
            temp = last_temp
        else:
            last_temp = temp

        if math.isnan(hum):
            hum = last_hum
        else:
            last_hum = hum

        # Read other sensors
        light = analogRead(LIGHT_SENSOR)
        sound = analogRead(SOUND_SENSOR)

        last_light = light
        last_sound = sound

        return temp, hum, light, sound
    except Exception as e:
        print(f"Error reading sensors: {e}")
        screen_color = [255, 255, 0]
        refresh_display_color()
        return last_temp, last_hum, last_light, last_sound
    
def send_data_to_api():
    global screen_color
    try:
        headers = {
            "device-id": device_id,
            "x-api-key": token
        }
        data = [
            {
                "value": last_temp,
                "identifier": "CELSIUS",
                "name": "TEMPERATURE"
            },
            {
                "value": last_hum,
                "identifier": "PERCENTAGE",
                "name": "HUMIDITY"
            },
            {
                "value": ((last_light / 1000) * 100),
                "identifier": "PERCENTAGE",
                "name": "LIGHT_LEVEL"
            },
            {
                "value": ((last_sound / 1000) * 100),
                "identifier": "PERCENTAGE",
                "name": "SOUND_LEVEL"
            }
            
            
        ]
        response = requests.post(base_url + "/data", json=data, headers=headers, timeout=5)

    except Exception as e:
        print(e)
        screen_color = [255, 150, 0]
        refresh_display_color()


def get_local_ip():
    with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as s:
        s.connect(("8.8.8.8", 80))
        return s.getsockname()[0]



def get_mac():
    mac = ''.join(re.findall('..', '%012x' % uuid.getnode()))
    return mac


def set_page(page_number):
    if page_number == 0:
        ip = get_local_ip()
        print(f"IP: {ip}")
        setText(f"IP:             {ip}")
    elif page_number == 1:
        mac_address = get_mac()
        print(f"MAC Address: {mac_address}")
        setText(f"MAC Address:    {mac_address}")
    elif page_number == 2:
        device_id = get_device_id()
        print(f"Device ID: {device_id}")
        setText(device_id)
    elif page_number == 3:
        setText("Temperature:    " + str(last_temp) + "c")
    elif page_number == 4:
        setText("Humidity:       " + str(last_hum) + "%")
    elif page_number == 5:
        setText("Light level:    " + str(last_light))
    elif page_number == 7:
        setText("Sound level:    " + str(last_sound))
        
class Pages:
    def __init__(self, start=0, total_pages=5, exclude_pages=None):
        self.current_page = start
        self.total_pages = total_pages
        self.exclude_pages = exclude_pages if exclude_pages else []
        set_page(self.current_page)

    def next_page(self):
        self.current_page = (self.current_page + 1) % (self.total_pages + 1)
        set_page(self.current_page)
    def refresh_page(self):
        if self.current_page not in self.exclude_pages:
            set_page(self.current_page)












light_mode_activated = False



try:
    setText("Starting...")

    init()
    

    pages = Pages(start=0, total_pages=5, exclude_pages=excluded_pages)

    print("Loop start")


    while True:
        try:
            # Handle button press for page navigation
            button_status = digitalRead(BUTTON)

            if button_status == 1 and last_button_state == 0:
                if not light_mode_activated and (
                        datetime.datetime.now().time() >= datetime.datetime.strptime(night_mode_start,
                                                                                     "%H:%M").time() and datetime.datetime.now().time() <= datetime.datetime.strptime(
                        night_mode_stop, "%H:%M").time()):
                    # If the screen is in dark mode and the first button press happens, activate light mode
                    setRGB(255, 255, 255)  # Set the light mode color (adjust RGB values as needed)
                    light_mode_activated = True
                    last_button_time = time.time()
                else:
                    # Proceed to navigate to the next page
                    pages.next_page()
                    last_button_time = time.time()

            last_button_state = button_status

            # Reset light mode after the specified duration
            if light_mode_activated and (time.time() - last_button_time > button_night_mode_duration):
                light_mode_activated = False

            # Read sensors every second
            current_time = time.time()
            if current_time - last_sensor_time >= 1:
                try:
                    read_sensors()
                    #send_data_to_api()
                    last_sensor_time = current_time
                except IOError:
                    print("Unable to send data to api")


            # Refresh the current page every 5 seconds
            if current_time - last_refresh_time >= page_refresh_interval:
                pages.refresh_page()
                last_refresh_time = current_time
            refresh_display_color()


            # Display sensor readings


        except Exception as e:
            screen_color = [255, 0, 0]
            refresh_display_color()
            setText("Error1")
            print("Error occurred")
except Exception as e:
    screen_color = [255, 0, 0]
    refresh_display_color()
    setText(str(e))
    print(e)
    
