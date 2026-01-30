#include <windows.h>
#include <stdio.h>

int main()
{
    STARTUPINFOW si1 = { sizeof(si1) };
    STARTUPINFOW si2 = { sizeof(si2) };

    PROCESS_INFORMATION pi1 = { 0 };
    PROCESS_INFORMATION pi2 = { 0 };

    wchar_t cmd1[] =
        L"\"D:\\лабораторные работы\\ос\\лабы\\7\\lab07a\\x64\\Debug\\lab07x.exe\" 60";

    wchar_t cmd2[] =
        L"\"D:\\лабораторные работы\\ос\\лабы\\7\\lab07a\\x64\\Debug\\lab07x.exe\" 120";

    if (!CreateProcessW(NULL, cmd1, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si1, &pi1)) {
        wprintf(L"P1 failed PROBLEMO: %lu\n", GetLastError()); 
        return 1; 
    }
    printf("started\n");


    if (!CreateProcessW(NULL, cmd2, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si2, &pi2)) {
        wprintf(L"P2 failed PROBLEMO: %lu\n", GetLastError()); 
        return 1; 
    }
    printf("started\n");


    HANDLE processes[2] = { pi1.hProcess, pi2.hProcess };

    WaitForMultipleObjects(2, processes, TRUE, INFINITE);

    

    CloseHandle(pi1.hProcess);
    CloseHandle(pi1.hThread);
    CloseHandle(pi2.hProcess);
    CloseHandle(pi2.hThread);

    printf("processess finished\n");

    return 0;
}
