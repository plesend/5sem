#include "windows.h"
#include <iostream>
#include <thread>
#include <cstdlib>

void Handle(int n) {
	const char array[12] = {
	'u', 's', 'e', 'r',
	'7', '4', '8', 'c',
	'2', 'f', '7', '5'
	};
	for (int i = 0; i < n; i++) {
		int index = i % 12;
		printf("PID: %lu  TID: %lu  process %d - %c\n",
			GetCurrentProcessId(),
			GetCurrentThreadId(),
			i,
			array[index]);

		Sleep(300);
	}
}

int main(int argc, char** argv) {
	if (argc < 2) { 
		std::cout << "Usage: " << argv[0] << " <number>" << std::endl;
		return 1;
	}

	int N = atoi(argv[1]);
	Handle(N);

	return 0;
}