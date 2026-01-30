#include <windows.h>
#include <tlhelp32.h>
#include <iostream>

int main() {
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapShot == INVALID_HANDLE_VALUE) {
		std::cout << "Error occured while making a snapsshot" << std::endl;
		return 1;
	}

	PROCESSENTRY32 pe;
	pe.dwSize = sizeof(PROCESSENTRY32);

	if (!Process32First(hSnapShot, &pe)) {
		std::cout << "Can't reach the first process" << std::endl;
		CloseHandle(hSnapShot);
		return 1;
	}

	std::cout << "Process list:\n";
	std::cout << "Process name: \t";

	do {
		std::wcout << pe.szExeFile << "\t"
			<< pe.th32ParentProcessID << "\t"
			<< pe.th32ProcessID << std::endl;
	} while (Process32Next(hSnapShot, &pe));

	CloseHandle(hSnapShot);
	return 0;
}