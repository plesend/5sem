#include <unistd.h>
#include <iostream>
#include <string>
#include <csignal>

using namespace std;

void HandleUsr1(int sig) {
	cout << "Received USR1 signal" << endl;
}

void HandleUsr2(int sig) {
	cout << "Received USR2 signal" << endl;
}

void HandleCtrlC(int sig) {
	cout << "Ctrl+C" << endl;
}

void HandleTerminate(int sig) {
	cout << "Terminated" << endl;
}

int main() {
	signal(SIGINT, HandleCtrlC);
	signal(SIGINT, HandleUsr1);
	signal(SIGINT, HandleUsr2);
	signal(SIGINT, HandleTerminate);

	cout << "Program running with PID" << getpid() << endl;

	while (true) {
		sleep(1);
	}
	return 0;
}