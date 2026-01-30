#include "os13_lib.h"
#include <iostream>
#include <windows.h>

void PrintUsage() {
    std::cout << "Usage: OS13_CREATE <filename> <capacity> <maxKeyLength> <maxPayloadLength> <snapshotInterval>" << std::endl;
}

int main(int argc, char* argv[]) {

    if (argc != 6) {
        PrintUsage();
        return 1;
    }

    HRESULT hr = InitializeCOM();
    if (FAILED(hr)) {
        std::cerr << "Error: Failed to initialize COM. HRESULT: 0x" << std::hex << hr << std::endl;
        return 1;
    }

    const char* fileName = argv[1];
    int capacity = atoi(argv[2]);
    int maxKeyLength = atoi(argv[3]);
    int maxPayloadLength = atoi(argv[4]);
    int snapshotInterval = atoi(argv[5]);


    if (capacity <= 0 || maxKeyLength <= 0 || maxPayloadLength <= 0 || snapshotInterval <= 0) {
        std::cerr << "Error: Invalid parameters - all values must be positive integers" << std::endl;
        UninitializeCOM();
        return 1;
    }

    HTHANDLE* ht = nullptr;
    hr = HTCreate(capacity, snapshotInterval, maxKeyLength, maxPayloadLength, fileName, &ht);

    if (FAILED(hr) || ht == nullptr) {
        std::cerr << "Error: Failed to create HT storage. HRESULT: 0x" << std::hex << hr << std::endl;
        UninitializeCOM();
        return 1;
    }

    std::cout << "HT-Storage Created successfully:" << std::endl;
    std::cout << "  Filename: " << fileName << std::endl;
    std::cout << "  Capacity: " << capacity << std::endl;
    std::cout << "  Max Key Length: " << maxKeyLength << std::endl;
    std::cout << "  Max Payload Length: " << maxPayloadLength << std::endl;
    std::cout << "  Snapshot Interval: " << snapshotInterval << " seconds" << std::endl;

    std::cout << "\nDetailed information:" << std::endl;
    HTPrintInfo(ht);

    hr = HTClose(ht);
    if (FAILED(hr)) {
        std::cerr << "Warning: Failed to close HT storage properly. HRESULT: 0x" << std::hex << hr << std::endl;
    }

    hr = UninitializeCOM();
    if (FAILED(hr)) {
        std::cerr << "Warning: Failed to uninitialize COM properly. HRESULT: 0x" << std::hex << hr << std::endl;
    }

    std::cout << "\nHT-Storage creation completed successfully!" << std::endl;
    return 0;
}