import time
import datetime
import threading
import sys
import os

# This is to install the required packages
from helpers.installer import install_packages
#install_packages()

from helpers.config import initialize_config, get_setting
from helpers.sensors import init_sensors, read_sensors, read_button, sensor_count
from helpers.api import init_token, control_api_access, send_data_to_api, get_device_info
from helpers.pages import Pages, special_pages_count
from helpers.display import button_pressed, refresh_display, get_last_button_time
from helpers.websocket import init_websocket

error_message = None

def set_error_message(message):
    global error_message
    error_message = message

def send_data():
    time.sleep(5)
    global error_message
    while True:
        try:
            send_data_to_api()
            pass
        except Exception as e:
            error_message = "Error sending data to the API"

        time.sleep((int(get_device_info()["frequency"]) / 1000))




# Main entry point
def main():
    global error_message
    while True:
        try:
            print("Starting program")

            # Clear the last error message on fresh start
            error_message = None

            # Loads the config file
            initialize_config()

            # Blue starting screen
            refresh_display(color=[0, 0, 32], text="Starting...")

            # Control that the device has access to the api
            control_api_access()

            # Initialize sensors, API & websockets
            init_sensors()
            init_token()
            init_websocket()

            # Initialize pages
            pages = Pages()

            light_mode_activated = False
            last_button_state = 0
            button_hold_start = None
            last_sensor_time = time.time()

            # All has been initialized change screen color to green
            refresh_display(color=[0, 64, 0])

            print("Starting loop")

            # Start the thread that is responsible for sending the collected data to the api
            loop_thread_send_data = threading.Thread(target=send_data)
            loop_thread_send_data.start()


            while True:
                if error_message:
                    raise Exception(error_message)

                if get_device_info()["status"] != "ACTIVE":
                    raise  Exception("Device is not \nactive!")
                # Logic for button press and page navigation
                button_status = read_button()

                if button_status == 1 and last_button_state == 0:
                    # Button press started, track hold duration
                    button_hold_start = time.time()


                if button_status == 0 and last_button_state == 1:
                    if not light_mode_activated and (
                            datetime.datetime.now().time() >= datetime.datetime.strptime(get_setting("night_mode_start"),
                                                                                         "%H:%M").time() and datetime.datetime.now().time() <= datetime.datetime.strptime(
                        get_setting("night_mode_stop"), "%H:%M").time()):
                        # If the screen is in dark mode and the first button press happens, activate light mode
                        light_mode_activated = True
                        button_pressed()
                    else:
                        # Proceed to navigate to the next page
                        pages.next_page()
                        button_pressed()

                if button_status == 1 and button_hold_start and (time.time() - button_hold_start >= 5):
                    # This is for restarting the program if the button is held for 5 sec
                    print("Button held for 5 seconds, restarting loop")
                    refresh_display(color=[0, 0, 32], text="Restarting...")
                    break  # Restart the outer loop

                last_button_state = button_status

                # Reset light mode
                if light_mode_activated and (time.time() - get_last_button_time() > get_setting("button_night_mode_duration")):
                    light_mode_activated = False

                # Read sensors every given interval, I would have put it in the send data thread but the grovepi library can not handle that
                # Reading the sensors in the interval of the send data frequency or the page refresh interval depending on the lowest one
                current_time = time.time()
                if current_time - last_sensor_time >= min((int(get_device_info()["frequency"]) / 1000), get_setting("page_refresh_interval")):
                    read_sensors()
                    last_sensor_time = current_time

                pages.refresh_page()
                refresh_display()

        except Exception as e:
            try:
                refresh_display(color=[255, 0, 0], text=str(e), is_error=True)
            except Exception as e:
                pass
        time.sleep(5)


if __name__ == "__main__":
    main()
