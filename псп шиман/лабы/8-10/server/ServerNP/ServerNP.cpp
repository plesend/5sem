#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <windows.h>

char* SetErrorMsgText(const char* msgText, int code);
//net use \\MyServerName\ipc$ /user:MyServerName\bubblechuk 1379
int main()
{
    try 
    {
        HANDLE sH = CreateNamedPipe(L"\\\\.\\pipe\\Tube",
            PIPE_ACCESS_DUPLEX,
            PIPE_TYPE_MESSAGE | PIPE_WAIT,
            1, NULL, NULL, INFINITE, NULL);
        DWORD sbread, sbwrite;
        if (sH == INVALID_HANDLE_VALUE)
        {
            throw SetErrorMsgText("CreateNamedPipe: ", GetLastError());
        }

        if (!ConnectNamedPipe(sH, NULL))
        {
            throw SetErrorMsgText("ConnectNamedPipe: ", GetLastError());
        }
        
        while (true)
        {
            char rbuf[256];
            if (ReadFile(sH, rbuf, 256, &sbread, NULL) == FALSE)
            {
                throw SetErrorMsgText("ReadFile: ", GetLastError());
            }
            printf("Recieved message: %s\n", rbuf);
            //char wbuf[256];
            WriteFile(sH, rbuf, 256, &sbwrite, NULL);
            printf("Echoed message: %s\n", rbuf);
        }
        DisconnectNamedPipe(sH);
        CloseHandle(sH);
    }
    catch (const char* message)
    {
        if (GetLastError() == (DWORD)109)
            return 0;
        printf("%s\n", message);
    }
}

char* SetErrorMsgText(const char* msgText, int code)
{
    static char buffer[256];
    sprintf(buffer, "%s %d", msgText, code);
    return buffer;
}
