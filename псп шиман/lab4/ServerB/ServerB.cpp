#pragma warning(disable:4996) 
#include "WinSock2.h"
#include "WS2tcpip.h"
#pragma comment(lib, "WS2_32.lib")
#include <iostream>
#include <cstring>
#include <tchar.h>
#include <string>
#include <chrono>
#include <set>
using namespace std;

string GetErrorMsgText(int code)
{
    string msgText;
    switch (code)
    {
    case WSAEINTR: msgText = "WSAEINTR"; break;
    case WSAEACCES:          msgText = "WSAEACCES"; break;
    case WSAEFAULT:          msgText = "WSAEFAULT"; break;
    case WSAEINVAL:          msgText = "WSAEINVAL"; break;
    case WSAEMFILE:          msgText = "WSAEMFILE"; break;
    case WSAEWOULDBLOCK:     msgText = "WSAEWOULDBLOCK"; break;
    case WSAEINPROGRESS:     msgText = "WSAEINPROGRESS"; break;
    case WSAEALREADY:        msgText = "WSAEALREADY"; break;
    case WSAENOTSOCK:        msgText = "WSAENOTSOCK"; break;
    case WSAEDESTADDRREQ:    msgText = "WSAEDESTADDRREQ"; break;
    case WSAEMSGSIZE:        msgText = "WSAEMSGSIZE"; break;
    case WSAEPROTOTYPE:      msgText = "WSAEPROTOTYPE"; break;
    case WSAENOPROTOOPT:     msgText = "WSAENOPROTOOPT"; break;
    case WSAEPROTONOSUPPORT: msgText = "WSAEPROTONOSUPPORT"; break;
    case WSAESOCKTNOSUPPORT: msgText = "WSAESOCKTNOSUPPORT"; break;
    case WSAEOPNOTSUPP:      msgText = "WSAEOPNOTSUPP"; break;
    case WSAEPFNOSUPPORT:    msgText = "WSAEPFNOSUPPORT"; break;
    case WSAEAFNOSUPPORT:    msgText = "WSAEAFNOSUPPORT"; break;
    case WSAEADDRINUSE:      msgText = "WSAEADDRINUSE"; break;
    case WSAEADDRNOTAVAIL:   msgText = "WSAEADDRNOTAVAIL"; break;
    case WSAENETDOWN:        msgText = "WSAENETDOWN"; break;
    case WSAENETUNREACH:     msgText = "WSAENETUNREACH"; break;
    case WSAENETRESET:       msgText = "WSAENETRESET"; break;
    case WSAECONNABORTED:    msgText = "WSAECONNABORTED"; break;
    case WSAECONNRESET:      msgText = "WSAECONNRESET"; break;
    case WSAENOBUFS:         msgText = "WSAENOBUFS"; break;
    case WSAEISCONN:         msgText = "WSAEISCONN"; break;
    case WSAENOTCONN:        msgText = "WSAENOTCONN"; break;
    case WSAESHUTDOWN:       msgText = "WSAESHUTDOWN"; break;
    case WSAETIMEDOUT:       msgText = "WSAETIMEDOUT"; break;
    case WSAECONNREFUSED:    msgText = "WSAECONNREFUSED"; break;
    case WSAEHOSTDOWN:       msgText = "WSAEHOSTDOWN"; break;
    case WSAEHOSTUNREACH:    msgText = "WSAEHOSTUNREACH"; break;
    case WSAEPROCLIM:        msgText = "WSAEPROCLIM"; break;
    case WSASYSNOTREADY:          msgText = "WSASYSNOTREADY"; break;
    case WSAVERNOTSUPPORTED:      msgText = "WSAVERNOTSUPPORTED"; break;
    case WSANOTINITIALISED:       msgText = "WSANOTINITIALISED"; break;
    case WSAEDISCON:              msgText = "WSAEDISCON"; break;
    case WSATYPE_NOT_FOUND:       msgText = "WSATYPE_NOT_FOUND"; break;
    case WSAHOST_NOT_FOUND:       msgText = "WSAHOST_NOT_FOUND"; break;
    case WSATRY_AGAIN:            msgText = "WSATRY_AGAIN"; break;
    case WSANO_RECOVERY:          msgText = "WSANO_RECOVERY"; break;
    case WSANO_DATA:              msgText = "WSANO_DATA"; break;
    case WSA_INVALID_HANDLE:      msgText = "WSA_INVALID_HANDLE"; break;
    case WSA_INVALID_PARAMETER:   msgText = "WSA_INVALID_PARAMETER"; break;
    case WSA_IO_INCOMPLETE:       msgText = "WSA_IO_INCOMPLETE"; break;
    case WSA_IO_PENDING:          msgText = "WSA_IO_PENDING"; break;
    case WSA_NOT_ENOUGH_MEMORY:   msgText = "WSA_NOT_ENOUGH_MEMORY"; break;
    case WSA_OPERATION_ABORTED:   msgText = "WSA_OPERATION_ABORTED"; break;
    case WSASYSCALLFAILURE:       msgText = "WSASYSCALLFAILURE"; break;
    }
    return msgText;
}

string SetErrorMsgText(string msgText, int errorCode)
{
    return msgText + GetErrorMsgText(errorCode);
};

