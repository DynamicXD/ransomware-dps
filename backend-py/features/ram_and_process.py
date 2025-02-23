import os
import psutil
import subprocess
import time
import hashlib
import logging
from watchdog.observers import Observer
from watchdog.events import FileSystemEventHandler

# Configure logging
logging.basicConfig(level=logging.INFO, format="%(asctime)s - %(levelname)s - %(message)s")

# ⚡ Configuration
THRESHOLD_CPU = 50  # High CPU Usage (%)
THRESHOLD_MEMORY = 30  # High Memory Usage (%)
DUMP_FILE = "C:\\Users\\Public\\memdump.raw"
HASH_FILE = "ransomware_hashes.txt"  # File containing known ransomware hashes
VOLATILITY_PATH = "volatility.exe"
WINPMEM_PATH = "winpmem.exe"

# 🚀 Load Known Ransomware Hashes
def load_known_hashes():
    hashes = set()
    if os.path.exists(HASH_FILE):
        with open(HASH_FILE, "r") as f:
            hashes = {line.strip() for line in f if line.strip()}
    return hashes

KNOWN_RANSOMWARE_HASHES = load_known_hashes()

# 🔍 Get Running Processes
def get_running_processes():
    processes = {}
    logging.info("🔎 Scanning running processes...")
    for process in psutil.process_iter(['pid', 'name', 'cpu_percent', 'memory_percent']):
        processes[process.info['pid']] = process.info['name']
        logging.debug(f"PID: {process.info['pid']} - {process.info['name']}")
    return processes

# 📌 RAM Analysis using Volatility
def analyze_ram():
    try:
        logging.info("🛠️ Dumping RAM for analysis...")
        os.system(f"{WINPMEM_PATH} {DUMP_FILE}")  # Memory dump

        cmd = [VOLATILITY_PATH, "-f", DUMP_FILE, "windows.pslist"]
        result = subprocess.run(cmd, capture_output=True, text=True)

        ram_processes = set()
        for line in result.stdout.split("\n"):
            parts = line.split()
            if len(parts) > 1 and "System" not in line:
                ram_processes.add(parts[-1])  # Last column is process name

        logging.info(f"🧠 Found {len(ram_processes)} processes in RAM")
        return ram_processes
    except Exception as e:
        logging.error(f"⚠️ RAM Analysis Error: {str(e)}")
        return set()

# ⚠️ Detect Suspicious Processes
def detect_ransomware():
    running_processes = get_running_processes()
    ram_processes = analyze_ram()

    logging.info(f"🔍 Running Processes: {len(running_processes)} | RAM Processes: {len(ram_processes)}")

    ransomware_candidates = set(running_processes.values()) & ram_processes

    if ransomware_candidates:
        logging.warning(f"🚨 Potential Ransomware Found: {ransomware_candidates}")
        stop_ransomware(ransomware_candidates)
    else:
        logging.info("✅ No ransomware-like behavior detected.")

# ❌ Kill Ransomware-like Process
def stop_ransomware(process_names):
    for proc in psutil.process_iter(['pid', 'name']):
        if proc.info['name'] in process_names:
            try:
                psutil.Process(proc.info['pid']).terminate()
                logging.info(f"🛑 Terminated Suspicious Process: {proc.info['name']} (PID: {proc.info['pid']})")
            except Exception as e:
                logging.error(f"⚠️ Error terminating {proc.info['name']}: {str(e)}")

# 📌 Hash-Based Detection
def get_file_hash(file_path):
    try:
        with open(file_path, 'rb') as f:
            return hashlib.md5(f.read()).hexdigest()
    except Exception as e:
        logging.error(f"⚠️ Error hashing file {file_path}: {str(e)}")
        return None

def check_file_hash(file_path):
    global KNOWN_RANSOMWARE_HASHES
    KNOWN_RANSOMWARE_HASHES = load_known_hashes()  # Reload hashes dynamically

    file_hash = get_file_hash(file_path)
    if file_hash in KNOWN_RANSOMWARE_HASHES:
        logging.critical(f"🚨 Detected ransomware file: {file_path} (Hash: {file_hash})")
        stop_ransomware(["ransomware.exe"])  # Example process name

# 🚀 Continuous Monitoring
if __name__ == '__main__':
    logging.info("🛡️ Ransomware Detection System Running...\n")
    
    while True:
        detect_ransomware()
        time.sleep(10)  # Check every 10 seconds
