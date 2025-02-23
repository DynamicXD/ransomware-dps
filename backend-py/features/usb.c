#include <windows.h>
#include <stdio.h>
#include <setupapi.h>
#include <devguid.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <tchar.h>
#include <openssl/evp.h>
#include <winreg.h>
#include <initguid.h>
#include <usbiodef.h>
#include <dbt.h>
#include <string.h>

// ... [Previous includes and definitions remain the same]
#pragma comment(lib, "setupapi.lib")
#pragma comment(lib, "shlwapi.lib")

#define LOG_FILE "usb_activity.log"
#define WHITELIST_FILE "whitelisted.txt"
#define MAX_DEVICES 10
#define MAX_DEVICE_LENGTH 256
#define MAX_DEVICE_ID_LEN 200  // Adjust the size if necessary


// Function prototype for log_usb_event
void log_usb_event(const char *event, const char *device) {
    FILE *log = fopen(LOG_FILE, "a");
    if (log) {
        fprintf(log, "[%s] USB Event: %s - %s\n", __TIME__, event, device);
        fclose(log);
    }
}






// Function to check if a USB device is allowed by reading whitelist.txt
int is_device_allowed(const char *device) {
    FILE *whitelist_file = fopen(WHITELIST_FILE, "r");
    if (!whitelist_file) {
        printf("Error: Could not open whitelist file.\n");
        return 0;
    }
    char line[MAX_DEVICE_LENGTH];
    while (fgets(line, sizeof(line), whitelist_file)) {
        line[strcspn(line, "\n")] = 0;
        if (strstr(device, line)) {
            fclose(whitelist_file);
            return 1;
        }
    }
    fclose(whitelist_file);
    return 0;
}

// Function to add a new USB device to the whitelist
void add_device_to_whitelist(const char *device) {
    FILE *whitelist_file = fopen(WHITELIST_FILE, "a");
    if (whitelist_file) {
        fprintf(whitelist_file, "%s\n", device);
        fclose(whitelist_file);
        printf("Device added to whitelist: %s\n", device);
        log_usb_event("Device Whitelisted", device);
    } else {
        printf("Error: Could not open whitelist file for writing.\n");
    }
}

// Function to disable a USB device
/*void disable_usb_device(const char *deviceInstanceId) {
    HDEVINFO deviceInfoSet = SetupDiGetClassDevs(NULL, "USB", NULL, DIGCF_PRESENT | DIGCF_ALLCLASSES);
    if (deviceInfoSet == INVALID_HANDLE_VALUE) {
        printf("❌ Error: Could not get device list.\n");
        return;
    }

    printf("running");

    SP_DEVINFO_DATA deviceInfoData;
    deviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

    for (DWORD i = 0; SetupDiEnumDeviceInfo(deviceInfoSet, i, &deviceInfoData); i++) {
        printf("for loop running\n");
        char instanceId[MAX_DEVICE_ID_LEN];
        if (SetupDiGetDeviceInstanceId(deviceInfoSet, &deviceInfoData, instanceId, sizeof(instanceId), NULL)) {
            printf("if 1 is running\n");
            if (strstr(instanceId, deviceInstanceId)) {
                printf("if 2 is running\n");
                SP_PROPCHANGE_PARAMS params;
                params.ClassInstallHeader.cbSize = sizeof(SP_CLASSINSTALL_HEADER);
                params.ClassInstallHeader.InstallFunction = DIF_PROPERTYCHANGE;
                params.StateChange = DICS_DISABLE;
                params.Scope = DICS_FLAG_GLOBAL;
                params.HwProfile = 0;

                if (SetupDiSetClassInstallParams(deviceInfoSet, &deviceInfoData, &params.ClassInstallHeader, sizeof(params)) &&
                    SetupDiCallClassInstaller(DIF_PROPERTYCHANGE, deviceInfoSet, &deviceInfoData)) {
                    printf("🚫 USB device blocked: %s\n", instanceId);
                } else {
                    printf("❌ Failed to disable USB device.\n");
                }
                break;
            }
        }
    }
    SetupDiDestroyDeviceInfoList(deviceInfoSet);
}
*/

