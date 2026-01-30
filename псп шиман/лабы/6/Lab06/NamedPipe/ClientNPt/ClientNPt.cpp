#include <iostream>
#include <windows.h>
#include <string>

#pragma warning(disable:4996)

using namespace std;

#define PIPE_NAME L"\\\\DESKTOP-T12FKAO\\pipe\\Tube111"
#define MAX_SIZE_OF_BUFFER 512

string GetErrorMsgText(int code)
{
    switch (code)
    {
    case ERROR_FILE_NOT_FOUND: return "Server not found";
    case ERROR_ACCESS_DENIED: return "Access denied";
    case ERROR_PIPE_BUSY: return "All pipe instances are busy";
    case ERROR_NO_DATA: return "The pipe has been closed";
    case ERROR_PIPE_CONNECTED: return "The pipe is already connected to another process";
    default: return "Unknown error: " + to_string(code);
    }
}

string SetPipeError(string msgText, int code)
{
    return msgText + GetErrorMsgText(code) + "\n";
}

int main()
{
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    char buffer[] = "Hello Server";
    char outbuffer[MAX_SIZE_OF_BUFFER] = { 0 };
    DWORD bytesRead = 0;

    try
    {
        HANDLE hPipe = CreateFile(
            PIPE_NAME,
            GENERIC_READ | GENERIC_WRITE,
            0,            
            NULL,          
            OPEN_EXISTING,  
            0,
            NULL
        );

        if (hPipe == INVALID_HANDLE_VALUE)
            throw SetPipeError("CreateFile: ", GetLastError());

        DWORD mode = PIPE_READMODE_MESSAGE;
        if (!SetNamedPipeHandleState(hPipe, &mode, NULL, NULL))
            throw SetPipeError("SetNamedPipeHandleState: ", GetLastError());

        int countOfMessages;
        cout << "Enter the number of messages: ";
        cin >> countOfMessages;
        cin.ignore();

        for (int i = 0; i < countOfMessages; i++) {

            cout << "Sending to server: " << buffer << endl;

            if (!TransactNamedPipe(
                hPipe,
                buffer,
                (DWORD)strlen(buffer),
                outbuffer,
                MAX_SIZE_OF_BUFFER,
                &bytesRead,
                NULL))
            {
                throw SetPipeError("TransactNamedPipe: ", GetLastError());
            }

            outbuffer[bytesRead] = '\0';
            cout << "Server replied: " << outbuffer << endl;
        }

        CloseHandle(hPipe);
    }
    catch (string errorMsg)
    {
        cout << "Client error: " << errorMsg << endl;
    }

    return 0;
}
