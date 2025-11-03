#include <iostream>
#include <string>
#include "Header.h"

using namespace std;
using namespace HT;

int main(int argc, char* argv[])
{
    if (argc != 6)
    {
        cout << "Usage: OS11_CREATE <filename> <snapshotinterval> <capacity> <maxkeylength> <maxdatalength>\n";
        return 1;
    }

    const char* filename = argv[1];
    int snapshotinterval = stoi(argv[2]);
    int capacity = stoi(argv[3]);
    int maxkeylength = stoi(argv[4]);
    int maxdatalength = stoi(argv[5]);

    HTHANDLE* ht = HT::Create(capacity, snapshotinterval, maxkeylength, maxdatalength, filename, 0);

    if (!ht)
    {
        cout << "Error: cannot create HT-Storage.\n";
        return 1;
    }

    cout << "HT-Storage Created filename=" << filename
        << ", snapshotinterval=" << snapshotinterval << "\n"
        << "capacity=" << capacity
        << ", maxkeylength=" << maxkeylength
        << ", maxdatalength=" << maxdatalength << "\n";

    HT::CleanupHandle(ht);

    return 0;
}
