#include "../OS11_HTAPI/HT.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <windows.h> // дл€ CreateEventA, WaitForSingleObject, CloseHandle
#include <cstring>

using namespace std;

static uint hashFunction(const void* key, int keyLength)
{
    int hash = 5381;


    const char* str = static_cast<const char*>(key);

    for (int i = 0; i < keyLength; ++i) {


        hash = ((hash << 5) + hash) + str[i];

    }

    cout << "--Hash: current Hash value: " << hash << endl;


    return hash;
}

int main(int argc, char** argv)
{
    if (argc < 2) {
        cout << "Usage: " << argv[0] << " <filename>\n";
        return 1;
    }

    const char* filename = argv[1];
    HTHANDLE* h = Open(filename);
    if (!h) {
        cerr << "Open failed: " << HTGetLastError(nullptr) << endl;
        return 1;
    }

    // --- create/open shutdown event for this storage (same naming as library) ---
    unsigned int hsh = hashFunction(filename, (int)strlen(filename));
    char evnamebuf[64];
    sprintf_s(evnamebuf, sizeof(evnamebuf), "Global\\HT_shutdown_%08X", hsh);

    HANDLE hShutdownEvent = CreateEventA(NULL, TRUE, FALSE, evnamebuf);
    if (hShutdownEvent == NULL) {
        std::cerr << "Warning: CreateEventA(shutdown) failed: " << GetLastError()
            << " Ч continuing without global shutdown support." << std::endl;
    }
    else {
        std::cout << "Shutdown event opened: " << evnamebuf << std::endl;
    }

    srand((unsigned)time(NULL));

    while (true) {
        
        if (hShutdownEvent) {
            DWORD s = WaitForSingleObject(hShutdownEvent, 0); 
            if (s == WAIT_OBJECT_0) {
                cout << "Shutdown event signaled Ч exiting main loop." << endl;
                break;
            }
        }

        int key = rand() % 50;
        int value = rand() % 1000; // пример значени€
        Element e(&key, sizeof(key), &value, sizeof(value));

        if (!Insert(h, &e)) {
            cerr << "Insert failed: " << HTGetLastError(h) << endl;
        }
        else {
            cout << "[INSERT] key=" << key << " value=" << value << endl;
        }

        // ѕосле вставки ждЄм либо 1 секунду, либо пока не придЄт сигнал shutdown
        if (hShutdownEvent) {
            DWORD waitRes = WaitForSingleObject(hShutdownEvent, 1000); 
            if (waitRes == WAIT_OBJECT_0) {
                cout << "Shutdown event signaled during wait Ч exiting." << endl;
                break;
            }
        }
        else {
            this_thread::sleep_for(chrono::seconds(1));
        }
    }


    if (hShutdownEvent) {
        CloseHandle(hShutdownEvent);
        hShutdownEvent = NULL;
    }


    Snap(h);
    Close(h);

    cout << "Worker exited cleanly." << endl;
    return 0;
}