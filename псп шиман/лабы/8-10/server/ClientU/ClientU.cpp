#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <Winsock2.h>
#pragma comment(lib, "WS2_32.lib")

char* SetErrorMsgText(const char* msgText, int code);

int main()
{
    WSADATA wsdata;
    SOCKET cC;
    SOCKADDR_IN from;
    SOCKADDR_IN to;
    int messages = 1000;
    const char* message = "Hello from ClientU";
    try
    {
        if (WSAStartup(MAKEWORD(2, 2), &wsdata) != 0) {
            throw SetErrorMsgText("WSAStartup:", WSAGetLastError());
        }
        if ((cC = socket(AF_INET, SOCK_DGRAM, NULL)) == INVALID_SOCKET)
        {
            throw SetErrorMsgText("socket:", WSAGetLastError());
        }
        to.sin_family = AF_INET;
        to.sin_port = htons(2000);
        to.sin_addr.s_addr = inet_addr("172.25.1.5");
        char bto[256];
        char bfrom[256];
        int len = sizeof(to);
        int lenfrom = sizeof(from);
        for (int i = 0; i < messages; i++)
        {
            sprintf(bto, "%s %d", message, i);
            int send = sendto(cC, bto, sizeof(bto) -1, NULL, (LPSOCKADDR)&to, len);
            if (send == SOCKET_ERROR)
            {
                throw SetErrorMsgText("send:", WSAGetLastError());
            }
            int recv = recvfrom(cC, bfrom, sizeof(bfrom) - 1, NULL, (LPSOCKADDR)&from, &lenfrom);
            if (recv == SOCKET_ERROR)
            {
                throw SetErrorMsgText("recvfrom: ", WSAGetLastError());
            }
            printf("%s: %s\n", "Package recieved", bfrom);
            Sleep(10);
        }
    }
    catch(const char* message)
    {
        printf("%s %s\n", "An error occured!", message);
        closesocket(cC);
        WSACleanup();
    }
    
    closesocket(cC);
    WSACleanup();
    system("pause");
}

char* SetErrorMsgText(const char* msgText, int code)
{
    char buffer[256];
    sprintf(buffer, "%s %d", msgText, code);
    return buffer;
}