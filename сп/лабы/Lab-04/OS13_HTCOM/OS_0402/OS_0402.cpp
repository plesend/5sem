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

std::vector<std::string> GenerateKeys02(HTStorage* ht) {
    int maxKeyLength = ht->pHeader->maxKeyLength;
    const std::string charset = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    std::mt19937 gen(11111);
    std::uniform_int_distribution<> dis(0, charset.size() - 1);

    std::vector<std::string> keys;
    for (int i = 0; i < 50; ++i) {
        std::string key;
        for (int j = 0; j < maxKeyLength; ++j) {
            key += charset[dis(gen)];
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
    std::cout << "Usage: OS_0402 <filename>" << std::endl;
    std::cout << "Example: OS_0402 storage.ht" << std::endl;
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

    std::cout << "=== OS_0402 Client Application ===" << std::endl;
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
    std::cout << "[INFO] Max key length: " << ht->pHeader->maxKeyLength << std::endl;
    std::cout << "[INFO] Max data length: " << ht->pHeader->maxDataLength << std::endl;

    std::vector<std::string> keys = GenerateKeys02(ht);
    std::cout << "[INFO] Generated " << keys.size() << " keys for insertion" << std::endl;
    for (int i = 0; i < keys.size(); ++i) {
        std::cout << "[KEY] " << (i + 1) << ": " << keys[i] << std::endl;
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> keyDist(0, keys.size() - 1);

    int operationCount = 0;
    int successCount = 0;
    int failCount = 0;
    const char* data = "0";

    std::cout << "\n[INFO] Starting data insertion (1 operation per second)" << std::endl;
    std::cout << "[INFO] Data value: " << data << " (32-bit integer)" << std::endl;
    std::cout << "-------------------------------" << std::endl;

    while (!stopFlag) {
        if (!IsStorageConnected(ht)) {
            std::cout << "[ERROR] Connection to HT storage lost! Server might be stopped." << std::endl;
            stopFlag = true;
            break;
        }
        int keyIndex = keyDist(gen);
        const std::string& key = keys[keyIndex];

        operationCount++;
        BOOL insertResult = HTInsert(ht, key.c_str(), data);
        DWORD error = GetLastErrorHT();

        if (insertResult) {
            successCount++;
            std::cout << "[SUCCESS] Operation #" << operationCount
                << " | INSERT key: '" << key << "', data: " << data << std::endl;
        }
        else {
            failCount++;

            std::string errorType = "UNKNOWN_ERROR";
            if (error == 0) {
                errorType = "KEY_ALREADY_EXISTS or CAPACITY_FULL";
            }
            else if (error == 6) {
                errorType = "STORAGE_DISCONNECTED";
                std::cout << "[ERROR] Storage connection lost! Server stopped." << std::endl;
                stopFlag = true;
                break;
            }

            std::cout << "[FAILED] Operation #" << operationCount
                << " | INSERT key: '" << key << "', data: " << data
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

    std::cout << "--------------------------------------" << std::endl;
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
    std::cout << "Successful: " << successCount << std::endl;
    std::cout << "Failed: " << failCount << std::endl;
    std::cout << "Success rate: " << (operationCount > 0 ? (successCount * 100 / operationCount) : 0) << "%" << std::endl;
    std::cout << "[INFO] OS_0402 stopped gracefully" << std::endl;

    return 0;
}
