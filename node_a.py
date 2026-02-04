import socket
import json
import time
import random

NODE_A_PORT = 54324
GATEWAY_IP = "127.0.0.1"
GATEWAY_PORT = 54321

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind(("", NODE_A_PORT))

gateway_sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

print("[Node A] Started and listening for Node B")

while True:
    data, addr = sock.recvfrom(1024)
    node_b_data = json.loads(data.decode())

    print("[Node A] Received from Node B:", node_b_data)

    node_a_data = {
        "node_id": "A",
        "soil": round(random.uniform(60, 80), 2),
        "tilt": round(random.uniform(0.5, 1.5), 2),
        "vibration": round(random.uniform(0.01, 0.03), 3),
        "battery": round(random.uniform(3.7, 4.2), 2)
    }

    combined_packet = {
        "timestamp": time.time(),
        "node_A": node_a_data,
        "node_B": node_b_data
    }

    gateway_sock.sendto(
        json.dumps(combined_packet).encode(),
        (GATEWAY_IP, GATEWAY_PORT)
    )

    print("[Node A] Forwarded combined data to Gateway\n")
