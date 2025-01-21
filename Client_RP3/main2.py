from helpers.config import initialize_config, get_setting
from helpers.sensors import init_sensors, read_sensors, read_button
from helpers.api import init_token, control_api_access, send_data_to_api
from helpers.pages import Pages
from helpers.display import button_pressed, refresh_display, get_last_button_time
from helpers.websocket import init_websocket
import time
import datetime


# Main entry point
def main():
    while True:
        try:
            print("Starting main loop")

            # Loads the config file
            initialize_config()

            print(get_setting("base_url"))

            # Blue starting screen
            refresh_display(color=[0, 0, 32], text="Starting...")

            # Initialize sensors and API

            has_api_access = control_api_access()

            if not has_api_access:
                raise Exception("API is not responding")

            init_sensors()
            init_token()
            init_websocket()

            # Initialize pages
            pages = Pages(start=0, total_pages=6, exclude_pages=get_setting("excluded_pages"))

            light_mode_activated = False
            last_button_state = 0
            button_hold_start = None
            last_sensor_time = time.time()
            last_refresh_time = time.time()

            # All has ben initialized change screen color to green
            refresh_display(color=[0, 64, 0])

            print("Starting program loop")

            while True:
                #print("Loop start")
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
                        print("Next page")
                        pages.next_page()
                        button_pressed()

                if button_status == 1 and button_hold_start and (time.time() - button_hold_start >= 5):
                    print("Button held for 5 seconds, restarting loop")
                    refresh_display(color=[0, 0, 32], text="Restarting...")
                    break  # Restart the outer loop

                last_button_state = button_status

                # Reset light mode
                if light_mode_activated and (time.time() - get_last_button_time() > get_setting("button_night_mode_duration")):
                    light_mode_activated = False

                # Read sensors every second
                current_time = time.time()
                if current_time - last_sensor_time >= 1:
                    #print("Read sensor")
                    read_sensors()
                    send_data_to_api()
                    last_sensor_time = current_time

                # Refresh the current page every 5 seconds
                if current_time - last_refresh_time >= get_setting("page_refresh_interval"):
                    pages.refresh_page()
                    last_refresh_time = current_time
                refresh_display()

        except Exception as e:
            try:
                refresh_display(color={255, 0, 0})
            except Exception as e:
                pass
        time.sleep(5)


if __name__ == "__main__":
    main()
