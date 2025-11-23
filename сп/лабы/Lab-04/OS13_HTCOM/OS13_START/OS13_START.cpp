#include <iostream>
#include <windows.h>
#include <conio.h>
#include "OS13_HTCOM_LIB.h"

void PrintUsage() {
    std::cout << "Usage: OS13_START <filename>" << std::endl;
    std::cout << "Example: OS13_START storage.ht" << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        PrintUsage();
        return 1;
    }

    std::cout << "[DEBUG] Starting application..." << std::endl;

    // Инициализация COM
    std::cout << "[DEBUG] Initializing COM..." << std::endl;
    if (!InitializeHTCOM()) {
        std::cerr << "Error: Failed to initialize COM. Error code: " << GetLastErrorHT() << std::endl;
        return 1;
    }
    std::cout << "[DEBUG] COM initialized successfully" << std::endl;

    const char* fileName = argv[1];
    std::cout << "[DEBUG] Checking file: " << fileName << std::endl;

    HANDLE hFile = CreateFileA(fileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        std::cout << "File not found: " << fileName << " (Error: " << GetLastError() << ")" << std::endl;
        std::cout << "Current directory: ";
        system("cd");
        UninitializeHTCOM();
        return 1;
    }
    CloseHandle(hFile);

    std::cout << "File exists, opening HT storage..." << std::endl;
    std::cout << "[DEBUG] Calling HTOpen..." << std::endl;

    HTStorage* ht = HTOpen(fileName);
    std::cout << "[DEBUG] HTOpen returned: " << ht << std::endl;
    std::cout << "[DEBUG] Last error: " << GetLastErrorHT() << std::endl;

    if (ht == NULL) {
        std::cerr << "Error: Failed to open HT storage. Error code: " << GetLastErrorHT() << std::endl;
        UninitializeHTCOM();
        return 1;
    }

    std::cout << "[DEBUG] Calling HTStart..." << std::endl;
    WaitForSingleObject(ht->hMutex, INFINITE);
    ht->pHeader->isStorageStopped = FALSE;  // Снимаем блокировку
    FlushViewOfFile(ht->pHeader, sizeof(HTHeader)); // Сохраняем на диск
    ReleaseMutex(ht->hMutex);
    if (!HTStart(ht)) {
        std::cerr << "Error: Failed to start HT storage. Error code: " << GetLastErrorHT() << std::endl;
        HTClose(ht);
        UninitializeHTCOM();
        return 1;
    }
    std::cout << "[DEBUG] HTStart successful" << std::endl;

    // Проверка структуры перед доступом
    std::cout << "[DEBUG] Checking HT structure..." << std::endl;
    std::cout << "[DEBUG] ht pointer: " << ht << std::endl;
    std::cout << "[DEBUG] ht->pHeader pointer: " << ht->pHeader << std::endl;

    if (ht->pHeader == NULL) {
        std::cerr << "ERROR: ht->pHeader is NULL!" << std::endl;
        HTStop(ht);
        HTClose(ht);
        UninitializeHTCOM();
        return 1;
    }

    std::cout << "HT-Storage Start filename=" << fileName
        << ", snapshotinterval=" << ht->pHeader->snapshotInterval
        << ", capacity=" << ht->pHeader->capacity
        << ", maxkeylength=" << ht->pHeader->maxKeyLength
        << ", maxdatalength=" << ht->pHeader->maxDataLength << std::endl;

    std::cout << "Press any key to stop..." << std::endl;

    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);

    DWORD snapshotIntervalMs = ht->pHeader->snapshotInterval;

    while (!_kbhit()) {
        LARGE_INTEGER currentTime;
        QueryPerformanceCounter(&currentTime);

        LONGLONG elapsedMs = (currentTime.QuadPart - ht->pHeader->lastSnapshotTime.QuadPart) * 1000 / frequency.QuadPart;

        if (elapsedMs >= snapshotIntervalMs) {
            std::cout << "[SNAPSHOT] Creating snapshot... (" << elapsedMs << "ms elapsed)" << std::endl;
            HTSnapshot(ht);
            std::cout << "[SNAPSHOT] Snapshot created successfully" << std::endl;
        }

        Sleep(100);
    }
    _getch();

    HTStop(ht);
    HTClose(ht);
    UninitializeHTCOM();

    std::cout << "HT-Storage stopped" << std::endl;

    return 0;
}