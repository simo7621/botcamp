File Integrity Monitor (FIM)
Python script that monitors a file by computing its SHA-256 hash periodically and raising a critical alert when the content changes.
Usage

python file_integrity_monitor.py target.txt 3

First argument: file to monitor
Second argument: check interval in seconds (default 3)
Press Ctrl+C to stop.
