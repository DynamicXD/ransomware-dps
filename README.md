# Module Documentation

This repository contains a collection of modules written in C and Python. Each module serves a specific purpose, ranging from security monitoring to system utilities. Below is a detailed description of each module and instructions on how to use them.

---

## Modules:

# honeypot.c

## Description
`honeypot.c` is a C program designed to simulate a **honeypot** by creating fake system files and monitoring them for unauthorized access or modifications. When an intrusion is detected, the program logs the event, quarantines the offending process, and provides an admin interface for managing quarantined processes.

---

## Purpose
The purpose of this program is to:
- Detect unauthorized access or modifications to critical system files.
- Log intrusion attempts for further analysis.
- Quarantine malicious processes by suspending them.
- Provide an admin interface to manage quarantined processes.

---

## Features
- **Fake System Files**: Creates fake system files to lure potential attackers.
- **Real-time Monitoring**: Monitors fake files for unauthorized access or modifications.
- **Intrusion Logging**: Logs all detected intrusions to a file.
- **Process Quarantine**: Suspends processes that attempt to access or modify fake files.
- **Admin Interface**: Allows administrators to list and unquarantine processes.

---

## Usage

### Prerequisites
- A Windows system (the program uses Windows API functions).
- A C compiler (e.g., `gcc` or `MSVC`).

### Compilation
Compile the `honeypot.c` program using `gcc`:
```bash
gcc -o honeypot honeypot.c -lpsapi -lshlwapi
```

---

# file_sign.py

## Description
`file_sign.py` is a Python script designed to monitor a directory for ransomware-like activity by analyzing file signatures, entropy, and suspicious file extensions. It uses the `watchdog` library to detect file modifications in real-time and can terminate processes that exhibit malicious behavior.

---

## Purpose
The purpose of this script is to:
- Detect ransomware activity by monitoring file modifications.
- Analyze file signatures and entropy to identify potential encryption.
- Identify and terminate suspicious processes.
- Log alerts for further investigation.

---

## Features
- **Real-time Monitoring**: Watches a specified directory for file changes.
- **File Signature Analysis**: Detects unknown file signatures that may indicate encryption.
- **Entropy Calculation**: Identifies high-entropy files, which are often encrypted.
- **Suspicious Extension Detection**: Flags files with extensions commonly used by ransomware.
- **Process Termination**: Terminates processes that modify suspicious files.

---

## Usage

### Prerequisites
- Python 3.x installed on your system.
- The following Python libraries:
  - `watchdog`
  - `psutil`

Install the required libraries using pip:
```bash
pip install watchdog psutil
```

---

# usb.c

## Description
`usb.c` is a C program designed to monitor USB device activity on a Windows system. It detects when USB devices are inserted or removed, checks if they are whitelisted, and takes action against potentially malicious devices. The program can block unauthorized USB devices, log events, and scan for suspicious files (e.g., `.inf` files) on removable drives.

---

## Purpose
The purpose of this program is to:
- Monitor USB device activity in real-time.
- Detect and block unauthorized or potentially malicious USB devices.
- Log USB events for auditing and analysis.
- Scan removable drives for suspicious files and remove them.

---

## Features
- **Real-time USB Monitoring**: Detects USB device insertion and removal.
- **Whitelist Management**: Allows or blocks USB devices based on a whitelist.
- **Device Blocking**: Disables unauthorized USB devices.
- **File Scanning**: Scans removable drives for suspicious `.inf` files and removes them.
- **Event Logging**: Logs all USB-related events to a file.

---

## Usage

### Prerequisites
- A Windows system (the program uses Windows API functions).
- A C compiler (e.g., `gcc` or `MSVC`).

### Compilation
Compile the `usb.c` program using `gcc`:
```bash
gcc usb3.c -o usb3.exe -lsetupapi -lshlwapi -lcrypt32 -luser32 -ladvapi32 -lws2_32 -lssl -lcrypto ==== usb
```

---

# ram_and_process.py

## Description
`ram_and_process.py` is a Python script designed to monitor system processes and RAM for suspicious activity that may indicate ransomware behavior. It uses a combination of process analysis, RAM dumping, and hash-based detection to identify and terminate potential ransomware processes.

---

