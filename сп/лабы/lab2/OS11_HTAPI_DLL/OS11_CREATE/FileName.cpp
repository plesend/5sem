
#pragma comment(lib, "D:\\лабораторные работы\\сп\\лабы\\lab02\\OS11_HTAPI_DLL\\x64\\Debug\\OS11_HTAPI_DLL.lib")

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

    HTHANDLE* ht = HT::Create(capacity, snapshotInterval, maxKeyLength, maxDataLength, filename, 0);
    if (!ht) {
        cout << "Failed to create HT-storage.\n";
        return 1;
    }

    cout << "HT-Storage Created "
        << "filename=" << filename
        << ", snapshotinterval=" << snapshotInterval
        << ", capacity=" << capacity
        << ", maxkeylength=" << maxKeyLength
        << ", maxdatalength=" << maxDataLength
        << "\n";

    HT::CleanupHandle(ht);
    return 0;
}
