import argparse
import hashlib
import secrets
import string

LOWER = string.ascii_lowercase
UPPER = string.ascii_uppercase
DIGITS = string.digits
SYMBOLS = string.punctuation
ALL_CHARS = string.ascii_letters + string.digits + string.punctuation


def generate_password(length=16):
    if length < 4:
        raise ValueError("Password length must be at least 4.")
    while True:
        password = "".join(secrets.choice(ALL_CHARS) for _ in range(length))
        if (
            any(c in LOWER for c in password)
            and any(c in UPPER for c in password)
            and any(c in DIGITS for c in password)
            and any(c in SYMBOLS for c in password)
        ):
            return password


def sha256_hash(text):
    return hashlib.sha256(text.encode("utf-8")).hexdigest()


def main():
    parser = argparse.ArgumentParser(description="Password Generator and SHA-256 Hasher")
    parser.add_argument("-l", "--length", type=int, default=16, help="password length (default 16)")
    parser.add_argument("-t", "--text", type=str, help="hash this text instead of a generated password")
    args = parser.parse_args()

    if args.text is not None:
        print(f"Text    : {args.text}")
        print(f"SHA-256 : {sha256_hash(args.text)}")
        return

    try:
        password = generate_password(args.length)
    except ValueError as error:
        parser.error(str(error))
    print(f"Generated Password : {password}")
    print(f"SHA-256 Hash       : {sha256_hash(password)}")


if __name__ == "__main__":
    main()
