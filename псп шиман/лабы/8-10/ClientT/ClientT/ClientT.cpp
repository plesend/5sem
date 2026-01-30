#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "stdafx.h"
#include "Winsock2.h"
#include <ws2tcpip.h>
#include <string>
#include <iostream>
#include <ctime>
#include <windows.h>
#include <stdexcept>
#include <chrono>

//WIN-UCLB12VI625

#pragma comment(lib, "WS2_32.lib")

using namespace std;

#define IP_SERVER "192.168.151.114"
#define DEFAULT_PORT "2000"

using namespace std;
using namespace chrono;
SOCKET  cC;
SOCKET  cS;

bool ICAN = true;
volatile bool serverDisconnected = false; 
HANDLE hListener = NULL;  

string  GetErrorMsgText(int code)
{
	string msgText;
	switch (code)
	{
	case WSAEINTR:          msgText = "WSAEINTR";         break;
	case WSAEACCES:         msgText = "WSAEACCES";        break;
	case WSAEFAULT:          msgText = "WSAEFAULT";         break;
	case WSAEINVAL:         msgText = "WSAEINVAL";        break;
	case WSAEMFILE:          msgText = "WSAEMFILE";         break;
	case WSAEWOULDBLOCK:         msgText = "WSAEWOULDBLOCK";        break;
	case WSAEINPROGRESS:          msgText = "WSAEINPROGRESS";         break;
	case WSAEALREADY:         msgText = "WSAEALREADY";        break;
	case WSAENOTSOCK:          msgText = "WSAENOTSOCK";         break;
	case WSAEDESTADDRREQ:         msgText = "WSAEDESTADDRREQ";        break;
	case WSAEMSGSIZE:          msgText = "WSAEMSGSIZE";         break;
	case WSAEPROTOTYPE:         msgText = "WSAEPROTOTYPE";        break;
	case WSAENOPROTOOPT:          msgText = "WSAENOPROTOOPT";         break;
	case WSAEPROTONOSUPPORT:         msgText = "WSAEPROTONOSUPPORT";        break;
	case WSAESOCKTNOSUPPORT:          msgText = "WSAESOCKTNOSUPPORT";         break;
	case WSAEOPNOTSUPP:         msgText = "WSAEOPNOTSUPP";        break;
	case WSAEPFNOSUPPORT:          msgText = "WSAEPFNOSUPPORT";         break;
	case WSAEAFNOSUPPORT:         msgText = "WSAEAFNOSUPPORT";        break;
	case WSAEADDRINUSE:          msgText = "WSAEADDRINUSE";         break;
	case WSAEADDRNOTAVAIL:         msgText = "WSAEADDRNOTAVAIL";        break;
	case WSAENETDOWN:          msgText = "WSAENETDOWN";         break;
	case WSAENETUNREACH:         msgText = "WSAENETUNREACH";        break;
	case WSAENETRESET:          msgText = "WSAENETRESET";         break;
	case WSAECONNABORTED:         msgText = "WSAECONNABORTED";        break;
	case WSAECONNRESET:          msgText = "WSAECONNRESET";         break;
	case WSAENOBUFS:         msgText = "WSAENOBUFS";        break;
	case WSAEISCONN:          msgText = "WSAEISCONN";         break;
	case WSAENOTCONN:         msgText = "WSAENOTCONN";        break;
	case WSAESHUTDOWN:          msgText = "WSAESHUTDOWN";         break;
	case WSAETIMEDOUT:         msgText = "WSAETIMEDOUT";        break;
	case WSAECONNREFUSED:          msgText = "WSAECONNREFUSED";         break;
	case WSAEHOSTDOWN:         msgText = "WSAEHOSTDOWN";        break;
	case WSAEHOSTUNREACH:          msgText = "WSAEHOSTUNREACH";         break;
	case WSAEPROCLIM:         msgText = "WSAEPROCLIM";        break;
	case WSASYSNOTREADY:          msgText = "WSASYSNOTREADY";         break;
	case WSAVERNOTSUPPORTED:         msgText = "WSAVERNOTSUPPORTED";        break;
	case WSANOTINITIALISED:          msgText = "WSANOTINITIALISED";         break;
	case WSAEDISCON:         msgText = "WSAEDISCON";        break;
	case WSATYPE_NOT_FOUND:          msgText = "WSATYPE_NOT_FOUND";         break;
	case WSAHOST_NOT_FOUND:         msgText = "WSAHOST_NOT_FOUND";        break;
	case WSATRY_AGAIN:          msgText = "WSATRY_AGAIN";         break;
	case WSANO_RECOVERY:         msgText = "WSANO_RECOVERY";        break;
	case WSANO_DATA:          msgText = "WSANO_DATA";         break;
	case WSA_INVALID_HANDLE:         msgText = "WSA_INVALID_HANDLE";        break;
	case WSA_INVALID_PARAMETER:          msgText = "WSA_INVALID_PARAMETER";         break;
	case WSA_IO_INCOMPLETE:         msgText = "WSA_IO_INCOMPLETE";        break;
	case WSA_IO_PENDING:          msgText = "WSA_IO_PENDING";         break;
	case WSA_NOT_ENOUGH_MEMORY:         msgText = "WSA_NOT_ENOUGH_MEMORY";        break;
	case WSA_OPERATION_ABORTED:          msgText = "WSA_OPERATION_ABORTED";         break;
	case WSAEINVALIDPROCTABLE:         msgText = "WSAEINVALIDPROCTABLE";        break;
	case WSAEINVALIDPROVIDER:          msgText = "WSAEINVALIDPROVIDER";         break;
	case WSAEPROVIDERFAILEDINIT:         msgText = "WSAEPROVIDERFAILEDINIT";        break;

	case WSASYSCALLFAILURE: msgText = "WSASYSCALLFAILURE"; break;
	default:                msgText = "***ERROR***";      break;
	};
	return msgText;
};

