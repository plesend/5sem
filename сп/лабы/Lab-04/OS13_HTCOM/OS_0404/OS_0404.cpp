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

std::vector<std::string> GenerateKeys04(HTStorage* ht) {
    int maxKeyLength = ht->pHeader->maxKeyLength;
    const std::string charset = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

    std::mt19937 gen02(11111);
    std::mt19937 gen04(33333);
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
            key += charset[dis(gen04)];
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
    std::cout << "Usage: OS_0404 <filename>" << std::endl;
    std::cout << "Example: OS_0404 storage.ht" << std::endl;
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

    std::cout << "=== OS_0404 Read/Update Client Application ===" << std::endl;
    std::cout << "[INFO] Connecting to HT storage: " << fileName << std::endl;
    std::cout << "[INFO] Press Ctrl+C to stop application" << std::endl;

    HTStorage* ht = HTOpen(fileName);
    if (ht == NULL) {
        std::cerr << "[ERROR] Failed to open HT storage!" << std::endl;
        UninitializeHTCOM();
        return 1;
    }

    std::cout << "[SUCCESS] Connected to HT storage" << std::endl;
    std::cout << "[INFO] Storage capacity: " << ht->pHeader->capacity << std::endl;
    std::cout << "[INFO] Current element count: " << ht->pHeader->elementCount << std::endl;
    std::cout << "[INFO] Max key length: " << ht->pHeader->maxKeyLength << std::endl;
    std::cout << "[INFO] Max data length: " << ht->pHeader->maxDataLength << std::endl;

    std::vector<std::string> keys = GenerateKeys04(ht);
    std::cout << "[INFO] Generated " << keys.size() << " keys for read/update:" << std::endl;
    std::cout << "[INFO] First 15 keys match OS_0402, rest 35 are different" << std::endl;

    for (int i = 0; i < keys.size(); ++i) {
        std::cout << "[KEY] " << (i + 1) << ": " << keys[i] << std::endl;
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> keyDist(0, keys.size() - 1);

    int operationCount = 0;
    int readSuccessCount = 0;
    int updateSuccessCount = 0;
    int failCount = 0;

    std::cout << "\n[INFO] Starting READ/UPDATE operations (1 operation per second)" << std::endl;
    std::cout << "[INFO] If data found, value will be incremented by 1" << std::endl;
    std::cout << "------------------------------------" << std::endl;

    while (!stopFlag) {
        if (!IsStorageConnected(ht)) {
            std::cout << "[ERROR] Connection lost! Server stopped." << std::endl;
            stopFlag = true;
            break;
        }

        int keyIndex = keyDist(gen);
        const std::string& key = keys[keyIndex];
        bool isSharedKey = (keyIndex < 15);

        operationCount++;

        char* existingData = HTGet(ht, key.c_str());

        if (existingData != NULL) {
            readSuccessCount++;

            try {
                int currentValue = std::stoi(existingData);
                int newValue = currentValue + 1;
                std::string newData = std::to_string(newValue);

                if (HTUpdate(ht, key.c_str(), newData.c_str())) {
                    updateSuccessCount++;
                    std::string type = isSharedKey ? " [SHARED]" : " [UNIQUE]";
                    std::cout << "[SUCCESS] Operation #" << operationCount
                        << " | KEY: '" << key << "'" << type
                        << " | READ: " << currentValue
                        << " | UPDATE: " << newValue << " (+1)" << std::endl;
                }
                else {
                    failCount++;
                    std::cout << "[ERROR] Update failed for key: '" << key << "'" << std::endl;
                }
            }
            catch (const std::exception& e) {
                failCount++;
                std::cout << "[ERROR] Invalid data format for key: '" << key
                    << "', data: '" << existingData << "'" << std::endl;
            }

            delete[] existingData;
        }
        else {
            failCount++;
            std::string type = isSharedKey ? " [SHARED-NOT-FOUND]" : " [UNIQUE-NOT-FOUND]";
            std::cout << "[FAILED] Operation #" << operationCount
                << " | KEY: '" << key << "'" << type
                << " | Data not found" << std::endl;
        }

        for (int i = 0; i < 10 && !stopFlag; ++i) {
            Sleep(100);
        }
    }

    std::cout << "-------------------------------" << std::endl;
    std::cout << "[INFO] Application shutdown initiated" << std::endl;

    if (IsStorageConnected(ht)) {
        HTStop(ht);
        HTClose(ht);
    }

    UninitializeHTCOM();

    std::cout << "=== Final Statistics ===" << std::endl;
    std::cout << "Total operations: " << operationCount << std::endl;
    std::cout << "Successful reads: " << readSuccessCount << std::endl;
    std::cout << "Successful updates: " << updateSuccessCount << std::endl;
    std::cout << "Failed operations: " << failCount << std::endl;

    if (operationCount > 0) {
        double readRate = (readSuccessCount * 100.0) / operationCount;
        double updateRate = (updateSuccessCount * 100.0) / operationCount;
        std::cout << "Read success rate: " << readRate << "%" << std::endl;
        std::cout << "Update success rate: " << updateRate << "%" << std::endl;
    }

    std::cout << "[INFO] OS_0404 stopped gracefully" << std::endl;

    return 0;
}