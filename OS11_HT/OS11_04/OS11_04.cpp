#ifdef _DEBUG
#pragma comment(lib, "../x64/Debug/OS11_HTAPI.lib")
#endif

#include "../OS11_HTAPI/HT.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <windows.h>
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


    unsigned int hsh = hashFunction(filename, (int)strlen(filename));

    char startupName[64], shutdownName[64];
    sprintf_s(startupName, "Global\\HT_startup_%08X", hsh);
    sprintf_s(shutdownName, "Global\\HT_shutdown_%08X", hsh);

    HANDLE hStartupEvent = OpenEventA(SYNCHRONIZE, FALSE, startupName);
    HANDLE hShutdownEvent = OpenEventA(SYNCHRONIZE, FALSE, shutdownName);

    cout << "Worker started. Waiting for storage startup..." << endl;

    while (true) {
        if (!hStartupEvent) {
            
            hStartupEvent = OpenEventA(SYNCHRONIZE, FALSE, startupName);
            if (!hStartupEvent) {
                this_thread::sleep_for(chrono::seconds(1));
                continue;
            }
        }
        WaitForSingleObject(hStartupEvent, INFINITE);

        HTHANDLE* h = Open(filename);
        if (!h) {
            cerr << "Storage not ready yet, waiting..." << endl;
            this_thread::sleep_for(chrono::seconds(1));
            continue;
        }

        cout << "Connected to storage!" << endl;

        srand((unsigned)time(NULL));

        bool connected = true;
        while (connected) {
            // провер€ем shutdown-ивент
            if (hShutdownEvent) {
                DWORD s = WaitForSingleObject(hShutdownEvent, 0);
                if (s == WAIT_OBJECT_0) {
                    cout << "Shutdown event detected! Closing storage..." << endl;
                    Close(h);
                    connected = false;
                    ResetEvent(hStartupEvent); // сбрасываем ожидание
                    break;
                }
            }

            int key = rand() % 50;
            Element e(&key, sizeof(key));
            Element* found = Get(h, &e);

            if (found) {
                int newValue = *(int*)found->payload + 1;
                if (!Update(h, &e, &newValue, sizeof(newValue))) {
                    cerr << "Update failed: " << HTGetLastError(h) << endl;
                }
                else {
                    cout << "[UPDATE] key=" << key << " -> " << newValue << endl;
                }
                delete found;
            }
            else {
                cout << "[GET] key=" << key << " not found" << endl;
            }
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
    }

    // cleanup
    if (hShutdownEvent) {
        CloseHandle(hShutdownEvent);
        hShutdownEvent = NULL;
    }

    // final snapshot and close
    Snap(h);
    Close(h);

    cout << "Delete worker exited cleanly." << endl;
    return 0;
}

