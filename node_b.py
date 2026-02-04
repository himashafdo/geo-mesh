import socket
import json
import time
import random

NODE_A_IP = "127.0.0.1"
NODE_A_PORT = 54324

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

print("[Node B] Started")

while True:
    data = {
        "node_id": "B",
        "soil": round(random.uniform(65, 90), 2),
        "tilt": round(random.uniform(0.8, 2.0), 2),
        "vibration": round(random.uniform(0.01, 0.05), 3),
        "battery": round(random.uniform(3.6, 4.2), 2)
    }

    sock.sendto(json.dumps(data).encode(), (NODE_A_IP, NODE_A_PORT))
    print("[Node B] Sent:", data)

    time.sleep(5)
