import socket
import sys
import time
from concurrent.futures import ThreadPoolExecutor, as_completed


def scan_port(target_ip, port):
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.settimeout(1)
    try:
        return s.connect_ex((target_ip, port)) == 0
    except OSError:
        return False
    finally:
        s.close()


def main():
    if len(sys.argv) > 1:
        target = sys.argv[1]
    else:
        target = input("Enter target IP or hostname: ").strip()

    try:
        if not target:
            raise ValueError
        target_ip = socket.gethostbyname(target)
    except (OSError, ValueError):
        print("[-] Invalid IP address or hostname.")
        return

    print(f"Scanning {target_ip} (ports 1-1024)...")
    start = time.time()
    open_ports = []

    with ThreadPoolExecutor(max_workers=50) as executor:
        futures = {executor.submit(scan_port, target_ip, p): p for p in range(1, 1025)}
        for future in as_completed(futures):
            port = futures[future]
            if future.result():
                open_ports.append(port)
                print(f"[+] Port {port} is open")

    elapsed = time.time() - start
    print(f"\nOpen ports: {sorted(open_ports) if open_ports else 'none'}")
    print(f"Scan completed in {elapsed:.2f} seconds")


if __name__ == "__main__":
    main()
