Password Generator and SHA-256 Hasher
Python script that generates strong random passwords using the secrets module and computes their SHA-256 hash.
Usage

python password_hash_tool.py
python password_hash_tool.py -l 20
python password_hash_tool.py -t "hello"

-l: password length (default 16, minimum 4)
-t: hash a given text instead of generating a password
