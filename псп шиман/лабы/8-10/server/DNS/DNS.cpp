#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <windows.h>
#include <iostream>
#include <stdio.h>
#pragma comment(lib, "WS2_32.lib")

char* SetErrorMsgText(const char* msgText, int code);

bool GetServerByBroadcast(short port, struct sockaddr* from, int* flen);
bool GetServerByName(const char* dnsName, short port, struct sockaddr* from, int* flen);

bool GetServerByBroadcast(short port, struct sockaddr* from, int* flen)
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
        closesocket(cC);
        throw SetErrorMsgText("setsockopt:", WSAGetLastError());
    }

    SOCKADDR_IN all;
    char buf[256];
    all.sin_family = AF_INET;
    all.sin_port = htons(port);
    all.sin_addr.s_addr = inet_addr("10.72.32.255");

    const char* REQUEST_TEXT = "DISCOVER_SERVER_REQUEST";
    const char* RESPONSE_TEXT = "DISCOVER_SERVER_RESPONSE";

    DWORD timeout = 5000;
    setsockopt(cC, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));

    int sendLen = sendto(cC, REQUEST_TEXT, (int)strlen(REQUEST_TEXT), 0, (LPSOCKADDR)&all, sizeof(all));
    if (sendLen == SOCKET_ERROR)
    {
        closesocket(cC);
        throw SetErrorMsgText("sendto:", WSAGetLastError());
    }

    printf("[DNS] Broadcast request sent: %s\n", REQUEST_TEXT);

    while (true)
    {
        int recvLen = recvfrom(cC, buf, sizeof(buf) - 1, 0, from, flen);
        if (recvLen == SOCKET_ERROR)
        {
            int error = WSAGetLastError();
            if (error == WSAETIMEDOUT)
            {
                printf("[DNS] Timeout waiting for server response\n");
                closesocket(cC);
                return false;
            }
            closesocket(cC);
            throw SetErrorMsgText("recvfrom:", error);
        }

        buf[recvLen] = '\0';

        if (strcmp(buf, RESPONSE_TEXT) == 0)
        {
            SOCKADDR_IN* serverAddr = (SOCKADDR_IN*)from;
            printf("[DNS] Server found! Address: %s, Port: %d\n", 
                inet_ntoa(serverAddr->sin_addr), ntohs(serverAddr->sin_port));
            closesocket(cC);
            return true;
        }
    }
}

bool GetServerByName(const char* dnsName, short port, struct sockaddr* from, int* flen)
{
    SOCKET cC;
    cC = socket(AF_INET, SOCK_DGRAM, 0);
    if (cC == INVALID_SOCKET)
    {
        throw SetErrorMsgText("socket:", WSAGetLastError());
    }

    DWORD timeout = 5000;
    if (setsockopt(cC, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout)) == SOCKET_ERROR)
    {
        closesocket(cC);
        throw SetErrorMsgText("setsockopt (timeout):", WSAGetLastError());
    }

    // Resolve DNS name to IP address
    hostent* host = gethostbyname(dnsName);
    if (!host)
    {
        closesocket(cC);
        throw SetErrorMsgText("gethostbyname:", WSAGetLastError());
    }

    SOCKADDR_IN serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    memcpy(&serverAddr.sin_addr, host->h_addr, host->h_length);

    printf("[DNS] Resolved DNS name '%s' to IP: %s\n", dnsName, inet_ntoa(serverAddr.sin_addr));

    const char* REQUEST_TEXT = "DISCOVER_SERVER_REQUEST";
    const char* RESPONSE_TEXT = "DISCOVER_SERVER_RESPONSE";

    char buf[256];

    int sendLen = sendto(cC, REQUEST_TEXT, (int)strlen(REQUEST_TEXT), 0, (LPSOCKADDR)&serverAddr, sizeof(serverAddr));
    if (sendLen == SOCKET_ERROR)
    {
        closesocket(cC);
        throw SetErrorMsgText("sendto:", WSAGetLastError());
    }

    printf("[DNS] Request sent to %s (%s): %s\n", dnsName, inet_ntoa(serverAddr.sin_addr), REQUEST_TEXT);

    while (true)
    {
        int recvLen = recvfrom(cC, buf, sizeof(buf) - 1, 0, from, flen);
        if (recvLen == SOCKET_ERROR)
        {
            int error = WSAGetLastError();
            if (error == WSAETIMEDOUT)
            {
                printf("[DNS] Timeout waiting for server response from %s\n", dnsName);
                closesocket(cC);
                return false;
            }
            closesocket(cC);
            throw SetErrorMsgText("recvfrom:", error);
        }

        buf[recvLen] = '\0';

        if (strcmp(buf, RESPONSE_TEXT) == 0)
        {
            SOCKADDR_IN* serverAddrResp = (SOCKADDR_IN*)from;
            printf("[DNS] Server %s found! Address: %s, Port: %d\n", 
                dnsName, inet_ntoa(serverAddrResp->sin_addr), ntohs(serverAddrResp->sin_port));
            closesocket(cC);
            return true;
        }
    }
}

