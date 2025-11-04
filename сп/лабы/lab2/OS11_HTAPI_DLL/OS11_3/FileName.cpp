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
    if (!h) {
        cerr << "Open failed: " << GetLastError() << endl;
        return 1;
    }

    // --- создаём shutdown event ---
    unsigned int hash = HT::HashFunction(filename, (int)strlen(filename));
    char evnamebuf[64];
    sprintf_s(evnamebuf, sizeof(evnamebuf), "Global\\HT_shutdown_%08X", hash);

    HANDLE hShutdownEvent = CreateEventA(NULL, TRUE, FALSE, evnamebuf);
    if (!hShutdownEvent)
        cerr << "Warning: CreateEventA failed (" << GetLastError() << "). Continue..." << endl;
    else
        cout << "Shutdown event opened: " << evnamebuf << endl;

    srand((unsigned)time(NULL));
    ofstream log("OS11_03.log", ios::app);
    if (!log.is_open()) {
        cerr << "Cannot open log file." << endl;
        return 1;
    }

    cout << "Started OS11_03. Deleting elements every 1 second..." << endl;
    log << "=== OS11_03 started at " << time(nullptr) << " ===" << endl;

    while (true) {
        // Проверяем сигнал завершения
        if (hShutdownEvent && WaitForSingleObject(hShutdownEvent, 0) == WAIT_OBJECT_0) {
            cout << "Shutdown signal received. Exiting..." << endl;
            log << "[EXIT] Shutdown event signaled." << endl;
            break;
        }

        int key = rand() % 50; // 50 вариантов ключей
        Element e(&key, sizeof(key));

        if (!Delete(h, &e)) {
            cerr << "[NOT FOUND] key=" << key << endl;
            log << "[NOT FOUND] key=" << key << endl;
        }
        else {
            cout << "[DELETE] key=" << key << endl;
            log << "[DELETE] key=" << key << endl;
        }

        // Ожидание 1 секунды или выхода
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

    log << "=== OS11_03 finished ===" << endl;
    log.close();

    cout << "OS11_03 exited cleanly." << endl;
    return 0;
}