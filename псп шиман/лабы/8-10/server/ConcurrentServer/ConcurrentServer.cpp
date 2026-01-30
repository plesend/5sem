#define _CRT_SECURE_NO_WARNINGS
#define AS_SQUIRT 10
#include <iostream>
#include <windows.h>
#include <list>
#include "Contact.h"
#pragma comment(lib, "WS2_32.lib")

int port = 2000;
int uport = 2000;
char dllName[256] = "ServiceLibrary.dll";
char hostname[256] = "kvasik";
char callname[256] = "kvasik";
SOCKET sS;
HANDLE(*ts) (char*, LPVOID);
HMODULE st = LoadLibraryA(dllName);
HANDLE hClientConnectedEvent;
CRITICAL_SECTION scListContact;
ListContact contacts;
volatile long opened = 0;

volatile long connectionCount = 0;
volatile long sayNoCount = 0;
volatile long successConnections = 0;
volatile long currentActiveConnections = 0;



enum TalkersCommand
{
    START,
    STOP,
    EXIT,
    GETCOMMAND,
    WAIT,
    SHUTDOWN
};

HANDLE hAcceptServer,
       hDispatchServer,
       hConsolePipe,
       hGarbageCleaner,
       hResponseServer;
DWORD WINAPI AcceptServer(LPVOID pPrm);
DWORD WINAPI DispatchServer(LPVOID pPrm);
DWORD WINAPI GarbageCleaner(LPVOID pPrm);
DWORD WINAPI ConsolePipe(LPVOID pPrm);
DWORD WINAPI ResponseServer(LPVOID pPrm);
void CALLBACK ASWTimer(LPVOID Prm, DWORD, DWORD);

void CommandsCycle(TalkersCommand& cmd);
bool AcceptCycle(int squirt);
bool GetRequestFromClient(const char* expectedRequest, short port, struct sockaddr* from, int* flen);
bool PutAnswerToClient(const char* answer, struct sockaddr* to, int* lto);
char* SetErrorMsgText(const char* msgText, int code);

int main(int argc, char* argv[])
{

    SetConsoleOutputCP(1251);
    setlocale(LC_ALL, 0);

    if (argc > 1) port = atoi(argv[1]);
    if (argc > 2) strcpy(dllName, argv[2]);
    if (argc > 3) strcpy(hostname, argv[3]);
    if (argc > 4) uport = atoi(argv[4]);
    if (argc > 5) strcpy(callname, argv[5]);

    printf("\t\t Concurrent Server \n\n");
    printf("TCP Port : %d\n", port);
    printf("UDP Port : %d\n", uport);
    printf("Library  : %s\n", dllName);
    printf("Hostname : %s\n", hostname);
    printf("Callname : %s\n", callname);
    printf("\n=======================\n");

    if (st == NULL)
    {
        throw SetErrorMsgText("LoadLibrary: ", GetLastError());
    }
    ts = (HANDLE (*)(char*, LPVOID))GetProcAddress(st, "SSS");
    volatile TalkersCommand cmd = START;
    InitializeCriticalSection(&scListContact);

    hAcceptServer = CreateThread(NULL, NULL, AcceptServer, (LPVOID)&cmd, NULL, NULL);
    hDispatchServer = CreateThread(NULL, NULL, DispatchServer, (LPVOID)&cmd, NULL, NULL);
    hGarbageCleaner = CreateThread(NULL, NULL, GarbageCleaner, (LPVOID)&cmd, NULL, NULL);
    hConsolePipe = CreateThread(NULL, 0, ConsolePipe, (LPVOID)&cmd, 0, NULL);
    hResponseServer = CreateThread(NULL, 0, ResponseServer, (LPVOID)&cmd, 0, NULL);

    SetThreadPriority(hAcceptServer, THREAD_PRIORITY_ABOVE_NORMAL);
    SetThreadPriority(hDispatchServer, THREAD_PRIORITY_HIGHEST);
    SetThreadPriority(hGarbageCleaner, THREAD_PRIORITY_LOWEST);
    SetThreadPriority(hConsolePipe, THREAD_PRIORITY_BELOW_NORMAL);
    SetThreadPriority(hResponseServer, THREAD_PRIORITY_ABOVE_NORMAL);

    WaitForSingleObject(hAcceptServer, INFINITE);
    WaitForSingleObject(hDispatchServer, INFINITE);
    WaitForSingleObject(hGarbageCleaner, INFINITE);
    WaitForSingleObject(hConsolePipe, INFINITE);
    WaitForSingleObject(hResponseServer, INFINITE);
    CloseHandle(hAcceptServer);
    CloseHandle(hDispatchServer);
    CloseHandle(hGarbageCleaner);
    CloseHandle(hConsolePipe);
    CloseHandle(hResponseServer);
    DeleteCriticalSection(&scListContact);
    FreeLibrary(st);
    return 0;
}

