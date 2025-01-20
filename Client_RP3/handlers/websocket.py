import websocket
import asyncio
from helpers.api import  get_device_info, get_token
ws = websocket.WebSocket()

async def on_message(message):
    """Method to be called when a new message is received."""
    print(f"Received message: {message}")

async def websocket_client():
    uri = "ws://10.130.64.251/api/device"  # Replace with your WebSocket server URI
    async with websockets.connect(uri) as websocket:
        while True:
            print("This is a test")
            message = await websocket.recv()  # Wait for a new message
            await on_message(message)  # Call the on_message method with the received message

def init_websocket():
    try:
        print("Initializing websocket")
        #asyncio.get_event_loop().run_until_complete(websocket_client())
        #ws.connect(f"ws://10.130.64.251/api/device", header=[f"x-api-key: {get_token()}", f"device-id: {get_device_info()}"])
        print("Successfully connected to websocket")
    except Exception as e:
        pass

