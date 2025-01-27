from helpers.pages import Pages
from helpers.sensors import read_button
from helpers.config import get_setting
from helpers.display import button_pressed
from helpers.display import refresh_display, get_last_button_time
import time
import datetime

light_mode_activated = False
last_button_state = 0
button_hold_start = None


def button_control(pages):
    global last_button_state, button_hold_start, light_mode_activated
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
        #break  # Restart the outer loop
        #raise Exception("Restarting...")
        return True

    last_button_state = button_status

    # Reset light mode
    if light_mode_activated and (time.time() - get_last_button_time() > get_setting("button_night_mode_duration")):
        light_mode_activated = False

