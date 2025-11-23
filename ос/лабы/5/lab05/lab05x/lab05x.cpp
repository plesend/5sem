#include <iostream>
#include <windows.h>
#include <ctime>
using namespace std;

string toBinary(ULONG_PTR mask) {
    string s;
    for (int i = sizeof(ULONG_PTR) * 8 - 1; i >= 0; --i) {
        s += (mask & ((ULONG_PTR)1 << i)) ? '1' : '0';
    }
    return s;
}

int main()
{
    clock_t start = clock();  

    for (int i = 1; i <= 1000000; i++) {

        if (i % 1000 == 0) {

            DWORD_PTR procAffinityMask, systemAffinityMask;
            GetProcessAffinityMask(GetCurrentProcess(), &procAffinityMask, &systemAffinityMask);

            cout << "Iter: " << i
                << " PID: " << GetCurrentProcessId()
                << " TID: " << GetCurrentThreadId()
                << " PriorProc: " << GetPriorityClass(GetCurrentProcess())
                << " PriorThread: " << GetThreadPriority(GetCurrentThread())
                << " AffinMask: " << toBinary(procAffinityMask)
                << " SysMask: " << toBinary(systemAffinityMask)
                << " CPU: " << GetCurrentProcessorNumber()
                << endl;

            Sleep(200);
        }
    }

    clock_t end = clock();
    double elapsed_sec = double(end - start) / CLOCKS_PER_SEC;

    cout << "\nTime elapsed: " << elapsed_sec << " sec" << endl;

    return 0;
}
