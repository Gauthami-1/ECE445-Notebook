import requests

ESP32_IP = "192.168.4.1"  

def send_command(command):
    url = f"http://{ESP32_IP}/{command}"
    try:
        response = requests.get(url, timeout=3)
        print(f"[{command.upper()}] {response.text}")
    except requests.exceptions.RequestException as e:
        print(f"Error sending {command}: {e}")

if __name__ == "__main__":
    while True:
        cmd = input("Enter command (forward, backward, left, right, stop, tombstone_on, tombstone_off, exit): ").strip().lower()
        if cmd in ["forward", "backward", "left", "right", "stop", "tombstone_on", "tombstone_off"]:
            send_command(cmd)
        elif cmd == "exit":
            print("Exiting...")
            break
        else:
            print("Invalid command. Try again.")
