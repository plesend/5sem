#include <windows.h>
#include <stdio.h>
#include <time.h>

int main() {
    clock_t start = clock();

    for (int i = 1; i <= 1000000; i++) {

        if (i % 1000 == 0) {
            printf(
                "Iter: %d  PID: %lu  TID: %lu  PriorProc: %lu  PriorThread: %ld  CPU: %lu\n",
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

    printf("\ntime: %.3f sec\n", elapsed);

    Sleep(1000000);

    return 0;
}
