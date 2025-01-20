from helpers.sensors import get_last_temp, get_last_hum, get_last_light, get_last_sound
from helpers.display import refresh_display
import re
import uuid
import socket


def get_local_ip():
    with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as s:
        s.connect(("8.8.8.8", 80))
        return s.getsockname()[0]


def get_mac():
    mac = ''.join(re.findall('..', '%012x' % uuid.getnode()))
    return mac


def get_device_id():
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


def set_page(page_number):
    if page_number == 0:
        ip = get_local_ip()
        print(f"IP: {ip}")
        refresh_display(text=f"IP:             {ip}")
    elif page_number == 1:
        mac_address = get_mac()
        print(f"MAC Address: {mac_address}")
        refresh_display(text=f"MAC Address:    {mac_address}")
    elif page_number == 2:
        device_id = get_device_id()
        print(f"Device ID: {device_id}")
        refresh_display(text=device_id)
    elif page_number == 3:
        refresh_display(text="Temperature:    " + str(get_last_temp()) + "c")
    elif page_number == 4:
        refresh_display(text="Humidity:       " + str(get_last_hum()) + "%")
    elif page_number == 5:
        refresh_display(text="Light level:    " + str(get_last_light()))
    elif page_number == 6:
        refresh_display(text="Sound level:    " + str(get_last_sound()))