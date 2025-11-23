#include <iostream>
#include <windows.h>
#include "OS13_HTCOM_LIB.h"

void PrintUsage() {
    std::cout << "Usage: OS13_CREATE <filename> <capacity> <maxKeyLength> <maxDataLength> <snapshotInterval>" << std::endl;
    std::cout << "Example: OS13_CREATE storage.ht 1000 256 1024 5000" << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc != 6) {
        PrintUsage();
        return 1;
    }

    // Инициализация COM
    if (!InitializeHTCOM()) {
        std::cerr << "Error: Failed to initialize COM. Error code: " << GetLastErrorHT() << std::endl;
        return 1;
    }

    const char* fileName = argv[1];
    DWORD capacity = atoi(argv[2]);
    DWORD maxKeyLength = atoi(argv[3]);
    DWORD maxDataLength = atoi(argv[4]);
    DWORD snapshotInterval = atoi(argv[5]);

    if (capacity == 0 || maxKeyLength == 0 || maxDataLength == 0) {
        std::cerr << "Error: Invalid parameters" << std::endl;
        UninitializeHTCOM();
        return 1;
    }

    HTStorage* ht = HTCreate(fileName, capacity, maxKeyLength, maxDataLength, snapshotInterval);

    if (ht == NULL) {
        std::cerr << "Error: Failed to create HT storage. Error code: " << GetLastErrorHT() << std::endl;
        UninitializeHTCOM();
        return 1;
    }

    std::cout << "HT-Storage Created filename=" << fileName
        << ", snapshotinterval=" << snapshotInterval
        << ", capacity=" << capacity
        << ", maxkeylength=" << maxKeyLength
        << ", maxdatalength=" << maxDataLength << std::endl;

    HTClose(ht);
    UninitializeHTCOM();

    return 0;
}