DWORD WINAPI AcceptServer(LPVOID pPrm)
{
    WSADATA wsaData;
    hClientConnectedEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (!hClientConnectedEvent)
    {
        printf("CreateEvent PROBLEMO: %d\n", GetLastError());
        ExitThread(1);
    }

    try {
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
            throw SetErrorMsgText("WSAStartup", WSAGetLastError());

        sS = socket(AF_INET, SOCK_STREAM, 0);
        if (sS == INVALID_SOCKET)
            throw SetErrorMsgText("socket:", WSAGetLastError());

        SOCKADDR_IN serv;
        serv.sin_family = AF_INET;
        serv.sin_port = htons(port);
        serv.sin_addr.s_addr = INADDR_ANY;

        if (bind(sS, (LPSOCKADDR)&serv, sizeof(serv)) == SOCKET_ERROR)
            throw SetErrorMsgText("bind:", WSAGetLastError());

        if (listen(sS, SOMAXCONN) == SOCKET_ERROR)
            throw SetErrorMsgText("listen:", WSAGetLastError());

        u_long nonblk = 1;
        if (ioctlsocket(sS, FIONBIO, &nonblk) == SOCKET_ERROR)
            throw SetErrorMsgText("ioctlsocket:", WSAGetLastError());

        TalkersCommand& cmd = *((TalkersCommand*)pPrm);

        while (cmd != EXIT)
        {
            CommandsCycle(cmd);
            if (cmd == EXIT)
                break;

            SleepEx(50, TRUE);
        }

        printf("AcceptServer: SHUTTIN' DOWN\n");

        closesocket(sS);
        WSACleanup();
    }
    catch (const char* message)
    {
        printf("%s\n", message);
    }

    SetEvent(hClientConnectedEvent);
    CloseHandle(hClientConnectedEvent);

    printf("AcceptServer: ExitThread\n");
    ExitThread(0);
}

