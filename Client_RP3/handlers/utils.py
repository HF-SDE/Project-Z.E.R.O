from helpers.sensors import get_sensor_value, get_display_format, sensor_count
from helpers.display import refresh_display
import re
import uuid
import socket


def get_local_ip():
    """
    This is for getting the local ip address of the device
    """
    try:
        with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as s:
            s.connect(("8.8.8.8", 80))
            return s.getsockname()[0]
    except:
        return None


def get_mac():
    """
    This is for getting the mac address of the device
    """
    mac = ''.join(re.findall('..', '%012x' % uuid.getnode()))
    return mac


def get_device_id():
    """
    This is for getting the device id of the device based on the linux machine id
    """
    global device_id
    try:
        with open("/etc/machine-id", "r") as f:
            device_id = f.read().strip()
            return device_id
    except FileNotFoundError:
        try:
            with open("/var/lib/dbus/machine-id", "r") as f:
                device_id = f.read().strip()
                return device_id
        except Exception as e:
            return f"Error: {e}"

        
special_pages = {
    0: lambda: f"IP:             {get_local_ip()}",
    1: lambda: f"MAC Address:    {get_mac()}",
    2: lambda: get_device_id()
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