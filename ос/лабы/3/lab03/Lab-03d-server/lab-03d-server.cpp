#include <windows.h>
#include <iostream>
#include <vector>
#include <string>

using namespace std;

string makeArgs(const string& path, int lower, int upper) {
    return "\"" + path + "\" " + to_string(lower) + " " + to_string(upper);
}

int main(int argc, char* argv[]) {
    if (argc < 4) {
        printf("usage: l3.exe {threadCount} {start} {end}\n");
        return 1;
    }

    int numProcesses = stoi(argv[1]);
    int lower = stoi(argv[2]);
    int upper = stoi(argv[3]);

    if (numProcesses <= 0 || lower > upper || lower < 0) return 1;

    int range = upper - lower + 1;
    int chunk = range / numProcesses;
    int remainder = range % numProcesses;

    vector<PROCESS_INFORMATION> processes;
    const string path = "D:\\лабораторные работы\\ос\\лабы\\3\\lab03\\Release\\Lab-03d-client.exe";

    int start = lower;

    clock_t begin = clock();
    for (int i = 0; i < numProcesses; ++i) {
        int end = start + chunk - 1;
        if (i == numProcesses - 1) end += remainder;

        string argsStr = makeArgs(path, start, end);
        vector<char> args(argsStr.begin(), argsStr.end());
        args.push_back('\0');

        SECURITY_ATTRIBUTES sa = { sizeof(sa), NULL, TRUE };
        HANDLE readPipe, writePipe;
        if (!CreatePipe(&readPipe, &writePipe, &sa, 0)) {
            cout << "CreatePipe failed\n";
            return 1;
        }
        SetHandleInformation(readPipe, HANDLE_FLAG_INHERIT, 0);

        STARTUPINFOA si = { sizeof(si) };
        si.hStdOutput = writePipe;
        si.hStdError = writePipe;
        si.dwFlags |= STARTF_USESTDHANDLES;

        PROCESS_INFORMATION pi;
        if (!CreateProcessA(NULL, args.data(), NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
            cout << "Failed to create process for range " << start << "-" << end
                << " Error: " << GetLastError() << endl;
            CloseHandle(readPipe);
            CloseHandle(writePipe);
        }
        else {
            Sleep(300);
            CloseHandle(writePipe);

            char buffer[256];
            DWORD bytesRead;
            cout << "\n=== Output from process " << i << " (" << start << "-" << end << ") ===\n";
            while (ReadFile(readPipe, buffer, sizeof(buffer) - 1, &bytesRead, NULL) && bytesRead > 0) {
                buffer[bytesRead] = '\0';
                cout << buffer;
            }
            CloseHandle(readPipe);

            processes.push_back(pi);
        }
        start = end + 1;
    }

    clock_t end = clock();
    cout << "\nLoop time: " << (end - begin) << " ms\n";

    for (auto& pi : processes) {
        WaitForSingleObject(pi.hProcess, INFINITE);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }

    cout << "\nAll child processes finished.\n";
    return 0;
}
