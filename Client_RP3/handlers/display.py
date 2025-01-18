import time
import datetime
from config import button_night_mode_duration, night_mode_start, night_mode_stop

from grovepi import *
from grove_rgb_lcd import *

# Initialize global variables
display_color = [0, 0, 0]
display_text = ""
last_button_time = time.time()


def button_pressed():
    global last_button_time
    last_button_time = time.time()


def refresh_display(color=None, text=None):
    """
    Refresh the display with optional color and text arguments.
    If not provided, use the last set values.
    """
    global display_color, display_text

    # Update color if provided
    if color is not None:
        display_color = color

    # Update text if provided
    if text is not None:
        display_text = text

    try:
        # Refresh display color
        current_time = datetime.datetime.now()
        night_start = datetime.datetime.strptime(night_mode_start, "%H:%M").time()
        night_stop = datetime.datetime.strptime(night_mode_stop, "%H:%M").time()

        # Calculate the time elapsed since the button was last pressed
        time_elapsed_since_button = time.time() - last_button_time

        if (night_start <= current_time.time() <= night_stop) and (
            time_elapsed_since_button > button_night_mode_duration
        ):
            setRGB(0, 0, 0)
        else:
            setRGB(display_color[0], display_color[1], display_color[2])

        # Refresh display text
        setText(display_text)

    except Exception as e:
        print(f"Error refreshing display: {e}")
