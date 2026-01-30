#include <iostream>
#include <windows.h>
#include <thread>
#include <chrono>
#include "os13_lib.h"

using namespace std;

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        cerr << "usage: os0402.exe {filename}" << endl;
        return 1;
    }

    char* filepath = argv[1];

    HANDLE hStartEvent = nullptr;
    while (!hStartEvent)
    {
        hStartEvent = OpenEventA(SYNCHRONIZE, FALSE, "ht_start_event");
        if (!hStartEvent) Sleep(100);
    }

    HRESULT hr = InitializeCOM();
    if (FAILED(hr)) {
        cerr << "Initialize COM failed. HRESULT: 0x" << hex << hr << endl;
        CloseHandle(hStartEvent);
        return 1;
    }

    IHT* pIHT = nullptr;
    hr = GetIHTInterface(&pIHT);
    if (FAILED(hr) || !pIHT) {
        cerr << "Failed to get IHT interface. HRESULT: 0x" << hex << hr << endl;
        UninitializeCOM();
        CloseHandle(hStartEvent);
        return 1;
    }

    HTHANDLE* ht = nullptr;
    hr = pIHT->Open(filepath, &ht);
    if (FAILED(hr) || !ht) {
        cerr << "Failed to open HT storage via COM. HRESULT: 0x" << hex << hr << endl;
        pIHT->Release();
        UninitializeCOM();
        CloseHandle(hStartEvent);
        return 1;
    }

    int maxKeys = ht->Capacity;
    cout << "HT storage opened. Capacity: " << maxKeys << endl;

    int i = 0;

    while (true) {
        DWORD waitResult = WaitForSingleObject(hStartEvent, 0);
        if (waitResult != WAIT_OBJECT_0) {
            cout << "Waiting for START..." << endl;
            do {
                this_thread::sleep_for(chrono::milliseconds(500));
                waitResult = WaitForSingleObject(hStartEvent, 0);
            } while (waitResult != WAIT_OBJECT_0);
            cout << "START resumed, resuming insertions..." << endl;
        }

        string key = "key" + to_string((rand() + maxKeys) % maxKeys);
        Element e;
        e.key = key.c_str();
        e.keylength = (int)key.length();
        e.payload = &i;
        e.payloadlength = sizeof(int);

        if (SUCCEEDED(pIHT->Insert(ht, &e)))
            cout << "Inserted element " << i << " with key: " << key << endl;
        else
            cerr << "Failed to insert element " << i << " with key: " << key << endl;

        i++;
        this_thread::sleep_for(chrono::seconds(1));
    }

    hr = pIHT->Close(ht);
    if (FAILED(hr))
        cerr << "Warning: Failed to close HT storage properly. HRESULT: 0x" << hex << hr << endl;

    pIHT->Release();
    UninitializeCOM();
    CloseHandle(hStartEvent);

    return 0;
}
