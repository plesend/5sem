#include <iostream>
#include <string>
#include "Winsock2.h"              

using namespace std;

#pragma comment(lib, "WS2_32.lib")   
#pragma warning(disable:4996)

using namespace std;

string GetErrorMsgText(int code)
{
	string msgText;
	switch (code)
	{
	case WSAEINTR: msgText = "WSAEINTR"; break;
	case WSAEACCES:          msgText = "WSAEACCES"; break;
	case WSAEFAULT:          msgText = "WSAEFAULT"; break;
	case WSAEINVAL:          msgText = "WSAEINVAL"; break;
	case WSAEMFILE:          msgText = "WSAEMFILE"; break;
	case WSAEWOULDBLOCK:     msgText = "WSAEWOULDBLOCK"; break;
	case WSAEINPROGRESS:     msgText = "WSAEINPROGRESS"; break;
	case WSAEALREADY:        msgText = "WSAEALREADY"; break;
	case WSAENOTSOCK:        msgText = "WSAENOTSOCK"; break;
	case WSAEDESTADDRREQ:    msgText = "WSAEDESTADDRREQ"; break;
	case WSAEMSGSIZE:        msgText = "WSAEMSGSIZE"; break;
	case WSAEPROTOTYPE:      msgText = "WSAEPROTOTYPE"; break;
	case WSAENOPROTOOPT:     msgText = "WSAENOPROTOOPT"; break;
	case WSAEPROTONOSUPPORT: msgText = "WSAEPROTONOSUPPORT"; break;
	case WSAESOCKTNOSUPPORT: msgText = "WSAESOCKTNOSUPPORT"; break;
	case WSAEOPNOTSUPP:      msgText = "WSAEOPNOTSUPP"; break;
	case WSAEPFNOSUPPORT:    msgText = "WSAEPFNOSUPPORT"; break;
	case WSAEAFNOSUPPORT:    msgText = "WSAEAFNOSUPPORT"; break;
	case WSAEADDRINUSE:      msgText = "WSAEADDRINUSE"; break;
	case WSAEADDRNOTAVAIL:   msgText = "WSAEADDRNOTAVAIL"; break;
	case WSAENETDOWN:        msgText = "WSAENETDOWN"; break;
	case WSAENETUNREACH:     msgText = "WSAENETUNREACH"; break;
	case WSAENETRESET:       msgText = "WSAENETRESET"; break;
	case WSAECONNABORTED:    msgText = "WSAECONNABORTED"; break;
	case WSAECONNRESET:      msgText = "WSAECONNRESET"; break;
	case WSAENOBUFS:         msgText = "WSAENOBUFS"; break;
	case WSAEISCONN:         msgText = "WSAEISCONN"; break;
	case WSAENOTCONN:        msgText = "WSAENOTCONN"; break;
	case WSAESHUTDOWN:       msgText = "WSAESHUTDOWN"; break;
	case WSAETIMEDOUT:       msgText = "WSAETIMEDOUT"; break;
	case WSAECONNREFUSED:    msgText = "WSAECONNREFUSED"; break;
	case WSAEHOSTDOWN:       msgText = "WSAEHOSTDOWN"; break;
	case WSAEHOSTUNREACH:    msgText = "WSAEHOSTUNREACH"; break;
	case WSAEPROCLIM:        msgText = "WSAEPROCLIM"; break;
	case WSASYSNOTREADY:          msgText = "WSASYSNOTREADY"; break;
	case WSAVERNOTSUPPORTED:      msgText = "WSAVERNOTSUPPORTED"; break;
	case WSANOTINITIALISED:       msgText = "WSANOTINITIALISED"; break;
	case WSAEDISCON:              msgText = "WSAEDISCON"; break;
	case WSATYPE_NOT_FOUND:       msgText = "WSATYPE_NOT_FOUND"; break;
	case WSAHOST_NOT_FOUND:       msgText = "WSAHOST_NOT_FOUND"; break;
	case WSATRY_AGAIN:            msgText = "WSATRY_AGAIN"; break;
	case WSANO_RECOVERY:          msgText = "WSANO_RECOVERY"; break;
	case WSANO_DATA:              msgText = "WSANO_DATA"; break;
	case WSA_INVALID_HANDLE:      msgText = "WSA_INVALID_HANDLE"; break;
	case WSA_INVALID_PARAMETER:   msgText = "WSA_INVALID_PARAMETER"; break;
	case WSA_IO_INCOMPLETE:       msgText = "WSA_IO_INCOMPLETE"; break;
	case WSA_IO_PENDING:          msgText = "WSA_IO_PENDING"; break;
	case WSA_NOT_ENOUGH_MEMORY:   msgText = "WSA_NOT_ENOUGH_MEMORY"; break;
	case WSA_OPERATION_ABORTED:   msgText = "WSA_OPERATION_ABORTED"; break;
	case WSASYSCALLFAILURE:       msgText = "WSASYSCALLFAILURE"; break;
	}
	return msgText;
}

string SetErrorMsgText(string msgText, int code)
{
	return msgText + GetErrorMsgText(code);
}

int main()
{
	SOCKET  sC;
	WSADATA wsaData;

	try
	{
		int count;
		cout << "Enter number of messages:\n";
		cin >> count;

		if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
			throw  SetErrorMsgText("Startup:", WSAGetLastError());

		if ((sC = socket(AF_INET, SOCK_DGRAM, NULL)) == INVALID_SOCKET)
			throw  SetErrorMsgText("socket:", WSAGetLastError());

		int timeout = 1000;
		setsockopt(sC, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));

		cout << "Enter server ip adress:";
		
		char addr[256];
		
		cin >> addr;
		
		SOCKADDR_IN serv;
		serv.sin_family = AF_INET;
		serv.sin_port = htons(2000);
		serv.sin_addr.s_addr = inet_addr(addr);
		
		int  lobuf = 0;
		int lc = sizeof(serv);

		int time = clock();
		for (int i = 0; i < count; i++)
		{
			char obuf[50] = "Hello from ClientU";
			char recvBuf[50];
			char j[10];
			itoa(i + 1, j, 10);
			strcat(obuf, j);
			if ((lobuf = sendto(sC, obuf, strlen(obuf) + 1, NULL, (sockaddr*)&serv, sizeof(serv))) == SOCKET_ERROR)
				std::cout << SetErrorMsgText("sendto:", WSAGetLastError());
			cout << obuf << " " << i + 1 << endl;

		}
		cout << "\nProgram was running for " << time << " ticks or " << ((float)time) / CLOCKS_PER_SEC << " seconds.\n";

		if (closesocket(sC) == SOCKET_ERROR)
			throw  SetErrorMsgText("closesocket:", WSAGetLastError());
		if (WSACleanup() == SOCKET_ERROR)
			throw SetErrorMsgText("Cleanup:", WSAGetLastError());
	}
	catch (string errorMsgText)
	{
		cout << endl << "WSAGetLastError: " << errorMsgText;
	}
}