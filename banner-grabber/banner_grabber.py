import socket

HOST = "127.0.0.1"
PORTS = [21, 22, 25, 80, 8080]
WEB_PORTS = [80, 8080]
TIMEOUT = 3


def grab_banner(host, port):
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.settimeout(TIMEOUT)
        s.connect((host, port))
        if port in WEB_PORTS:
            s.send(b"HEAD / HTTP/1.1\r\nHost: " + host.encode() + b"\r\n\r\n")
        data = s.recv(1024)
        s.close()
        text = data.decode("utf-8", errors="ignore").strip()
        if not text:
            return None
        return text.splitlines()[0]
    except (socket.timeout, ConnectionRefusedError, OSError):
        return None


for port in PORTS:
    banner = grab_banner(HOST, port)
    if banner:
        print(f"[+] {port}: {banner}")
    else:
        print(f"[-] {port}: no banner")
