#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <ws2tcpip.h>
#include <tchar.h>
#include <iostream>
#include <string>
#include "ErrorHandler.h"
#pragma comment(lib,"WS2_32.lib")

using namespace std;
SOCKET clientSocket;

bool GetServer(char* call, short port, sockaddr* from, int* flen)
{
	memset(from, 0, sizeof(flen));

	if ((clientSocket = socket(AF_INET, SOCK_DGRAM, NULL)) == INVALID_SOCKET)
	{
		throw  SetErrorMsgText("socket:", WSAGetLastError());
	}

	int optval = 1;
	if (setsockopt(clientSocket,
		SOL_SOCKET,
		SO_BROADCAST,
		(char*)&optval,
		sizeof(int)) == SOCKET_ERROR)
	{
		throw  SetErrorMsgText("opt:", WSAGetLastError());
	}

	SOCKADDR_IN all;
	all.sin_family = AF_INET;
	all.sin_port = htons(2000);
	all.sin_addr.s_addr = INADDR_BROADCAST;

	if ((sendto(clientSocket, call, strlen(call) + 1, NULL, (sockaddr*)&all, sizeof(all))) == SOCKET_ERROR)
	{
		throw  SetErrorMsgText("sendto:", WSAGetLastError());
	}

	char nameServer[50];
	char buffer[50];
	int pntr = 0;
	if ((pntr = recvfrom(clientSocket, nameServer, sizeof(nameServer), NULL, from, flen)) == SOCKET_ERROR)
	{
		if (WSAGetLastError() == WSAETIMEDOUT)
		{
			return false;
		}
		else
		{
			throw  SetErrorMsgText("recv:", WSAGetLastError());
		}
	}

	nameServer[pntr] = 0x00;
	cout << "Ответ от сервера: " << nameServer << endl;

	SOCKADDR_IN* addr = (SOCKADDR_IN*)from;
	std::cout << std::endl << "Порт сервера: " << addr->sin_port;
	std::cout << std::endl << "IP-адрес сервера: " << inet_ntoa(addr->sin_addr);

	if (!strcmp(nameServer, call))
	{
		std::cout << std::endl << "Сервер с таким именем найден.";
		return true;
	}
	else
	{
		std::cout << std::endl << "Сервер с таким именем не найден.";
		return false;
	}

	return true;
}

bool GetServerByName(const char* name, const char* call, sockaddr* from, int* flen) {
	hostent* host = gethostbyname(name);
	if (!host) throw SetErrorMsgText("gethostbyname:", WSAGetLastError());


	char* ip_addr = inet_ntoa(*(in_addr*)(host->h_addr));
	cout << "\nИмя сервера: " << host->h_name << endl << "IP-адрес сервера: " << ip_addr << endl;

	SOCKADDR_IN server;
	server.sin_family = AF_INET;                 //IP-адресация  
	server.sin_port = htons(2000);
	server.sin_addr.s_addr = inet_addr(ip_addr);
	char message[50];

	if (sendto(clientSocket, call, strlen(call), NULL, (const sockaddr*)&server, *flen) == SOCKET_ERROR)
		throw SetErrorMsgText("sendto:", WSAGetLastError());

	int buf = 0;
	if ((buf = recvfrom(clientSocket, message, sizeof(message), NULL, (sockaddr*)&server, flen)) == SOCKET_ERROR)
		throw SetErrorMsgText("recvfrom:", WSAGetLastError());

	message[buf] = 0x00;
	cout << "Ответ от сервера: " << message << endl;
	*from = *((sockaddr*)&server);
	return true;
}


int main()
{
	setlocale(LC_ALL, "rus");
	try {

		WSADATA wsaData;
		if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0)
		{
			throw  SetErrorMsgText("Startup:", WSAGetLastError());
		}

		char call[] = "Hello";

		SOCKADDR_IN clnt;
		int lc = sizeof(clnt);

		GetServer(call, 2000, (sockaddr*)&clnt, &lc);

		GetServerByName("MY-PC", call, (sockaddr*)&clnt, &lc);
		
		if (closesocket(clientSocket) == SOCKET_ERROR)
		{
			throw  SetErrorMsgText("closesocket:", WSAGetLastError());
		}
		if (WSACleanup() == SOCKET_ERROR)
		{
			throw  SetErrorMsgText("Cleanup:", WSAGetLastError());
		}

	}
	catch (string errorMsgText) {
		cout << endl << "WSAGetLastError: " << errorMsgText;
	}
}
