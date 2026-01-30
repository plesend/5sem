#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <ws2tcpip.h>
#include <tchar.h>
#include "ErrorHandler.h"
#pragma comment(lib,"WS2_32.lib")
using namespace std;

SOCKET sS;
SOCKET cS;

bool GetServer(char* call, short port, sockaddr* from, int* flen)
{
    SOCKET cS;
    if ((cS = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
    {
        throw SetErrorMsgText("Socket creation error: ", WSAGetLastError());
    }

    int optval = 1;
    if (setsockopt(cS, SOL_SOCKET, SO_BROADCAST, (char*)&optval, sizeof(int)) == SOCKET_ERROR)
    {
        throw SetErrorMsgText("setsockopt (broadcast) error: ", WSAGetLastError());
    }

    DWORD timeout = 1000;
    if (setsockopt(cS, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(int)) == SOCKET_ERROR)
    {
        throw SetErrorMsgText("setsockopt (timeout) error: ", WSAGetLastError());
    }

    SOCKADDR_IN all;
    all.sin_family = AF_INET;
    all.sin_port = htons(port);
    all.sin_addr.s_addr = INADDR_BROADCAST;
    memcpy(from, &all, sizeof(all));

    int err = sendto(cS, call, strlen(call) + 1, 0, (sockaddr*)&all, sizeof(all));
    if (err == SOCKET_ERROR)
    {
        throw SetErrorMsgText("sendto error: ", WSAGetLastError());
    }

    int count = 1;
    while (true)
    {
        char buffer[100];
        sockaddr_in sender;
        int senderLen = sizeof(sender);

        int recvBytes = recvfrom(cS, buffer, sizeof(buffer), 0, (sockaddr*)&sender, &senderLen);
        if (recvBytes == SOCKET_ERROR)
        {
            int errCode = WSAGetLastError();
            if (errCode == WSAETIMEDOUT) break;
            throw SetErrorMsgText("recvfrom error: ", errCode);
        }

        count++;
        cout << "Servers in network " << count << ": " << buffer
            << " (IP: " << inet_ntoa(sender.sin_addr)
            << ", port: " << ntohs(sender.sin_port) << ")" << endl;
    }

    cout << "Total responses received: " << count << endl;
    closesocket(cS);

    return count > 0;
}

bool GetRequestFromClient(char* name, short port, sockaddr* from, int* flen)
{
    char bfrom[50];
    int err = 0;
    while (true)
    {
        if ((err = recvfrom(sS, bfrom, sizeof(bfrom), NULL, from, flen)) == SOCKET_ERROR)
        {
            throw SetErrorMsgText("recvfrom error: ", WSAGetLastError());
        }
        if (strcmp(name, bfrom) == 0)
        {
            return true;
        }
    }
}

bool PutAnswerToClient(char* name, sockaddr* to, int* lto)
{
    int err = 0;
    if ((err = sendto(sS, name, strlen(name) + 1, NULL, to, *lto)) == SOCKET_ERROR)
    {
        throw SetErrorMsgText("sendto error: ", WSAGetLastError());
        return false;
    }
    return true;
}

void main()
{
    SetConsoleCP(1251);
    setlocale(LC_ALL, "ru");

    try
    {
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0)
            throw SetErrorMsgText("WSAStartup error: ", WSAGetLastError());

        char hostName[256];
        if (gethostname(hostName, sizeof(hostName)) == SOCKET_ERROR)
            throw SetErrorMsgText("gethostname error: ", WSAGetLastError());

        cout << "Current host name: " << hostName << endl;

        SOCKADDR_IN all;
        int allLen = sizeof(all);

        char name[10] = "Hello";
        GetServer(name, 2000, (sockaddr*)&all, &allLen);

        if ((sS = socket(AF_INET, SOCK_DGRAM, NULL)) == INVALID_SOCKET)
            throw SetErrorMsgText("Socket creation error: ", WSAGetLastError());

        SOCKADDR_IN serv;
        serv.sin_family = AF_INET;
        serv.sin_port = htons(2000);
        serv.sin_addr.s_addr = INADDR_ANY;

        BOOL optval2 = TRUE;
        setsockopt(sS, SOL_SOCKET, SO_REUSEADDR, (char*)&optval2, sizeof(optval2));

        if (bind(sS, (LPSOCKADDR)&serv, sizeof(serv)) == SOCKET_ERROR)
            throw SetErrorMsgText("bind error: ", WSAGetLastError());

        SOCKADDR_IN from;
        memset(&from, 0, sizeof(from));
        int lbfrom = sizeof(from);

        while (true)
        {
            if (!GetRequestFromClient(name, 2000, (sockaddr*)&from, &lbfrom))
                throw SetErrorMsgText("GetRequestFromClient error: ", WSAGetLastError());

            cout << "\n========= Client request received =========" << endl;
            cout << "IP: " << inet_ntoa(from.sin_addr)
                << "\tport: " << htons(from.sin_port) << endl;

            lbfrom = sizeof(from);

            if (!PutAnswerToClient(name, (sockaddr*)&from, &lbfrom))
                throw SetErrorMsgText("PutAnswerToClient error: ", WSAGetLastError());

            struct in_addr ip_addr = from.sin_addr;
            struct hostent* addr = gethostbyaddr((const char*)&ip_addr, sizeof(ip_addr), AF_INET);

            if (addr != NULL) {
                cout << "Client name: " << addr->h_name << endl;
            }
        }

        if (closesocket(sS) == SOCKET_ERROR)
            throw SetErrorMsgText("closesocket error: ", WSAGetLastError());
        if (WSACleanup() == SOCKET_ERROR)
            throw SetErrorMsgText("WSACleanup error: ", WSAGetLastError());
    }
    catch (string errorMsgText) {
        cout << endl << "Error: " << errorMsgText;
    }

}
