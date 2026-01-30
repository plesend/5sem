#include <windows.h>
#include <stdio.h>

#define EVENT L"Global\\event"

void mainProc(const char* procName, HANDLE event) {
	const char array[13] = {
	'u', 's', 'e', 'r', '-',
	'7', '4', '8', 'c',
	'2', 'f', '7', '5'
	};

	for (int i = 0; i <= 90; i++) {
		int lock = (i >= 30 && i <= 60);

		int index = i % 13;
		char symb = array[index];
		printf("[%s] iteration %d char: %c\n", procName, i, symb);

		if (i == 15) {
			SetEvent(event);
		}

		Sleep(100);
	}

}

int main() {
	HANDLE event = CreateEventW(NULL, TRUE, FALSE, EVENT);
	if (!event) {
		printf("cant create event\n");
		return 1;
	}

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
		printf("problemo %d\n", GetLastError());
	}

	mainProc("MAIN", event);

	WaitForSingleObject(piA.hProcess, INFINITE);
	WaitForSingleObject(piB.hProcess, INFINITE);

	CloseHandle(piA.hProcess);
	CloseHandle(piA.hThread);
	CloseHandle(piB.hProcess);
	CloseHandle(piB.hThread);
	CloseHandle(event);

	return 0;
}