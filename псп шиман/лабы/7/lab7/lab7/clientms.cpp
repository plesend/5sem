#include <windows.h>
#include <iostream>
#include <string>
#include <ctime>

using namespace std;

string GetErrorMsgText(DWORD code)
{
    switch (code)
    {
    case ERROR_FILE_NOT_FOUND: return "File not found";
    case ERROR_ACCESS_DENIED:  return "Access denied";
    case ERROR_INVALID_HANDLE:return "Invalid handle";
    case ERROR_NOT_ENOUGH_MEMORY: return "Not enough memory";
    default:
        return "Error code: " + to_string(code);
    }
}

string SetErrorMsgText(const string& msgText, DWORD code)
{
    return msgText + ": " + GetErrorMsgText(code);
}

#define MAILSLOT_NAME L"\\\\.\\mailslot\\Box"

int main()
{
    cout << "Mailslot Client\n";

    HANDLE hM;
    DWORD wb;
    clock_t start, end;

    char obuf[50];

    try
    {
        hM = CreateFileW(
            MAILSLOT_NAME,
            GENERIC_WRITE,
            FILE_SHARE_READ,
            nullptr,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            nullptr
        );

        if (hM == INVALID_HANDLE_VALUE)
            throw SetErrorMsgText("CreateFile failed", GetLastError());

        start = clock();

        for (int i = 0; i < 1000; i++)
        {
            string msg = "hello " + to_string(i + 1);

            strcpy_s(obuf, sizeof(obuf), msg.c_str());

            if (!WriteFile(hM, obuf, (DWORD)strlen(obuf) + 1, &wb, nullptr))
                throw SetErrorMsgText("WriteFile failed", GetLastError());

            cout << obuf << endl;
        }

        end = clock();

        cout << "\nTime elapsed: "
            << double(end - start) / CLOCKS_PER_SEC
            << " seconds\n";

        CloseHandle(hM);
    }
    catch (const string& error)
    {
        cout << "\nERROR: " << error << endl;
    }

    return 0;
}
