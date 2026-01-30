#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

CRITICAL_SECTION cs;

const char array[13] = {
	'u', 's', 'e', 'r', '-',
	'7', '4', '8', 'c',
	'2', 'f', '7', '5'
};

void ThreadFunc(const char* threadName) {
	for (int i = 0; i <= 90; i++) {
		
		int lock = (i >= 30 && i <= 60);
		if (lock) EnterCriticalSection(&cs);

		int index = i % 13;
		char symb = array[index];

		printf("[%s] iter %d char: %c\n", threadName, i, symb);

		if (lock) LeaveCriticalSection(&cs);

		Sleep(100);
	}
}

DWORD WINAPI AFunc(LPVOID parm) {
	ThreadFunc("A");
	return 0;
}

DWORD WINAPI BFunc(LPVOID parm) {
	ThreadFunc("B");
	return 0;
}

int main() {
	InitializeCriticalSection(&cs);

	HANDLE A = CreateThread(NULL, 0, AFunc, NULL, 0, NULL);
	HANDLE B = CreateThread(NULL, 0, BFunc, NULL, 0, NULL);
	
	ThreadFunc("M");

	WaitForSingleObject(A, INFINITE);
	WaitForSingleObject(B, INFINITE);

	DeleteCriticalSection(&cs);

	return 0;
}