#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <psapi.h>
#include <shlwapi.h>


#pragma comment(lib, "Psapi.lib")
#pragma comment(lib, "Shlwapi.lib")

//#define FAKE_FILE_PATH "C:\\Users\\Public\\fake_sys_file.dll"
#define LOG_FILE "C:\\Users\\Public\\intrusion_log.txt"
#define QUARANTINE_FILE "C:\\Users\\Public\\quarantined_processes.txt"
#define NUM_FAKE_FILES 3

// Structure to store quarantined process details
typedef struct {
    DWORD processID;
    TCHAR processName[MAX_PATH];
} QuarantinedProcess;

QuarantinedProcess quarantinedProcesses[100];
int quarantineCount = 0;

const char* FAKE_FILE_PATHS[NUM_FAKE_FILES] = {
    "C:\\Users\\Public\\fake_sys_file1.dll",
    "C:\\Users\\Dynamic\\Videos\\fake_sys_file2.exe",
    "C:\\Users\\Dynamic\\Music\\fake_sys_file3.reg"
};


// Function to create a fake system file
void create_fake_files() {
    for (int i = 0; i < NUM_FAKE_FILES; i++) {
        SetFileAttributes(FAKE_FILE_PATHS[i], FILE_ATTRIBUTE_NORMAL); // Remove read-only attribute if exists
        DeleteFile(FAKE_FILE_PATHS[i]); // Delete the file if it already exists

        FILE *file = fopen(FAKE_FILE_PATHS[i], "w");
        if (file) {
            fprintf(file, "This is a fake system file. Do not modify.\n");
            fclose(file);
            SetFileAttributes(FAKE_FILE_PATHS[i], FILE_ATTRIBUTE_READONLY); // Set file as read-only
            printf("Fake system file created at: %s\n", FAKE_FILE_PATHS[i]);
        } else {
            printf("ERROR: Could not create fake system file at %s! Check permissions.\n", FAKE_FILE_PATHS[i]);
        }
    }
}

// Function to log detected intrusions
void log_intrusion(const char *event_type, DWORD processID, const char *processName) {
    FILE *log = fopen(LOG_FILE, "a");
    if (log) {
        SYSTEMTIME st;
        GetSystemTime(&st);
        fprintf(log, "[%04d-%02d-%02d %02d:%02d:%02d] ALERT: %s detected on fake system file! Process: %s (PID: %lu)\n",
                st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, event_type, processName, processID);
        fclose(log);
    }
    printf("ALERT: %s detected on fake system file! Process: %s (PID: %lu)\n", event_type, processName, processID);
}

// Function to get the process name from a process ID
void get_process_name(DWORD processID, TCHAR *processName) {
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID);
    if (hProcess) {
        HMODULE hMod;
        DWORD cbNeeded;
        if (EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded)) {
            GetModuleBaseName(hProcess, hMod, processName, MAX_PATH);
        }
        CloseHandle(hProcess);
    } else {
        _tcscpy(processName, TEXT("<unknown>"));
    }
}

// Function to quarantine a process by suspending it
void quarantine_process(DWORD processID, TCHAR *processName) {
    HANDLE hProcess = OpenProcess(PROCESS_SUSPEND_RESUME, FALSE, processID);
    if (hProcess) {
        typedef LONG (WINAPI *NtSuspendProcess)(IN HANDLE);
        NtSuspendProcess pfnNtSuspendProcess = (NtSuspendProcess)GetProcAddress(GetModuleHandle("ntdll.dll"), "NtSuspendProcess");

        if (pfnNtSuspendProcess) {
            pfnNtSuspendProcess(hProcess);
            printf("Process %s (PID: %lu) has been quarantined (suspended).\n", processName, processID);

            // Store process details
            quarantinedProcesses[quarantineCount].processID = processID;
            _tcscpy(quarantinedProcesses[quarantineCount].processName, processName);
            quarantineCount++;

            // Log the quarantine event
            FILE *log = fopen(QUARANTINE_FILE, "a");
            if (log) {
                fprintf(log, "%lu %s\n", processID, processName);
                fclose(log);
            }
        }
        CloseHandle(hProcess);
    } else {
        printf("ERROR: Could not suspend process %lu\n", processID);
    }
}

void monitor_directory();

typedef struct {
    char directory[MAX_PATH];
    char file[MAX_PATH];
} MonitorArgs;

void monitor_directory_wrapper(LPVOID lpParam) {
    MonitorArgs *args = (MonitorArgs*)lpParam;
    monitor_directory(args->directory, args->file);
    free(args); // Free allocated memory after function returns
}