/* 'this is the better one' void disable_usb_device(const char *deviceInstanceId) {
    HDEVINFO deviceInfoSet = SetupDiGetClassDevs(NULL, "USB", NULL, DIGCF_PRESENT | DIGCF_ALLCLASSES);
    if (deviceInfoSet == INVALID_HANDLE_VALUE) {
        printf("❌ Error: Could not get device list.\n");
        return;
    }

    // Add debug output to see what we're looking for
    printf("Trying to disable device: %s\n", deviceInstanceId);

    SP_DEVINFO_DATA deviceInfoData;
    deviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

    for (DWORD i = 0; SetupDiEnumDeviceInfo(deviceInfoSet, i, &deviceInfoData); i++) {
        char instanceId[MAX_DEVICE_ID_LEN];
        if (SetupDiGetDeviceInstanceId(deviceInfoSet, &deviceInfoData, instanceId, sizeof(instanceId), NULL)) {
            // Add debug output to see what we're comparing against
            printf("Checking device: %s\n", instanceId);

            // Convert the device path to a device instance ID
            char *convertedId = strstr(deviceInstanceId, "USB");
            if (convertedId && strstr(instanceId, convertedId)) {
                printf("Match found! Attempting to disable...\n");
                
                SP_PROPCHANGE_PARAMS params;
                params.ClassInstallHeader.cbSize = sizeof(SP_CLASSINSTALL_HEADER);
                params.ClassInstallHeader.InstallFunction = DIF_PROPERTYCHANGE;
                params.StateChange = DICS_DISABLE;
                params.Scope = DICS_FLAG_GLOBAL;
                params.HwProfile = 0;

                if (SetupDiSetClassInstallParams(deviceInfoSet, &deviceInfoData, &params.ClassInstallHeader, sizeof(params)) &&
                    SetupDiCallClassInstaller(DIF_PROPERTYCHANGE, deviceInfoSet, &deviceInfoData)) {
                    printf("🚫 USB device successfully blocked: %s\n", instanceId);
                } else {
                    DWORD error = GetLastError();
                    printf("❌ Failed to disable USB device. Error code: %lu\n", error);
                }
                break;
            }
        }
    }
    SetupDiDestroyDeviceInfoList(deviceInfoSet);
}
*/

void disable_usb_device(const char *deviceInstanceId) {
    HDEVINFO deviceInfoSet = SetupDiGetClassDevs(NULL, "USB", NULL, DIGCF_PRESENT | DIGCF_ALLCLASSES);
    if (deviceInfoSet == INVALID_HANDLE_VALUE) {
        printf("❌ Error: Could not get device list.\n");
        return;
    }

    printf("Trying to disable device: %s\n", deviceInstanceId);
    
    // Convert the input path to a comparable format
    char searchPattern[MAX_DEVICE_ID_LEN];
    const char *usbPart = strstr(deviceInstanceId, "USB");
    if (usbPart) {
        // Extract VID and PID
        char vid[10], pid[10], serial[50];
        if (sscanf(usbPart, "USB#VID_%[^&]&PID_%[^#]#%[^{]", vid, pid, serial) == 3) {
            // Remove any trailing characters from serial
            char *p = serial;
            while (*p) {
                if (*p == '#' || *p == '{' || *p == '\r' || *p == '\n') {
                    *p = '\0';
                    break;
                }
                p++;
            }
            
            // Create the pattern in the correct format
            snprintf(searchPattern, sizeof(searchPattern), "USB\\VID_%s&PID_%s\\%s", vid, pid, serial);
            printf("Looking for pattern: %s\n", searchPattern);
        }
    }

    SP_DEVINFO_DATA deviceInfoData;
    deviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

    BOOL found = FALSE;
    for (DWORD i = 0; SetupDiEnumDeviceInfo(deviceInfoSet, i, &deviceInfoData); i++) {
        char instanceId[MAX_DEVICE_ID_LEN];
        if (SetupDiGetDeviceInstanceId(deviceInfoSet, &deviceInfoData, instanceId, sizeof(instanceId), NULL)) {
            printf("Checking device: %s\n", instanceId);

            if (strcmp(instanceId, searchPattern) == 0) {  // Using exact match now
                printf("Match found! Attempting to disable...\n");
                found = TRUE;
                
                SP_PROPCHANGE_PARAMS params;
                params.ClassInstallHeader.cbSize = sizeof(SP_CLASSINSTALL_HEADER);
                params.ClassInstallHeader.InstallFunction = DIF_PROPERTYCHANGE;
                params.StateChange = DICS_DISABLE;
                params.Scope = DICS_FLAG_GLOBAL;
                params.HwProfile = 0;

                if (SetupDiSetClassInstallParams(deviceInfoSet, &deviceInfoData, &params.ClassInstallHeader, sizeof(params))) {
                    if (SetupDiCallClassInstaller(DIF_PROPERTYCHANGE, deviceInfoSet, &deviceInfoData)) {
                        printf("🚫 USB device successfully blocked: %s\n", instanceId);
                    } else {
                        DWORD error = GetLastError();
                        printf("❌ Failed to call class installer. Error code: %lu\n", error);
                    }
                } else {
                    DWORD error = GetLastError();
                    printf("❌ Failed to set class install params. Error code: %lu\n", error);
                }
                break;
            }
        }
    }

    if (!found) {
        printf("❌ Device not found with pattern: %s\n", searchPattern);
    }

    SetupDiDestroyDeviceInfoList(deviceInfoSet);
}

