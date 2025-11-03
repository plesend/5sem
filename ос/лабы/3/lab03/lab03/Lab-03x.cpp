#include <Windows.h> 
#include <stdlib.h> 
#include <iostream>
#include <stdio.h>
#include <cstring>
#include <string>

const int DELAY = 500;

void Cycle(int n, DWORD pid) {
	std::cout << n << " iterations at all\n";
	for (int i = 0; i < n; i++) {
		std::cout << "Iteration " << i << " PID: " << pid << std::endl;
		Sleep(DELAY);
	}
}

void DieWithError(const char* message) {
	std::cerr << "Failure: " << message << std::endl;
	ExitProcess(1);
}

int main(int argc, char* argv[]) {
	DWORD pid = GetCurrentProcessId();
	int itCount = 0;

	if (argc > 1) {
		try {
			itCount = std::stoi(argv[1]);
		}
		catch (const std::exception& e) {
			DieWithError("not enough params");
		}
	}

	if (itCount <= 0) {
		char* envValue = nullptr;
		size_t requiredSize = 0;

		errno_t err = _dupenv_s(&envValue, &requiredSize, "ITER_NUM");

		if (err == 0 && envValue != nullptr) {
			try {
				itCount = std::stoi(envValue);
			}
			catch (const std::exception& e) {
				free(envValue); 
				DieWithError("INVALID PARAMS.");
			}
			free(envValue); 
		}
	}

	Cycle(itCount, pid);
}