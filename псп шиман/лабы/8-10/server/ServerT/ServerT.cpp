#pragma once
#include <iostream>
#include <string>
#include <sstream>
#include "Winsock2.h"
#include <ws2tcpip.h> 
#include <ctime>
#pragma comment(lib, "WS2_32.lib")
using namespace std;
string SetErrorMsgText(const string& msgText, int code);

int main()
{
    WSADATA wsaData;

    try {
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            throw SetErrorMsgText("WSAStartup", WSAGetLastError());
        }

        SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, NULL);
        if (serverSocket == INVALID_SOCKET) {
            throw SetErrorMsgText("socket:", WSAGetLastError());
        }

        SOCKADDR_IN serv;
        serv.sin_family = AF_INET;
        serv.sin_port = htons(2000);
        serv.sin_addr.s_addr = INADDR_ANY;

        if (bind(serverSocket, (LPSOCKADDR)&serv, sizeof(serv)) == SOCKET_ERROR)
            throw SetErrorMsgText("bind:", WSAGetLastError());

        if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR)
            throw SetErrorMsgText("listen:", WSAGetLastError());

        cout << "Server is listening on port 2000..." << endl;

        SOCKADDR_IN clientAddr;
        int clientAddrSize = sizeof(clientAddr);

        while (true) {
            SOCKET clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientAddrSize);
            if (clientSocket == INVALID_SOCKET) {
                throw SetErrorMsgText("accept:", WSAGetLastError());
            }

            char clientIP[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &(clientAddr.sin_addr), clientIP, INET_ADDRSTRLEN);
            int port = ntohs(clientAddr.sin_port);
            cout << "Client connected! IP: " << clientIP << " Port: " << port << endl;

            char buffer[1024];
            int messageCount = 0;
            clock_t startTime = clock();

            while (true) {
                int receivedBytes = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
                if (receivedBytes == SOCKET_ERROR) {
                    throw SetErrorMsgText("recv:", WSAGetLastError());
                }

                if (receivedBytes == 0) {
                    cout << "Received zero-length message. Closing connection." << endl;
                    break;
                }

                buffer[receivedBytes] = '\0';
                messageCount++;

                int bytesSent = send(clientSocket, buffer, receivedBytes, 0);
                if (bytesSent == SOCKET_ERROR) {
                    throw SetErrorMsgText("send:", WSAGetLastError());
                }

                cout << "Echoed message #" << messageCount << ": " << buffer << endl;
            }

            clock_t endTime = clock();
            double totalTime = (double)(endTime - startTime) / CLOCKS_PER_SEC;

            cout << "Total messages processed: " << messageCount << endl;
            cout << "Total time: " << totalTime << " seconds" << endl;

            if (closesocket(clientSocket) == SOCKET_ERROR)
                throw SetErrorMsgText("closesocket (client):", WSAGetLastError());

        }
        if (closesocket(serverSocket) == SOCKET_ERROR)
            throw SetErrorMsgText("closesocket (server):", WSAGetLastError());

        if (WSACleanup() == SOCKET_ERROR)
            throw SetErrorMsgText("WSACleanup", WSAGetLastError());

        cout << "Server finished successfully!" << endl;
    }
    catch (const string& errorMsg) {
        cout << "Error: " << errorMsg << endl;
        WSACleanup();
    }

    return 0;
}

string SetErrorMsgText(const string& msgText, int code) {
    stringstream ss;
    ss << msgText << " Error code: " << code;
    return ss.str();
}