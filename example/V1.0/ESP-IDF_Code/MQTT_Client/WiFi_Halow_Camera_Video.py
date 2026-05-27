import paho.mqtt.client as mqtt
from paho.mqtt.enums import CallbackAPIVersion
import numpy as np
import cv2
import sys

# --- Configuration Area ---
MQTT_BROKER = "your-ip"  # Replace with the actual MQTT server address
MQTT_TOPIC = "halow/up"
MQTT_USER = "client_2"        # Unified use of available client_2
MQTT_PASSWORD = "123"

WINDOW_NAME = "WiFi HaLow Video Feed"
latest_img = None
window_created = False       # New: Mark whether the window has actually been created

def on_connect(client, userdata, flags, rc, properties=None):
    status_code = rc.value if hasattr(rc, 'value') else rc
    if status_code == 0:
        print(f"✅ Successfully connected to server: {MQTT_BROKER}")
        client.subscribe(MQTT_TOPIC)
        print(f"📥 Successfully subscribed to topic: {MQTT_TOPIC}, continuously listening for data...")
    else:
        print(f"❌ Failed to connect to server, error code: {status_code}")

def on_message(client, userdata, msg):
    global latest_img
    print(f"📥 Received new data! Size: {len(msg.payload)} bytes")
    try:
        nparr = np.frombuffer(msg.payload, np.uint8)
        img = cv2.imdecode(nparr, cv2.IMREAD_COLOR)
        if img is not None:
            latest_img = img
        else:
            print("⚠️ Data received but OpenCV failed to decode (packet might be incomplete)")
    except Exception as e:
        print(f"❌ Exception occurred during data processing: {e}")

# Create client
client = mqtt.Client(callback_api_version=CallbackAPIVersion.VERSION2)
client.on_connect = on_connect
client.on_message = on_message
client.username_pw_set(MQTT_USER, MQTT_PASSWORD)

try:
    print(f"Attempting to connect to {MQTT_BROKER}...")
    client.connect(MQTT_BROKER, 1883, 60)
    client.loop_start() 

    print("Entering main loop, waiting to receive the first frame...")
    while True:
        if latest_img is not None:
            cv2.imshow(WINDOW_NAME, latest_img)
            window_created = True # Mark window as existing only after successfully displaying the image
        
        # 1. Keyboard exit detection
        if cv2.waitKey(30) & 0xFF == ord('q'):
            print("\nDetected 'q' key press, exiting...")
            break
            
        # 2. Core improvement: Detect X button only after the window has been created
        if window_created:
            try:
                if cv2.getWindowProperty(WINDOW_NAME, cv2.WND_PROP_VISIBLE) < 1:
                    print("\nDetected window manually closed, exiting...")
                    break
            except cv2.error:
                # Catch potential unexpected internal OpenCV errors
                pass

except Exception as e:
    print(f"An error occurred while running the program: {e}")

finally:
    print("Safely releasing resources and exiting...")
    client.loop_stop()
    client.disconnect()
    cv2.destroyAllWindows()
    sys.exit(0)