// Function to monitor the fake file for access
void monitor_directory(char location[], char file[]) {
    HANDLE hDir = CreateFile(
        location,  
        FILE_LIST_DIRECTORY,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS,
        NULL);

    if (hDir == INVALID_HANDLE_VALUE) {
        printf("ERROR: Could not monitor directory! Check permissions.\n");
        return;
    }

    char buffer[1024];
    DWORD bytesReturned;
    FILE_NOTIFY_INFORMATION *pNotify;

    WCHAR wFile[MAX_PATH];
    MultiByteToWideChar(CP_ACP, 0, file, -1, wFile, MAX_PATH);

    while (1) {
        if (ReadDirectoryChangesW(
                hDir, buffer, sizeof(buffer), FALSE,
                FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_ATTRIBUTES | FILE_NOTIFY_CHANGE_CREATION,
                &bytesReturned, NULL, NULL)) {

            pNotify = (FILE_NOTIFY_INFORMATION *)buffer;

            WCHAR fileName[MAX_PATH];
            wcsncpy(fileName, pNotify->FileName, pNotify->FileNameLength / sizeof(WCHAR));
            fileName[pNotify->FileNameLength / sizeof(WCHAR)] = L'\0';

            if (wcscmp(fileName, wFile) == 0) {
                DWORD processID = GetCurrentProcessId();
                TCHAR processName[MAX_PATH];
                get_process_name(processID, processName);
                log_intrusion("Unauthorized Access/Modification", processID, processName);
                quarantine_process(processID, processName);
            }
        }
    }

    CloseHandle(hDir);
}


// Function to unquarantine (resume) a process based on admin request
void unquarantine_process(DWORD processID) {
    HANDLE hProcess = OpenProcess(PROCESS_SUSPEND_RESUME, FALSE, processID);
    if (hProcess) {
        typedef LONG (WINAPI *NtResumeProcess)(IN HANDLE);
        NtResumeProcess pfnNtResumeProcess = (NtResumeProcess)GetProcAddress(GetModuleHandle("ntdll.dll"), "NtResumeProcess");

        if (pfnNtResumeProcess) {
            pfnNtResumeProcess(hProcess);
            printf("Process with PID %lu has been resumed (unquarantined).\n", processID);
        }
        CloseHandle(hProcess);
    } else {
        printf("ERROR: Could not resume process %lu. It may no longer exist.\n", processID);
    }
}

// Function to handle admin commands
void admin_interface() {
    while (1) {
        printf("\nADMIN COMMANDS:\n1. List quarantined processes\n2. Unquarantine a process\n3. Exit\nEnter your choice: ");
        int choice;
        scanf("%d", &choice);

        if (choice == 1) {
            printf("\nQuarantined Processes:\n");
            for (int i = 0; i < quarantineCount; i++) {
                printf("[%d] Process: %s (PID: %lu)\n", i + 1, quarantinedProcesses[i].processName, quarantinedProcesses[i].processID);
            }
        } else if (choice == 2) {
            printf("\nEnter PID to unquarantine: ");
            DWORD pid;
            scanf("%lu", &pid);
            unquarantine_process(pid);
        } else if (choice == 3) {
            break;
        } else {
            printf("Invalid option.\n");
        }
    }
}

int main() {
    DWORD pid = GetCurrentProcessId();
    printf("Process ID: %d\n", pid);

    create_fake_files();

    HANDLE hThreads[NUM_FAKE_FILES];

    for (int i = 0; i < NUM_FAKE_FILES; i++) {
        MonitorArgs *args = (MonitorArgs*)malloc(sizeof(MonitorArgs));
        if (!args) {
            printf("ERROR: Memory allocation failed!\n");
            continue;
        }

        // Extract directory path
        strcpy(args->directory, FAKE_FILE_PATHS[i]);
        PathRemoveFileSpec(args->directory);

        // Extract file name
        strcpy(args->file, strrchr(FAKE_FILE_PATHS[i], '\\') + 1);

        // Create thread
        hThreads[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)monitor_directory_wrapper, (void*)args, 0, NULL);
        if (!hThreads[i]) {
            printf("ERROR: Failed to create monitoring thread for %s\n", FAKE_FILE_PATHS[i]);
            free(args);
        }
    }

    admin_interface(); 

    // Wait for all threads to finish
    WaitForMultipleObjects(NUM_FAKE_FILES, hThreads, TRUE, INFINITE);

    return 0;
}