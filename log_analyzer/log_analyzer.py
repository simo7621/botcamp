import re
import sys
from collections import Counter
from urllib.parse import unquote_plus

PATTERNS = {
    "SQL Injection": [
        re.compile(r"union(\s|/\*.*?\*/)+(all\s+)?select", re.IGNORECASE),
        re.compile(r"\b(or|and)\s+['\"]?\d+['\"]?\s*=\s*['\"]?\d+", re.IGNORECASE),
        re.compile(r"['\"]\s*(or|and)\s+['\"]", re.IGNORECASE),
        re.compile(r";\s*(drop|delete|insert|update)\s+", re.IGNORECASE),
        re.compile(r"\bsleep\s*\(\s*\d+\s*\)", re.IGNORECASE),
        re.compile(r"information_schema", re.IGNORECASE),
    ],
    "Path Traversal": [
        re.compile(r"\.\./|\.\.\\"),
        re.compile(r"/etc/(passwd|shadow)", re.IGNORECASE),
        re.compile(r"(boot|win)\.ini", re.IGNORECASE),
    ],
    "XSS": [
        re.compile(r"<\s*script", re.IGNORECASE),
        re.compile(r"javascript\s*:", re.IGNORECASE),
        re.compile(r"\bon(error|load|click|mouseover)\s*=", re.IGNORECASE),
    ],
    "Command Injection": [
        re.compile(r"(;|\|\||&&|\|)\s*(cat|ls|id|whoami|wget|curl|nc|bash|sh)\b", re.IGNORECASE),
        re.compile(r"/bin/(ba)?sh|cmd\.exe", re.IGNORECASE),
    ],
}

IP_PATTERN = re.compile(r"^\s*(\d{1,3}(?:\.\d{1,3}){3})")


def normalize(line):
    decoded = line
    for _ in range(2):
        decoded = unquote_plus(decoded)
    return decoded


def extract_ip(line):
    match = IP_PATTERN.search(line)
    return match.group(1) if match else "Unknown"


def detect_attacks(line):
    text = normalize(line)
    found = []
    for attack_type, patterns in PATTERNS.items():
        if any(pattern.search(text) for pattern in patterns):
            found.append(attack_type)
    return found


def analyze(log_path):
    counts = Counter()
    total_lines = 0
    with open(log_path, "r", encoding="utf-8", errors="replace") as log_file:
        for line_number, line in enumerate(log_file, start=1):
            total_lines += 1
            line = line.rstrip("\n")
            if not line.strip():
                continue
            ip = extract_ip(line)
            for attack_type in detect_attacks(line):
                counts[attack_type] += 1
                print(f"[ALERT] Attack: {attack_type} | Line: {line_number} | Attacker IP: {ip}")
    return total_lines, counts


def print_summary(total_lines, counts):
    print("\n" + "=" * 50)
    print("Scan Summary")
    print("=" * 50)
    print(f"Total lines scanned : {total_lines}")
    print(f"Total attacks found : {sum(counts.values())}")
    if counts:
        for attack_type, number in counts.most_common():
            print(f"  - {attack_type}: {number}")
    else:
        print("  No attacks detected.")


def main():
    log_path = sys.argv[1] if len(sys.argv) > 1 else "sample_access.log"
    try:
        total_lines, counts = analyze(log_path)
    except FileNotFoundError:
        print(f"Error: log file '{log_path}' not found.")
        sys.exit(1)
    print_summary(total_lines, counts)


if __name__ == "__main__":
    main()
