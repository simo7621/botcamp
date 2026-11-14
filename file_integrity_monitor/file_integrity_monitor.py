import hashlib
import os
import sys
import time
from datetime import datetime

CHUNK_SIZE = 4096


def calculate_hash(file_path):
    sha256 = hashlib.sha256()
    with open(file_path, "rb") as file:
        while True:
            chunk = file.read(CHUNK_SIZE)
            if not chunk:
                break
            sha256.update(chunk)
    return sha256.hexdigest()


def safe_hash(file_path):
    try:
        return calculate_hash(file_path)
    except (FileNotFoundError, PermissionError):
        return None


def timestamp():
    return datetime.now().strftime("%Y-%m-%d %H:%M:%S")


def monitor(file_path, interval=3):
    baseline = calculate_hash(file_path)
    print(f"[{timestamp()}] [INFO] Monitoring started: {file_path}")
    print(f"[{timestamp()}] [INFO] Baseline SHA-256: {baseline}")

    last_reported = baseline
    while True:
        time.sleep(interval)
        current = safe_hash(file_path)

        if current == last_reported:
            continue

        if current is None:
            print(f"[{timestamp()}] [CRITICAL ALERT] File missing or unreadable: {file_path}")
        elif current == baseline:
            print(f"[{timestamp()}] [INFO] File restored to baseline state: {file_path}")
        else:
            print(f"[{timestamp()}] [CRITICAL ALERT] File modified: {file_path}")
            print(f"    Baseline : {baseline}")
            print(f"    Current  : {current}")
        last_reported = current


def main():
    if len(sys.argv) < 2:
        print("Usage: python file_integrity_monitor.py <file_path> [interval_seconds]")
        sys.exit(1)

    file_path = sys.argv[1]
    interval = float(sys.argv[2]) if len(sys.argv) > 2 else 3

    if not os.path.isfile(file_path):
        print(f"Error: file '{file_path}' not found.")
        sys.exit(1)

    try:
        monitor(file_path, interval)
    except KeyboardInterrupt:
        print(f"\n[{timestamp()}] [INFO] Monitoring stopped.")


if __name__ == "__main__":
    main()
