#pragma once
#include "Global.h"

DWORD WINAPI EchoServer(LPVOID lParam) {
	DWORD rc = 0; 
	Contact* client = (Contact*)lParam;
	char ibuf[50], obuf[50] = "Close: finish;";

	try 
	{
		QueueUserAPC(ASStartMessage, client->hAcceptServer, (DWORD)client);	
	
		client->sthread = Contact::WORK;
		while (client->TimerOff == false) {	
			if (recv(client->s, ibuf, sizeof(ibuf), NULL) == SOCKET_ERROR) {
				if (WSAGetLastError() == WSAEWOULDBLOCK) Sleep(100);
				else throw  SetErrorMsgText("Recv:", WSAGetLastError());
			}
			else {
				if (strcmp(ibuf, "exit") != 0) {
					if (client->TimerOff != false) break;	
					if ((send(client->s, ibuf, sizeof(ibuf), NULL)) == SOCKET_ERROR) throw  SetErrorMsgText("Send:", WSAGetLastError());
				}
				else break;
			}
		}
		if (client->TimerOff == false) {
			CancelWaitableTimer(client->htimer);	
			if ((send(client->s, obuf, sizeof(obuf) + 1, NULL)) == SOCKET_ERROR) throw  SetErrorMsgText("Send:", WSAGetLastError());
			client->sthread = Contact::FINISH;
			QueueUserAPC(ASFinishMessage, client->hAcceptServer, (DWORD)client);	
		}
	}
	catch (string errorMsgText) {
		std::cout << errorMsgText << std::endl;
		CancelWaitableTimer(client->htimer);
		client->sthread = Contact::ABORT;
	}
	ExitThread(rc);
}