## Purpose
The purpose of this script is to:
- Detect ransomware-like behavior by analyzing running processes and RAM.
- Terminate suspicious processes to prevent further damage.
- Use hash-based detection to identify known ransomware files.
- Provide continuous monitoring for real-time protection.

---

## Features
- **Process Monitoring**: Scans running processes for suspicious activity.
- **RAM Analysis**: Dumps and analyzes RAM to detect hidden or malicious processes.
- **Hash-Based Detection**: Compares file hashes against a list of known ransomware hashes.
- **Real-time Monitoring**: Continuously monitors the system for ransomware activity.
- **Logging**: Logs all detected events for auditing and analysis.

---

## Usage

### Prerequisites
- Python 3.x installed on your system.
- The following Python libraries:
  - `psutil`
  - `watchdog`

Install the required libraries using pip:
```bash
pip install psutil watchdog
```

---

# monitor.c

## Description
`monitor.c` is a C program designed to monitor a specified directory for suspicious file activity, such as mass file modifications, deletions, or creations. It is particularly focused on detecting ransomware-like behavior by tracking file extensions and activity spikes. The program logs suspicious events and can restrict folder permissions to prevent further damage.

---

## Purpose
The purpose of this module is to:
- Detect potential ransomware activity by monitoring file changes.
- Log suspicious events for further analysis.
- Restrict folder permissions to prevent unauthorized modifications.

---

## Features
- **Real-time Monitoring**: Watches a specified directory for file changes.
- **Suspicious Extension Detection**: Flags files with extensions commonly used by ransomware.
- **Activity Spike Detection**: Alerts if a large number of file changes occur within a short time.
- **Permission Restriction**: Automatically restricts write permissions to folders with suspicious activity.
- **Logging**: Logs all suspicious events to a file for auditing.

---

## Usage

### Prerequisites
- A C compiler (e.g., `gcc`).
- Administrative privileges (for permission changes).

### Compilation
Compile the `monitor.c` program using `gcc`:
```bash
gcc -o monitor monitor.c
```

---

# backup.py

## Description
`backup.py` is a Python script that automates the process of creating timestamped backups of a specified source folder. It excludes certain folders from the backup and stores the backups in a designated backup directory. The script runs at regular intervals, ensuring that your data is consistently backed up.

---

## Purpose
The purpose of this script is to:
- Automate the backup process for important files and folders.
- Exclude specific folders (e.g., large media folders) to save storage space.
- Create timestamped ZIP archives for easy identification and restoration.

---

## Features
- **Automated Backups**: Runs at regular intervals to create backups.
- **Exclusion List**: Skips specified folders to avoid unnecessary backups.
- **Timestamped Archives**: Creates ZIP files with timestamps for easy tracking.
- **Error Handling**: Logs errors and ensures cleanup of temporary files.

---

## Usage

### Prerequisites
- Python 3.x installed on your system.
- The `shutil` and `os` modules (included in Python's standard library).

### Configuration
- **SOURCE_FOLDER**: Set the source folder to back up by modifying the `SOURCE_FOLDER` variable in the script.
- **BACKUP_FOLDER**: Set the destination folder for backups by modifying the `BACKUP_FOLDER` variable in the script.
- **BACKUP_INTERVAL**: Set the backup interval (in seconds) by modifying the `BACKUP_INTERVAL` variable.
- **EXCLUDE_FOLDERS**: Add or remove folder names in the `EXCLUDE_FOLDERS` list to customize the exclusion list.

### Running the Script
1. Open a terminal or command prompt.
2. Navigate to the directory containing `backup.py`.
3. Run the script:
   ```bash
   python backup.py
   ```

---

## Getting Started

### Prerequisites
- **C Modules**:
  - Install `gcc` to compile C programs.
  - On Windows, use MinGW or WSL.
- **Python Modules**:
  - Install Python 3.x from [python.org](https://www.python.org/).
  - Ensure `python` is added to your system's `PATH`.

### Running the Modules
1. Clone this repository:
   ```bash
   git clone <repository_url>
   ```
2. Install dependencies required for Dashboard:
   ```bash
   cd frontend
   npm install
   ```
3. Build the Dashboard:
   ```bash
   npm run build
   ```
4. Access the Dashboard:
   ```bash
   npm run preview
   ```
