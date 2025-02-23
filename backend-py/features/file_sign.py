import os
import psutil
import hashlib
import math
import time
import sys
from watchdog.observers import Observer
from watchdog.events import FileSystemEventHandler

# Known file signatures (Magic Numbers)
KNOWN_SIGNATURES = {
    b"\xFF\xD8\xFF\xE0": "JPEG Image",
    b"\x89\x50\x4E\x47": "PNG Image",
    b"\x50\x4B\x03\x04": "ZIP Archive",
    b"\x25\x50\x44\x46": "PDF Document",
}

# Suspicious file extensions commonly used by ransomware
SUSPICIOUS_EXTENSIONS = {".ABYSS", ".akira", ".avdn", ".blackcat", ".blackmatter", ".hydra",
                         ".newbot", ".clop", ".conti", ".dharma", ".ELCTRONIC", ".elibe", ".crypt",
                         ".haron", ".hive", ".lethal", ".lockbit", ".makop", ".matrix", ".MEOW",
                         ".newlive.team", ".lalo", ".nightcrow", ".phobos", ".ping", ".quantum",
                         ".ryuk", ".snet", ".revil", ".tpc", ".unkno", ".xamcle", ".ecc", ".ezz",
                         ".exx", ".zzz", ".xyz", ".aaa", ".cryp1", ".abc", ".ccc", ".vvv", ".zepto",
                         ".xxx", ".ttt", ".micro", ".encrypted", ".locked", ".crypto", "_crypt",
                         ".crinf", ".r5a", ".XRNT", ".XTBL", ".crypt", ".R16M01D05", ".pzdc",
                         ".good", ".LOL!", ".OMG!", ".RDM", ".RRK", ".encryptedRSA", ".crjoker",
                         ".EnCiPhErEd", ".LeChiffre", ".keybtc@inbox_com", ".0x0", ".bleep", ".1999",
                         ".vault", ".HA3", ".toxcrypt", ".magic", ".SUPERCRYPT", ".CTBL", ".CTB2",
                         ".diablo6", ".Lukitus", ".locky"}

# Suspicious process names
SUSPICIOUS_PROCESS_NAMES = {"ransom", "encrypt", "locker", "malware", "trojan", "worm", "crypto", "script"}

# Threshold for high entropy (Encrypted files have entropy > 7.5)
ENTROPY_THRESHOLD = 7.5

WATCH_DIRECTORY = "D:\\crypton-hackathon\\testing"  # Change to the folder you want to monitor

def get_file_signature(file_path):
    """Extract first 4 bytes of a file to check its signature."""
    try:
        with open(file_path, "rb") as f:
            signature = f.read(4)
        return signature
    except:
        return None

def calculate_entropy(file_path):
    """Calculate Shannon entropy to detect encrypted files."""
    try:
        with open(file_path, "rb") as f:
            data = f.read()
        if not data:
            return 0

        freq = {byte: data.count(byte) for byte in set(data)}
        entropy = -sum((count / len(data)) * math.log2(count / len(data)) for count in freq.values())
        return entropy
    except:
        return 0

def find_suspicious_process(file_path):
    """Find the process that is modifying a given file or has a suspicious name."""
    for proc in psutil.process_iter(attrs=["pid", "name", "open_files"]):
        try:
            if proc.info["name"] and any(name in proc.info["name"].lower() for name in SUSPICIOUS_PROCESS_NAMES):
                return proc
            if proc.info["open_files"]:
                for file in proc.info["open_files"]:
                    if file.path == file_path:
                        return proc
        except (psutil.NoSuchProcess, psutil.AccessDenied):
            continue
    return None

def kill_suspicious_process(proc):
    """Kill a process that exhibits ransomware-like behavior."""
    if proc:
        try:
            print(f"[ALERT] Killing suspicious process: {proc.info['name']} (PID {proc.info['pid']})")
            proc.terminate()
        except (psutil.NoSuchProcess, psutil.AccessDenied):
            print(f"[ERROR] Could not terminate process {proc.info['name']} (PID {proc.info['pid']})")

class RansomwareMonitor(FileSystemEventHandler):
    """Watchdog event handler for monitoring file changes."""

    def on_modified(self, event):
        if event.is_directory:
            return

        file_path = event.src_path
        file_name = os.path.basename(file_path)

        # ✅ Ignore quarantine folder
        if "Quarantine" in file_path:
            return

        file_extension = os.path.splitext(file_path)[1].lower()
        file_signature = get_file_signature(file_path)
        entropy = calculate_entropy(file_path)

        print(f"[INFO] File modified: {file_path}")
        print(f" - Signature: {file_signature}")
        print(f" - Entropy: {entropy:.2f}")

        # Check if file signature is unknown (possible encryption)
        if file_signature not in KNOWN_SIGNATURES and entropy > ENTROPY_THRESHOLD:
            print(f"[ALERT] High entropy detected in {file_path}! Possible ransomware activity.")

            # ✅ Find and terminate the process modifying the file
            proc = find_suspicious_process(file_path)
            if proc:
                kill_suspicious_process(proc)

        # Check for suspicious extensions
        if file_extension in SUSPICIOUS_EXTENSIONS:
            print(f"[WARNING] File {file_path} has a suspicious extension: {file_extension}")

            # ✅ Find and terminate the process modifying the file
            proc = find_suspicious_process(file_path)
            if proc:
                kill_suspicious_process(proc)

def start_monitoring():
    """Start real-time monitoring on the directory."""
    print(f"[INFO] Monitoring {WATCH_DIRECTORY} for ransomware activity...")
    observer = Observer()
    event_handler = RansomwareMonitor()
    observer.schedule(event_handler, WATCH_DIRECTORY, recursive=True)
    observer.start()

    try:
        while True:
            time.sleep(1)
    except KeyboardInterrupt:
        observer.stop()
        print("[INFO] Monitoring stopped.")

    observer.join()

if __name__ == "__main__":
    start_monitoring()
