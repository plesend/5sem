#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "Winsock2.h"
#include <iostream>
#include <cstring>
#include <string>

#pragma comment(lib, "WS2_32.lib")

using namespace std;

string GetErrorMsgText(int err) {
	string msgText;

	switch (err) {
		case WSAEINTR: msgText = "WSAEINTR: Function is interrupted"; break;
		case WSAEACCES: msgText = "WSAEACCES: Permission denied"; break;
		case WSAEFAULT: msgText = "WSAEFAULT: Wrong address"; break;
		case WSAEINVAL: msgText = "WSAEINVAL: Invalid argument"; break;
		case WSAEMFILE: msgText = "WSAEMFILE: Too many open files"; break;
		case WSAEWOULDBLOCK: msgText = "WSAEWOULDBLOCK: Resource temporarily unavailable"; break;
		case WSAEINPROGRESS: msgText = "WSAEINPROGRESS: Operation now in progress"; break;
		case WSAEALREADY: msgText = "WSAEALREADY: Operation already in progress"; break;
		case WSAENOTSOCK: msgText = "WSAENOTSOCK: Socket operation on non-socket"; break;
		case WSAEDESTADDRREQ: msgText = "WSAEDESTADDRREQ: Destination address required"; break;
		case WSAEMSGSIZE: msgText = "WSAEMSGSIZE: Message too long"; break;
		case WSAEPROTOTYPE: msgText = "WSAEPROTOTYPE: Wrong protocol type for socket"; break;
		case WSAENOPROTOOPT: msgText = "WSAENOPROTOOPT: Bad protocol option"; break;
		case WSAEPROTONOSUPPORT: msgText = "WSAEPROTONOSUPPORT: Protocol not supported"; break;
		case WSAESOCKTNOSUPPORT: msgText = "WSAESOCKTNOSUPPORT: Socket type not supported"; break;
		case WSAEOPNOTSUPP: msgText = "WSAEOPNOTSUPP: Operation not supported"; break;
		case WSAEPFNOSUPPORT: msgText = "WSAEPFNOSUPPORT: Protocol family not supported"; break;
		case WSAEAFNOSUPPORT: msgText = "WSAEAFNOSUPPORT: Address family not supported by protocol"; break;
		case WSAEADDRINUSE: msgText = "WSAEADDRINUSE: Address already in use"; break;
		case WSAEADDRNOTAVAIL: msgText = "WSAEADDRNOTAVAIL: Cannot assign requested address"; break;
		case WSAENETDOWN: msgText = "WSAENETDOWN: Network is down"; break;
		case WSAENETUNREACH: msgText = "WSAENETUNREACH: Network is unreachable"; break;
		case WSAENETRESET: msgText = "WSAENETRESET: Network dropped connection on reset"; break;
		case WSAECONNABORTED: msgText = "WSAECONNABORTED: Software caused connection abort"; break;
		case WSAECONNRESET: msgText = "WSAECONNRESET: Connection reset by peer"; break;
		case WSAENOBUFS: msgText = "WSAENOBUFS: No buffer space available"; break;
		case WSAEISCONN: msgText = "WSAEISCONN: Socket is already connected"; break;
		case WSAENOTCONN: msgText = "WSAENOTCONN: Socket is not connected"; break;
		case WSAESHUTDOWN: msgText = "WSAESHUTDOWN: Cannot send after socket shutdown"; break;
		case WSAETIMEDOUT: msgText = "WSAETIMEDOUT: Connection timed out"; break;
		case WSAECONNREFUSED: msgText = "WSAECONNREFUSED: Connection refused"; break;
		case WSAEHOSTDOWN: msgText = "WSAEHOSTDOWN: Host is down"; break;
		case WSAEHOSTUNREACH: msgText = "WSAEHOSTUNREACH: No route to host"; break;
		case WSAEPROCLIM: msgText = "WSAEPROCLIM: Too many processes"; break;
		case WSASYSNOTREADY: msgText = "WSASYSNOTREADY: Network subsystem not ready"; break;
		case WSAVERNOTSUPPORTED: msgText = "WSAVERNOTSUPPORTED: Winsock version not supported"; break;
		case WSANOTINITIALISED: msgText = "WSANOTINITIALISED: WSAStartup not yet called"; break;
		case WSAEDISCON: msgText = "WSAEDISCON: Graceful shutdown in progress"; break;
		case WSATYPE_NOT_FOUND: msgText = "WSATYPE_NOT_FOUND: Class type not found"; break;
		case WSAHOST_NOT_FOUND: msgText = "WSAHOST_NOT_FOUND: Host not found"; break;
		case WSATRY_AGAIN: msgText = "WSATRY_AGAIN: Non-authoritative host not found"; break;
		case WSANO_RECOVERY: msgText = "WSANO_RECOVERY: Non-recoverable error"; break;
		case WSANO_DATA: msgText = "WSANO_DATA: Requested name has no data"; break;
		case WSA_INVALID_HANDLE: msgText = "WSA_INVALID_HANDLE: Specified event handle is invalid"; break;
		case WSA_INVALID_PARAMETER: msgText = "WSA_INVALID_PARAMETER: One or more parameters are invalid"; break;
		case WSA_IO_INCOMPLETE: msgText = "WSA_IO_INCOMPLETE: Overlapped I/O event object not in signaled state"; break;
		case WSA_IO_PENDING: msgText = "WSA_IO_PENDING: Overlapped I/O operation will complete later"; break;
		case WSA_NOT_ENOUGH_MEMORY: msgText = "WSA_NOT_ENOUGH_MEMORY: Insufficient memory"; break;
		case WSA_OPERATION_ABORTED: msgText = "WSA_OPERATION_ABORTED: Overlapped operation aborted"; break;
		case WSAEINVALIDPROCTABLE: msgText = "WSAEINVALIDPROCTABLE: Invalid procedure table from service provider"; break;
		case WSAEINVALIDPROVIDER: msgText = "WSAEINVALIDPROVIDER: Invalid service provider version"; break;
		case WSAEPROVIDERFAILEDINIT: msgText = "WSAEPROVIDERFAILEDINIT: Unable to initialize service provider"; break;
		case WSASYSCALLFAILURE: msgText = "WSASYSCALLFAILURE: System call failure"; break;
		default: msgText = "Unknown error"; break;
	}
	return msgText;
}

