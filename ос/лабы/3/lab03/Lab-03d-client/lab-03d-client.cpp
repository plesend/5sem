#include <windows.h>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <list>
using namespace std;

bool isPrime(int n) {
	if (n < 2) return false;
	for (int i = 2; i * i <= n; ++i) {
		if (n % i == 0) return false;
	}
	return true;
}

string toString(list<int> buf, string output) {
	for (int x : buf) {
		output += to_string(x) + " ";
	}
	return output;
}

int main(int argc, char* argv[]) {
	if (argc != 3) {
		cout << "Use it like: app-client.exe <lower> <upper>" << endl;
		return 1;
	}

	int lower = stoi(argv[1]);
	int upper = stoi(argv[2]);

	if (lower > upper || lower < 0) {
		cout << "Wrong limit";
		return 1;
	}

	HANDLE hPipe = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hPipe == INVALID_HANDLE_VALUE || hPipe == NULL) {
		std::cerr << "Failed to get pipe handle." << std::endl;
		return 1;
	}

	list<int> buf;

	for (int i = lower; i <= upper; ++i) {
		if (isPrime(i) == true) {
			buf.push_back(i);
		}
	}

	string output;

	std::string buffer = toString(buf, output);

	DWORD bytesWritten;
	if (!buffer.empty()) {
		if (!WriteFile(hPipe, buffer.c_str(), static_cast<DWORD>(buffer.size()), &bytesWritten, NULL)) {
			cerr << "Failed to write to pipe. Error: " << GetLastError() << endl;
			return 1;
		}
	}

	return 0;
}