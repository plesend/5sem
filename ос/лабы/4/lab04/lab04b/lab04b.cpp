#include "windows.h"
#include <iostream>
#include <thread>
#include <vector>
#include <string>

using namespace std;
#define MAX_THREADS 2
DWORD WINAPI Handle(LPVOID lpParm) {
    int n = ((int*)lpParm)[0];

    const char array[12] = {
    'u', 's', 'e', 'r',
    '7', '4', '8', 'c',
    '2', 'f', '7', '5'
    };
    for (int i = 0; i < n; i++) {
        int index = i % 12;
        printf("PID: %lu  TID: %lu  process %d - %c\n",
            GetCurrentProcessId(),
            GetCurrentThreadId(),
            i,
            array[index]);
        Sleep(300);
    }
    return 0;
}

int main() {
    cout << "PID of main process: " << GetCurrentProcessId() << endl;

    HANDLE threads[MAX_THREADS];
    int params[MAX_THREADS][2] = { {50, 1}, {125, 2} };
    int parm3 = 100;
    threads[0] = CreateThread(NULL, 0, Handle, params[0], 0, NULL);
    threads[1] = CreateThread(NULL, 0, Handle, params[1], 0, NULL);

    if (!threads[0] || !threads[1]) {
        cout << "Threads 1 or 2 : FAILURE\n";
        ExitProcess(1);
    }

    cout << "==== Main thread work =====\n";

    const char array[12] = {
    'u', 's', 'e', 'r',
    '7', '4', '8', 'c',
    '2', 'f', '7', '5'
    };
    for (int i = 0; i < parm3; i++) {
        int index = i % 12;
        printf("PID: %lu  TID: %lu  process %d - %c\n",
            GetCurrentProcessId(),
            GetCurrentThreadId(),
            i,
            array[index]);
        Sleep(300);

        if (i == 20) {
            printf("Suspending thread 1 at iteration %d\n", i);
            SuspendThread(threads[0]);
        }
        if (i == 60) {
            printf("\n Resuming thread 1 at iteration %d\n", i);
            ResumeThread(threads[0]);
        }
        if (i == 40) {
            printf("\nSuspending thread 2 at iteration %d\n", i);
            SuspendThread(threads[1]);
        }
    }

    cout << "\nWaiting for threads....\n";
    WaitForMultipleObjects(MAX_THREADS, threads, TRUE, INFINITE);

    cout << "\nWaiting for threads...\n";
    for (int i = 0; i < MAX_THREADS; i++) {
        CloseHandle(threads[i]);
    }

    cout << "==== All threads finished =====\n";
    return 0;
}