string SetErrorMsgText(string msgText, int err) {
	return msgText + GetErrorMsgText(err);
};
void processClientApp(SOCKET sC)
{
	char inputBuffer[50];
	char outputBuffer[50] = "server: understood have a nice day\n";
	int lengthInputBuffer = 0;
	int lengthOutputBuffer = 0;
	const char* eof = "\0";
	string buff;
	int messageCount = 0;

	while (true)
	{
		if ((lengthInputBuffer = recv(sC, inputBuffer, sizeof(inputBuffer), NULL)) == SOCKET_ERROR)
			throw SetErrorMsgText("recv: ", WSAGetLastError());
		if (lengthInputBuffer == 0)
		{
			cout << "client disconnected\n";
			break;
		}
		buff.append(inputBuffer, lengthInputBuffer);

		size_t endlinePos;
		while ((endlinePos = buff.find('\n')) != string::npos)
		{
			messageCount++;

			string singleMessage = buff.substr(0, endlinePos);
			string output = singleMessage + " :" + to_string(messageCount);
			cout << "client says: " << output << endl;
			buff.erase(0, endlinePos + 1);

			const char* messageToClient = output.c_str();
			if ((lengthOutputBuffer = send(sC, messageToClient, strlen(messageToClient), NULL)) == SOCKET_ERROR)
				throw SetErrorMsgText("send: ", WSAGetLastError());

			if ((lengthInputBuffer = recv(sC, inputBuffer, sizeof(inputBuffer), NULL)) == SOCKET_ERROR)
				throw SetErrorMsgText("recv: ", WSAGetLastError());

			string answerBuffer = inputBuffer;
			while ((endlinePos = answerBuffer.find('\n')) != string::npos)
			{
				singleMessage = answerBuffer.substr(0, endlinePos);
				cout << "client says AGAIN: " << singleMessage << endl;
				answerBuffer.erase(0, endlinePos + 1);
			}

		}

		memset(inputBuffer, 0, sizeof(inputBuffer));

	}

	// CLOSE SOCKET
	if (closesocket(sC) == SOCKET_ERROR)
		throw SetErrorMsgText("closesocket: ", WSAGetLastError());
}

int main() {
	SOCKET sS;// сокет сервер
	WSADATA wsadata;
	try
	{
		if (WSAStartup(MAKEWORD(2, 0), &wsadata) != 0)
			throw SetErrorMsgText("Startup: ", WSAGetLastError());
		if ((sS = socket(AF_INET, SOCK_STREAM, NULL)) == INVALID_SOCKET)
			throw SetErrorMsgText("socket: ", WSAGetLastError());

		sockaddr_in servSettings;
		servSettings.sin_family = AF_INET;
		servSettings.sin_port = htons(2000);
		servSettings.sin_addr.S_un.S_addr = INADDR_ANY;

		cout << servSettings.sin_port << endl;

		if (bind(sS, (sockaddr*)&servSettings, sizeof(servSettings)) == SOCKET_ERROR)
			throw SetErrorMsgText("bind connect: ", WSAGetLastError());

		if (listen(sS, SOMAXCONN) == SOCKET_ERROR)
			throw SetErrorMsgText("listen: ", WSAGetLastError());

		while (true)
		{

			fd_set readfds;
			FD_ZERO(&readfds);
			FD_SET(sS, &readfds);

			timeval timeout; timeout.tv_sec = 60; timeout.tv_usec = 0;

			int selRes = select(0, &readfds, NULL, NULL, &timeout);
			if (selRes == SOCKET_ERROR)
				throw SetErrorMsgText("select: ", WSAGetLastError());
			else if (selRes == 0)
			{
				cout << "no client connected in 60s, not waiting anymore\n";
				break;
			}

			SOCKET sC; // сокет клиент
			sockaddr_in client;
			memset(&client, 0, sizeof(client));
			int lengthClient = sizeof(client);

			if ((sC = accept(sS, (sockaddr*)&client, &lengthClient)) == INVALID_SOCKET)
				throw SetErrorMsgText("accept client: ", WSAGetLastError());

			cout << "client address: " << inet_ntoa(client.sin_addr) << ":" << ntohs(client.sin_port) << "\n";

			processClientApp(sC);


		}
		if (closesocket(sS) == SOCKET_ERROR)
			throw SetErrorMsgText("closesocket: ", WSAGetLastError());
		if (WSACleanup() == SOCKET_ERROR)
			throw SetErrorMsgText("Cleaunp: ", WSAGetLastError());
	}

	catch (string msg)
	{
		cout << "\n" << msg;
	}
	return 0;
}