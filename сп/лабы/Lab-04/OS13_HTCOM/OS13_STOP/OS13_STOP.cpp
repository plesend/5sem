#include <iostream>
#include <windows.h>
#include "OS13_HTCOM_LIB.h"

void PrintUsage() {
    std::cout << "Usage: OS13_STOP <filename>" << std::endl;
    std::cout << "Example: OS13_STOP storage.ht" << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        PrintUsage();
        return 1;
    }

    // Инициализация COM
    if (!InitializeHTCOM()) {
        std::cerr << "Error: Failed to initialize COM. Error code: " << GetLastErrorHT() << std::endl;
        return 1;
    }

    const char* fileName = argv[1];

    // Создаем COM-объект через фабрику
    IHT* pHT = nullptr;
    HRESULT hr = CoCreateInstance(
        CLSID_OS13,
        NULL,
        CLSCTX_INPROC_SERVER,
        IID_IHT,
        (void**)&pHT
    );

    if (FAILED(hr) || pHT == nullptr) {
        std::cerr << "Error: Failed to create COM object. HRESULT: " << std::hex << hr << std::endl;
        UninitializeHTCOM();
        return 1;
    }

    std::cout << "Attempting to open HT storage via COM: " << fileName << std::endl;

    // Пытаемся открыть хранилище через COM-интерфейс
    HTStorage* ht = nullptr;
    hr = pHT->HTOpen(fileName, &ht);

    if (FAILED(hr) || ht == nullptr) {
        std::cerr << "Error: Failed to open HT storage via COM. HRESULT: " << std::hex << hr << std::endl;

        DWORD error = 0;
        pHT->GetLastErrorHT(&error);
        std::cerr << "Last HT error: " << error << std::endl;

        // Проверяем, существует ли файл вообще
        HANDLE hFile = CreateFileA(fileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hFile == INVALID_HANDLE_VALUE) {
            DWORD fileError = GetLastError();
            if (fileError == ERROR_FILE_NOT_FOUND) {
                std::cout << "File does not exist: " << fileName << std::endl;
            }
            else {
                std::cout << "File exists but cannot be opened (may be in use). Error: " << fileError << std::endl;
            }
        }
        else {
            std::cout << "File exists but COM cannot access it" << std::endl;
            CloseHandle(hFile);
        }

        pHT->Release();
        UninitializeHTCOM();
        return 1;
    }

    std::cout << "Successfully opened HT storage via COM" << std::endl;
    // Проверяем глобальное состояние хранилища
    WaitForSingleObject(ht->hMutex, INFINITE);
    BOOL isStopped = ht->pHeader->isStorageStopped;
    ReleaseMutex(ht->hMutex);

    if (isStopped) {
        std::cout << "HT-Storage is already stopped" << std::endl;
        pHT->HTClose(ht);
        pHT->Release();
        UninitializeHTCOM();
        return 0;
    }

    std::cout << "Stopping HT-Storage via COM..." << std::endl;

    // Выполняем snapshot перед остановкой через COM
    std::cout << "Creating snapshot via COM..." << std::endl;
    hr = pHT->HTSnapshot(ht);
    if (FAILED(hr)) {
        std::cerr << "Warning: Failed to create snapshot via COM. HRESULT: " << std::hex << hr << std::endl;
    }
    else {
        std::cout << "Snapshot created successfully via COM" << std::endl;
    }

    // Останавливаем хранилище через COM
    hr = pHT->HTStop(ht);
    if (FAILED(hr)) {
        std::cerr << "Error: Failed to stop HT storage via COM. HRESULT: " << std::hex << hr << std::endl;
        pHT->HTClose(ht);
        pHT->Release();
        UninitializeHTCOM();
        return 1;
    }

    std::cout << "HT-Storage successfully stopped!" << std::endl;
    std::cout << "Filename: " << fileName << std::endl;
    std::cout << "Snapshot interval: " << ht->pHeader->snapshotInterval << std::endl;
    std::cout << "Capacity: " << ht->pHeader->capacity << std::endl;
    std::cout << "Max key length: " << ht->pHeader->maxKeyLength << std::endl;
    std::cout << "Max data length: " << ht->pHeader->maxDataLength << std::endl;

    // Закрываем хранилище через COM
    pHT->HTClose(ht);
    pHT->Release();
    UninitializeHTCOM();

    std::cout << "HT-Storage successfully closed" << std::endl;

    return 0;
}