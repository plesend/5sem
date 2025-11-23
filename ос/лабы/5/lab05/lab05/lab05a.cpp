#include <iostream>
#include <windows.h>
using namespace std;

string toBinary(ULONG_PTR mask) {
	string s;
	for (int i = sizeof(ULONG_PTR) * 8 - 1; i >= 0; --i) {
		s += (mask & ((ULONG_PTR)1 << i)) ? '1' : '0';
	}
	return s;
}

int main() {
	DWORD_PTR procAffinityMask, systemAffinityMask;
	GetProcessAffinityMask(GetCurrentProcess(), &procAffinityMask, &systemAffinityMask);
	SYSTEM_INFO sysinfo;
	GetSystemInfo(&sysinfo);
	DWORD cpuCount = sysinfo.dwNumberOfProcessors;
	cout << "PID: " << GetCurrentProcessId() << " TID: " << GetCurrentThreadId() <<
		" Priorproc: " << GetPriorityClass(GetCurrentProcess()) <<
		" Priorthread: " << GetThreadPriority(GetCurrentThread()) <<
		" AffinMask: " << toBinary(procAffinityMask) <<
		" SysMask: " << toBinary(systemAffinityMask) << " ProcNum: " << GetCurrentProcessorNumber();
}