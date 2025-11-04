
#include "../OS11_HTAPI/HT.h"
#include <iostream>

using namespace std;

int main(int argc, char** argv)
{

	if (argc < 6) {
		cout << "Usage: " << argv[0] << " <filename> <snapshotinterval> <capacity> <maxkeylength> <maxdatalength>\n";
		return 1;
	}

	const char* filename = argv[1];
	int snapshotinterval = atoi(argv[2]);
	int capacity = atoi(argv[3]);
	int maxkeylength = atoi(argv[4]);
	int maxdatalength = atoi(argv[5]);

	HTHANDLE* h = Create(capacity, snapshotinterval, maxkeylength, maxdatalength, filename);
	if (!h) {
		cerr << "Create failed: " << HTGetLastError(nullptr) << endl;
		return 1;
	}

	cout << "HT-Storage Created filename=" << filename
		<< ", snapshotinterval=" << snapshotinterval
		<< ", capacity=" << h->Capacity
		<< ", maxkeylength=" << h->MaxKeyLength
		<< ", maxdatalength=" << h->MaxPayloadLength << endl;

	if (!Close(h)) {
		cerr << "Close failed: " << HTGetLastError(h) << endl;
		return 1;
	}

	return 0;
}
