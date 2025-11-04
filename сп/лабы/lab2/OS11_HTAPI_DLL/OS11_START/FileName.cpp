#pragma comment(lib, "D:\\лабораторные работы\\сп\\лабы\\lab2\\OS11_HTAPI_DLL\\x64\\Debug\\OS11_HTAPI_DLL.lib")

#include "Header.h"
#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>
#include <conio.h> // для _kbhit и _getch

using namespace std;
using namespace HT;

int main(int argc, char* argv[])
{
    if (argc < 2) {
        cout << "Usage: OS11_START <filename>\n";
        return 1;
    }

    const char* filename = argv[1];

    HTHANDLE* ht = Open(filename);
    
    cout << "h->File: " << ht->File << endl;
    cout << "h->FileMapping: " << ht->FileMapping << endl;
    cout << "h->Addr: " << (void*)ht->Addr << endl;
    cout << "h->DataAddr: " << (void*)ht->DataAddr << endl;
    cout << "h->Mutex: " << ht->Mutex << endl;
    cout << "Capacity: " << ht->Capacity << " MaxKeyLength: " << ht->MaxKeyLength
        << " MaxPayloadLength: " << ht->MaxPayloadLength << endl;

    cout << "HT-Storage Start filename=" << filename
        << ", snapshotinterval=" << ht->SecSnapshotInterval
        << ", capacity=" << ht->Capacity
        << ", maxkeylength=" << ht->MaxKeyLength
        << ", maxdatalength=" << ht->MaxPayloadLength
        << "\n";

    cout << "Press any key to stop...\n";

    atomic<bool> running(true);

    thread snapshotThread([&]() {
        while (running) {
            int interval = ht->SecSnapshotInterval > 0 ? ht->SecSnapshotInterval : 1;
            for (int i = 0; i < interval * 10 && running; ++i) { 
                this_thread::sleep_for(chrono::milliseconds(100));
            }
            if (!running) break;
            HT::Snap(ht);
        }
        });

    while (running) {
        if (_kbhit()) {
            _getch();
            running = false;
        }
        this_thread::sleep_for(chrono::milliseconds(50));
    }

    snapshotThread.join();

    HT::Snap(ht);

    HT::CleanupHandle(ht);

    return 0;
}