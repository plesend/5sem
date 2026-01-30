#include <windows.h>
#include <stdio.h>

#define EVENT L"Global\\event"

int main()
{
	HANDLE evt = OpenEventW(EVENT_ALL_ACCESS, FALSE, EVENT);
	if (!evt) {
		printf("cant open event\n");
		return 1;
	}

	const char array[13] = {
	'u', 's', 'e', 'r', '-',
	'7', '4', '8', 'c',
	'2', 'f', '7', '5'
	};

	WaitForSingleObject(evt, INFINITE);

	for (int i = 0; i <= 90; i++) {
		int index = i % 13;
		char symb = array[index];
		printf("[B] iteration %d char: %c\n", i, symb);

		Sleep(100);
	}

	Sleep(10000);
}

