#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <ws2tcpip.h>
#include <tchar.h>
#include "ErrorHandler.h"
#pragma comment(lib,"WS2_32.lib")

using namespace std;

SOCKET sS;
SOCKET cS;


bool GetServer(char* call, short port, sockaddr* from, int* flen)
{
	SOCKET cS;     
	if ((cS = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
	{
		throw SetErrorMsgText("socket create: ", WSAGetLastError());
	}

	BOOL bcast = TRUE;
	if (setsockopt(cS, SOL_SOCKET, SO_BROADCAST, (char*)&bcast, sizeof(bcast)) == SOCKET_ERROR)
	{
		throw SetErrorMsgText("setsockopt SO_BROADCAST: ", WSAGetLastError());
	}

	SOCKADDR_IN all;
	all.sin_family = AF_INET;
	all.sin_port = htons(port);
	all.sin_addr.s_addr = INADDR_BROADCAST;
	memcpy(from, &all, sizeof(all));

	int err = sendto(cS, call, strlen(call) + 1, 0, (sockaddr*)&all, sizeof(all));
	if (err == SOCKET_ERROR)
	{
		throw SetErrorMsgText("sendto: ", WSAGetLastError());
	}

	timeval tv;
	tv.tv_sec = 1;   
	tv.tv_usec = 0;
	fd_set readfds;

	int count = 0;
	while (true)
	{
		FD_ZERO(&readfds);
		FD_SET(cS, &readfds);

		int sel = select(0, &readfds, NULL, NULL, &tv);
		if (sel > 0 && FD_ISSET(cS, &readfds))
		{
			char buffer[100];
			sockaddr_in sender;
			int senderLen = sizeof(sender);

			int recvBytes = recvfrom(cS, buffer, sizeof(buffer), 0, (sockaddr*)&sender, &senderLen);
			if (recvBytes == SOCKET_ERROR)
			{
				int errCode = WSAGetLastError();
				if (errCode == WSAETIMEDOUT) break;     
				throw SetErrorMsgText("recvfrom: ", errCode);
			}

			count++;
			cout << "Ответ от сервера " << count << ": " << buffer
				<< " (IP: " << inet_ntoa(sender.sin_addr) << ", порт: " << ntohs(sender.sin_port) << ")" << endl;
		}
		else
		{
			break;    
		}
	}
	cout << "Всего серверов в сети: " << ++count << endl;
	closesocket(cS);

	return count > 0;      
}

bool GetRequestFromClient(char* name, short port, sockaddr* from, int* flen)
{
	char bfrom[50];
	int err = 0;
	while (true)
	{

		if ((err = recvfrom(sS, bfrom, sizeof(bfrom), NULL, from, flen)) == SOCKET_ERROR)
		{
			throw SetErrorMsgText("rexevfrom: ", WSAGetLastError());
		}
		if (strcmp(name, bfrom) == 0)
		{
			return true;
		}
	}

}

bool PutAnswerToClient(char* name, sockaddr* to, int* lto)
{
	int err = 0;
	if ((err = sendto(sS, name, strlen(name) + 1, NULL, to, *lto)) == SOCKET_ERROR)
	{
		throw SetErrorMsgText("sendto: ", WSAGetLastError());
		return false;
	}
	return true;
}


void main()
{
	try
	{

		setlocale(LC_ALL, "rus");
		char name[10] = "Hello";
		WSADATA wsaData;
		if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0)
		{
			throw SetErrorMsgText("wsaSartUp: ", WSAGetLastError());
		}

		if ((cS = socket(AF_INET, SOCK_DGRAM, NULL)) == INVALID_SOCKET)
		{
			throw SetErrorMsgText("socket create: ", WSAGetLastError());
		}

		DWORD timeout = 1000;
		if (setsockopt(cS, SOL_SOCKET, SO_BROADCAST, (char*)&timeout, sizeof(int)) == SOCKET_ERROR)
		{
			throw SetErrorMsgText("setsockopt: ", WSAGetLastError());
		}

		int optval = 1;
		if (setsockopt(cS, SOL_SOCKET, SO_RCVTIMEO, (char*)&optval, sizeof(int)) == SOCKET_ERROR)
		{
			throw SetErrorMsgText("setsockopt: ", WSAGetLastError());
		}

		SOCKADDR_IN all;
		int allLen = sizeof(all);

		GetServer(name, 2000, (sockaddr*)&all, &allLen);


		if (closesocket(cS) == SOCKET_ERROR)
		{
			throw SetErrorMsgText("close socket: ", WSAGetLastError());
		}

		if ((sS = socket(AF_INET, SOCK_DGRAM, NULL)) == INVALID_SOCKET)
		{
			throw SetErrorMsgText("creaate socket: ", WSAGetLastError());
		}

		SOCKADDR_IN  serv;
		serv.sin_addr.s_addr = INADDR_ANY;
		serv.sin_family = AF_INET;
		serv.sin_port = htons(2000);
		BOOL optval2 = TRUE;
		setsockopt(sS, SOL_SOCKET, SO_REUSEADDR, (char*)&optval2, sizeof(optval));

		if (bind(sS, (LPSOCKADDR)&serv, sizeof(serv)) == SOCKET_ERROR)
		{
			throw SetErrorMsgText("bind: ", WSAGetLastError());
		}



		SOCKADDR_IN  from;
		memset(&from, 0, sizeof(from));
		int lbfrom = sizeof(from);
		char hostName[20];
		int err = 0;
		hostent* addr;

		if ((err = gethostname(hostName, sizeof(hostName))) == SOCKET_ERROR)
		{
			throw SetErrorMsgText("get host name: ", WSAGetLastError());
		}

		cout << "host name: " << hostName;
		while (true)
		{
			if (!GetRequestFromClient(name, 2000, (sockaddr*)&from, &lbfrom))
			{
				throw SetErrorMsgText("getRequestFromClient: ", WSAGetLastError());
			}

			std::cout << endl << "IP: " << inet_ntoa(from.sin_addr) << "\tport: " << htons(from.sin_port) << endl;
			lbfrom = sizeof(from);

			if (!PutAnswerToClient(name, (sockaddr*)&from, &lbfrom))
			{
				throw SetErrorMsgText("put answer: ", WSAGetLastError());
			}

			struct in_addr ip_addr = from.sin_addr;
			struct hostent* addr = gethostbyaddr((const char*)&ip_addr, sizeof(ip_addr), AF_INET);

			if (addr != NULL) {
				cout << "host name: " << addr->h_name << endl;
			}

		
		}


		if (closesocket(sS) == SOCKET_ERROR)
		{
			throw SetErrorMsgText("close socket: ", WSAGetLastError());
		}
		if (WSACleanup() == SOCKET_ERROR)
		{
			throw SetErrorMsgText("cleanup: ", WSAGetLastError());
		}
	}
	catch (string errorMsgText) {
		cout << endl << "WSAGetLastError: " << errorMsgText;
	}

}
