#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
    if (argc < 4) {
        printf("Usage: lab05b.exe <P1_mask> <P2_priority> <P3_priority>\n");
        return 1;
    }

    const char* path = "lab05x.exe";

    DWORD_PTR affinityMask = (DWORD_PTR)atol(argv[1]);
    DWORD priority1 = (DWORD)atol(argv[2]);
    DWORD priority2 = (DWORD)atol(argv[3]);

    printf("Affinity mask (P1): %llu\n", (unsigned long long)affinityMask);
    printf("Child 1 priority (P2): %lu\n", priority1);
    printf("Child 2 priority (P3): %lu\n", priority2);

    STARTUPINFOA si1;
    PROCESS_INFORMATION pi1;
    STARTUPINFOA si2;
    PROCESS_INFORMATION pi2;

    ZeroMemory(&si1, sizeof(si1));
    si1.cb = sizeof(si1);
    ZeroMemory(&pi1, sizeof(pi1));

    ZeroMemory(&si2, sizeof(si2));
    si2.cb = sizeof(si2);
    ZeroMemory(&pi2, sizeof(pi2));

    printf("1 child\n");

    if (!CreateProcessA(
        path,
        NULL,
        NULL, NULL,
        FALSE,
        CREATE_NEW_CONSOLE,
        NULL, NULL,
        &si1, &pi1))
    {
        printf("FAILURE IN CreateProcess: %lu\n", GetLastError());
        return 1;
    }

    SetPriorityClass(pi1.hProcess, priority1);
    SetProcessAffinityMask(pi1.hProcess, affinityMask);

    printf("2 child\n");

    if (!CreateProcessA(
        path,
        NULL,
        NULL, NULL,
        FALSE,
        CREATE_NEW_CONSOLE,
        NULL, NULL,
        &si2, &pi2))
    {
        printf("FAILURE IN CreateProcess: %lu\n", GetLastError());
        return 1;
    }

    SetPriorityClass(pi2.hProcess, priority2);
    SetProcessAffinityMask(pi2.hProcess, affinityMask);

    printf("Child 1 PID: %lu\n", pi1.dwProcessId);
    printf("Child 2 PID: %lu\n", pi2.dwProcessId);

    WaitForSingleObject(pi1.hProcess, INFINITE);
    WaitForSingleObject(pi2.hProcess, INFINITE);

    CloseHandle(pi1.hProcess);
    CloseHandle(pi1.hThread);
    CloseHandle(pi2.hProcess);
    CloseHandle(pi2.hThread);

    printf("Finished eeeeerrrere\n");

    return 0;
}
