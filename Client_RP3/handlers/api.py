import requests
from helpers.config import get_base_url, get_token_file_path
from helpers.sensors import last_temp, last_hum, last_light, last_sound
from helpers.display import refresh_display
from helpers.utils import get_device_id
import os

token = ""
server_device_info = None


def control_api_access():
    global screen_color
    try:
        response = requests.get(f"{get_base_url()}/health", timeout=5)
        if response.status_code != 200:
            raise Exception("The response status was not 200")

        return response.status_code == 200
    except Exception as e:
        print(f"API Access Error: {e}")
        refresh_display(color=[255, 0, 0])
        return False


def get_device_info():
    """
    Makes a request to the API to get device information.
    """
    global server_device_info

    try:
        url = f"{get_base_url()}/device"
        headers = {
            "device-id": get_device_id(),
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

    if os.path.exists(get_token_file_path()) and token_from_api_call is None:
        # Token file exists and the server has not sent a new one, read the token
        print("From file")
        try:
            with open(get_token_file_path(), "r") as f:
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
            with open(get_token_file_path(), "w") as f:
                f.write(token_from_api_call)
                print(f"Token saved to file: {get_token_file_path()}")
        except Exception as e:
            print(f"Error writing token file: {e}")
            raise Exception("Error writing token file")
            token = ""
    else:
        raise Exception("No token from \nAPI or in file")
    print("Token has ben initialized")



def send_data_to_api():
    try:
        headers = {
            "device-id": get_device_id(),
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
        response = requests.post(get_base_url() + "/data", json=data, headers=headers, timeout=5)

    except Exception as e:
        print(e)
        refresh_display(color=[255, 150, 0])

def get_token():
    return token