#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <WinSock2.h>
#pragma comment(lib, "WS2_32.lib")

char* SetErrorMsgText(const char* msgText, int code);
bool GetRequestFromClient(char* name, short port, struct sockaddr* from, int* flen);
bool PutAnswerToClient(char* name, struct sockaddr*, int* lto);
bool CheckForOtherServers(char* name, short port);

int main()
{
    char name[] = "DISCOVER_SERVER_RESPONSE"; 
    char hostname[256];
    WSADATA wsdata;
    SOCKADDR_IN from;
    int flen = sizeof(from);

    try
    {
        if (WSAStartup(MAKEWORD(2, 2), &wsdata) != 0)
            throw SetErrorMsgText("WSAStartup", WSAGetLastError());
        if (gethostname(hostname, sizeof(hostname)) == SOCKET_ERROR)
        {
            throw SetErrorMsgText("gethostname", WSAGetLastError());
        }
        printf("Server hostname: %s\n", hostname);
        if (CheckForOtherServers(name, 2000))
        {
            printf("[ WARNING ] Another server with the same call sign detected!\n\n");
        }

        while (true)
        {
            if (GetRequestFromClient(name, 2000, (LPSOCKADDR)&from, &flen))
            {
                auto remoteHost = gethostbyaddr((char*)&from.sin_addr, sizeof(from.sin_addr), AF_INET);

                printf("Client connected: Hostname = %s, IP = %s, Port = %d\n",
                       remoteHost->h_name, inet_ntoa(from.sin_addr), ntohs(from.sin_port));

                PutAnswerToClient(name, (LPSOCKADDR)&from, &flen);
            }
        }
    }
    catch (const char* message)
    {
        printf("%s\n", message);
    }

    WSACleanup();
    return 0;
}

char* SetErrorMsgText(const char* msgText, int code)
{
    static char buffer[256]; 
    sprintf(buffer, "%s %d", msgText, code);
    return buffer;
}

bool GetRequestFromClient(char* name, short port, struct sockaddr* from, int* flen)
{
    SOCKET sS = socket(AF_INET, SOCK_DGRAM, 0);
    if (sS == INVALID_SOCKET)
        throw SetErrorMsgText("socket:", WSAGetLastError());

    SOCKADDR_IN serv;
    serv.sin_family = AF_INET;
    serv.sin_port = htons(port);
    serv.sin_addr.s_addr = INADDR_ANY;

    if (bind(sS, (LPSOCKADDR)&serv, sizeof(serv)) == SOCKET_ERROR)
        throw SetErrorMsgText("bind:", WSAGetLastError());

    u_long mode = 1;
    if (ioctlsocket(sS, FIONBIO, &mode) == SOCKET_ERROR)
        throw SetErrorMsgText("ioctlsocket:", WSAGetLastError());

    char msgfrom[256];
    SOCKADDR_IN temp;
    int recvSize;

    DWORD start = GetTickCount(); 
    while (true)
    {
        recvSize = recvfrom(sS, msgfrom, sizeof(msgfrom) - 1, 0, (LPSOCKADDR)&temp, flen);
        if (recvSize == SOCKET_ERROR)
        {
            int err = WSAGetLastError();
            if (err == WSAEWOULDBLOCK)
            {
                if (GetTickCount() - start > 5000)
                {
                    closesocket(sS);
                    return false;
                }
                Sleep(100);
                continue;
            }
            else
            {
                closesocket(sS);
                throw SetErrorMsgText("recvfrom:", err);
            }
        }
        else
        {
            msgfrom[recvSize] = '\0';
            if (strcmp(msgfrom, name) == 0)
            {
                memcpy(from, &temp, sizeof(SOCKADDR_IN));
                closesocket(sS);
                return true;
            }
        }
    }
}


bool PutAnswerToClient(char* name, struct sockaddr* to, int* lto)
{
    SOCKET scS = socket(AF_INET, SOCK_DGRAM, 0);
    if (scS == INVALID_SOCKET)
        throw SetErrorMsgText("socket:", WSAGetLastError());

    u_long mode = 1;
    if (ioctlsocket(scS, FIONBIO, &mode) == SOCKET_ERROR)
        throw SetErrorMsgText("ioctlsocket:", WSAGetLastError());

    int sendSize;
    DWORD start = GetTickCount();

    while (true)
    {
        sendSize = sendto(scS, name, (int)strlen(name), 0, to, *lto);
        if (sendSize == SOCKET_ERROR)
        {
            int err = WSAGetLastError();
            if (err == WSAEWOULDBLOCK)
            {
                if (GetTickCount() - start > 3000) 
                {
                    closesocket(scS);
                    return false;
                }
                Sleep(100);
                continue;
            }
            else
            {
                closesocket(scS);
                throw SetErrorMsgText("sendto:", err);
            }
        }
        else
        {
            closesocket(scS);
            return true;
        }
    }
}


bool CheckForOtherServers(char* name, short port)
{
    SOCKET chS = socket(AF_INET, SOCK_DGRAM, 0);
    if (chS == INVALID_SOCKET)
        throw SetErrorMsgText("socket:", WSAGetLastError());

    int optval = 1;
    DWORD timeout = 3000;
    setsockopt(chS, SOL_SOCKET, SO_BROADCAST, (char*)&optval, sizeof(optval));
    setsockopt(chS, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));

    SOCKADDR_IN all;
    all.sin_family = AF_INET;
    all.sin_port = htons(port);
    all.sin_addr.s_addr = inet_addr("10.123.229.255"); 

    sendto(chS, name, (int)strlen(name), 0, (LPSOCKADDR)&all, sizeof(all));

    char buf[256];
    SOCKADDR_IN from;
    int flen = sizeof(from);
    bool found = false;

    while (true)
    {
        int recv = recvfrom(chS, buf, sizeof(buf) - 1, 0, (LPSOCKADDR)&from, &flen);
        if (recv == SOCKET_ERROR)
            break;

        buf[recv] = '\0';
        if (strcmp(buf, name) == 0)
        {
            printf("[ Another server detected! ] IP = %s, Port = %d\n",
                   inet_ntoa(from.sin_addr), ntohs(from.sin_port));
            found = true;
        }
    }

    closesocket(chS);
    return found;
}