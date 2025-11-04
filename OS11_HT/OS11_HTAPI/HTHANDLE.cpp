#include "pch.h"
#include "HT.h"


HTHANDLE::HTHANDLE()
	: Capacity(0),
	SecSnapshotInterval(0),
	MaxKeyLength(0),
	MaxPayloadLength(0),
	File(INVALID_HANDLE_VALUE),
	FileMapping(NULL),
	Addr(NULL),
	lastsnaptime(0)
{
	FileName[0] = '\0';
	LastErrorMessage[0] = '\0';
}

HTHANDLE::HTHANDLE(int Capacity_, int SecSnapshotInterval_, int MaxKeyLength_, int MaxPayloadLength_, const char FileName_[512])
	: Capacity(Capacity_),
	SecSnapshotInterval(SecSnapshotInterval_),
	MaxKeyLength(MaxKeyLength_),
	MaxPayloadLength(MaxPayloadLength_),
	File(INVALID_HANDLE_VALUE),
	FileMapping(NULL),
	Addr(NULL),
	lastsnaptime(0)
{
	memset(FileName, 0, sizeof(FileName));
	if (FileName_) {
		strncpy_s(FileName, FileName_, _countof(FileName) - 1);
	}
	LastErrorMessage[0] = '\0';
}
