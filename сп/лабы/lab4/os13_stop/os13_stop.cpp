#include "os13_lib.h"
#include "os13_htcom.h"
#include <iostream>
#include <windows.h>

void PrintUsage() {
    std::cout << "Usage: OS13_STOP <filename>" << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        PrintUsage();
        return 1;
    }

    HRESULT hr = InitializeCOM();
    if (FAILED(hr)) {
        std::cerr << "Error: Failed to initialize COM. HRESULT: 0x" << std::hex << hr << std::endl;
        return 1;
    }

    const char* fileName = argv[1];

    IHT* pIHT = nullptr;
    hr = GetIHTInterface(&pIHT);
    if (FAILED(hr) || pIHT == nullptr) {
        std::cerr << "Error: Failed to get IHT interface. HRESULT: 0x" << std::hex << hr << std::endl;
        UninitializeCOM();
        return 1;
    }

    std::cout << "Attempting to open HT storage: " << fileName << std::endl;

    HTHANDLE* ht = nullptr;
    hr = pIHT->Open(fileName, &ht);

    if (FAILED(hr) || ht == nullptr) {
        std::cerr << "Error: Failed to open HT storage. HRESULT: 0x" << std::hex << hr << std::endl;
        pIHT->Release();
        UninitializeCOM();
        return 1;
    }

    std::cout << "Successfully opened HT storage" << std::endl;

   
    std::cout << "Stopping HT-Storage..." << std::endl;

    std::cout << "Creating snapshot..." << std::endl;
    hr = pIHT->Snap(ht);
    if (FAILED(hr)) {
        std::cerr << "Warning: Failed to create snapshot. HRESULT: 0x" << std::hex << hr << std::endl;
    }
    else {
        std::cout << "Snapshot created successfully" << std::endl;
    }

    hr = pIHT->Stop(ht);
    if (FAILED(hr)) {
        std::cerr << "Error: Failed to stop HT storage. HRESULT: 0x" << std::hex << hr << std::endl;
        pIHT->Close(ht);
        pIHT->Release();
        UninitializeCOM();
        return 1;
    }
    HANDLE stopMutex = CreateMutexA(NULL, FALSE, "ht_stop_mutex");
    WaitForSingleObject(stopMutex, INFINITE);

    std::cout << "HT-Storage successfully stopped! press any key to continue" << std::endl;
    std::cin.get();
    ReleaseMutex(stopMutex);
    CloseHandle(stopMutex);

    pIHT->Close(ht);
    pIHT->Release();

    hr = UninitializeCOM();
    if (FAILED(hr)) {
        std::cerr << "Warning: Failed to uninitialize COM properly. HRESULT: 0x" << std::hex << hr << std::endl;
    }

    std::cout << "HT-Storage successfully closed" << std::endl;

    return 0;
}