#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <windows.h>
#include <iostream>
#include <string>

#pragma comment(lib, "WS2_32.lib")

sockaddr_in GetServer(SOCKET bcSock, int broadcastPort, int serverPort, const std::string& discoverMsg) {
    sockaddr_in broadcastAddr{};
    broadcastAddr.sin_family = AF_INET;
    broadcastAddr.sin_port = htons(broadcastPort);
    broadcastAddr.sin_addr.s_addr = inet_addr("255.255.255.255");

    const char* query = "DISCOVER_SERVER_REQUEST";
    sendto(bcSock, query, (int)strlen(query), 0, (sockaddr*)&broadcastAddr, sizeof(broadcastAddr));

    fd_set readfds;
    timeval timeout{};
    timeout.tv_sec = 3;
    timeout.tv_usec = 0;

    FD_ZERO(&readfds);
    FD_SET(bcSock, &readfds);

    sockaddr_in serverAddr{};
    int addrLen = sizeof(serverAddr);

    if (select(0, &readfds, nullptr, nullptr, &timeout) > 0) {
        char buffer[256];
        int received = recvfrom(bcSock, buffer, sizeof(buffer) - 1, 0, (sockaddr*)&serverAddr, &addrLen);
        if (received > 0) {
            buffer[received] = '\0';
            if (std::string(buffer) == discoverMsg) {
                std::cout << "Server found: " << inet_ntoa(serverAddr.sin_addr) << "\n";
                serverAddr.sin_port = htons(serverPort);
                return serverAddr;
            }
        }
    }

    throw std::runtime_error("Server not found via broadcast.");
}

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed\n";
        return 1;
    }

    int serverPort = 2000;
    int broadcastPort = 2000;
    std::string discoverMsg = "DISCOVER_SERVER_RESPONSE";

    SOCKET bcSock = socket(AF_INET, SOCK_DGRAM, 0);
    if (bcSock == INVALID_SOCKET) {
        std::cerr << "Broadcast socket creation failed\n";
        WSACleanup();
        return 1;
    }

    BOOL broadcast = TRUE;
    setsockopt(bcSock, SOL_SOCKET, SO_BROADCAST, (char*)&broadcast, sizeof(broadcast));

    sockaddr_in clientAddr{};
    clientAddr.sin_family = AF_INET;
    clientAddr.sin_port = htons(0);
    clientAddr.sin_addr.s_addr = INADDR_ANY;
    bind(bcSock, (sockaddr*)&clientAddr, sizeof(clientAddr));

    sockaddr_in serverAddr{};
    try {
        serverAddr = GetServer(bcSock, broadcastPort, serverPort, discoverMsg);
    }
    catch (const std::exception& ex) {
        std::cerr << ex.what() << "\n";
        closesocket(bcSock);
        WSACleanup();
        return 1;
    }
    closesocket(bcSock);

    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        std::cerr << "Socket creation failed\n";
        WSACleanup();
        return 1;
    }

    if (connect(sock, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Connect failed\n";
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    std::cout << "Connected to server.\n";

    std::string command;
    std::cout << "Choose initial command (echo / rand / time): ";
    std::getline(std::cin, command);
    if (command != "echo" && command != "rand" && command != "time") {
        std::cout << "Invalid choice. Defaulting to 'echo'.\n";
        command = "echo";
    }

    int sent = send(sock, command.c_str(), (int)command.size(), 0);
    if (sent == SOCKET_ERROR) {
        std::cerr << "Failed to send initial command.\n";
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    char buffer[256];
    int received = recv(sock, buffer, sizeof(buffer) - 1, 0);
    if (received > 0) {
        buffer[received] = '\0';
        std::cout << "Server initial reply: " << buffer << "\n";
    }
    else if (received == 0) {
        std::cout << "Server closed connection.\n";
        closesocket(sock);
        WSACleanup();
        return 0;
    }
    else {
        std::cerr << "Recv failed\n";
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    while (true) {
        std::string msg;
        std::cout << "Enter message (or 'exit' to quit): ";
        std::getline(std::cin, msg);

        if (msg == "exit") break;

        sent = send(sock, msg.c_str(), (int)msg.size(), 0);
        if (sent == SOCKET_ERROR) {
            std::cerr << "Send failed\n";
            break;
        }

        received = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (received > 0) {
            buffer[received] = '\0';
            std::cout << "Server replied: " << buffer << "\n";
        }
        else if (received == 0) {
            std::cout << "Server closed connection\n";
            break;
        }
        else {
            std::cerr << "Recv failed\n";
            break;
        }
    }

    closesocket(sock);
    WSACleanup();
    std::cout << "Program terminated.\n";
    return 0;
}