DWORD WINAPI DispatchServer(LPVOID pPrm)
{
    while (hClientConnectedEvent == NULL)
    {
        Sleep(10);
    }

    TalkersCommand& command = *(TalkersCommand*)pPrm;

    while (command != EXIT)
    {
        if (command != STOP)
        {
            WaitForSingleObjectEx(hClientConnectedEvent, 100, TRUE);
            ResetEvent(hClientConnectedEvent);

            EnterCriticalSection(&scListContact);
            time_t now = time(NULL);

            for (auto it = contacts.begin(); it != contacts.end(); )
            {
                Contact& contact = *it;

                if (contact.type != Contact::EMPTY && now - contact.lastActivityTime > 180)
                {
                    printf("Client timeout due to inactivity\n");
                    send(contact.s, "Timeout due to inactivity", 26, 0);
                    closesocket(contact.s);
                    contact.type = Contact::EMPTY;
                    InterlockedDecrement(&currentActiveConnections);
                    ++it;
                    continue;
                }

                if (contact.type == Contact::ACCEPT) //òîëüêî íîâûå ïîäêë
                {
                    char message[256];
                    int r = recv(contact.s, message, sizeof(message) - 1, 0);

                    if (r == SOCKET_ERROR)
                    {
                        int err = WSAGetLastError();
                        if (err == WSAEWOULDBLOCK)
                        {
                            ++it;
                            continue;
                        }
                        printf("recv error: %d\n", err);
                        closesocket(contact.s);
                        contact.type = Contact::EMPTY;
                        InterlockedIncrement(&sayNoCount);
                        InterlockedDecrement(&currentActiveConnections);
                        ++it;
                        continue;
                    }
                    else if (r == 0)
                    {
                        printf("Client disconnected before sending command\n");
                        closesocket(contact.s);
                        contact.type = Contact::EMPTY;
                        InterlockedDecrement(&currentActiveConnections);
                        ++it;
                        continue;
                    }

                    message[r] = '\0';
                    contact.lastActivityTime = now;

                    if (strcmp(message, "echo") == 0)
                    {
                        printf("Starting ECHO service for client\n");
                        send(contact.s, "ECHO mode started\r\n", 19, 0);

                        contact.type = Contact::CONTACT;
                        strcpy(contact.srvname, "echo");

                        u_long blocking = 0;
                        ioctlsocket(contact.s, FIONBIO, &blocking);

                        contact.hthread = CreateThread(NULL, 0, [](LPVOID param) -> DWORD {
                            Contact* c = (Contact*)param;
                            char buffer[1024];

                            while (true)
                            {
                                int received = recv(c->s, buffer, sizeof(buffer), 0);
                                if (received <= 0)
                                {
                                    if (received == 0)
                                        printf("Echo client disconnected gracefully\n");
                                    else
                                        printf("Echo recv error: %d\n", WSAGetLastError());
                                    break;
                                }

                                c->lastActivityTime = time(NULL);

                                send(c->s, buffer, received, 0);
                            }

                            closesocket(c->s);
                            EnterCriticalSection(&scListContact);
                            c->type = Contact::EMPTY;
                            c->sthread = Contact::FINISH;
                            LeaveCriticalSection(&scListContact);
                            InterlockedDecrement(&currentActiveConnections);

                            return 0;
                            }, &contact, 0, NULL);

                        LARGE_INTEGER Li;
                        Li.QuadPart = -(10000000LL * 180);
                        contact.htimer = CreateWaitableTimer(NULL, FALSE, NULL);
                        SetWaitableTimer(contact.htimer, &Li, 0, ASWTimer, &contact, FALSE);
                    }
                    else if (strcmp(message, "time") == 0 || strcmp(message, "rand") == 0)
                    {
                        printf("Request received: %s\n", message);
                        if (send(contact.s, "Request received", 17, 0) == SOCKET_ERROR) 
                        {
                            closesocket(contact.s);
                            contact.type = Contact::EMPTY;
                            InterlockedDecrement(&currentActiveConnections);
                            ++it;
                            continue;
                        }

                        contact.type = Contact::CONTACT;
                        strcpy(contact.srvname, message);

                        u_long blocking = 0;
                        ioctlsocket(contact.s, FIONBIO, &blocking);

                        contact.hthread = ts(message, &contact);

                        LARGE_INTEGER Li;
                        Li.QuadPart = -(10000000LL * 180);
                        contact.htimer = CreateWaitableTimer(NULL, FALSE, NULL);
                        SetWaitableTimer(contact.htimer, &Li, 0, ASWTimer, &contact, FALSE);
                    }
                    // ====================== ÍÅÈÇÂÅÑÒÍÀß ÊÎÌÀÍÄÀ ======================
                    else
                    {
                        send(contact.s, "ErrorInquiry", 13, 0);
                        closesocket(contact.s);
                        contact.type = Contact::EMPTY;
                        contact.sthread = Contact::ABORT;
                        InterlockedIncrement(&sayNoCount);
                        InterlockedDecrement(&currentActiveConnections);
                    }
                }

                ++it;
            }

            LeaveCriticalSection(&scListContact);
        }

        SleepEx(100, TRUE);
    }

    ExitThread(0);
}

DWORD WINAPI GarbageCleaner(LPVOID pPrm)
{
    TalkersCommand& cmd = *(TalkersCommand*)pPrm;
    while (cmd != EXIT)
    {
        EnterCriticalSection(&scListContact);

        for (auto it = contacts.begin(); it != contacts.end(); )
        {
            if (it->sthread == it->FINISH) {
                InterlockedIncrement(&successConnections);
                printf("Client disconnected gracefully\n");
            }
            if (it->sthread == it->ABORT || it->sthread == it->TIMEOUT)
                InterlockedIncrement(&sayNoCount);
            if (it->type == Contact::EMPTY)
            {
                InterlockedIncrement(&successConnections);
                it = contacts.erase(it);
                continue;
            }
            ++it;
        }


        LeaveCriticalSection(&scListContact);
        Sleep(2000);
    }

    printf("GarbageCleaner: ExitThread\n");
    ExitThread(0);
}

