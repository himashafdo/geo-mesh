import socket
import json
import time

PORT = 54321

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

sock.bind(("", PORT))

print("[Gateway] Listening...\n")

while True:
    data, addr = sock.recvfrom(2048)
    packet = json.loads(data.decode())

    print("========== GATEWAY RECEIVED ==========")
    print("Time:", time.ctime(packet["timestamp"]))
    print("Node A:", packet["node_A"])
    print("Node B:", packet["node_B"])
    print("=====================================\n")
