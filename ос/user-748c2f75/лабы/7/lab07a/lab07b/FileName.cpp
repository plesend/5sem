#include <windows.h>
#include <stdio.h>

int main() {

	ULONGLONG startTime = GetTickCount64();
	ULONGLONG currentTime;

	unsigned long long it = 0;

	int five = 0;
	int ten = 0;

	for (;;) {
		it++;

		currentTime = GetTickCount64();
		ULONGLONG elapsed = currentTime - startTime;

		if (elapsed >= 5000 && !five) {
			printf("5 seconds: iterations = %llu\n", it);
			five = 1;
		}

		if (elapsed >= 10000 && !ten) {
			printf("10 seconds: iterations = %llu\n", it);
			ten = 1;
		}

		if (elapsed >= 15000) {
			printf("THE END: iterations = %llu\n", it);
			break;
		}
	}
}