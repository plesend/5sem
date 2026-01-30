#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

DWORD WINAPI ThreadFunc(LPVOID param) {
    int threadNum = *((int*)param);

    clock_t start = clock();

    for (int i = 1; i <= 1000000; i++) {

        if (i % 1000 == 0) {
            printf(
                "[Thread %d] Iter: %d  PID: %lu  TID: %lu  PriorProc: %lu  PriorThread: %ld CPU: %lu\n",
                threadNum,
                i,
                GetCurrentProcessId(),
                GetCurrentThreadId(),
                GetPriorityClass(GetCurrentProcess()),
                GetThreadPriority(GetCurrentThread()),
                GetCurrentProcessorNumber()
            );

            Sleep(200);
        }
    }

    clock_t end = clock();
    double elapsed = (double)(end - start) / CLOCKS_PER_SEC;

    printf("\n[Thread %d] time: %.3f sec\n", threadNum, elapsed);

    Sleep(1000000);

    return 0;
}

int main(int argc, char** argv) {
    if (argc < 5) {
        printf("Usage: lab05b.exe <P1_mask> <P2_priorityClass> <P3_threadPriority1> <P4_threadPriority2>\n");
        return 1;
    }

    DWORD_PTR affinityMask = (DWORD_PTR)atol(argv[1]);
    DWORD priorityClass = (DWORD)atol(argv[2]);
    int threadPriority1 = atoi(argv[3]);
    int threadPriority2 = atoi(argv[4]);

    printf("Affinity mask (P1): %llu\n", (unsigned long long)affinityMask);
    printf("Process priority class (P2): %lu\n", priorityClass);
    printf("Thread 1 priority (P3): %d\n", threadPriority1);
    printf("Thread 2 priority (P4): %d\n", threadPriority2);

    SetProcessAffinityMask(GetCurrentProcess(), affinityMask);
    SetPriorityClass(GetCurrentProcess(), priorityClass);

    int t1Num = 1;
    int t2Num = 2;

    HANDLE t1 = CreateThread(NULL, 0, ThreadFunc, &t1Num, 0, NULL);
    HANDLE t2 = CreateThread(NULL, 0, ThreadFunc, &t2Num, 0, NULL);

    if (!t1 || !t2) {
        printf("FAILURE IN CREATING threads. Error: %lu\n", GetLastError());
        return 1;
    }

    SetThreadPriority(t1, threadPriority1);
    SetThreadPriority(t2, threadPriority2);

    WaitForSingleObject(t1, INFINITE);
    WaitForSingleObject(t2, INFINITE);

    CloseHandle(t1);
    CloseHandle(t2);

    printf("Finished veeee\n");

    return 0;
}
