import os
import shutil
import time
from datetime import datetime

SOURCE_FOLDER = "E:/ransomeware/front-end/src"
BACKUP_FOLDER = "E:/Docker/backup_storage"
BACKUP_INTERVAL = 600  # 10 minutes

# List of folders to ignore
EXCLUDE_FOLDERS = ["My Music", "My Pictures", "My Videos"]

def create_backup():
    """Creates a timestamped ZIP backup of the source folder."""
    if not os.path.exists(SOURCE_FOLDER):
        print(f"Error: Source folder {SOURCE_FOLDER} does not exist.")
        return

    # Create a temporary directory to store files before zipping
    temp_backup_folder = os.path.join(BACKUP_FOLDER, "temp_backup")
    if os.path.exists(temp_backup_folder):
        shutil.rmtree(temp_backup_folder)
    os.makedirs(temp_backup_folder, exist_ok=True)

    try:
        # Copy files and directories, excluding the ones in EXCLUDE_FOLDERS, to the temporary folder
        for item in os.listdir(SOURCE_FOLDER):
            if item in EXCLUDE_FOLDERS:
                print(f"[⚠] Skipping protected folder: {item}")
                continue

            src = os.path.join(SOURCE_FOLDER, item)
            dest = os.path.join(temp_backup_folder, item)

            if os.path.isdir(src):
                shutil.copytree(src, dest, dirs_exist_ok=True)
            else:
                shutil.copy2(src, dest)

        # Create a timestamped ZIP archive of the temporary backup folder
        timestamp = datetime.now().strftime("%Y-%m-%d_%H-%M-%S")
        zip_filename = os.path.join(BACKUP_FOLDER, f"backup_{timestamp}")
        shutil.make_archive(zip_filename, 'zip', temp_backup_folder)
        print(f"[✔] Backup created at: {zip_filename}.zip")

    except Exception as e:
        print(f"[❌] Backup failed: {e}")

    finally:
        # Clean up the temporary folder
        if os.path.exists(temp_backup_folder):
            shutil.rmtree(temp_backup_folder)

if __name__ == "__main__":
    print(f"🔄 Auto-backup started. Backing up {SOURCE_FOLDER} every {BACKUP_INTERVAL} seconds...")

    while True:
        create_backup()
        time.sleep(BACKUP_INTERVAL)
