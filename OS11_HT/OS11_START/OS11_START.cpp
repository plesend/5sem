

#include "../OS11_HTAPI/HT.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>
#include <condition_variable>
#include <conio.h>
#include <memory>
#include <unordered_map>
#include <mutex>
#include <string>

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
    SetConsoleOutputCP(1251);
    SetConsoleCP(1251);

    if (argc < 2) {
        cout << "Usage: OS11_START <filename>\n";
        return 1;
    }

    const char* filename = argv[1];
    HTHANDLE* h = Open(filename);
    if (!h) {
        cerr << "Open failed: " << HTGetLastError(h) << endl;
        return 1;
    }

    unsigned int hsh = hashFunction(filename, (int)strlen(filename));

    char startupName[64], shutdownName[64];
    sprintf_s(startupName, "Global\\HT_startup_%08X", hsh);
    sprintf_s(shutdownName, "Global\\HT_shutdown_%08X", hsh);


    HANDLE hStartupEvent = CreateEventA(NULL, TRUE, FALSE, startupName);
    HANDLE hShutdownEvent = CreateEventA(NULL, TRUE, FALSE, shutdownName);

    if (!hStartupEvent || !hShutdownEvent) {
        cerr << "Event creation failed: " << GetLastError() << endl;
        return 1;
    }

    SetEvent(hStartupEvent);
    ResetEvent(hShutdownEvent);

    Snap(h);

    cout << "HT-Storage Start filename=" << filename
        << ", snapshotinterval=" << h->SecSnapshotInterval
        << ", capacity=" << h->Capacity
        << ", maxkeylength=" << h->MaxKeyLength
        << ", maxdatalength=" << h->MaxPayloadLength << endl;
    cout << "Startup event: " << startupName << endl;
    cout << "Shutdown event: " << shutdownName << endl;

    atomic<bool> running(true);
    condition_variable cv;
    mutex cv_m;

    thread snapshotThread([&]() {
        unique_lock<mutex> lock(cv_m);
        while (running.load()) {
            if (cv.wait_for(lock, chrono::seconds(h->SecSnapshotInterval),
                [&]() { return !running.load(); })) {
                break;
            }

            if (!Snap(h)) {
                cerr << "Snap failed: " << HTGetLastError(h) << endl;
            }
        }
        });

    cout << "Press any key to stop..." << endl;

    while (running.load()) {
        if (_kbhit()) {
            _getch();
            running.store(false);
            cv.notify_all();
            break;
        }
        this_thread::sleep_for(chrono::milliseconds(100));
    }

    if (snapshotThread.joinable())
        snapshotThread.join();

    
    Snap(h);

    cout << "\nStopping storage..." << endl;
    ResetEvent(hStartupEvent);
    SetEvent(hShutdownEvent);

    Close(h);

    return 0;
}
