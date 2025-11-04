#include <Windows.h> 
#include <iostream>
#include <string>
#include <cstring>

using namespace std;

int main() {
    STARTUPINFOA si[3]{};
    PROCESS_INFORMATION pi[3]{};
    BOOL processess[3]{ FALSE, FALSE, FALSE };

    for (auto& s : si)
        s.cb = sizeof(STARTUPINFOA);

    const char* path = "D:\\лабораторные работы\\ос\\лабы\\3\\lab03\\Release\\lab03.exe";

    string firstprm = string(path) + " 10";
    cout << "1 param with iteration in it\n";
    processess[0] = CreateProcessA(firstprm.c_str(), NULL, NULL, NULL,
        FALSE, 0, NULL, NULL, &si[0], &pi[0]);

    string secondprm = string(path) + " 10";
    char cmdLine[256];
    strcpy_s(cmdLine, secondprm.c_str());
    cout << "2 param with iteration in it\n";
    processess[1] = CreateProcessA(NULL, cmdLine, NULL, NULL,
        FALSE, 0, NULL, NULL, &si[1], &pi[1]);

    char cmdLine3[64];
    strcpy_s(cmdLine3, " 10");
    cout << "1 n 2 parms\n";
    processess[2] = CreateProcessA(path, cmdLine3, NULL, NULL,
        FALSE, 0, NULL, NULL, &si[2], &pi[2]);

    for (int i = 0; i < 3; ++i) {
        if (processess[i])
            cout << "Process n." << i + 1 << " created with PID: " << pi[i].dwProcessId << "\n";
        else
            cout << "Error creating process n." << i + 1 << ", Error Code: " << GetLastError() << "\n";
    }

    for (int i = 0; i < 3; ++i) {
        if (processess[i]) {
            CloseHandle(pi[i].hThread);
            CloseHandle(pi[i].hProcess);
        }
    }

    cout << "All processes started (main not waiting)\n";
    system("pause");
    return 0;
}
