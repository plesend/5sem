#pragma comment(lib, "D:\\лабораторные работы\\сп\\лабы\\lab02\\OS11_HTAPI_DLL\\x64\\Debug\\OS11_HTAPI_DLL.lib")

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
    if (argc != 2) {
        cout << "Usage: OS11_START <filename>\n";
        return 1;
    }

    const char* filename = argv[1];

    HTHANDLE* ht = Open(filename);
    if (!ht) {
        cerr << "HT-Storage not found. Creating new...\n";
        ht = Create(1000, 10, 64, 1024, filename, 0); 
        if (!ht) {
            cerr << "Failed to create HT-storage.\n";
            return 1;
        }
    }

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