int main(int argc, char* argv[])
{
    WSADATA wsdata;
    SOCKADDR_IN from;
    int flen = sizeof(from);

    int udpPort = 2000;
    const char* dnsName = "LenovoIdeaPadG3";

    // Parse command line arguments
    // Usage: DNS.exe [port] [dns_name]
    // or: DNS.exe [dns_name] (port defaults to 2000)
    if (argc > 1)
    {
        // Check if first argument is a number (port) or DNS name
        if (atoi(argv[1]) != 0 || strcmp(argv[1], "0") == 0)
        {
            udpPort = atoi(argv[1]);
            if (argc > 2)
                dnsName = argv[2];
        }
        else
        {
            // First argument is DNS name
            dnsName = argv[1];
            if (argc > 2)
                udpPort = atoi(argv[2]);
        }
    }

    try
    {
        if (WSAStartup(MAKEWORD(2, 2), &wsdata) != 0)
        {
            throw SetErrorMsgText("WSAStartup:", WSAGetLastError());
        }

        printf("=========================================\n");
        printf("         DNS Client v1.0                \n");
        printf("=========================================\n");

        bool found = false;

        if (dnsName != NULL)
        {
            // Search by DNS name
            printf("Searching for server by DNS name: %s\n", dnsName);
            printf("UDP port: %d\n", udpPort);
            printf("=========================================\n\n");
            
            found = GetServerByName(dnsName, udpPort, (struct sockaddr*)&from, &flen);
        }
        else
        {
            // Search by broadcast
            printf("Searching for server by broadcast on UDP port %d...\n", udpPort);
            printf("=========================================\n\n");
            
            found = GetServerByBroadcast(udpPort, (struct sockaddr*)&from, &flen);
        }

        if (found)
        {
            printf("\n[DNS] Successfully found server!\n");
            printf("Server IP: %s\n", inet_ntoa(from.sin_addr));
            printf("UDP Discovery Port: %d\n", ntohs(from.sin_port));
            
            // Connect to TCP server (TCP port is usually the same as UDP port or default 2000)
            int tcpPort = udpPort;  // Use the same port for TCP connection
            
            SOCKADDR_IN tcpServerAddr;
            tcpServerAddr.sin_family = AF_INET;
            tcpServerAddr.sin_port = htons(tcpPort);
            tcpServerAddr.sin_addr = ((SOCKADDR_IN*)&from)->sin_addr;
            
            SOCKET tcpSock = socket(AF_INET, SOCK_STREAM, 0);
            if (tcpSock == INVALID_SOCKET)
            {
                throw SetErrorMsgText("TCP socket creation failed:", WSAGetLastError());
            }
            
            // Set timeout to 3 minutes (180000 milliseconds)
            DWORD timeout = 180000;  // 3 minutes in milliseconds
            setsockopt(tcpSock, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));
            setsockopt(tcpSock, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(timeout));
            
            printf("\n[DNS] Connecting to TCP server at %s:%d...\n", 
                inet_ntoa(tcpServerAddr.sin_addr), tcpPort);
            
            if (connect(tcpSock, (SOCKADDR*)&tcpServerAddr, sizeof(tcpServerAddr)) == SOCKET_ERROR)
            {
                closesocket(tcpSock);
                throw SetErrorMsgText("TCP connect failed:", WSAGetLastError());
            }
            
            printf("[DNS] Connected to server successfully!\n\n");
            
            // Send initial command
            char command[256];
            printf("Choose initial command (echo / rand / time): ");
            if (fgets(command, sizeof(command), stdin) == NULL)
            {
                closesocket(tcpSock);
                WSACleanup();
                return 1;
            }
            
            // Remove newline character
            size_t len = strlen(command);
            if (len > 0 && command[len - 1] == '\n')
                command[len - 1] = '\0';
            
            if (strcmp(command, "echo") != 0 && 
                strcmp(command, "rand") != 0 && 
                strcmp(command, "time") != 0)
            {
                printf("Invalid choice. Defaulting to 'echo'.\n");
                strcpy(command, "echo");
            }
            
            int sent = send(tcpSock, command, (int)strlen(command), 0);
            if (sent == SOCKET_ERROR)
            {
                printf("Failed to send initial command.\n");
                closesocket(tcpSock);
                WSACleanup();
                return 1;
            }
            
            // Receive initial response
            char buffer[256];
            int received = recv(tcpSock, buffer, sizeof(buffer) - 1, 0);
            if (received > 0)
            {
                buffer[received] = '\0';
                printf("Server initial reply: %s\n", buffer);
            }
            else if (received == 0)
            {
                printf("Server closed connection.\n");
                closesocket(tcpSock);
                WSACleanup();
                return 0;
            }
            else
            {
                printf("Recv failed: %d\n", WSAGetLastError());
                closesocket(tcpSock);
                WSACleanup();
                return 1;
            }
            
            // Interactive mode
            printf("\n[DNS] Entering interactive mode. Type 'exit' to quit.\n");
            while (true)
            {
                char msg[256];
                printf("\nEnter message (or 'exit' to quit): ");
                if (fgets(msg, sizeof(msg), stdin) == NULL)
                    break;
                
                // Remove newline character
                len = strlen(msg);
                if (len > 0 && msg[len - 1] == '\n')
                    msg[len - 1] = '\0';
                
                if (strcmp(msg, "exit") == 0)
                    break;
                
                sent = send(tcpSock, msg, (int)strlen(msg), 0);
                if (sent == SOCKET_ERROR)
                {
                    printf("Send failed: %d\n", WSAGetLastError());
                    break;
                }
                
                received = recv(tcpSock, buffer, sizeof(buffer) - 1, 0);
                if (received > 0)
                {
                    buffer[received] = '\0';
                    printf("Server replied: %s\n", buffer);
                }
                else if (received == 0)
                {
                    printf("Server closed connection\n");
                    break;
                }
                else
                {
                    printf("Recv failed: %d\n", WSAGetLastError());
                    break;
                }
            }
            
            closesocket(tcpSock);
            printf("\n[DNS] Connection closed. Program terminated.\n");
        }
        else
        {
            printf("\n[DNS] Server not found\n");
        }
    }
    catch (const char* message)
    {
        printf("Error: %s\n", message);
        WSACleanup();
        return 1;
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

