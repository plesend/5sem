#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <Winsock2.h>
#pragma comment(lib, "WS2_32.lib")

char* SetErrorMsgText(const char* msgText, int code);
bool GetServer(char* call, short port, struct sockaddr* from, int* flen);
bool GetServerByName(char* name, char* call, struct sockaddr* from, int* flen);
int main()
{
    WSADATA wsdata;
    SOCKADDR_IN from;

    try
    {
        if (WSAStartup(MAKEWORD(2, 2), &wsdata) != 0) {
            throw SetErrorMsgText("WSAStartup:", WSAGetLastError());
        }
        char buf[256];
        char servname[256];
        int flen = sizeof(from);
        printf("Enter name: ");
        scanf("%s", buf);
        printf("Enter servername: ");
        scanf("%s", servname);
        //int result = GetServer(buf, 2000, (LPSOCKADDR)&from, &flen);
        //hostent* hostname = gethostbyaddr((char*)&from.sin_addr, sizeof(from.sin_addr), AF_INET);
        int result = GetServerByName(servname, buf, (LPSOCKADDR)&from, &flen);
        printf("Result: %d\n", result);
    }
    catch (const char* message)
    {
        printf("%s %s\n", "An error occured!", message);
    }

    WSACleanup();
    system("pause");
}

char* SetErrorMsgText(const char* msgText, int code)
{
    static char buffer[256];
    sprintf(buffer, "%s %d", msgText, code);
    return buffer;
}
bool GetServer(char* call, short port, struct sockaddr* from, int* flen)
{
    SOCKET cC;
    cC = socket(AF_INET, SOCK_DGRAM, 0);
    if (cC == INVALID_SOCKET)
    {
        throw SetErrorMsgText("socket:", WSAGetLastError());
    }
    int optval = 1;
    if (setsockopt(cC, SOL_SOCKET, SO_BROADCAST, (char*)&optval, sizeof(int)) == SOCKET_ERROR)
    {
        throw SetErrorMsgText("opt:", WSAGetLastError());
    }
    SOCKADDR_IN all;
    char buf[256];
    all.sin_family = AF_INET;
    all.sin_port = htons(2000);
    all.sin_addr.s_addr = inet_addr("10.123.229.255");
    DWORD timeout = 7000;
    setsockopt(cC, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));
    int send = sendto(cC, call, sizeof(call) - 1, 0, (LPSOCKADDR)&all, sizeof(all));
    if (send == SOCKET_ERROR)
    {
        throw SetErrorMsgText("sendto:", WSAGetLastError());
    }
    while(true)
    {
        int recv = recvfrom(cC, buf, sizeof(buf) - 1, 0, from, flen);
        if (recv == SOCKET_ERROR)
        {
            int error = WSAGetLastError();
            if (error == WSAETIMEDOUT)
            {
                closesocket(cC);
                return false;
            }
            throw SetErrorMsgText("GetRequestFromClient: ", error);
        }
        buf[recv] = '\0';

        if (strcmp(buf, call) == 0)
        {
            printf("[ Recieved request: %s ]\n", buf);
            closesocket(cC);
            return true;
        }
        else
        {
            closesocket(cC);
            throw SetErrorMsgText("Request refused", -1);
        }
    }
}
bool GetServerByName(char* name, char* call, struct sockaddr* from, int* flen)
{
    SOCKET cC;
    cC = socket(AF_INET, SOCK_DGRAM, 0);
    if (cC == INVALID_SOCKET)
        throw SetErrorMsgText("socket:", WSAGetLastError());

    DWORD timeout = 3000;
    if (setsockopt(cC, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout)) == SOCKET_ERROR)
        throw SetErrorMsgText("setsockopt (timeout):", WSAGetLastError());

    hostent* host = gethostbyname(name);
    if (!host)
    {
        closesocket(cC);
        throw SetErrorMsgText("gethostbyname:", WSAGetLastError());
    }

    SOCKADDR_IN serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(2000);
    memcpy(&serverAddr.sin_addr, host->h_addr, host->h_length);

    char buf[256];

    int sendLen = sendto(cC, call, strlen(call), 0, (LPSOCKADDR)&serverAddr, sizeof(serverAddr));
    if (sendLen == SOCKET_ERROR)
    {
        closesocket(cC);
        throw SetErrorMsgText("sendto:", WSAGetLastError());
    }

    while (true)
    {
        int recvLen = recvfrom(cC, buf, sizeof(buf) - 1, 0, from, flen);
        if (recvLen == SOCKET_ERROR)
        {
            int err = WSAGetLastError();
            if (err == WSAETIMEDOUT)
            {
                closesocket(cC);
                return false;
            }
            closesocket(cC);
            throw SetErrorMsgText("recvfrom:", err);
        }

        buf[recvLen] = '\0';

        if (strcmp(buf, call) == 0)
        {
            printf("[ Server %s responded: %s ]\n", name, buf);
            closesocket(cC);
            return true;
        }
        else
        {
            closesocket(cC);
            throw SetErrorMsgText("Unexpected response:", -1);
        }
    }
}
