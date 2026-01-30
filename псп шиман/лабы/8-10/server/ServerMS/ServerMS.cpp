#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <windows.h>
#include <ctime>

char* SetErrorMsgText(const char* msgText, int code);

int main()
{
    try
    {
        DWORD readword;
        HANDLE sH = CreateMailslot(L"\\\\.\\mailslot\\Box",
            (DWORD)300,
            (DWORD)180000,
            NULL);
        if (sH == INVALID_HANDLE_VALUE)
        {
            throw SetErrorMsgText("CreateMailslot: ", GetLastError());
        }

        char rbuf[300];
        int counter = 0;

        clock_t start = clock();

        while (true)
        {
            if (ReadFile(sH, rbuf, sizeof(rbuf), &readword, NULL) == FALSE)
            {
                if (GetLastError() == 121)
                    throw "Timed out.";
                throw SetErrorMsgText("ReadFile: ", GetLastError());
            }

            if (readword == 0) 
                break;

            counter++;
            if (counter >= 1000)
                break;
        }

        clock_t end = clock(); 
        double timeSec = double(end - start) / CLOCKS_PER_SEC;

        printf("Message text: %s\n", rbuf);
        printf("Received %d messages in %.3f seconds\n", counter, timeSec);
        printf("Average per message: %.6f sec\n", timeSec / counter);

        CloseHandle(sH);
    }
    catch (const char* message)
    {
        printf("%s\n", message);
    }
    system("pause");
}

char* SetErrorMsgText(const char* msgText, int code)
{
    static char buffer[256];
    sprintf(buffer, "%s %d", msgText, code);
    return buffer;
}
