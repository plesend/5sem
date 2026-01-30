#include <windows.h>
#include <iostream>
#include <string>

using namespace std;

#define MAILSLOT_NAME L"\\\\.\\mailslot\\Box"

string GetErrorMsgText(DWORD code)
{
    switch (code)
    {
    case ERROR_ALREADY_EXISTS: return "Mailslot already exists";
    case ERROR_ACCESS_DENIED:  return "Access denied";
    case ERROR_INVALID_HANDLE:return "Invalid handle";
    case ERROR_SEM_TIMEOUT:   return "Read timeout";
    default:
        return "Error code: " + to_string(code);
    }
}

string SetErrorMsgText(const string& msgText, DWORD code)
{
    return msgText + ": " + GetErrorMsgText(code);
}

int main()
{
    cout << "Mailslot Server\n";

    HANDLE hM;
    DWORD rb;
    char rbuf[300];
    DWORD timeout = 180000;

    try
    {
        hM = CreateMailslotW(
            MAILSLOT_NAME,
            500,
            timeout,
            nullptr
        );

        if (hM == INVALID_HANDLE_VALUE)
            throw SetErrorMsgText("CreateMailslot failed", GetLastError());

        cout << "Server started, waiting for messages...\n";

        while (true)
        {
            if (ReadFile(hM, rbuf, sizeof(rbuf) - 1, &rb, nullptr))
            {
                rbuf[rb] = '\0';  
                cout << rbuf << endl;
            }
            else
            {
                DWORD err = GetLastError();

                if (err == ERROR_SEM_TIMEOUT)
                    continue;

                throw SetErrorMsgText("ReadFile failed", err);
            }
        }

        CloseHandle(hM);
    }
    catch (const string& error)
    {
        cout << "\nERROR: " << error << endl;
    }

    return 0;
}
