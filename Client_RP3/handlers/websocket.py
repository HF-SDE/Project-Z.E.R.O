import websocket
from helpers.api import  get_device_info, get_token
ws = websocket.WebSocket()

def init_websocket():
    try:
        ws.connect(f"ws://10.130.64.251/api/device", header=[f"x-api-key: {get_token()}", f"device-id: {get_device_info()}"])
        print("Successfully connected to websocket")
        print("Websocket has ben initialized")
    except Exception as e:
        pass


def OnSettingsChange(newSettings):
    print("OnSettingsChange")
    settings = json.loads(newSettings)

    if (settings["status"] == "INACTIVE"):
        raise Exception("This device is inactive!")

    schedule.clear()

    if (settings["status"] == "INACTIVE"):
        return

    try:
        schedule.every(settings["frequency"] / 1000).seconds.do(job)
    except Exception as e:
        print(e)

def control_for_changes():
    schedule.run_pending()
    try:
        OnSettingsChange(ws.recv())
    except websocket.WebSocketTimeoutException:
        pass
    except:
        pass