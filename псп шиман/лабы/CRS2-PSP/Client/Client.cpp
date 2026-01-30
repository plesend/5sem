#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <tchar.h>
#include "iostream"
#include "Winsock2.h"                
#pragma comment(lib, "WS2_32.lib")   
#include <string>
#include <ctime>
#include "time.h"

using namespace std;

bool SystemMessage(char* ch) {

	bool result = false;
	try {

		char Timeout[50] = "Close: timeout;", Close[50] = "Close: finish;", Abort[50] = "Close: Abort;";
		if (strcmp(ch, Timeout) == 0) result = true;
		else if (strcmp(ch, Abort) == 0) result = true;
		else if (strcmp(ch, Close) == 0) result = true;
	}
	catch (...) {
		cout << "err" << endl;
	}

	return result;
}

string  GetErrorMsgText(int code) {
	string msgText;
	switch (code) {
	case WSAEINTR:                  msgText = "WSAEINTR";               break;    //Работа функции прервана
	case WSAEACCES:                 msgText = "WSAEACCES";              break;    //Разрешение отвергнуто
	case WSAEFAULT:                 msgText = "WSAEFAULT";              break;    //Ошибочный адрес
	case WSAEINVAL:                 msgText = "WSAEINVAL";              break;    //Ошибка в аргументе
	case WSAEMFILE:                 msgText = "WSAEMFILE";              break;    //Слишком много файлов открыто
	case WSAEWOULDBLOCK:            msgText = "WSAEWOULDBLOCK";         break;    //Ресурс временно недоступен
	case WSAEINPROGRESS:            msgText = "WSAEINPROGRESS";         break;    //Операция в процессе развития
	case WSAEALREADY:               msgText = "WSAEALREADY";            break;    //Операция уже выполняется
	case WSAENOTSOCK:               msgText = "WSAENOTSOCK";            break;    //Сокет задан неправильно
	case WSAEDESTADDRREQ:           msgText = "WSAEDESTADDRREQ";        break;    //Требуется адрес расположения
	case WSAEMSGSIZE:               msgText = "WSAEMSGSIZE";            break;    //Сообщение слишком длинное
	case WSAEPROTOTYPE:             msgText = "WSAEPROTOTYPE";          break;    //Неправильный тип протокола для сокета
	case WSAENOPROTOOPT:            msgText = "WSAENOPROTOOPT";         break;    //Ошибка в опции протокола
	case WSAEPROTONOSUPPORT:        msgText = "WSAEPROTONOSUPPORT";     break;    //Протокол не поддерживается
	case WSAESOCKTNOSUPPORT:        msgText = "WSAESOCKTNOSUPPORT";     break;    //Тип сокета не поддерживается
	case WSAEOPNOTSUPP:             msgText = "WSAEOPNOTSUPP";          break;    //Операция не поддерживается
	case WSAEPFNOSUPPORT:           msgText = "WSAEPFNOSUPPORT";        break;    //Тип протоколов не поддерживается
	case WSAEAFNOSUPPORT:           msgText = "WSAEAFNOSUPPORT";        break;    //Тип адресов не поддерживается протоколом
	case WSAEADDRINUSE:             msgText = "WSAEADDRINUSE";          break;    //Адрес уже используется
	case WSAEADDRNOTAVAIL:          msgText = "WSAEADDRNOTAVAIL";       break;    //Запрошенный адрес не может быть использован
	case WSAENETDOWN:               msgText = "WSAENETDOWN";            break;    //Сеть отключена
	case WSAENETUNREACH:            msgText = "WSAENETUNREACH";         break;    //Сеть не достижима
	case WSAENETRESET:              msgText = "WSAENETRESET";           break;    //Сеть разорвала соединение
	case WSAECONNABORTED:           msgText = "WSAECONNABORTED";        break;    //Программный отказ связи
	case WSAECONNRESET:             msgText = "WSAECONNRESET";          break;    //Связь восстановлена
	case WSAENOBUFS:                msgText = "WSAENOBUFS";             break;    //Не хватает памяти для буферов
	case WSAEISCONN:                msgText = "WSAEISCONN";             break;    //Сокет уже подключен
	case WSAENOTCONN:               msgText = "WSAENOTCONN";            break;    //Сокет не подключен
	case WSAESHUTDOWN:              msgText = "WSAESHUTDOWN";           break;    //Нельзя выполнить send: сокет завершил работу
	case WSAETIMEDOUT:              msgText = "WSAETIMEDOUT";           break;    //Закончился отведенный интервал  времени
	case WSAECONNREFUSED:           msgText = "WSAECONNREFUSED";        break;    //Соединение отклонено
	case WSAEHOSTDOWN:              msgText = "WSAEHOSTDOWN";           break;    //Хост в неработоспособном состоянии
	case WSAEHOSTUNREACH:           msgText = "WSAEHOSTUNREACH";        break;    //Нет маршрута для хоста
	case WSAEPROCLIM:               msgText = "WSAEPROCLIM";            break;    //Слишком много процессов
	case WSASYSNOTREADY:            msgText = "WSASYSNOTREADY";         break;    //Сеть не доступна
	case WSAVERNOTSUPPORTED:        msgText = "WSAVERNOTSUPPORTED";     break;    //Данная версия недоступна
	case WSANOTINITIALISED:         msgText = "WSANOTINITIALISED";      break;    //Не выполнена инициализация WS2_32.DLL
	case WSAEDISCON:                msgText = "WSAEDISCON";             break;    //Выполняется отключение
	case WSATYPE_NOT_FOUND:         msgText = "WSATYPE_NOT_FOUND";      break;    //Класс не найден
	case WSAHOST_NOT_FOUND:         msgText = "WSAHOST_NOT_FOUND";      break;    //Хост не найден
	case WSATRY_AGAIN:              msgText = "WSATRY_AGAIN";           break;    //Неавторизированный хост не найден
	case WSANO_RECOVERY:            msgText = "WSANO_RECOVERY";         break;    //Неопределенная  ошибка
	case WSANO_DATA:                msgText = "WSANO_DATA";             break;    //Нет записи запрошенного типа
	case WSA_INVALID_HANDLE:      msgText = "WSA_INVALID_HANDLE";     break;    //Указанный дескриптор события  с ошибкой
	case WSA_INVALID_PARAMETER:   msgText = "WSA_INVALID_PARAMETER";  break;    //Один или более параметров с ошибкой
	case WSA_IO_INCOMPLETE:       msgText = "WSA_IO_INCOMPLETE";      break;    //Объект ввода-вывода не в сигнальном состоянии
	case WSA_IO_PENDING:          msgText = "WSA_IO_PENDING";         break;    //Операция завершится позже
	case WSA_NOT_ENOUGH_MEMORY:   msgText = "WSA_NOT_ENOUGH_MEMORY";  break;    //Не достаточно памяти
	case WSA_OPERATION_ABORTED:   msgText = "WSA_OPERATION_ABORTED";  break;    //Операция отвергнута
	case WSASYSCALLFAILURE:         msgText = "WSASYSCALLFAILURE";      break;
	default:                msgText = "***ERROR***";      break;
	};
	return msgText;
}

