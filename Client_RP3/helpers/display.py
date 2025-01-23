import time
import datetime
from helpers.config import get_setting

from grovepi import *
from grove_rgb_lcd import *

# Initialize global variables
display_color = [0, 0, 0]
display_text = None
last_button_time = time.time()


def button_pressed():
    """
    This is called when the button was pressed.
    """
    global last_button_time
    last_button_time = time.time()


def get_last_button_time():
    """
    This is called to get the last button pressed time.
    """
    return last_button_time


def refresh_display(color=None, text=None, is_error=False):
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

        # Load the night mode start and stop from the settings
        night_start = datetime.datetime.strptime(get_setting("night_mode_start"), "%H:%M").time()
        night_stop = datetime.datetime.strptime(get_setting("night_mode_stop"), "%H:%M").time()

        # Calculate the time elapsed since the button was last pressed
        time_elapsed_since_button = time.time() - last_button_time

        if (((night_start <= current_time.time() <= night_stop) and (
                time_elapsed_since_button > get_setting("button_night_mode_duration")
        )) or display_text is None) and is_error is False:
            # Turn of the display if it is in night mode
            setRGB(0, 0, 0)
        else:
            # If it is not in night mode set the display to the stored color
            setRGB(display_color[0], display_color[1], display_color[2])

        # Refresh display text
        if display_text is not None and has_text_changed:
            setText(display_text)


    except Exception as e:
        print(f"Error refreshing display: {e}")
