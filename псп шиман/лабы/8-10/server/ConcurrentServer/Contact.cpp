#define _CRT_SECURE_NO_WARNINGS
#include <list>
#include "Winsock2.h"
#include "Contact.h"
Contact::Contact(TE t, const char* namesrv)
{
	memset(&prms, 0, sizeof(SOCKADDR_IN));
	lprms = sizeof(SOCKADDR_IN);
	type = t;
	strcpy(srvname, namesrv);
	msg[0] = 0;
}
void Contact::SetST(ST sth, const char* m)
{
	sthread = sth;
	strcpy(msg, m);
}