bool GetRequestFromClient(SOCKET* sock, char* uname, struct sockaddr* from, int* flen, const set<string>& ignoreIPs)
{
    while (true)
    {
        char inputBuffer[50];
        int inputLength;

        if ((inputLength = recvfrom(*sock, inputBuffer, sizeof(inputBuffer) - 1, 0, from, flen)) == SOCKET_ERROR)
        {
            if (WSAGetLastError() == WSAETIMEDOUT)
            {
                return false;
            }
            else throw SetErrorMsgText("getRequestFromClient recvfrom: ", WSAGetLastError());
        }

        inputBuffer[inputLength] = '\0';

        char fromIP[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &((sockaddr_in*)from)->sin_addr, fromIP, INET_ADDRSTRLEN);

        if (ignoreIPs.find(string(fromIP)) != ignoreIPs.end()) {
            continue;
        }

        cout << "GetRequestFromClient from " << fromIP << ": " << inputBuffer << endl;
        if (strcmp(uname, inputBuffer) == 0)
            return true;
    }
}

bool PutAnswerToClient(SOCKET* sock, char* uname, struct sockaddr* to, int* lto)
{
    int lengthOutput;
    char outputBuffer[50] = "hello";

    if ((lengthOutput = sendto(*sock, outputBuffer, strlen(outputBuffer), 0, to, *lto)) == SOCKET_ERROR)
        throw SetErrorMsgText("PutAnswerToClient sendto: ", WSAGetLastError());

    cout << "Sent client: " << outputBuffer << endl;
    return true;
}

set<string> GetLocalIPs() {
    set<string> localIPs;
    char hostname[256];

    if (gethostname(hostname, sizeof(hostname)) == 0) {
        struct hostent* host = gethostbyname(hostname);
        if (host != NULL) {
            for (int i = 0; host->h_addr_list[i] != NULL; i++) {
                char ip[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, host->h_addr_list[i], ip, INET_ADDRSTRLEN);
                localIPs.insert(string(ip));
                cout << "local ip: " << ip << endl;
            }
        }
    }
    return localIPs;
}

int main()
{
    SOCKET sS;
    WSADATA wsadata;
    set<string> duplicateServers;
    set<string> localIPs;

    try
    {
        if (WSAStartup(MAKEWORD(2, 0), &wsadata) != 0)
            throw SetErrorMsgText("Startup: ", WSAGetLastError());
        if ((sS = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == INVALID_SOCKET)
            throw SetErrorMsgText("socket: ", WSAGetLastError());

        sockaddr_in servSettings;
        servSettings.sin_family = AF_INET;
        servSettings.sin_port = htons(2000);
        servSettings.sin_addr.S_un.S_addr = INADDR_ANY;


        if (bind(sS, (sockaddr*)&servSettings, sizeof(servSettings)) == SOCKET_ERROR)
            throw SetErrorMsgText("bind: ", WSAGetLastError());

        char callsign[50] = "hello";

        SOCKADDR_IN client;
        memset(&client, 0, sizeof(client));
        int lClient = sizeof(client);
        int optval = 1;

        if (setsockopt(sS, SOL_SOCKET, SO_BROADCAST, (char*)&optval, sizeof(int)) == SOCKET_ERROR)
            throw SetErrorMsgText("setsockopt: ", WSAGetLastError());

        localIPs = GetLocalIPs();

        SOCKADDR_IN all;
        all.sin_family = AF_INET;
        all.sin_port = htons(2000);
        all.sin_addr.S_un.S_addr = inet_addr("10.118.203.255");

        sendto(sS, "hello", strlen("hello"), 0, (sockaddr*)&all, sizeof(all));

        timeval timeWaiting{};
        timeWaiting.tv_sec = 5;
        timeWaiting.tv_usec = 0;
        setsockopt(sS, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeWaiting, sizeof(timeWaiting));

        char inputBuffer[50];
        int inputLength;
        SOCKADDR_IN duplicateServer;
        int duplicateSize = sizeof(duplicateServer);


        while (true)
        {
            if ((inputLength = recvfrom(sS, inputBuffer, sizeof(inputBuffer) - 1, 0,
                (sockaddr*)&duplicateServer, &duplicateSize)) == SOCKET_ERROR)
            {
                if (WSAGetLastError() == WSAETIMEDOUT) {
                    cout << "timeout reached" << endl;
                    break;
                }
                else throw SetErrorMsgText("duplicate recvfrom: ", WSAGetLastError());
            }

            inputBuffer[inputLength] = '\0';

            char duplicateIP[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &duplicateServer.sin_addr, duplicateIP, sizeof(duplicateIP));

            cout << "Received from IP: " << duplicateIP << " message: " << inputBuffer << " (length: " << inputLength << ")" << endl;

            if (strcmp(inputBuffer, callsign) == 0)
            {
                if (localIPs.find(string(duplicateIP)) == localIPs.end()) {
                    cout << "duplicate server found" << duplicateIP << endl;
                    duplicateServers.insert(string(duplicateIP));
                }
            }
        }

        cout << "Found " << duplicateServers.size() << " duplicate server(s)" << endl;

        timeWaiting.tv_sec = 0;  
        timeWaiting.tv_usec = 0;
        setsockopt(sS, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeWaiting, sizeof(timeWaiting));


        while (true)
        {
            if (GetRequestFromClient(&sS, (char*)"hello", (sockaddr*)&client, &lClient, duplicateServers))
            {
                char clientIP[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &client.sin_addr, clientIP, INET_ADDRSTRLEN);
                PutAnswerToClient(&sS, (char*)"hello", (sockaddr*)&client, &lClient);
            }
        }

        if (closesocket(sS) == SOCKET_ERROR)
            throw SetErrorMsgText("closesocket: ", WSAGetLastError());

        if (WSACleanup() == SOCKET_ERROR)
            throw SetErrorMsgText("Cleanup: ", WSAGetLastError());
    }
    catch (string msg)
    {
        cout << "\n" << msg << endl;
    }
    return 0;
}