// New function to get drive letter from device name
char* get_drive_letter(const char* device_name) {
    static char drive_letter[3] = {0};
    // Look for "USB#" in device name
    const char* usb_pos = strstr(device_name, "USB#");
    if (!usb_pos) return NULL;

    // Get volume information
    char volume_name[MAX_PATH];
    char fs_name[MAX_PATH];
    DWORD serial_number;
    DWORD max_component_length;
    DWORD fs_flags;
    
    // Check all possible drive letters
    for (char letter = 'A'; letter <= 'Z'; letter++) {
        sprintf(drive_letter, "%c:", letter);
        if (GetDriveType(drive_letter) == DRIVE_REMOVABLE) {
            if (GetVolumeInformation(drive_letter, volume_name, MAX_PATH, 
                &serial_number, &max_component_length, &fs_flags, 
                fs_name, MAX_PATH)) {
                return drive_letter;
            }
        }
    }
    return NULL;
}

// New function to scan and remove .inf files
void scan_and_remove_inf_files(const char* drive_letter) {
    char search_path[MAX_PATH];
    WIN32_FIND_DATA find_data;
    HANDLE find_handle;
    
    // Create search path for .inf files
    sprintf(search_path, "%s\\*.inf", drive_letter);
    
    // Start searching for .inf files
    find_handle = FindFirstFile(search_path, &find_data);
    if (find_handle != INVALID_HANDLE_VALUE) {
        do {
            char file_path[MAX_PATH];
            sprintf(file_path, "%s\\%s", drive_letter, find_data.cFileName);
            
            // Try to delete the file
            if (DeleteFile(file_path)) {
                printf("🗑️ Removed INF file: %s\n", file_path);
                log_usb_event("Removed INF file", file_path);
            } else {
                printf("❌ Failed to remove INF file: %s (Error: %lu)\n", 
                    file_path, GetLastError());
                log_usb_event("Failed to remove INF file", file_path);
            }
        } while (FindNextFile(find_handle, &find_data));
        
        FindClose(find_handle);
    }
}

// Modified detect_malicious_usb function to include INF file scanning
void detect_malicious_usb(const char *device) {
    // First, check if device is allowed (existing functionality)
    if (!is_device_allowed(device)) {
        printf("⚠️ Potential Malicious USB Detected: %s\n", device);
        log_usb_event("Malicious USB Detected", device);
        printf("Do you want to add this device to the whitelist? (y/n): ");
        char response;
        scanf(" %c", &response);
        if (response == 'y' || response == 'Y') {
            add_device_to_whitelist(device);
        } else {
            printf("🚫 Blocking USB device: %s\n", device);
            disable_usb_device(device);
            return;  // Don't scan blocked devices
        }
    }
    
    // Get drive letter and scan for INF files
    char* drive = get_drive_letter(device);
    if (drive) {
        printf("📝 Scanning for INF files on drive %s\n", drive);
        scan_and_remove_inf_files(drive);
    } else {
        printf("❌ Could not determine drive letter for device: %s\n", device);
    }
}

// ... [Rest of the existing code remains unchanged]

// Window procedure for handling USB events
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg == WM_DEVICECHANGE) {
        PDEV_BROADCAST_HDR pHdr = (PDEV_BROADCAST_HDR)lParam;
        if (pHdr && pHdr->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE) {
            PDEV_BROADCAST_DEVICEINTERFACE pDevInf = (PDEV_BROADCAST_DEVICEINTERFACE)pHdr;
            if (wParam == DBT_DEVICEARRIVAL) {
                printf("🔌 USB Inserted: %s\n", pDevInf->dbcc_name);
                log_usb_event("USB Inserted", pDevInf->dbcc_name);
                detect_malicious_usb(pDevInf->dbcc_name);
            } else if (wParam == DBT_DEVICEREMOVECOMPLETE) {
                printf("USB Removed: %s\n", pDevInf->dbcc_name);
                log_usb_event("USB Removed", pDevInf->dbcc_name);
            }
        }
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

// Function to create a hidden window for USB event monitoring
void monitor_usb() {
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = "USBMonitorWindow";

    if (!RegisterClass(&wc)) {
        printf("❌ Error: Could not register window class.\n");
        return;
    }

    HWND hwnd = CreateWindow("USBMonitorWindow", "USBMonitor", 0, 0, 0, 0, 0, NULL, NULL, wc.hInstance, NULL);
    if (!hwnd) {
        printf("❌ Error: Could not create hidden window.\n");
        return;
    }

    DEV_BROADCAST_DEVICEINTERFACE notificationFilter = {0};
    notificationFilter.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
    notificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
    notificationFilter.dbcc_classguid = GUID_DEVINTERFACE_USB_DEVICE;

    HDEVNOTIFY hDeviceNotify = RegisterDeviceNotification(hwnd, &notificationFilter, DEVICE_NOTIFY_WINDOW_HANDLE);
    if (!hDeviceNotify) {
        printf("❌ Error: RegisterDeviceNotification failed (%lu)\n", GetLastError());
        return;
    }

    printf("🔒 USB Protection System Running... Waiting for USB events.\n");

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

int main() {
    monitor_usb();
    return 0;
}