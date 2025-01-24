from helpers.sensors import get_sensor_value, get_display_format
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


