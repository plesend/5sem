#include <windows.h>
#include <iostream>
using namespace std;

int main(int argc, char** argv) {
    if (argc < 5) {
        cout << "Usage: lab05b.exe <P1_mask> <P2_priority> <P3_priority> <P4_priority_class>\n";
        return 1;
    }

    const char* path = "lab05x.exe";

    DWORD_PTR affinityMask = atol(argv[1]);
    DWORD priority1 = atol(argv[2]);
    DWORD priority2 = atol(argv[3]);
    DWORD priorityClass2 = atol(argv[4]);

    cout << "Affinity mask (P1): " << affinityMask << endl;
    cout << "Child 1 priority (P2): " << priority1 << endl;
    cout << "Child 2 priority (P3): " << priority2 << endl;
    cout << "Child 2 priority class (P4): " << priorityClass2 << endl;

    STARTUPINFOA si1{ sizeof(si1) };
    PROCESS_INFORMATION pi1;
    STARTUPINFOA si2 = { sizeof(si2) };
    PROCESS_INFORMATION pi2;

    cout << "1 child\n";

    if (!CreateProcessA(
        path,
        NULL,
        NULL, NULL,
        FALSE,
        CREATE_NEW_CONSOLE,
        NULL, NULL,
        &si1, &pi1))
    {
        cout << "FAILURE IN CreateProc: " << GetLastError() << endl;
        return 1;
    }

    cout << "2 child\n";

    if (!CreateProcessA(
        path,
        NULL,
        NULL, NULL,
        FALSE,
        CREATE_NEW_CONSOLE,
        NULL, NULL,
        &si2, &pi2))
    {
        cout << "FAILURE IN CreateProc: " << GetLastError() << endl;
        return 1;
    }

    SetProcessAffinityMask(pi1.hProcess, affinityMask);
    SetProcessAffinityMask(pi2.hProcess, affinityMask);

    // Установка приоритетов для первого процесса
    SetPriorityClass(pi1.hProcess, priority1);

    // Установка приоритетов для второго процесса
    SetPriorityClass(pi2.hProcess, priorityClass2);
    SetThreadPriority(pi2.hThread, priority2);

    cout << "Child 1 PID: " << pi1.dwProcessId << endl;
    cout << "Child 2 PID: " << pi2.dwProcessId << endl;

    WaitForSingleObject(pi1.hProcess, INFINITE);
    WaitForSingleObject(pi2.hProcess, INFINITE);

    CloseHandle(pi1.hProcess);
    CloseHandle(pi1.hThread);
    CloseHandle(pi2.hProcess);
    CloseHandle(pi2.hThread);

    cout << "Finished eeeeerrrere";

    return 0;
}