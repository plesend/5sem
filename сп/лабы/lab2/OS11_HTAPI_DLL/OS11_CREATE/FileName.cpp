#pragma comment(lib, "D:\\лабораторные работы\\сп\\лабы\\lab2\\OS11_HTAPI_DLL\\x64\\Debug\\OS11_HTAPI_DLL.lib")

#include "Header.h"
#include <iostream>
#include <string>

using namespace std;
using namespace HT;

int main(int argc, char* argv[])
{
    if (argc != 6) {
        cout << "Usage: OS11_CREATE <filename> <capacity> <snapshotinterval> <maxkeylength> <maxdatalength>\n";
        return 1;
    }

    const char* filename = argv[1];
    int capacity = stoi(argv[2]);
    int snapshotInterval = stoi(argv[3]);
    int maxKeyLength = stoi(argv[4]);
    int maxDataLength = stoi(argv[5]);

    if (capacity <= 0 || snapshotInterval <= 0 || maxKeyLength <= 0 || maxDataLength <= 0) {
        cout << "STOP IT GET SOME HELP 4 GB OF MEMORY RAAAH";
        return 0;
    }

    HTHANDLE* h = HT::Create(capacity, snapshotInterval, maxKeyLength, maxDataLength, filename, 0);
    if (!h) {
        cout << "Failed to create HT-storage.\n";
        return 1;
    }

    cout << "h->File: " << h->File << endl;
    cout << "h->FileMapping: " << h->FileMapping << endl;
    cout << "h->Addr: " << (void*)h->Addr << endl;
    cout << "h->DataAddr: " << (void*)h->DataAddr << endl;
    cout << "h->Mutex: " << h->Mutex << endl;
    cout << "Capacity: " << h->Capacity << " MaxKeyLength: " << h->MaxKeyLength
        << " MaxPayloadLength: " << h->MaxPayloadLength << endl;

    cout << "HT-Storage Created "
        << "filename=" << filename
        << ", snapshotinterval=" << snapshotInterval
        << ", capacity=" << capacity
        << ", maxkeylength=" << maxKeyLength
        << ", maxdatalength=" << maxDataLength
        << "\n";

    HT::CleanupHandle(h);
    return 0;
}
