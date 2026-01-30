#include <windows.h>
#include <tlhelp32.h>
#include <iostream>

int main() {
    DWORD pid = GetCurrentProcessId();
    std::cout << "PID: " << pid << std::endl;

    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (snap == INVALID_HANDLE_VALUE) {
        std::cout << "Error occurred while creating a snapshot!" << std::endl;
        return 1;
    }

    THREADENTRY32 te;
    te.dwSize = sizeof(THREADENTRY32);

    if (Thread32First(snap, &te)) {
        do {
            if (te.th32OwnerProcessID == pid) {
                std::cout << "TID: " << te.th32ThreadID << std::endl;
            }
        } while (Thread32Next(snap, &te));
    }
    else {
        std::cout << "Can't find the first thread!" << std::endl;
        CloseHandle(snap);
        return 1;
    }

    CloseHandle(snap);
    return 0;
}
