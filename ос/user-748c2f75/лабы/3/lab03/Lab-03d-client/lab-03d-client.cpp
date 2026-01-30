#include <windows.h>
#include <iostream>
#include <list>
#include <string>

using namespace std;

bool isPrime(int n) {
    if (n < 2) return false;
    for (int i = 2; i * i <= n; ++i) {
        if (n % i == 0) return false;
    }
    return true;
}

string toString(const list<int>& buf) {
    string output;
    for (int x : buf) {
        output += to_string(x) + " ";
    }
    return output;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        return 1;
    }

    int lower = stoi(argv[1]);
    int upper = stoi(argv[2]);

    if (lower > upper || lower < 0) return 1;

    HANDLE hPipe = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hPipe == INVALID_HANDLE_VALUE) return 1;

    list<int> buf;
    for (int i = lower; i <= upper; ++i) {
        if (isPrime(i)) buf.push_back(i);
    }

    string output = "\nprocess from " + to_string(lower) + " to " + to_string(upper) + "\n";
    output += toString(buf);

    DWORD bytesWritten;
    WriteFile(hPipe, output.c_str(), (DWORD)output.size(), &bytesWritten, NULL);

    return 0;
}