DWORD WINAPI ConsolePipe(LPVOID pPrm)
{
    TalkersCommand* pCmd = (TalkersCommand*)pPrm;
    char pipeName[256];
    sprintf(pipeName, "\\\\.\\pipe\\%s", hostname);

    HANDLE hPipe = CreateNamedPipeA(
        pipeName,
        PIPE_ACCESS_DUPLEX,
        PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
        1,
        1024, 1024,
        0,
        NULL
    );

    if (hPipe == INVALID_HANDLE_VALUE)
    {
        printf("CreateNamedPipe PROBLEMO: %d\n", GetLastError());
        ExitThread(1);
    }

    printf(":ConsolePipe: waiting for clients on %s...\n", pipeName);

    BOOL connected = ConnectNamedPipe(hPipe, NULL) ? TRUE :
        (GetLastError() == ERROR_PIPE_CONNECTED);

    if (!connected)
    {
        printf("ConnectNamedPipe PROBLEMO: %d\n", GetLastError());
        CloseHandle(hPipe);
        ExitThread(1);
    }

    printf("ConsolePipe: Client connected:) \n");

    char buffer[256];
    DWORD bytesRead = 0;

    while (true)
    {
        BOOL ok = ReadFile(hPipe, buffer, sizeof(buffer) - 1, &bytesRead, NULL);
        if (!ok || bytesRead == 0)
        {
            printf("ConsolePipe: Client disconnected :(\n");
            DisconnectNamedPipe(hPipe);
            ConnectNamedPipe(hPipe, NULL);
            continue;
        }
        buffer[bytesRead] = '\0';
        printf("ConsolePipe: Received: %s\n", buffer);
        for (char* p = buffer; *p; ++p) *p = tolower(*p);

        const char* response = "Unknown command";
        bool known = true;

        if (strcmp(buffer, "start") == 0)
        {
            *pCmd = START;
            response = "Server started";
        }
        else if (strcmp(buffer, "stop") == 0)
        {
            *pCmd = STOP;
            response = "Server stoped";
        }
        else if (strcmp(buffer, "help") == 0)
        {
            response = "API commands: start stop wait status help exit shutdown";
        }
        else if (strcmp(buffer, "shutdown") == 0)
        {
            *pCmd = SHUTDOWN;
            response = "Shutdown initiated. Waiting for clients to disconnect...";
        }
        else if (strcmp(buffer, "wait") == 0)
        {
            *pCmd = WAIT;
            response = "Waiting for clients...";
        }
        else if (strcmp(buffer, "exit") == 0)
        {
            *pCmd = EXIT;
            response = "Exiting...";
        }
        else if (strcmp(buffer, "status") == 0 || strcmp(buffer, "statistics") == 0)
        {
            size_t active = 0;
            EnterCriticalSection(&scListContact);
            active = contacts.size();
            LeaveCriticalSection(&scListContact);

                 
            extern volatile long connectionCount;         
            extern volatile long sayNoCount;             
            extern volatile long successConnections;      
            extern volatile long currentActiveConnections;

            char statbuf[256];
            sprintf(statbuf,
                "connectionCount=%ld; sayNoCount=%ld; successConnections=%ld; active=%zu",
                connectionCount,
                sayNoCount,
                successConnections,
                active 
            );

            response = statbuf;

            printf("ConsolePipe: STAT: %s\n", statbuf);
        }
        else
        {
            known = false;
            *pCmd = GETCOMMAND;
        }

        DWORD written;
        WriteFile(hPipe, response, strlen(response) + 1, &written, NULL);

        if (!known)
        {
            printf("ConsolePipe: Unknown command, sent 'nocmd'.\n");
            continue;
        }
        if (*pCmd != EXIT && *pCmd != SHUTDOWN && *pCmd != GETCOMMAND)
        {
            while (*pCmd != GETCOMMAND)
                Sleep(100);
        }

        if (*pCmd == EXIT)
        {
            printf("ConsolePipe: Exit command received shutting down.\n");
            break;
        }
    }
    DisconnectNamedPipe(hPipe);
    CloseHandle(hPipe);
    ExitThread(0);
}

