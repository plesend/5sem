#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <winsock2.h>
#include <windows.h>
#pragma comment(lib, "WS2_32.lib")

int main(int argc, char* argv[])
{
    int count = 1000;
    printf("Enter connections count: ");
    scanf("%d", &count);
    const char* serverIP = "127.0.0.1";
    int serverPort = 2000;

    if (argc > 1) serverIP = argv[1];
    if (argc > 2) serverPort = atoi(argv[2]);

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        std::cout << "WSAStartup failed: " << WSAGetLastError() << std::endl;
        return 1;
    }

    for (int i = 0; i < count; ++i)
    {
        SOCKET s = socket(AF_INET, SOCK_STREAM, 0);
        if (s == INVALID_SOCKET)
        {
            std::cout << "Socket creation failed: " << WSAGetLastError() << std::endl;
            break;
        }

        SOCKADDR_IN serv;
        serv.sin_family = AF_INET;
        serv.sin_port = htons(serverPort);
        serv.sin_addr.s_addr = inet_addr(serverIP);

        int res = connect(s, (SOCKADDR*)&serv, sizeof(serv));
        if (res == SOCKET_ERROR)
        {
            std::cout << "Connect failed: " << WSAGetLastError() << std::endl;
            closesocket(s);
            Sleep(50);
            continue;
        }
        const char* msg = "echo";
        send(s, msg, strlen(msg), 0);

        closesocket(s);
        Sleep(50);
    }

    WSACleanup();
    return 0;
}