string  SetErrorMsgText(string msgText, int code)
{
	string error = msgText + GetErrorMsgText(code);
	error.append(" ").append(to_string(code));
	return error;
};

char* get_message(int msg)
{
	switch (msg)
	{
	case 1: 	return "Echo";
	case 2: 	return "Time"; 
	case 3: 	return "Random"; 
	case 4:		return "close";
	case 5:		return "exit";
	default:
		return "";
	}
}

sockaddr_in DiscoverServer(int broadcastPort, int serverPort, const string& discoverMsg) {
    SOCKET bcSock = socket(AF_INET, SOCK_DGRAM, 0);
    if (bcSock == INVALID_SOCKET) {
        throw runtime_error("Broadcast socket creation failed");
    }

    BOOL broadcast = TRUE;
    setsockopt(bcSock, SOL_SOCKET, SO_BROADCAST, (char*)&broadcast, sizeof(broadcast));

    sockaddr_in clientAddr{};
    clientAddr.sin_family = AF_INET;
    clientAddr.sin_port = htons(0);
    clientAddr.sin_addr.s_addr = INADDR_ANY;
    bind(bcSock, (sockaddr*)&clientAddr, sizeof(clientAddr));

    sockaddr_in broadcastAddr{};
    broadcastAddr.sin_family = AF_INET;
    broadcastAddr.sin_port = htons(broadcastPort);
    broadcastAddr.sin_addr.s_addr = inet_addr("255.255.255.255");

    const char* query = "DISCOVER_SERVER_REQUEST";
    sendto(bcSock, query, (int)strlen(query), 0, (sockaddr*)&broadcastAddr, sizeof(broadcastAddr));

    fd_set readfds;
    timeval timeout{};
    timeout.tv_sec = 3;
    timeout.tv_usec = 0;
    FD_ZERO(&readfds);
    FD_SET(bcSock, &readfds);

    sockaddr_in serverAddr{};
    int addrLen = sizeof(serverAddr);
    if (select(0, &readfds, nullptr, nullptr, &timeout) > 0) {
        char buffer[256];
        int received = recvfrom(bcSock, buffer, sizeof(buffer) - 1, 0, (sockaddr*)&serverAddr, &addrLen);
        if (received > 0) {
            buffer[received] = '\0';
            if (string(buffer) == discoverMsg) {
                cout << "Server found via broadcast: " << inet_ntoa(serverAddr.sin_addr) << "\n";
                closesocket(bcSock);
                serverAddr.sin_port = htons(serverPort);
                return serverAddr;
            }
        }
    }
    closesocket(bcSock);
    throw runtime_error("Server not found via broadcast.");
}

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "WSAStartup failed\n";
        return 1;
    }

    int connectionType = 0;
    cout << "Choose connection method:\n";
    cout << "1 - Connect to localhost\n";
    cout << "2 - Discover server via broadcast\n";
    cout << "3 - Connect by hostname/IP and port\n";
    cout << "Enter choice (1-3): ";
    cin >> connectionType;
    cin.ignore(); 

    while (connectionType != 1 && connectionType != 2 && connectionType != 3) {
        cout << "Invalid choice. Enter 1, 2 or 3: ";
        cin >> connectionType;
        cin.ignore();
    }

    sockaddr_in serverAddr{};
    SOCKET sock = INVALID_SOCKET;
    bool connected = false;

    while (!connected) {
        try {
            if (connectionType == 1) {
                // localhost
                serverAddr.sin_family = AF_INET;
                serverAddr.sin_port = htons(2000);
                serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
            }
            else if (connectionType == 2) {
                // broadcast discovery
                serverAddr = DiscoverServer(2000, 2000, "DISCOVER_SERVER_RESPONSE");
            }
            else if (connectionType == 3) {
                // по hostname/IP
                string host;
                string portStr;
                cout << "Enter hostname or IP: ";
                getline(cin, host);
                cout << "Enter port (default 2000): ";
                getline(cin, portStr);

                if (portStr.empty()) portStr = "2000";

                struct addrinfo hints {}, * result = nullptr, * ptr = nullptr;
                hints.ai_family = AF_INET;
                hints.ai_socktype = SOCK_STREAM;
                hints.ai_protocol = IPPROTO_TCP;

                int iResult = getaddrinfo(host.c_str(), portStr.c_str(), &hints, &result);
                if (iResult != 0) {
                    throw runtime_error("getaddrinfo failed: " + to_string(iResult));
                }

                bool success = false;
                for (ptr = result; ptr != nullptr; ptr = ptr->ai_next) {
                    sock = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
                    if (sock == INVALID_SOCKET) continue;

                    if (connect(sock, ptr->ai_addr, (int)ptr->ai_addrlen) != SOCKET_ERROR) {
                        char ipStr[INET_ADDRSTRLEN];
                        sockaddr_in* addr_in = (sockaddr_in*)ptr->ai_addr;
                        inet_ntop(AF_INET, &(addr_in->sin_addr), ipStr, INET_ADDRSTRLEN);
                        cout << "Connected to " << host << " (" << ipStr << ":" << portStr << ")\n";
                        success = true;
                        break;
                    }
                    closesocket(sock);
                    sock = INVALID_SOCKET;
                }
                freeaddrinfo(result);

                if (!success) {
                    throw runtime_error("Failed to connect to any address");
                }
                connected = true;
                goto connection_success; 
            }

            if (sock == INVALID_SOCKET) {
                sock = socket(AF_INET, SOCK_STREAM, 0);
                if (sock == INVALID_SOCKET) {
                    throw runtime_error("Socket creation failed");
                }
            }

            if (connect(sock, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
                throw runtime_error("Connect failed: " + to_string(WSAGetLastError()));
            }

            cout << "Connected to server.\n";
            connected = true;
        }
        catch (const exception& ex) {
            cout << ex.what() << "\n";
            if (sock != INVALID_SOCKET) {
                closesocket(sock);
                sock = INVALID_SOCKET;
            }
            cout << "Retrying in 3 seconds...\n";
            Sleep(3000);
        }
    }

connection_success:

    cout << "Connected to server.\n\n";

    string command;
    cout << "Enter command (echo, time, or rand): ";
    getline(cin, command);

    if (command != "echo" && command != "rand" && command != "time") {
        cout << "Invalid command. Using 'echo' by default.\n";
        command = "echo";
    }

    int sent = send(sock, command.c_str(), (int)command.size(), 0);
    if (sent == SOCKET_ERROR) {
        cerr << "Failed to send initial command.\n";
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    char buffer[256];
    int received = recv(sock, buffer, sizeof(buffer) - 1, 0);
    if (received > 0) {
        buffer[received] = '\0';
        cout << "Server: " << buffer << "\n\n";
    }
    else if (received == 0) {
        cout << "Server closed connection.\n";
        closesocket(sock);
        WSACleanup();
        return 0;
    }
    else {
        cerr << "Receive failed: " << WSAGetLastError() << "\n";
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    while (true) {
        string msg;
        cout << "> ";
        getline(cin, msg);
        if (msg == "exit") break;

        sent = send(sock, msg.c_str(), (int)msg.size(), 0);
        if (sent == SOCKET_ERROR) {
            cerr << "Send failed.\n";
            break;
        }

        received = recv(sock, buffer, sizeof(buffer) - 1, 0);

        if (received > 0) {
            buffer[received] = '\0';
            cout << buffer << "\n";
        }
        

        else if (received == 0) {
            cout << "Server closed connection.\n";
            break;
        }
        else {
            cerr << "Receive failed.\n";
            break;
        }
    }

    closesocket(sock);
    WSACleanup();
    cout << "Disconnected.\n";
    return 0;
}
