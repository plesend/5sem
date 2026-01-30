#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <windows.h>

char* SetErrorMsgText(const char* msgText, int code);

int main()
{
    try
    {
        HANDLE cH = CreateFile(L"\\\\BUBBLE\\pipe\\Tube",
            GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL, OPEN_EXISTING, NULL, NULL);
        DWORD cbytes;
        if (cH == INVALID_HANDLE_VALUE)
        {
            throw SetErrorMsgText("CreateFile: ", GetLastError());
        }
        char* amount = (char*)malloc(sizeof(char) * 256);
        printf("Enter amount of messages to be sent: ");
        scanf("%s", amount);

        int amount2 = atoi(amount);

        for (int i = 1; i <= amount2; i++)
        {
            char wbuf[256] = "The Message ";
            char rbuf[256];
            char* number = (char*)malloc(sizeof(char) * 256);
            _itoa(i, number, 10);
            strcat(wbuf, number);
            int result = TransactNamedPipe(cH,
                              wbuf,
                              256,
                              rbuf,
                              256,
                              &cbytes,
                              NULL
                              );
            printf("Sended Message: %s\n", wbuf);
            printf("Echoed message: %s\n", rbuf);
            if (!result)
            {
                throw SetErrorMsgText("TransactNamedPipe: ", GetLastError());
            }
            free(number);
        }
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
