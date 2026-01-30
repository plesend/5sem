#include <windows.h>
#include <stdio.h>

#define SEMAPHORE L"Global\\semaphore"

void mainProc(const char* procName) {
	HANDLE sem = OpenSemaphoreW(SEMAPHORE_ALL_ACCESS, FALSE, SEMAPHORE);
	if (!sem) {
		printf("cant open mutex :(((((((");
		return;
	}

	const char array[13] = {
	'u', 's', 'e', 'r', '-',
	'7', '4', '8', 'c',
	'2', 'f', '7', '5'
	};

	for (int i = 0; i <= 90; i++) {
		int lock = (i >= 30 && i <= 60);

		if (lock) WaitForSingleObject(sem, INFINITE);

		int index = i % 13;
		char symb = array[index];
		printf("[%s] iteration %d char: %c\n", procName, i, symb);

		if (lock) ReleaseSemaphore(sem, 1, NULL);

		Sleep(100);
	}

	CloseHandle(sem);
}

int main() {
	HANDLE sem = CreateSemaphoreW(NULL, 1, 1, SEMAPHORE);

	DWORD mainThreadId;
	HANDLE mainThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)mainProc, (LPVOID)"MAIN", 0, &mainThreadId);

	STARTUPINFOW siA = { sizeof(siA) };
	STARTUPINFOW siB = { sizeof(siB) };
	PROCESS_INFORMATION piA = { 0 };
	PROCESS_INFORMATION piB = { 0 };

	if (!CreateProcessW(L"D:\\лабораторные работы\\ос\\лабы\\6\\lab06a\\x64\\Debug\\ProcA.exe", NULL, NULL, NULL, FALSE,
		CREATE_NEW_CONSOLE, NULL, NULL, &siA, &piA)) {
		printf("problemo %d\n", GetLastError());
	}

	if (!CreateProcessW(L"D:\\лабораторные работы\\ос\\лабы\\6\\lab06a\\x64\\Debug\\ProcB.exe", NULL, NULL, NULL, FALSE,
		CREATE_NEW_CONSOLE, NULL, NULL, &siB, &piB)) {
		printf("problemo%d\n", GetLastError());
	}

	WaitForSingleObject(mainThread, INFINITE);
	WaitForSingleObject(piA.hProcess, INFINITE);
	WaitForSingleObject(piB.hProcess, INFINITE);

	CloseHandle(mainThread);
	CloseHandle(piA.hProcess);
	CloseHandle(piA.hThread);
	CloseHandle(piB.hProcess);
	CloseHandle(piB.hThread);

	return 0;
}