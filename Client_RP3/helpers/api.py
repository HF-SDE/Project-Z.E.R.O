import requests
from helpers.config import get_setting
from helpers.sensors import get_api_format
from helpers.display import refresh_display
from helpers.utils import get_device_id
import os
import time

import paho.mqtt.client as mqtt
import json


token = ""
server_device_info = None
mqtt_client = None
mqtt_connected = False


def set_server_device_info(new_server_device_info):
    global server_device_info
    server_device_info = new_server_device_info


def get_device_info_from_api():
    """
    Makes a request to the API to get device information.
    """
    global server_device_info
    # print("Getting the device information.")
    try:
        # Get the base url from config and add the device endpoint
        url = get_setting("api_base_url") + "/device"

        # Make the headers for the request
        headers = {
            "device-id": get_device_id(),
        }
        if token:
            headers["x-api-key"] = token

        # Make the api call
        response = requests.get(url, timeout=5, headers=headers)

        # Return an answer based on the status code
        if response.status_code == 200:
            response_body = response.json()
            if (
                    response_body and
                    "data" in response_body and
                    isinstance(response_body["data"], list) and
                    len(response_body["data"]) > 0
            ):
                server_device_info = response_body["data"][0]
                return server_device_info
        else:
            return None
    except requests.RequestException as e:
        print(f"Error making API request: {e}")
        return None


def init_token():
    """
    Initializes the token by checking the file, or fetching and saving it.
    """
    global token
    token_from_api_call = None
    token_loaded_from = None
    # Call the device info to se if it returns the first token or a new one
    get_device_info_from_api()

    if server_device_info and "token" in server_device_info:
        # Set the token from the API response. The server has returned a token
        token_from_api_call = server_device_info["token"]

    if os.path.exists(get_setting("token_file_path")) and token_from_api_call is None:
        # Token file exists and the server has not sent a new one, read the token
        try:
            with open(get_setting("token_file_path"), "r") as f:
                token = f.read().strip()
                token_loaded_from = "file"
                # print(f"Token loaded from file")
        except Exception as e:
            print(f"Error reading token file: {e}")
            token = ""
            raise Exception("Error reading token file")
    elif token_from_api_call is not None:
        # Token file does not exist in a file or the server has sent a new token, fetch token from API
        try:
            token = token_from_api_call
            with open(get_setting("token_file_path"), "w") as f:
                f.write(token_from_api_call)
                # print(f"Token saved to file")
                token_loaded_from = "API and has ben saved to file"

        except Exception as e:
            print(f"Error writing token file: {e}")
            token = ""
            raise Exception("Error writing token file")
    else:
        raise Exception("No token from   API or in file  " + "Please create a new device. ID: " + get_device_id())
    print(f"Token has ben initialized from {token_loaded_from}")



def on_connect(client, userdata, flags, rc):
    global mqtt_connected
    if rc == 0:
        print("Connected to MQTT broker.")
        mqtt_connected = True
    else:
        print(f"Failed to connect, return code {rc}")


def on_disconnect(client, userdata, rc):
    global mqtt_connecteds
    print("Disconnected from MQTT broker. Attempting to reconnect...")
    mqtt_connected = False
    while not mqtt_connected:
        try:
            client.reconnect()
            time.sleep(2)
        except Exception as e:
            print(f"Reconnection failed: {e}")
            time.sleep(5)  # Wait before retrying


def init_mqtt():
    global mqtt_client
    mqtt_username = get_setting("mqtt_username")
    mqtt_password = get_setting("mqtt_password")
    mqtt_broker = get_setting("mqtt_base_url")
    mqtt_port = int(get_setting("mqtt_port"))

    mqtt_client = mqtt.Client()
    mqtt_client.username_pw_set(mqtt_username, mqtt_password)
    client.tls_set()
    client.tls_insecure_set(False)
    mqtt_client.on_connect = on_connect
    mqtt_client.on_disconnect = on_disconnect

    try:
        mqtt_client.connect(mqtt_broker, port=mqtt_port)
        mqtt_client.loop_start()  # Keep the connection alive in a separate thread
        print("MQTT connection initialized.")
    except Exception as e:
        print(f"Failed to connect to MQTT broker: {e}")

def send_data_to_api():
    """
    Sends the data from the sensor to the API.
    """

    try:
        sensor_data = get_api_format()
        topic = get_setting("mqtt_topic")



        if  mqtt_client and mqtt_connected:
            payload = json.dumps({
                "deviceId": get_device_id(),
                "data": sensor_data
            })
            mqtt_client.publish(topic, payload)
       
        else:
            # Fallback: use the HTTP API
            headers = {
                "device-id": get_device_id(),
                "x-api-key": token
            }
            api_url = get_setting("api_base_url") + "/data"
            requests.post(api_url, json=sensor_data, headers=headers, timeout=5)        

    except Exception as e:
        print(e)
        raise Exception("Error sending sensor data")
        # refresh_display(color=[255, 150, 0], is_error=True)


def get_token():
    return token


def get_device_info():
    if server_device_info is None:
        get_device_info_from_api()
    return server_device_info

def control_api_access():
    """
    This is for controlling that the program can contact the api
    """
    try:
        response = requests.get(get_setting("api_base_url") + "/health", timeout=5)
        if response.status_code != 200:
            raise Exception("API is not \nresponding")
        else:
            print("Access to API confirmed")

        return response.status_code == 200
    except Exception as e:
        print(f"API Access Error: {e}")
        raise Exception(str(e))
        #refresh_display(color=[255, 0, 0], is_error=True)


def control_device():
    retries = 5
    delay = 2  # Delay in seconds

    for attempt in range(retries):
        if get_device_info_from_api() is not None:
            return  # Device found, exit function
        
        if attempt < retries - 1:
            time.sleep(delay)
        else:
            raise Exception("No device found Please create it" + get_device_id())