DWORD WINAPI ResponseServer(LPVOID pPrm)
{
    WSAData wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
    DWORD rc = 0;
    SOCKADDR_IN from;
    int lc = sizeof(from);
    ZeroMemory(&from, lc);
    int numberOfClients = 0;

    const char* REQUEST_TEXT = "DISCOVER_SERVER_REQUEST";
    const char* RESPONSE_TEXT = "DISCOVER_SERVER_RESPONSE";

    while (*(TalkersCommand*)pPrm != EXIT)
    {
        try
        {
            if (GetRequestFromClient((char*)REQUEST_TEXT, uport, (sockaddr*)&from, &lc))
            {
                if (opened)
                {
                    printf("\nconnected Client: %d, port: %d, address: %s\n",
                        ++numberOfClients, ntohs(from.sin_port), inet_ntoa(from.sin_addr));
                }

                if (!PutAnswerToClient((char*)RESPONSE_TEXT, (sockaddr*)&from, &lc))
                {
                    if (opened) printf("Failed to send response to %s\n", inet_ntoa(from.sin_addr));
                }
                //*(TalkersCommand*)pPrm = EXIT;
            }
        }
        catch (const char* message)
        {
            printf("%s %s\n", "An error occured!", message);
        }
    }

    ExitThread(rc);
}

void CommandsCycle(TalkersCommand& cmd)
{
    int squirt = 0;
    bool allowConnections = false;
    bool waitForLastClient = false;

    while (cmd != EXIT)
    {
        switch (cmd)
        {
        case START:
            printf("\nCommandsCycle: START\n");
            allowConnections = true;
            squirt = AS_SQUIRT;
            cmd = GETCOMMAND;
            break;

        case STOP:
            printf("\nCommandsCycle: STOP\n");
            allowConnections = false;
            squirt = 0;
            cmd = GETCOMMAND;
            break;

        case WAIT:
            printf("\nCommandsCycle: WAIT\n");
            waitForLastClient = true;

            allowConnections = false;
            squirt = 0;

            while (!contacts.empty())
            {
                Sleep(100);
            }
            allowConnections = true;
            squirt = AS_SQUIRT;
            printf("\nCommandsCycle: WAIT COMPLETED\n");
            waitForLastClient = false;
            cmd = GETCOMMAND;
            break;

        case SHUTDOWN:
            printf("\nCommandsCycle: SHUTDOWN initiated\n");
            allowConnections = false;
            squirt = 0;
            printf("\nCommandsCycle: No more connections are accepted. Waiting for clients to disconnect...\n");
            
            // Wait for all clients to disconnect
            while (true)
            {
                EnterCriticalSection(&scListContact);
                size_t clientCount = 0;
                for (const auto& c : contacts)
                {
                    if (c.type != Contact::EMPTY)
                        clientCount++;
                }
                LeaveCriticalSection(&scListContact);
                
                if (clientCount == 0)
                {
                    printf("\nCommandsCycle: All clients disconnected. Shutting down server...\n");
                    break;
                }
                
                printf("\nCommandsCycle: Waiting for %zu client(s) to disconnect...\n", clientCount);
                Sleep(500);
            }
            
            printf("CommandsCycle: All clients disconnected. Exiting...\n");
            cmd = EXIT;
            break;

        case EXIT:
            printf("CommandsCycle: EXIT requested\n");
            break;

        default:
            SleepEx(50, TRUE);
            break;
        }

        if (allowConnections && cmd != EXIT)
        {
            if (AcceptCycle(squirt))
                cmd = GETCOMMAND;
        }
        else
        {
            SleepEx(50, TRUE);
        }
    }

    printf("CommandsCycle: EXIT complete. Closing all clients...\n");
    EnterCriticalSection(&scListContact);
    for (auto& c : contacts)
    {
        if (c.s != INVALID_SOCKET)
            closesocket(c.s);
    }
    contacts.clear();
    LeaveCriticalSection(&scListContact);

    printf("CommandsCycle: Finished. Exiting thread.\n");
}

