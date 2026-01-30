#include <iostream>
#include <windows.h>
#include <string.h>

int main(int argc, char** argv) {
	HWND window = GetConsoleWindow();
	char title[BUFSIZ] = { 0 };
	GetConsoleTitleA(title, BUFSIZ);

	std::cout << "text: " << title << std::endl;
	Sleep(5000);

}