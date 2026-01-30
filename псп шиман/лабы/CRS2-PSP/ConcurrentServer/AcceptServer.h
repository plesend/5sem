#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma warning(disable : 4996)
#include "Global.h"


static void WaitClients() {
	bool ListEmpty = false;
	while (!ListEmpty) {
		EnterCriticalSection(&scListContact);
		ListEmpty = Contacts.empty();
		LeaveCriticalSection(&scListContact);
		SleepEx(0, TRUE);
	}
}

bool AcceptCycle(int squirt, SOCKET* s) {
	bool rc = false;

	Contact c(Contact::ACCEPT, "EchoServer");
	c.hAcceptServer = hAcceptServer;

	if ((c.s = accept(*s, (sockaddr*)&c.prms, &c.lprms)) == INVALID_SOCKET) {
		if (WSAGetLastError() != WSAEWOULDBLOCK) throw  SetErrorMsgText("Accept:", WSAGetLastError()); 
	}
	else {
		rc = true;
		InterlockedIncrement(&Accept);
		InterlockedIncrement(&ClientServiceNumber);
		EnterCriticalSection(&scListContact);
		Contacts.push_front(c);
		LeaveCriticalSection(&scListContact);
		SetEvent(Event);
	}

	return rc;
};


void CommandsCycle(TalkersCommand& cmd, SOCKET* s)
{
	int squirt = 0; 
	while (cmd != EXIT) 
	{
		switch (cmd) {	
		case START:		cmd = GETCOMMAND;	
						squirt = AS_SQUIRT;
						break;

		case STOP:		cmd = GETCOMMAND;
						squirt = 0;
						break;

		case WAIT:		WaitClients();
						cmd = GETCOMMAND;
						squirt = 0;
						cmd = START;
						break;

		case SHUTDOWN:	WaitClients();
						cmd = EXIT;
						break;
		};
		if (cmd != EXIT && squirt > ClientServiceNumber) {
			if (AcceptCycle(squirt, s)) { 
				cmd = GETCOMMAND;
			}
			SleepEx(0, TRUE); 
		}
	}
};

DWORD WINAPI AcceptServer(LPVOID pPrm) {
	DWORD rc = 0;
	SOCKET  ServerSocket;
	WSADATA wsaData;

	try {
		if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0) throw  SetErrorMsgText("Startup:", WSAGetLastError());
		if ((ServerSocket = socket(AF_INET, SOCK_STREAM, NULL)) == INVALID_SOCKET) throw  SetErrorMsgText("Socket:", WSAGetLastError());

		SOCKADDR_IN Server_IN;
		Server_IN.sin_family = AF_INET;
		Server_IN.sin_port = htons(port);
		Server_IN.sin_addr.s_addr = ADDR_ANY;

		if (bind(ServerSocket, (LPSOCKADDR)&Server_IN, sizeof(Server_IN)) == SOCKET_ERROR) throw  SetErrorMsgText("Bind:", WSAGetLastError());
		if (listen(ServerSocket, SOMAXCONN) == SOCKET_ERROR) throw  SetErrorMsgText("Listen:", WSAGetLastError());

		u_long nonblk;
		if (ioctlsocket(ServerSocket, FIONBIO, &(nonblk = 1)) == SOCKET_ERROR) throw SetErrorMsgText("Ioctlsocket:", WSAGetLastError());

		TalkersCommand* command = (TalkersCommand*)pPrm;
		CommandsCycle(*((TalkersCommand*)command), &ServerSocket);

		if (closesocket(ServerSocket) == SOCKET_ERROR) throw  SetErrorMsgText("Сlosesocket:", WSAGetLastError());
		if (WSACleanup() == SOCKET_ERROR) throw  SetErrorMsgText("Cleanup:", WSAGetLastError());
	}
	catch (string errorMsgText) {
		std::cout << errorMsgText << endl;
	}
	catch (...) {
		cout << "AcceptServer error" << endl;
	}
	cout << "AcceptServer остановлен\n" << endl;
	ExitThread(rc);
}

