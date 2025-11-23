#include <iostream>
#include <windows.h>
#include <random>
#include <string>
#include <ctime>
#include <atomic>
#include <vector>
#include "OS13_HTCOM_LIB.h"

std::atomic<bool> stopFlag(false);

BOOL WINAPI ConsoleHandler(DWORD signal) {
    if (signal == CTRL_C_EVENT) {
        std::cout << "\n[INFO] Received Ctrl+C. Shutting down gracefully..." << std::endl;
        stopFlag = true;
        return TRUE;
    }
    return FALSE;
}

std::vector<std::string> GenerateKeys03(HTStorage* ht) {
    int maxKeyLength = ht->pHeader->maxKeyLength;
    const std::string charset = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

    std::mt19937 gen02(11111);
    std::mt19937 gen03(22222);
    std::uniform_int_distribution<> dis(0, charset.size() - 1);

    std::vector<std::string> keys;

    for (int i = 0; i < 15; ++i) {
        std::string key;
        for (int j = 0; j < maxKeyLength; ++j) {
            key += charset[dis(gen02)];
        }
        keys.push_back(key);
    }

    for (int i = 0; i < 35; ++i) {
        std::string key;
        for (int j = 0; j < maxKeyLength; ++j) {
            key += charset[dis(gen03)];
        }
        keys.push_back(key);
    }

    return keys;
}

bool IsStorageConnected(HTStorage* ht) {
    if (ht == NULL) return false;
    if (ht->pHeader == NULL) return false;
    if (ht->hMutex == NULL) return false;
    if (ht->hFileMapping == NULL) return false;
    if (ht->hFile == INVALID_HANDLE_VALUE) return false;
    if (ht->pHeader->capacity == 0 || ht->pHeader->capacity > 1000000) return false;
    if (ht->pHeader->maxKeyLength == 0 || ht->pHeader->maxKeyLength > 10000) return false;
    if (ht->pHeader->maxDataLength == 0 || ht->pHeader->maxDataLength > 100000) return false;

    LARGE_INTEGER frequency;
    LARGE_INTEGER currentTime;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&currentTime);

    LONGLONG elapsedMs = (currentTime.QuadPart - ht->pHeader->lastHeartbeat.QuadPart) * 1000 / frequency.QuadPart;

    if (elapsedMs > 10000) {
        std::cout << "[WARNING] Server heartbeat timeout: " << elapsedMs << "ms" << std::endl;
        return false;
    }

    return true;
}

void PrintUsage() {
    std::cout << "Usage: OS_0403 <filename>" << std::endl;
    std::cout << "Example: OS_0403 storage.ht" << std::endl;
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

    if (!SetConsoleCtrlHandler(ConsoleHandler, TRUE)) {
        std::cerr << "[ERROR] Failed to set control handler" << std::endl;
        UninitializeHTCOM();
        return 1;
    }

    std::cout << "=== OS_0403 Delete Client Application ===" << std::endl;
    std::cout << "[INFO] Connecting to HT storage: " << fileName << std::endl;
    std::cout << "[INFO] Press Ctrl+C to stop application" << std::endl;

    HTStorage* ht = HTOpen(fileName);
    if (ht == NULL) {
        std::cerr << "[ERROR] Failed to open HT storage! Error code: " << GetLastErrorHT() << std::endl;
        std::cerr << "[ERROR] Make sure OS13_START is running with the same file!" << std::endl;
        UninitializeHTCOM();
        return 1;
    }

   

    std::cout << "[SUCCESS] Connected to HT storage" << std::endl;
    std::cout << "[INFO] Storage capacity: " << ht->pHeader->capacity << std::endl;
    std::cout << "[INFO] Current element count: " << ht->pHeader->elementCount << std::endl;
    std::cout << "[INFO] Max key length: " << ht->pHeader->maxKeyLength << std::endl;
    std::cout << "[INFO] Max data length: " << ht->pHeader->maxDataLength << std::endl;

    std::vector<std::string> keys = GenerateKeys03(ht);
    std::cout << "[INFO] Generated " << keys.size() << " keys for deletion" << std::endl;
    std::cout << "[INFO] First 15 keys match OS_0402, rest are different" << std::endl;

    for (int i = 0; i < keys.size(); ++i) {
        std::cout << "[KEY] " << (i + 1) << ": " << keys[i] << std::endl;
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> keyDist(0, keys.size() - 1);

    int operationCount = 0;
    int successCount = 0;
    int failCount = 0;

    std::cout << "\n[INFO] Starting DELETE operations (1 operation per second)" << std::endl;
    std::cout << "[INFO] Will delete keys if they exist in storage" << std::endl;
    std::cout << "-------------------------------------------------" << std::endl;

    while (!stopFlag) {
        if (!IsStorageConnected(ht)) {
            std::cout << "[ERROR] Connection to HT storage lost! Server might be stopped." << std::endl;
            stopFlag = true;
            break;
        }

        int keyIndex = keyDist(gen);
        const std::string& key = keys[keyIndex];
        bool isSharedKey = (keyIndex < 15);

        operationCount++;

        BOOL keyExists = FALSE;
        char* existingData = HTGet(ht, key.c_str());
        if (existingData != NULL) {
            keyExists = TRUE;
            delete[] existingData;
        }

        BOOL deleteResult = HTDelete(ht, key.c_str());
        DWORD error = GetLastErrorHT();

        if (deleteResult) {
            successCount++;
            std::string type = isSharedKey ? " [SHARED]" : " [UNIQUE]";
            std::cout << "[SUCCESS] Operation #" << operationCount
                << " | DELETE key: '" << key << "' - SUCCESSFULLY DELETED" << std::endl;
        }
        else {
            failCount++;

            std::string errorType = "UNKNOWN_ERROR";
            if (error == 0) {
                if (!keyExists) {
                    errorType = "KEY_NOT_FOUND";
                }
                else {
                    errorType = "DELETE_FAILED";
                }
            }
            else if (error == 6) {
                errorType = "STORAGE_DISCONNECTED";
                std::cout << "[ERROR] Storage connection lost! Server stopped." << std::endl;
                stopFlag = true;
                break;
            }

            std::cout << "[FAILED] Operation #" << operationCount
                << " | DELETE key: '" << key << "'"
                << " | Reason: " << errorType << " (Error code: " << error << ")" << std::endl;
        }

        for (int i = 0; i < 10 && !stopFlag; ++i) {
            Sleep(100);
            if (!IsStorageConnected(ht)) {
                std::cout << "[ERROR] Connection lost during sleep!" << std::endl;
                stopFlag = true;
                break;
            }
        }
    }

    std::cout << "------------------------------------" << std::endl;
    std::cout << "[INFO] Application shutdown initiated" << std::endl;

    if (IsStorageConnected(ht)) {
        HTStop(ht);
        HTClose(ht);
        std::cout << "[INFO] Storage connection closed properly" << std::endl;
    }
    else {
        std::cout << "[INFO] Storage already disconnected" << std::endl;
    }

    UninitializeHTCOM();

    std::cout << "=== Final Statistics ===" << std::endl;
    std::cout << "Total operations: " << operationCount << std::endl;
    std::cout << "Successful deletions: " << successCount << std::endl;
    std::cout << "Failed operations: " << failCount << std::endl;

    if (operationCount > 0) {
        double successRate = (successCount * 100.0) / operationCount;
        std::cout << "Success rate: " << successRate << "%" << std::endl;
    }
    else {
        std::cout << "Success rate: 0%" << std::endl;
    }

    std::cout << "[INFO] OS_0403 stopped gracefully" << std::endl;

    return 0;
}