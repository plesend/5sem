#pragma comment(lib, "D:\\лабораторные работы\\сп\\лабы\\lab2\\OS11_HTAPI_DLL\\x64\\Debug\\OS11_HTAPI_DLL.lib")

#include "Header.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <windows.h>
#include <fstream>

using namespace std;
using namespace HT;
int main(int argc, char** argv)
{
    if (argc < 2) {
        cout << "Usage: " << argv[0] << " <filename>\n";
        return 1;
    }

    const char* filename = argv[1];
    HTHANDLE* h = Open(filename);
    cout << "h->File: " << h->File << endl;
    cout << "h->FileMapping: " << h->FileMapping << endl;
    cout << "h->Addr: " << (void*)h->Addr << endl;
    cout << "h->DataAddr: " << (void*)h->DataAddr << endl;
    cout << "h->Mutex: " << h->Mutex << endl;
    cout << "Capacity: " << h->Capacity << " MaxKeyLength: " << h->MaxKeyLength
        << " MaxPayloadLength: " << h->MaxPayloadLength << endl;

    if (!h) {
        cerr << "Open failed: " << (h ? h->LastErrorMessage : "unknown") << endl;
        return 1;
    }


    // --- создаЄм shutdown event (аналогично OS11_START и OS11_03) ---
    unsigned int hash = HT::HashFunction(filename, (int)strlen(filename));
    char evnamebuf[64];
    sprintf_s(evnamebuf, sizeof(evnamebuf), "Global\\HT_shutdown_%08X", hash);

    HANDLE hShutdownEvent = CreateEventA(NULL, TRUE, FALSE, evnamebuf);
    if (!hShutdownEvent)
        cerr << "Warning: CreateEventA failed (" << (h ? h->LastErrorMessage : "unknown") << "). Continue..." << endl;
    else
        cout << "Shutdown event opened: " << evnamebuf << endl;

    // --- инициализаци€ ---
    srand((unsigned)time(NULL));
    ofstream log("OS11_02.log", ios::app);
    if (!log.is_open()) {
        cerr << "Cannot open log file." << endl;
        return 1;
    }

    cout << "Started OS11_02. Inserting elements every 1 second..." << endl;
    log << "=== OS11_02 started at " << time(nullptr) << " ===" << endl;

    while (true) {
        // ѕровер€ем сигнал завершени€
        if (hShutdownEvent && WaitForSingleObject(hShutdownEvent, 0) == WAIT_OBJECT_0) {
            cout << "Shutdown signal received. Exiting..." << endl;
            log << "[EXIT] Shutdown event signaled." << endl;
            break;
        }

        int key = rand() % 50; // 50 вариантов ключей
        int value = 0;         // 32-битное число, равное 0
        Element e(&key, sizeof(key), &value, sizeof(value));

        if (!Insert(h, &e)) {
            cerr << "[FAILED] key=" << key << " err=" << (h ? h->LastErrorMessage : "unknown") << endl;
            log << "[FAILED] key=" << key << " err=" << (h ? h->LastErrorMessage : "unknown") << endl;
        }
        else {
            cout << "[INSERT] key=" << key << " value=0" << endl;
            log << "[INSERT] key=" << key << " value=0" << endl;
        }

        // ќжидание 1 секунды или выхода
        if (hShutdownEvent) {
            DWORD res = WaitForSingleObject(hShutdownEvent, 1000);
            if (res == WAIT_OBJECT_0) {
                cout << "Shutdown during wait. Exiting..." << endl;
                log << "[EXIT] Shutdown during wait." << endl;
                break;
            }
        }
        else {
            this_thread::sleep_for(chrono::seconds(1));
        }
    }

    if (hShutdownEvent)
        CloseHandle(hShutdownEvent);

    Snap(h);
    Close(h);

    log << "=== OS11_02 finished ===" << endl;
    log.close();

    cout << "OS11_02 exited cleanly." << endl;
    return 0;
}