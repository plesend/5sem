#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <windows.h>
#include <ctime>

char* SetErrorMsgText(const char* msgText, int code);

int main()
{
    try
    {
        Sleep(1000);
        DWORD writeword;
        HANDLE cH = CreateFile(L"\\\\*\\mailslot\\Box",
            GENERIC_WRITE,
            FILE_SHARE_WRITE,
            NULL, OPEN_EXISTING, NULL, NULL);
        if (cH == INVALID_HANDLE_VALUE)
        {
            throw SetErrorMsgText("CreateFile: ", GetLastError());
        }

        const int count = 1000;
        char wbuf[300] = "Hello from Mailslot-client";

        clock_t start = clock(); 

        for (int i = 0; i < count; i++)
        {
            if (!WriteFile(cH, wbuf, strlen(wbuf) + 1, &writeword, NULL))
                throw SetErrorMsgText("WriteFile: ", GetLastError());
        }

        WriteFile(cH, "", 0, &writeword, NULL);

        clock_t end = clock(); 
        double timeSec = double(end - start) / CLOCKS_PER_SEC;

        printf("Sent %d messages in %.3f seconds\n", count, timeSec);
        printf("Average per message: %.6f sec\n", timeSec / count);

        CloseHandle(cH);
    }
    catch (const char* message)
    {
        printf("%s\n", message);
    }
}

char* SetErrorMsgText(const char* msgText, int code)
{
    static char buffer[256];
    sprintf(buffer, "%s %d", msgText, code);
    return buffer;
}
