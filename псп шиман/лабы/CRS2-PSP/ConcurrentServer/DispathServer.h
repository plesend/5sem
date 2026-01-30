#pragma once
#include "Global.h"

DWORD WINAPI DispathServer(LPVOID pPrm) {
	DWORD rc = 0;
	try {
		while (*((TalkersCommand*)pPrm) != EXIT) {
			try {
				if (WaitForSingleObject(Event, 300) == WAIT_OBJECT_0) {

					if (&ClientServiceNumber > (volatile LONG*)0) {
						Contact* client = NULL;
						int libuf = 1;
						char CallBuf[50] = "", SendError[50] = "ErrorInquiry";
						EnterCriticalSection(&scListContact);

						for (ListContact::iterator p = Contacts.begin(); p != Contacts.end(); p++) {
							if (p->type == Contact::ACCEPT && p->sthread != Contact::ABORT)
							{
								client = &(*p);

								bool flag = false;
								while (flag == false) {
									if ((libuf = recv(client->s, CallBuf, sizeof(CallBuf), NULL)) == SOCKET_ERROR) {
										switch (WSAGetLastError()) {
											case WSAEWOULDBLOCK: Sleep(100); break;
											default: throw  SetErrorMsgText("Recv:", WSAGetLastError());
										}
									}
									else flag = true;
								}

								if (strcmp(CallBuf, "Echo") != 0 && strcmp(CallBuf, "Time") != 0 && strcmp(CallBuf, "Rand") != 0)
								{
									flag = false;
								}

								if (flag == true) {
									string time_error;

									
									client->type = Contact::CONTACT;
									strcpy_s(client->srvname, CallBuf);

									client->htimer = CreateWaitableTimer(NULL, false, NULL); 
									_int64 time = -1800000000; // в наносекундах
									SetWaitableTimer(client->htimer, (LARGE_INTEGER*)&time, 0, ASWTimer, client, false); 
									cout << CallBuf <<  " сервер вызван" << endl;
									if ((libuf = send(client->s, CallBuf, sizeof(CallBuf), NULL)) == SOCKET_ERROR) throw SetErrorMsgText("Send:", WSAGetLastError());
									client->hthread = sss(CallBuf, client);
								}
								else {

									if ((libuf = send(client->s, SendError, sizeof(SendError) + 1, NULL)) == SOCKET_ERROR) throw SetErrorMsgText("Send:", WSAGetLastError());
									closesocket(client->s);
									client->sthread = Contact::ABORT;
									CancelWaitableTimer(client->htimer);
									InterlockedIncrement(&Fail);
								}
							}
						}
						LeaveCriticalSection(&scListContact);
					}
					SleepEx(0, true);
				}
				SleepEx(0, true);
			}
			catch (string errorMsgText) {
				std::cout << errorMsgText << endl;
			}
		}
	}
	catch (string errorMsgText) {
		std::cout << errorMsgText << endl;
	}
	catch (...) {
		std::cout << "Error DispathServer" << std::endl;
	}
	cout << "DispathServer остановлен\n" << endl;
	ExitThread(rc);
}