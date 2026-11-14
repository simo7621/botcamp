Automated Log Analyzer
Python script that scans web server logs and detects SQL Injection, Path Traversal, XSS, and Command Injection attempts using regular expressions.
Usage
python log_analyzer.py sample_access.log
Each alert shows the attack type, line number, and attacker IP, followed by a summary at the end of the scan.
Files
log_analyzer.py: the analyzer
sample_access.log: sample log for testing
