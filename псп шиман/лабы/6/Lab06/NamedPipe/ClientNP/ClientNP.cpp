#include <iostream>
#include <windows.h>
#include <string>

using namespace std;

#define MAX_SIZE_OF_BUFFER 512
#define PIPE_NAME L"\\\\.\\pipe\\Tube"
#define PIPE_NAME_LAN L"\\\\DESKTOP-T12FKAO\\pipe\\Tube111"

string GetErrorMsgText(int code);
string SetPipeError(string msgText, int code);

int main()
{
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    HANDLE cH; 
    DWORD dwWrite;
    char buffer[50] = "start";

    try
    {

        if ((cH = CreateFileW(
            PIPE_NAME_LAN,      
            GENERIC_READ | GENERIC_WRITE, 
            0,                           
            NULL,                        
            OPEN_EXISTING,              
            0,
            NULL)) == INVALID_HANDLE_VALUE)
        {
            cout << GetLastError();
            throw SetPipeError("CreateFile: ", GetLastError());
        }

        int countOfMessages;

        cout << "Enter the number of messages: ";
        cin >> countOfMessages;

        cin.ignore();

        cout << "Enter the message: ";
        cin.getline(buffer, sizeof(buffer));

        for (int i = 1; i <= countOfMessages; i++)
        {
      
            if (!WriteFile(cH, buffer, strlen(buffer), &dwWrite, NULL))
            {
                throw SetPipeError("WriteFile: ", GetLastError());
            }
            cout << "Sent message to server: " << buffer << endl;

            DWORD dwRead;
            if (!ReadFile(cH, buffer, MAX_SIZE_OF_BUFFER, &dwRead, NULL))
            {
                cout << GetLastError();
                throw SetPipeError("ReadFile: ", GetLastError());
            }
            buffer[dwRead] = '\0';
            cout << "Server sent back: " << buffer << endl;
            cout << "====================" << endl;
        }

        CloseHandle(cH);
    }
    catch (string ErrorPipeText)
    {
        cout << "\nError in ClientNP: " << ErrorPipeText;
    }

    return 0;
}

string GetErrorMsgText(int code)
{
    switch (code)
    {
    case ERROR_PIPE_BUSY: return "All pipe instances are busy";
    case ERROR_FILE_NOT_FOUND: return "Server not found";
    case ERROR_ACCESS_DENIED: return "Access denied";
    case ERROR_PIPE_CONNECTED: return "There is a process at the other end of the pipe";
    default: return "**ERROR**";
    }
}


string SetPipeError(string msgText, int code)
{
    return msgText + GetErrorMsgText(code) + "\n";
}
