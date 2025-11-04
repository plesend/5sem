#pragma comment(lib, "D:\\лабораторные работы\\сп\\лабы\\lab2\\OS11_HTAPI_DLL\\x64\\Debug\\OS11_HTAPI_DLL.lib")

#include "Header.h"
#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <windows.h>

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
    if (!h) {
        cerr << "Open failed: " << GetLastError() << endl;
        return 1;
    }

    // --- shutdown event ---
    unsigned int hash = HT::HashFunction(filename, (int)strlen(filename));
    char evnamebuf[64];
    sprintf_s(evnamebuf, sizeof(evnamebuf), "Global\\HT_shutdown_%08X", hash);
    HANDLE hShutdownEvent = CreateEventA(NULL, TRUE, FALSE, evnamebuf);
    if (!hShutdownEvent)
        cerr << "Warning: CreateEventA failed (" << GetLastError() << "). Continue..." << endl;

    srand((unsigned)time(NULL));
    ofstream log("OS11_04.log", ios::app);
    if (!log.is_open()) {
        cerr << "Cannot open log file." << endl;
        return 1;
    }

    cout << "Started OS11_04. Reading+incrementing every 1 second..." << endl;
    log << "=== OS11_04 started at " << time(nullptr) << " ===" << endl;

    while (true) {
        if (hShutdownEvent && WaitForSingleObject(hShutdownEvent, 0) == WAIT_OBJECT_0) {
            cout << "Shutdown signal received. Exiting..." << endl;
            log << "[EXIT] Shutdown event signaled." << endl;
            break;
        }

        int key = rand() % 50;
        Element e(&key, sizeof(key));

        Element* found = Get(h, &e);
        if (found) {
            int value = 0;
            if (found->payloadlength >= (int)sizeof(int))
                memcpy(&value, found->payload, sizeof(int));

            int newvalue = value + 1;
            Update(h, found, &newvalue, sizeof(newvalue));

            cout << "[UPDATE] key=" << key << " old=" << value << " new=" << newvalue << endl;
            log << "[UPDATE] key=" << key << " old=" << value << " new=" << newvalue << endl;

            delete[](BYTE*)found->key;
            delete[](BYTE*)found->payload;
            delete found;
        }
        else {
            cout << "[MISS] key=" << key << endl;
            log << "[MISS] key=" << key << endl;
        }

        // ждем 1 секунду, но реагируем на shutdown
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

    if (hShutdownEvent) CloseHandle(hShutdownEvent);
    Snap(h);
    Close(h);

    log << "=== OS11_04 finished ===" << endl;
    log.close();
    cout << "OS11_04 exited cleanly." << endl;
    return 0;
}