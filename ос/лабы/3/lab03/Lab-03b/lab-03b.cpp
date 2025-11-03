#include <Windows.h> 
#include <stdlib.h> 
#include <iostream>
#include <stdio.h>
#include <cstring>
#include <string>

void CreateAndRunProcess(
	const char* appName,
	char* cmdLine,
	const char* label
) {
	STARTUPINFOA si = { sizeof(si) };
	PROCESS_INFORMATION pi;

	if (!CreateProcessA(
		appName,
		cmdLine,
		NULL, NULL,
		FALSE,
		0, NULL, NULL,
		&si, &pi
	)) {
		std::cout << "Startup error" << GetLastError() << std::endl;
		return;
	}

	WaitForSingleObject(pi.hProcess, INFINITE);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
}

int main() {
	const char* path = "D:\\лабораторные работы\\ос\\лабы\\3\\lab03\\Release\\lab03.exe";
	std::cout << "iter_num default\n";
	CreateAndRunProcess(path, NULL, "task1-2");

	SetEnvironmentVariableA("ITER_NUM", "9");
	std::cout << "iter_num overrided\n";
	CreateAndRunProcess(path, NULL, "task3-2");

	return 0;
}