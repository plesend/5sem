#include <windows.h>
#include <iostream>
#include <vector>
#include <string>

using namespace std;

string makeArgs(const string& path, int lower, int upper) {
    return "\"" + path + "\" " + to_string(lower) + " " + to_string(upper);
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        cout << "Usage: app.exe <num_processes> <lower> <upper>" << endl;
        return 1;
    }

    int numProcesses = stoi(argv[1]);
    int lower = stoi(argv[2]);
    int upper = stoi(argv[3]);

    if (numProcesses <= 0 || lower > upper || lower < 0) {
        cerr << "Wrong parameters" << endl;
        return 1;
    }

    int range = upper - lower + 1;
    int chunk = range / numProcesses;
    int remainder = range % numProcesses;

    vector<PROCESS_INFORMATION> processes;
    const string path = "D:\\лабораторные работы\\ос\\лабы\\3\\lab03\\Release\\Lab-03d-client.exe";

    int start = lower;
    for (int i = 0; i < numProcesses; ++i) {
        int end = start + chunk - 1;
        if (i == numProcesses - 1) end += remainder;

        string argsStr = makeArgs(path, start, end);
        vector<char> args(argsStr.begin(), argsStr.end());
        args.push_back('\0'); 

        STARTUPINFOA si = { sizeof(si) };
        PROCESS_INFORMATION pi;

        if (!CreateProcessA(NULL, args.data(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
            cout << "Failed to create process for range " << start << "-" << end
                << " Error: " << GetLastError() << endl;
        }
        else {
            processes.push_back(pi);
        }

        start = end + 1;
    }

    for (auto& pi : processes) {
        WaitForSingleObject(pi.hProcess, INFINITE);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }

    cout << "All child processes finished." << endl;
    return 0;
}