bool AcceptCycle(int squirt)
{
    bool rc = false;
    Contact c(Contact::ACCEPT, "EchoServer");
    while (squirt-- > 0 && rc == false)
    {
        if ((c.s = accept(sS, (sockaddr*)&c.prms, &c.lprms)) == INVALID_SOCKET)
        {
            if (WSAGetLastError() != WSAEWOULDBLOCK)
            {
                InterlockedIncrement(&sayNoCount);
                throw SetErrorMsgText("accept:", WSAGetLastError());
            }
                
        }
        else
        {
            rc = true;
            u_long nonblock = 1;
            ioctlsocket(c.s, FIONBIO, &nonblock);

            DWORD timeout = 180000;  
            setsockopt(c.s, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));
            setsockopt(c.s, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(timeout));

            c.connectTime = time(NULL);
            c.lastActivityTime = time(NULL);

            EnterCriticalSection(&scListContact);
            contacts.push_front(c);
            LeaveCriticalSection(&scListContact);
            SetEvent(hClientConnectedEvent);
            InterlockedIncrement(&currentActiveConnections);
            InterlockedIncrement(&connectionCount);
        }
    }
    return rc;
}

void CALLBACK ASWTimer(LPVOID Prm, DWORD, DWORD) {
    Contact* contact = (Contact*)(Prm);

    printf("ASWTimer SAYS HELLO\n");

    TerminateThread(contact->hthread, NULL);
    InterlockedIncrement(&sayNoCount);

    send(contact->s, "TimeOUT", strlen("TimeOUT") + 1, NULL);

    EnterCriticalSection(&scListContact);
    CancelWaitableTimer(contact->htimer);

    contact->type = contact->EMPTY;
    contact->sthread = contact->TIMEOUT;

    //closesocket(contact->s);
    LeaveCriticalSection(&scListContact);
    InterlockedDecrement(&currentActiveConnections);

}

bool GetRequestFromClient(const char* expectedRequest, short port, struct sockaddr* from, int* flen)
{
    SOCKET sS = socket(AF_INET, SOCK_DGRAM, 0);
    if (sS == INVALID_SOCKET)
        throw  SetErrorMsgText("socket:", WSAGetLastError());

    BOOL reuse = TRUE;
    setsockopt(sS, SOL_SOCKET, SO_REUSEADDR, (char*)&reuse, sizeof(reuse));

    SOCKADDR_IN serv;
    serv.sin_family = AF_INET;
    serv.sin_port = htons(port);
    serv.sin_addr.s_addr = INADDR_ANY;

    if (bind(sS, (LPSOCKADDR)&serv, sizeof(serv)) == SOCKET_ERROR) {
        int err = WSAGetLastError();
        closesocket(sS);
        throw SetErrorMsgText("bind:", err);
    }

    u_long mode = 1;
    if (ioctlsocket(sS, FIONBIO, &mode) == SOCKET_ERROR) {
        int err = WSAGetLastError();
        closesocket(sS);
        throw SetErrorMsgText("ioctlsocket:", err);
    }

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
                Sleep(50);
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
            if (strcmp(msgfrom, expectedRequest) == 0)
            {
                memcpy(from, &temp, sizeof(SOCKADDR_IN));
                closesocket(sS);
                return true;
            }
        }
    }
}


bool PutAnswerToClient(const char* answer, struct sockaddr* to, int* lto)
{
    SOCKET scS = socket(AF_INET, SOCK_DGRAM, 0);
    if (scS == INVALID_SOCKET)
        throw SetErrorMsgText("socket:", WSAGetLastError());

    BOOL bcast = TRUE;
    setsockopt(scS, SOL_SOCKET, SO_BROADCAST, (char*)&bcast, sizeof(bcast));

    u_long mode = 1;
    if (ioctlsocket(scS, FIONBIO, &mode) == SOCKET_ERROR) {
        int err = WSAGetLastError();
        closesocket(scS);
        throw SetErrorMsgText("ioctlsocket:", err);
    }

    int sendSize;
    DWORD start = GetTickCount();

    while (true)
    {
        sendSize = sendto(scS, answer, (int)strlen(answer), 0, to, *lto);
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
                Sleep(50);
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


char* SetErrorMsgText(const char* msgText, int code)
{
    static char buffer[256];
    sprintf(buffer, "%s %d", msgText, code);
    return buffer;
}
