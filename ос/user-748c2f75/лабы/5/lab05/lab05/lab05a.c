#include <windows.h>
#include <stdio.h>

void toBinary(ULONG_PTR mask, char* out) {
    int bits = sizeof(ULONG_PTR) * 8;
    for (int i = 0; i < bits; i++) {
        ULONG_PTR bit = (ULONG_PTR)1 << (bits - 1 - i);
        out[i] = (mask & bit) ? '1' : '0';
    }
    out[bits] = '\0';
}

int main() {
    DWORD_PTR procAffinityMask = 0, systemAffinityMask = 0;
    SYSTEM_INFO sysinfo;
    char procMaskStr[sizeof(ULONG_PTR) * 8 + 1];
    char sysMaskStr[sizeof(ULONG_PTR) * 8 + 1];

    GetProcessAffinityMask(GetCurrentProcess(), &procAffinityMask, &systemAffinityMask);
    GetSystemInfo(&sysinfo);

    toBinary(procAffinityMask, procMaskStr);
    toBinary(systemAffinityMask, sysMaskStr);

    printf("PID: %lu\n", GetCurrentProcessId());
    printf("TID: %lu\n", GetCurrentThreadId());
    printf("Priorproc: %lu\n", GetPriorityClass(GetCurrentProcess()));
    printf("Priorthread: %ld\n", GetThreadPriority(GetCurrentThread()));
    printf("AffinMask: %s\n", procMaskStr);
    printf("SysMask: %s\n", sysMaskStr);
    printf("ProcNum: %lu\n", GetCurrentProcessorNumber());
    printf("\n");

    return 0;
}
