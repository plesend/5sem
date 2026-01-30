#include <iostream>
#include <windows.h>
#include <string>

using namespace std;

#define PIPE_NAME L"\\\\.\\pipe\\Tube111"
#define MAX_SIZE_OF_BUFFER 512

string GetErrorMsgText(int code)
{
    switch (code)
    {
    case ERROR_PIPE_BUSY: return "All pipe instances are busy";
    case ERROR_FILE_NOT_FOUND: return "Server not found";
    case ERROR_ACCESS_DENIED: return "Access denied";
    case ERROR_PIPE_CONNECTED: return "There is a process at the other end of the pipe";
    case ERROR_BROKEN_PIPE: return "CONNECTION DISTURBED BY CLIENT";
    default: return "Change internet adapter permission properties";
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

    try
    {
        SECURITY_ATTRIBUTES sa;
        SECURITY_DESCRIPTOR sd;

        InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);
        SetSecurityDescriptorDacl(&sd, TRUE, NULL, FALSE);    
        sa.nLength = sizeof(sa);
        sa.lpSecurityDescriptor = &sd;
        sa.bInheritHandle = FALSE;

        HANDLE sH = CreateNamedPipe(
            PIPE_NAME,
            PIPE_ACCESS_DUPLEX,
            PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
            PIPE_UNLIMITED_INSTANCES,
            MAX_SIZE_OF_BUFFER,
            MAX_SIZE_OF_BUFFER,
            0,
            &sa
 
        );

        if (sH == INVALID_HANDLE_VALUE)
            throw SetPipeError("CreateNamedPipe: ", GetLastError());

        cout << "Server is running.Waiting for client...." << endl;

        while (true)
        {
            if (!ConnectNamedPipe(sH, NULL))
            {
                DWORD err = GetLastError();
                if (err != ERROR_PIPE_CONNECTED)
                    throw SetPipeError("ConnectNamedPipe: ", err);
            }

            cout << "! Client connected !" << endl;

            char buffer[MAX_SIZE_OF_BUFFER];
            DWORD dwRead, dwWritten;

            while (true)
            {
                BOOL fSuccess = ReadFile(sH, buffer, MAX_SIZE_OF_BUFFER - 1, &dwRead, NULL);
                if (!fSuccess)
                {
                    DWORD err = GetLastError();
                    if (err == ERROR_BROKEN_PIPE)
                    {
                        cout << "Client disconnected" << endl;
                        cout << "======================" << endl;
                        break;     
                    }
                    else
                        throw SetPipeError("ReadFile: ", err);
                }

                buffer[dwRead] = '\0';   
                cout << "Client sent: " << buffer << endl;

                fSuccess = WriteFile(sH, buffer, dwRead, &dwWritten, NULL);
                if (!fSuccess)
                    throw SetPipeError("WriteFile: ", GetLastError());
                cout << "Sent to client: " << buffer << endl;
            }

            DisconnectNamedPipe(sH);
            cout << "Waiting for new client..." << endl;
        }

        CloseHandle(sH);
    }
    catch (string ErrorPipeText)
    {
        cout << "\nError in ServerNP: " << ErrorPipeText << endl;
    }

    return 0;
}
