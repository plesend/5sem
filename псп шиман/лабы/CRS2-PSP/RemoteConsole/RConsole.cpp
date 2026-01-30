#include <stdio.h>
#include <tchar.h>
#include <iostream>
#include <Windows.h>
#include <ctime>
#include <string>

using std::string;
using namespace std;

string GetErrorMsgText(int code) 
{
    char buff[50];
    DWORD bufflen = sizeof(buff);
    DWORD errorMessageID = ::GetLastError();
    if (errorMessageID == 0) {
        return std::string(); 
    }
    else {
        return std::system_category().message(errorMessageID);
    }
}

string SetPipeError(string msgText, int code)
{
    return msgText + to_string(code) + ". " + GetErrorMsgText(code);
}

int _tmain(int argc, _TCHAR* argv[]) {
    setlocale(LC_ALL, "Russian");

    char ReadBuf[256] = "";
    char WriteBuf[10] = "";
    DWORD nBytesRead;
    DWORD nBytesWrite;

    int Code = 0;

    const char* serverName = "kvasik";
    const char* pipeName = "kvasik";
    char fullPipeName[512];

    sprintf_s(fullPipeName, "\\\\%s\\pipe\\%s", serverName, pipeName);

    try
    {
        printf_s("\n=== Доступные команды === \n");
        printf_s("1 - start  \t (разрешить подключение клиентов к серверу)\n");
        printf_s("2 - stop  \t (запретить подключение клиентов к серверу)\n");
        printf_s("3 - exit  \t (завершить работу сервера)\n");
        printf_s("4 - statistics\t (вывод статистики)\n");
        printf_s("5 - wait  \t (приостанавливает подключение клиентов)\n");
        printf_s("6 - shutdown  \t (короче ждать завершения, потом сервер выключается wait + exit\n");
        printf_s("0 - закрыть\n");
        printf_s("\n==================================================== \n");

        HANDLE hNamedPipe = CreateFile(
            fullPipeName,
            GENERIC_READ | GENERIC_WRITE,
            0,
            NULL,
            OPEN_EXISTING,
            0,
            NULL
        );

        if (hNamedPipe == INVALID_HANDLE_VALUE) {
            cout << "Не удалось подключиться к каналу: " << GetErrorMsgText(GetLastError()) << endl;
            return 1;
        }

        do {
            printf_s("Команда: ");
            scanf_s("%d", &Code);

            if (Code > 0 && Code < 7) {
                sprintf_s(WriteBuf, "%d", Code - 1);
                if (!WriteFile(hNamedPipe, WriteBuf, (DWORD)strlen(WriteBuf) + 1, &nBytesWrite, NULL))
                    throw "WriteFile: Ошибка ";
                if (!ReadFile(hNamedPipe, ReadBuf, sizeof(ReadBuf), &nBytesRead, NULL))
                    throw "ReadFile: Ошибка ";
                cout << ReadBuf << endl;
            }

            if (Code == 0 || Code == 3 || Code == 6) {
                cout << "Работа консоли завершена" << endl;
                break;
            }
        } while (true);

        if (!CloseHandle(hNamedPipe))
            throw SetPipeError("CloseHandle: ", GetLastError());
    }
    catch (string ErrorPipeText)
    {
        cout << endl << ErrorPipeText;
    }

    return 0;
}
