#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma warning(disable : 4996)
#include "Global.h"

DWORD WINAPI ResponseServer(LPVOID pPrm) {
	DWORD rc = 0;
	SOCKET  ServerSocket;
	WSADATA wsaData;

	SOCKADDR_IN From = { 0 };   
	int Flen = sizeof(From);
	SOCKADDR_IN serv; 	                 
	serv.sin_family = AF_INET;
	serv.sin_port = htons(uport);
	serv.sin_addr.s_addr = ADDR_ANY;

	try 
	{
		if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0)	throw  SetErrorMsgText("Startup:", WSAGetLastError());
		if ((ServerSocket = socket(AF_INET, SOCK_DGRAM, NULL)) == INVALID_SOCKET)throw  SetErrorMsgText("Socket:", WSAGetLastError());

		BOOL optval = TRUE;
		setsockopt(ServerSocket, SOL_SOCKET, SO_BROADCAST,
			(char*)&optval, sizeof(optval));


		u_long nonblk = 1; 
		if (ioctlsocket(ServerSocket, FIONBIO, &(nonblk = 1)) == SOCKET_ERROR) throw SetErrorMsgText("Ioctlsocket:", WSAGetLastError());
		if (bind(ServerSocket, (LPSOCKADDR)&serv, sizeof(serv)) == SOCKET_ERROR) throw  SetErrorMsgText("Bind:", WSAGetLastError());

		while (*((TalkersCommand*)pPrm) != EXIT) {
			char ibuf[50]; 
			int  libuf = 0;	

			bool result = false;

			if ((libuf = recvfrom(ServerSocket, ibuf, sizeof(ibuf), NULL, (LPSOCKADDR)&From, &Flen)) == SOCKET_ERROR) {
					switch (WSAGetLastError()) {
					case WSAEWOULDBLOCK: Sleep(10); break;
					default: throw SetErrorMsgText("Recv:", WSAGetLastError());
				}
			}
			else {
				result = true;
				cout << "Discovery from "
					<< inet_ntoa(From.sin_addr)
					<< " : " << ibuf << endl;
			}
			if (libuf > 0 && result == true) {
				if (strcmp(ibuf, ucall) == 0) {
					if ((libuf = sendto(ServerSocket, ucall, strlen(ucall) + 1, NULL, (LPSOCKADDR)&From, sizeof(From))) == SOCKET_ERROR) throw  SetErrorMsgText("Sendto:", WSAGetLastError());
				}
			}
		}
		if (closesocket(ServerSocket) == SOCKET_ERROR) throw  SetErrorMsgText("Closesocket:", WSAGetLastError());
		if (WSACleanup() == SOCKET_ERROR)throw  SetErrorMsgText("Cleanup:", WSAGetLastError());
	}
	catch (string errorMsgText) {
		cout << errorMsgText << endl;
	}
	catch (...) {
		cout << "Error ResponseServer" << endl;
	}
	cout << "ResponseServer остановлен\n" << endl;
	ExitThread(rc);
}