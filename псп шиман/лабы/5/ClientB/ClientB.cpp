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

    int optval = 1;
    setsockopt(clientSocket, SOL_SOCKET, SO_BROADCAST, (char*)&optval, sizeof(int));

    int timeout = 1000;
    setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));

    SOCKADDR_IN all;
    all.sin_family = AF_INET;
    all.sin_port = htons(port);
    all.sin_addr.s_addr = INADDR_BROADCAST;

    if (sendto(clientSocket, call, strlen(call) + 1, NULL, (sockaddr*)&all, sizeof(all)) == SOCKET_ERROR)
        throw SetErrorMsgText("sendto: ", WSAGetLastError());

    cout << "Waiting for responses..." << endl;

    while (true)
    {
        char nameServer[50];
        int pntr = recvfrom(clientSocket, nameServer, sizeof(nameServer), NULL, from, flen);

        if (pntr == SOCKET_ERROR)
        {
            if (WSAGetLastError() == WSAETIMEDOUT)
                break;
            else
                throw SetErrorMsgText("recvfrom: ", WSAGetLastError());
        }

        nameServer[pntr] = 0;

        SOCKADDR_IN* addr = (SOCKADDR_IN*)from;
        cout << "Response from server: " << nameServer << endl;
        cout << "Server IP: " << inet_ntoa(addr->sin_addr) << endl;
        cout << "Server port: " << htons(addr->sin_port) << endl;
    }

    return true;
}

bool GetServerByName(const char* name, const char* call, sockaddr* from, int* flen)
{
    if (clientSocket == INVALID_SOCKET)
        throw SetErrorMsgText("GetServerByName: clientSocket not initialized", WSAGetLastError());

    hostent* host = gethostbyname(name);

    if (!host)
        throw SetErrorMsgText("gethostbyname: ", WSAGetLastError());

    char* ip_addr = inet_ntoa(*(in_addr*)(host->h_addr));

    cout << "\nServer name: " << host->h_name
        << "\nServer IP address: " << ip_addr << endl;

    SOCKADDR_IN server;
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(2000);
    server.sin_addr.s_addr = inet_addr(ip_addr);

    if (sendto(clientSocket, call, strlen(call) + 1, 0,
        (sockaddr*)&server, sizeof(server)) == SOCKET_ERROR)
    {
        throw SetErrorMsgText("sendto: ", WSAGetLastError());
    }

    char message[50];
    int msg_len = recvfrom(clientSocket,
        message, sizeof(message), 0,
        (sockaddr*)&server, flen);

    if (msg_len == SOCKET_ERROR)
        throw SetErrorMsgText("recvfrom: ", WSAGetLastError());

    message[msg_len] = 0;

    cout << "Response from server: " << message << endl;

    memcpy(from, &server, sizeof(server));

    return true;
}

int main()
{
    SetConsoleCP(1251);
    setlocale(LC_ALL, "en");

    try {

        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0)
            throw SetErrorMsgText("WSAStartup: ", WSAGetLastError());

        if ((clientSocket = socket(AF_INET, SOCK_DGRAM, NULL)) == INVALID_SOCKET)
            throw SetErrorMsgText("socket: ", WSAGetLastError());

        char call[] = "Hello";
        SOCKADDR_IN clnt;
        int lc = sizeof(clnt);

        // 4
        GetServer(call, 2000, (sockaddr*)&clnt, &lc);

        // 5
        string server_name;
        cout << "Enter server name: ";
        cin >> server_name;

        GetServerByName(server_name.c_str(), call, (sockaddr*)&clnt, &lc);

        if (closesocket(clientSocket) == SOCKET_ERROR)
            throw SetErrorMsgText("closesocket: ", WSAGetLastError());

        if (WSACleanup() == SOCKET_ERROR)
            throw SetErrorMsgText("WSACleanup: ", WSAGetLastError());
    }
    catch (string errorMsgText) {
        cout << endl << "Error: " << errorMsgText;
    }
}
