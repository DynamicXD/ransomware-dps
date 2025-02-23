import os
import time
import logging
import threading
import ctypes
from collections import deque, Counter
from watchdog.observers import Observer
from watchdog.events import FileSystemEventHandler

# Configuration
WATCH_DIR = r"G:/test"  # Change this to the directory you want to monitor
LOG_FILE = r"G:/test/log.txt"

SUSPICIOUS_EXTENSIONS = {".ABYSS", ".akira", ".avdn", ".blackcat", ".blackmatter", ".hydra" ,".newbot", ".clop", ".conti", ".dharma", ".ELCTRONIC", ".elibe", ".crypt", ".haron", ".hive", ".lethal",".lockbit", ".makop", ".matrix", ".MEOW", ".newlive.team", ".lalo", ".nightcrow", ".phobos", ".ping", ".quantum", ".ryuk", ".snet", ".revil", ".tpc", ".unkno", ".xamcle", ".ecc", ".ezz", ".exx", ".zzz", ".xyz", ".aaa", ".cryp1", ".abc", ".ccc", ".vvv", ".zepto", ".xxx", ".ttt", ".micro", ".encrypted", ".locked", ".crypto", "_crypt", ".crinf", ".r5a", ".XRNT", ".XTBL", ".crypt", ".R16M01D05", ".pzdc", ".good", ".LOL!", ".OMG!", ".RDM", ".RRK", ".encryptedRSA", ".crjoker", ".EnCiPhErEd", ".LeChiffre", ".keybtc@inbox_com", ".0x0", ".bleep", ".1999", ".vault", ".HA3", ".toxcrypt", ".magic", ".SUPERCRYPT", ".CTBL", ".CTB2", ".diablo6", ".Lukitus", ".locky"}  # Extensions commonly used by ransomware
SPIKE_THRESHOLD = 10  # If more than 10 modifications happen in 1 minute, trigger alert
SPAM_INTERVAL = 10  # Ignore repeated events within this time (seconds)
EVENT_QUEUE_SIZE = 50  # Track last 50 events to prevent redundancy

# Set up logging
logging.basicConfig(filename=LOG_FILE, level=logging.INFO,
                    format="%(asctime)s - ALERT: %(message)s", datefmt="%Y-%m-%d %H:%M:%S")

# Track recent events to avoid spamming logs
recent_events = deque(maxlen=EVENT_QUEUE_SIZE)
last_log_time = {}

# Track file change spikes
event_counter = Counter()
locked_folders = set()  # Stores already locked folders to prevent duplicate actions


class RansomwareMonitor(FileSystemEventHandler):
    def __init__(self):
        super().__init__()
        self.lock = threading.Lock()
        self.last_check_time = time.time()

    def log_event(self, event_type, path):
        """Logs events while preventing spam logging."""
        global last_log_time
        folder_path = os.path.dirname(path) if os.path.isfile(path) else path
        current_time = time.time()

        # Avoid redundant logging
        if folder_path in last_log_time and (current_time - last_log_time[folder_path]) < SPAM_INTERVAL:
            return

        last_log_time[folder_path] = current_time

        with self.lock:
            if (event_type, folder_path) not in recent_events:
                logging.info(f"{event_type}: {folder_path}")
                print(f"🚨 ALERT: {event_type} - {folder_path}")
                recent_events.append((event_type, folder_path))  # Add event to history

    def detect_spike(self):
        """Detects if too many file changes happen within a short period (possible ransomware activity)."""
        global event_counter
        current_minute = int(time.time() // 60)  # Get current minute
        total_changes = sum(event_counter.values())

        if total_changes > SPIKE_THRESHOLD:
            print("🚨 **WARNING: Unusual file activity detected! Locking affected folders...**")
            logging.warning(f"🚨 Mass file changes detected: {total_changes} changes in the last minute.")
            self.restrict_permissions()  # Take action!

        # Reset counter every minute
        if current_minute != self.last_check_time:
            event_counter.clear()
            self.last_check_time = current_minute

    def restrict_permissions(self, folder_path=None):
        """Restricts write permissions to a specific folder (or all affected ones)."""
        if folder_path:
            if folder_path in locked_folders:
                return  # Prevent duplicate locking
            locked_folders.add(folder_path)
            print(f"⚠ Locking folder: {folder_path} to prevent data loss...")
            logging.warning(f"⚠ Locking folder: {folder_path}")
            os.system(f'icacls "{folder_path}" /deny Everyone:(W,RD)')  # Deny write & delete permissions
        else:
            for folder in last_log_time.keys():  # Lock all folders with recent anomalies
                self.restrict_permissions(folder)

    def unlock_permissions(self, folder_path):
        """Unlocks write permissions for a folder (manual intervention needed)."""
        try:
            if folder_path in locked_folders:
                os.system(f'icacls "{folder_path}" /grant Everyone:(F)')  # Restore full access
                print(f"🔓 Unlocked folder: {folder_path}")
                logging.info(f"🔓 Unlocked folder: {folder_path}")
                locked_folders.remove(folder_path)
        except Exception as e:
            print(f"⚠ Error unlocking permissions: {e}")

    def on_modified(self, event):
        if not event.is_directory:
            ext = os.path.splitext(event.src_path)[-1].lower()
            if ext in SUSPICIOUS_EXTENSIONS:
                self.log_event("Suspicious File Modified", event.src_path)
                self.restrict_permissions(os.path.dirname(event.src_path))
            event_counter["modified"] += 1
            self.detect_spike()

    def on_deleted(self, event):
        self.log_event("File/Folder Deleted", event.src_path)
        self.restrict_permissions(os.path.dirname(event.src_path))
        event_counter["deleted"] += 1
        self.detect_spike()

    def on_moved(self, event):
        """Detects file renaming and folder renaming."""
        src_folder = os.path.dirname(event.src_path)
        dest_folder = os.path.dirname(event.dest_path)

        # If the entire folder is renamed, show only the folder change
        if src_folder != dest_folder:
            self.log_event("Folder Renamed", f"{src_folder} → {dest_folder}")
            self.restrict_permissions(src_folder)
        else:
            self.log_event("File Renamed", f"{event.src_path} → {event.dest_path}")
            self.restrict_permissions(os.path.dirname(event.src_path))

        event_counter["renamed"] += 1
        self.detect_spike()

    def on_created(self, event):
        self.log_event("New File/Folder Created", event.src_path)
        event_counter["created"] += 1
        self.detect_spike()


# Start monitoring
if __name__ == "__main__":
    event_handler = RansomwareMonitor()
    observer = Observer()
    observer.schedule(event_handler, path=WATCH_DIR, recursive=True)
    observer.start()

    print(f"🔍 Monitoring {WATCH_DIR} for suspicious activity...")

    try:
        while True:
            time.sleep(5)
    except KeyboardInterrupt:
        observer.stop()
        print("\n🚨 Monitoring stopped.")

    observer.join()
