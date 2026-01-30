#include <iostream>
#include <windows.h>
#include <conio.h>
#include <csignal>
#include "os13_lib.h"

using namespace std;

bool active = true;

void sigintHandler(int) {
    cout << "\nStopping START..." << endl;
    active = false;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        cerr << "usage: os13_start.exe {filename}" << endl;
        return 1;
    }

    const char* filename = argv[1];
    signal(SIGINT, sigintHandler);

    HANDLE hStartEvent = CreateEventA(NULL, TRUE, FALSE, "ht_start_event"); // manual-reset
    HANDLE hAliveEvent = CreateEventA(NULL, TRUE, FALSE, "ht_alive_event");  // manual-reset

    if (!hStartEvent || !hAliveEvent) {
        cerr << "Failed to create global events" << endl;
        if (hStartEvent) CloseHandle(hStartEvent);
        if (hAliveEvent) CloseHandle(hAliveEvent);
        return 1;
    }

    HRESULT hr = InitializeCOM();
    if (FAILED(hr)) {
        cerr << "Failed to initialize COM. HRESULT: 0x" << hex << hr << endl;
        CloseHandle(hStartEvent);
        CloseHandle(hAliveEvent);
        return 1;
    }

    IHT* pIHT = nullptr;
    hr = GetIHTInterface(&pIHT);
    if (FAILED(hr) || !pIHT) {
        cerr << "Failed to get IHT interface. HRESULT: 0x" << hex << hr << endl;
        UninitializeCOM();
        CloseHandle(hStartEvent);
        CloseHandle(hAliveEvent);
        return 1;
    }

    HTHANDLE* ht = nullptr;
    hr = pIHT->Open(filename, &ht);
    if (FAILED(hr) || !ht) {
        cerr << "Failed to open HT storage. HRESULT: 0x" << hex << hr << endl;
        pIHT->Release();
        UninitializeCOM();
        CloseHandle(hStartEvent);
        CloseHandle(hAliveEvent);
        return 1;
    }

    SetEvent(hAliveEvent);   // START жив
    SetEvent(hStartEvent);   // разрешаем работу клиентам

    cout << "START running. Press Ctrl+C to stop..." << endl;

    while (active) {
        Sleep(ht->SecSnapshotInterval * 1000);

        hr = pIHT->Snap(ht);
        if (FAILED(hr))
            cerr << "Snapshot failed. HRESULT: 0x" << hex << hr << endl;
        else
            cout << "Snapshot created successfully" << endl;
    }

    ResetEvent(hStartEvent); 
    ResetEvent(hAliveEvent); 

    hr = pIHT->Close(ht);
    pIHT->Release();
    UninitializeCOM();

    CloseHandle(hStartEvent);
    CloseHandle(hAliveEvent);

    cout << "START stopped" << endl;
    return 0;
}
