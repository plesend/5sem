#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <windows.h>

char* SetErrorMsgText(const char* msgText, int code);

int main()
{
    char wbuf[256] = "The Message";
    char rbuf[256];
    DWORD cbytes;
    try
    {
        int result = CallNamedPipe(L"\\\\BUBBLE\\pipe\\Tube",
                      wbuf,
                      256,
                      rbuf,
                      256,
                      &cbytes,
                      (DWORD)5000);
        if (!result)
        {
            throw SetErrorMsgText("CallNamedPipe: ", GetLastError());
        }
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
