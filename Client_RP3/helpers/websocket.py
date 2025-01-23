import threading
import websocket
import time
import json
from datetime import datetime, timedelta
from helpers.api import get_token, set_server_device_info, get_device_info_from_api
from helpers.utils import get_device_id
from helpers.config import get_setting



ws = websocket.WebSocket()

def websocket_loop():
    # Outer loop makes it possible to reconnect in case of a disconnect
    while True:
        # Loop for keep trying to connect to the websocket until it success
        while True:
            try:
                ws.connect(
                    get_setting("websocket_base_url") + "/device",
                    header=[f"x-api-key: {get_token()}", f"device-id: {get_device_id()}"]
                )
                print("Successfully connected to websocket")
                break
            except Exception as e:
                print(f"Connection failed: {e}")
                time.sleep(5)

        ws.settimeout(1)

        # Loop for reading if the websocket connection has a new message
        while True:
            try:
                message = ws.recv()  # Receive a new message
                new_message = json.loads(message)
                set_server_device_info(new_message)
            except websocket.WebSocketTimeoutException:
                # Ignore timeout exceptions to allow the loop to continue
                pass
            except websocket.WebSocketConnectionClosedException:
                print("WebSocket connection closed. Reconnecting...")
                break  # Exit to reconnect
            except Exception as e:
                print(f"WebSocket error: {e}")
                break # Exit to reconnect
            time.sleep(1)

def api_loop():
    while True:
        # Call the API function
        get_device_info_from_api()

        # Get the current time
        now = datetime.now()

        # Calculate the next top of the hour
        next_hour = (now + timedelta(hours=1)).replace(minute=0, second=0, microsecond=0)

        # Calculate the sleep duration
        sleep_duration = (next_hour - now).total_seconds()

        # Sleep until the next whole hour
        time.sleep(sleep_duration)

def init_websocket():
    try:
        if get_setting('websocket_base_url') is None:
            # This is if there is not set a websocket url in the configfile, then we fall back to make periodic calls
            loop_thread_websocket_loop = threading.Thread(target=api_loop)
            loop_thread_websocket_loop.start()
            print("WebSocket not available. Falling back to periodic API calls")

        else:
            # Start the websocket in a separate thread so it can run in the background
            loop_thread_websocket_loop = threading.Thread(target=websocket_loop)
            loop_thread_websocket_loop.start()
            print("WebSocket has been initialized")


    except Exception as e:
        print(f"Failed to initialize WebSocket: {e}")
