from handlers.config import initialize_config, excluded_pages, button_sensor, button_night_mode_duration, night_mode_start, night_mode_stop, page_refresh_interval
from handlers.sensors import init_sensors, read_sensors
from handlers.api import init_token, control_api_access, send_data_to_api
from handlers.pages import Pages
from handlers.display import button_pressed, refresh_display, last_button_time
import time
import datetime

# Main entry point
def main():
    # Blue starting screen
    refresh_display(color=[0, 0, 255], text="Starting...")
    
    # Loads the config file
    initialize_config()

    # Initialize sensors and API
    control_api_access()

    init_sensors()
    init_token()

    # Initialize pages
    pages = Pages(start=0, total_pages=5, exclude_pages=excluded_pages)

    light_mode_activated = False
    last_button_state = 0
    last_sensor_time = time.time()
    last_refresh_time = time.time()


    # All has ben initialized change screen color to green
    refresh_display(color=[0, 64, 0])


    while True:
        try:
            # Logic for button press and page navigation
            button_status = digitalRead(button_sensor)
            if button_status == 1 and last_button_state == 0:
                if not light_mode_activated and (
                        datetime.datetime.now().time() >= datetime.datetime.strptime(night_mode_start,
                                                                                     "%H:%M").time() and datetime.datetime.now().time() <= datetime.datetime.strptime(
                        night_mode_stop, "%H:%M").time()):
                    # If the screen is in dark mode and the first button press happens, activate light mode
                    light_mode_activated = True
                    button_pressed()
                else:
                    # Proceed to navigate to the next page
                    pages.next_page()
                    button_pressed()

            last_button_state = button_status

            # Reset light mode
            if light_mode_activated and (time.time() - last_button_time > button_night_mode_duration):
                light_mode_activated = False

            # Read sensors every second
            current_time = time.time()
            if current_time - last_sensor_time >= 1:
                read_sensors()
                send_data_to_api()
                last_sensor_time = current_time
            
            # Refresh the current page every 5 seconds
            if current_time - last_refresh_time >= page_refresh_interval:
                pages.refresh_page()
                last_refresh_time = current_time
            refresh_display()

        except Exception as e:
            refresh_display(color={255, 0, 0})

if __name__ == "__main__":
    main()
