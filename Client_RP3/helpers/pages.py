from helpers.display import refresh_display
from helpers.utils import get_local_ip, get_mac, get_device_id
from helpers.sensors import sensor_count, get_display_format
from helpers.api import  get_device_info


class Pages:
    """
    This class manages what page to display on the screen.
    """
    def __init__(self, start=0, total_pages=3, exclude_pages=None):
        """
        This is for initializing the pages settings
        """
        self.current_page = start
        self.total_pages = total_pages - 1
        self.exclude_pages = exclude_pages if exclude_pages else []
        set_page(self.current_page)

    def next_page(self):
        """
        This is for switching view to the next page
        """
        self.current_page = (self.current_page + 1) % (self.total_pages + 1)
        set_page(self.current_page)

    def refresh_page(self):
        """
        This is for refreshing the page in case of value change by the sensor
        """
        if self.current_page not in self.exclude_pages:
            set_page(self.current_page)


special_pages = {
    0: lambda: "Device name:    " + get_device_info()["name"],
    1: lambda: "Device location:" + "Demo_Location", #get_device_info()["location"]["name"],
    2: lambda: "Device name:    " + get_device_info()["name"],
    2: lambda: "IP:             " + get_local_ip(),
    3: lambda: "MAC Address:    " + get_mac(),
    4: lambda: get_device_id()
}


def set_page(page_number):
    """
    This is for setting what to display on the screen based on the page number
    """
    if page_number in special_pages:
        text = special_pages[page_number]()
        refresh_display(text=text)
    else:
        # Adjust the sensor pages dynamically based on the sensor_data dictionary
        sensor_index = page_number - len(special_pages)
        if 0 <= sensor_index < sensor_count():
            sensor_name = list(get_display_format().keys())[sensor_index]
            sensor_value = get_display_format()[sensor_name]["value"]
            display_text = get_display_format()[sensor_name]["display_label"](sensor_value)
            refresh_display(text=display_text)
        else:
            print("Invalid page number")


def special_pages_count():
    return len(special_pages)