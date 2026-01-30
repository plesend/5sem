#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include "Winsock2.h"
#pragma comment(lib, "WS2_32.lib")

char* SetErrorMsgText(const char* msgText, int code);

int main()
{
    WSAData wsdata;
    SOCKET sS;
    SOCKADDR_IN serv;
    SOCKADDR_IN to;
    try 
    {
        if (WSAStartup(MAKEWORD(2, 2), &wsdata) != 0) {
            throw SetErrorMsgText("WSAStartup", WSAGetLastError());
        }
        sS = socket(AF_INET, SOCK_DGRAM, NULL);
        if (sS == INVALID_SOCKET)
        {
            throw SetErrorMsgText("socket:", WSAGetLastError());
        }
        serv.sin_family = AF_INET;
        serv.sin_port = htons(2000);
        serv.sin_addr.s_addr = INADDR_ANY;
        if (bind(sS, LPSOCKADDR(&serv), sizeof(serv)) == SOCKET_ERROR)
        {
            throw SetErrorMsgText("bind:", WSAGetLastError());
        }
        while (true) {
            char bfrom[256];
            int len = sizeof(serv);
            int lento = sizeof(to);
            int recv = recvfrom(sS, bfrom, sizeof(bfrom) - 1, NULL, (LPSOCKADDR)&to, &len);
            if (recv == SOCKET_ERROR)
            {
                throw SetErrorMsgText("recvfrom: ", WSAGetLastError());
            }
            printf("%s: %s\n", "Package recieved", bfrom);
            sendto(sS, bfrom, sizeof(bfrom) - 1, NULL, (LPSOCKADDR)&to, lento);
        }
    }
    catch (const char* message)
    {
        printf("%s %s\n", "An error occured!", message);
        closesocket(sS);
        WSACleanup();
    }
    closesocket(sS);
    WSACleanup();
    system("pause");
}

char* SetErrorMsgText(const char* msgText, int code)
{
    char buffer[256];
    sprintf(buffer, "%s %d", msgText, code);
    return buffer;
}