string  SetErrorMsgText(string msgText, int code) {
	return  msgText + GetErrorMsgText(code);
}

string error = "Error";

string GetError() {
	return error;
}

bool GetServer(char* call, SOCKADDR_IN* from, int* flen, SOCKET* cC, SOCKADDR_IN* all) {
	char ibuf[50], obuf[50];
	int  libuf = 0, lobuf = 0;

	if ((lobuf = sendto(*cC, call, strlen(call) + 1, NULL, (sockaddr*)all, sizeof(*all))) == SOCKET_ERROR) throw  SetErrorMsgText("Sendto:", WSAGetLastError());
	if ((libuf = recvfrom(*cC, ibuf, sizeof(ibuf), NULL, (sockaddr*)from, flen)) == SOCKET_ERROR) {
		if (WSAGetLastError() == WSAETIMEDOUT) return false;
		else   {
			std::cout << WSAGetLastError();
			throw SetErrorMsgText("Recv:", WSAGetLastError());
		}
	}
	if (strcmp(call, ibuf) == 0) return true;
	else return false;
}

const char* DEFAULT_SERVER_NAME = "kvasik";
const int DEFAULT_PORT = 2000;

int _tmain(int argc, _TCHAR* argv[]) {
    setlocale(LC_ALL, "Russian");
    int port = DEFAULT_PORT;
    SOCKET SocketTCP;
    WSADATA wsaData;

    try {
        if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0)
            throw SetErrorMsgText("Startup:", WSAGetLastError());
        if ((SocketTCP = socket(AF_INET, SOCK_STREAM, NULL)) == INVALID_SOCKET)
            throw SetErrorMsgText("Socket:", WSAGetLastError());

        char serverName[256] = "";
        char call[256] = "";
        int connectionType = 0;

        cout << "1 - Подключение по имени сервера \t 2 - Подключение по позывному (Enter = дефолт: kvasik)" << endl;
        string connInput;
        getline(cin, connInput);
        connectionType = connInput.empty() ? 1 : stoi(connInput); // дефолт = 1

        SOCKADDR_IN Server_IN = { 0 };
        Server_IN.sin_family = AF_INET;

        if (connectionType == 1) {
            cout << "Введите имя сервера (Enter = kvasik): ";
            cin.getline(serverName, sizeof(serverName));
            if (strlen(serverName) == 0) strcpy_s(serverName, sizeof(serverName), DEFAULT_SERVER_NAME);

            hostent* s = gethostbyname(serverName);
            if (!s) throw "Сервер не найден;";
            Server_IN.sin_addr = *(struct in_addr*)s->h_addr_list[0];

            cout << "Порт сервера (Enter = 2000): ";
            string portInput;
            getline(cin, portInput);
            port = portInput.empty() ? DEFAULT_PORT : stoi(portInput);
        }
        else if (connectionType == 2) {
            cout << "Введите позывной сервера (Enter = kvasik): ";
            cin.getline(call, sizeof(call));
            if (strlen(call) == 0) strcpy_s(call, sizeof(call), DEFAULT_SERVER_NAME);

            SOCKET SocketUDP;
            if ((SocketUDP = socket(AF_INET, SOCK_DGRAM, NULL)) == INVALID_SOCKET) throw SetErrorMsgText("Socket:", WSAGetLastError());
            int optval = 1;
            if (setsockopt(SocketUDP, SOL_SOCKET, SO_BROADCAST, (char*)&optval, sizeof(optval)) == SOCKET_ERROR)
                throw SetErrorMsgText("Opt:", WSAGetLastError());

            SOCKADDR_IN all = { 0 };
            all.sin_family = AF_INET;
            all.sin_port = htons(DEFAULT_PORT);
            all.sin_addr.s_addr = INADDR_BROADCAST;

            SOCKADDR_IN from = { 0 };
            int lc = sizeof(from);

            bool bsr = GetServer(call, &from, &lc, &SocketUDP, &all);
            if (!bsr) throw "Server not found;";
            Server_IN.sin_addr.s_addr = from.sin_addr.s_addr;

            if (closesocket(SocketUDP) == SOCKET_ERROR) throw SetErrorMsgText("Closesocket:", WSAGetLastError());

            cout << "Порт сервера (Enter = 2000): ";
            string portInput;
            getline(cin, portInput);
            port = portInput.empty() ? DEFAULT_PORT : stoi(portInput);
        }
        else throw "Неверный код подключения";

        Server_IN.sin_port = htons(port);

        if (connect(SocketTCP, (sockaddr*)&Server_IN, sizeof(Server_IN)) == SOCKET_ERROR)
            throw "Ошибка соединения";

        printf_s("\n\nКОМАНДЫ, РЕАЛИЗУЕМЫЕ СЕРВЕРОМ :)\n");
        int cCall;
        char Call[50];
        cout << "1 - Rand\n2 - Time\n3 - Echo\nВведите номер команды: ";
        cin >> cCall;
        cin.ignore(); // очищаем буфер после ввода числа
        switch (cCall) {
        case 1: strcpy_s(Call, "Rand"); break;
        case 2: strcpy_s(Call, "Time"); break;
        case 3: strcpy_s(Call, "Echo"); break;
        default: throw "Введён неверный код команды";
        }
        printf_s("\n===================================\n");

        // Отправка команды серверу
        if (send(SocketTCP, Call, sizeof(Call), 0) == SOCKET_ERROR)
            throw SetErrorMsgText("Send:", WSAGetLastError());

        char rCall[50];
        cout << "Ожидание ответа сервера..." << endl;
        if (recv(SocketTCP, rCall, sizeof(rCall), 0) == SOCKET_ERROR)
            throw SetErrorMsgText("Recv:", WSAGetLastError());

        if (strcmp(Call, rCall) != 0) throw "Сервис недоступен";

        u_long nonblk = 1;
        if (ioctlsocket(SocketTCP, FIONBIO, &nonblk) == SOCKET_ERROR)
            throw SetErrorMsgText("Ioctlsocket:", WSAGetLastError());

        char command[50], obuf[50];
        cout << "Введите команду для сервера: ";
        cin.getline(command, sizeof(command));

        bool rcv = true;
        clock_t StartTime = clock();

        try {
            while (true) {
                if (rcv) {
                    if (send(SocketTCP, command, sizeof(command), 0) == SOCKET_ERROR)
                        throw "Ошибка отправки";
                    rcv = false;
                }

                int recvLen = recv(SocketTCP, obuf, sizeof(obuf), 0);
                if (recvLen == SOCKET_ERROR) {
                    if (WSAGetLastError() == WSAEWOULDBLOCK) Sleep(100);
                    else throw SetErrorMsgText("Recv:", WSAGetLastError());
                }
                else {
                    if (SystemMessage(obuf)) {
                        printf("Завершение соединения: %s\n", obuf);
                        break;
                    }
                    else printf("Полученное сообщение: %s\n", obuf);
                    rcv = true;
                }
            }
        }
        catch (...) {
            cout << "Произошла ошибка при обмене сообщениями" << endl;
        }

        clock_t FinishTime = clock();
        printf("Время работы: %lf сек.\n", (double)(FinishTime - StartTime) / CLOCKS_PER_SEC);

        closesocket(SocketTCP);
        WSACleanup();
    }
    catch (const char* msg) {
        cout << msg << endl;
    }
    catch (...) {
        cout << "Произошла неизвестная ошибка" << endl;
    }

    system("pause");
    return 0;
}