import time
import datetime
from helpers.config import get_button_night_mode_duration, get_night_mode_start, get_night_mode_stop

from grovepi import *
from grove_rgb_lcd import *

# Initialize global variables
display_color = [0, 0, 0]
display_text = None
last_button_time = time.time()


def button_pressed():
    global last_button_time
    last_button_time = time.time()

def get_last_button_time():
    return last_button_time

def refresh_display(color=None, text=None):
    """
    Refresh the display with optional color and text arguments.
    If not provided, use the last set values.
    """
    global display_color, display_text

    has_text_changed = False

    # Update color if provided
    if color is not None and display_color != color:
        display_color = color

    # Update text if provided
    if text is not None and display_text != text:
        display_text = text
        has_text_changed = True
        


    try:
        # Refresh display color
        current_time = datetime.datetime.now()

        night_start = datetime.datetime.strptime(get_night_mode_start(), "%H:%M").time()
        night_stop = datetime.datetime.strptime(get_night_mode_stop(), "%H:%M").time()


        # Calculate the time elapsed since the button was last pressed
        time_elapsed_since_button = time.time() - last_button_time

        if ((night_start <= current_time.time() <= night_stop) and (
            time_elapsed_since_button > get_button_night_mode_duration()
        )) or display_text is None:
            setRGB(0, 0, 0)
        else:
            setRGB(display_color[0], display_color[1], display_color[2])
            


        # Refresh display text
        if display_text is not None and has_text_changed:
            setText(display_text)


    except Exception as e:
        print(f"Error refreshing display: {e}")
