#include <iostream>
#include <cstring>
#include <ctime>
#include "ОССП_Лабораторная_работа_10_HT.h"

namespace HT {

	static void setError(HTHANDLE* ht, const char* msg)
	{
		if (ht && msg)
			strncpy_s(ht->LastErrorMessage, msg, sizeof(ht->LastErrorMessage) - 1);
	}

	HTHANDLE::HTHANDLE() {
		Capacity = 0;
		SecSnapshotInterval = 0;
		MaxKeyLength = 0;
		MaxPayloadLength = 0;
		FileName[0] = '\0';
		File = NULL;
		FileMapping = NULL;
		Addr = NULL;
		LastErrorMessage[0] = '\0';
		lastsnaptime = time(NULL);
	}

	HTHANDLE::HTHANDLE(int Capacity, int SecSnapshotInterval,
		int MaxKeyLength, int MaxPayloadLength,
		const char FileName[512]) {
		this->Capacity = Capacity;
		this->SecSnapshotInterval = SecSnapshotInterval;
		this->MaxKeyLength = MaxKeyLength;
		this->MaxPayloadLength = MaxPayloadLength;
		this->FileName;

		strncpy(this->FileName, FileName, sizeof(this->FileName));
		this->FileName[sizeof(this->FileName) - 1] = '\0';

		File = NULL;
		FileMapping = NULL;
		Addr = NULL;
		LastErrorMessage[0] = '\0';
		lastsnaptime = time(NULL);
	}

	Element::Element() {
		key = nullptr;
		keylength = 0;
		payload = nullptr;
		payloadlength = 0;
	}

	Element::Element(const void* key, int keylength) {
		this->key = key;
		this->keylength = keylength;
		payload = nullptr;
		payloadlength = 0;
	}

	Element::Element(const void* key, int keylength, 
		const void* payload, int  payloadlength) {
		this->key = key;
		this->keylength = keylength;
		this->payload = payload;
		this->payloadlength = payloadlength;
	}

	Element::Element(Element* oldelement, const void* newpayload,
		int  newpayloadlength) {
		key = oldelement->key;
		keylength = oldelement->keylength;
		payload = newpayload;
		payloadlength = newpayloadlength;
	}

	HTHANDLE* Create(int	  Capacity,					   // емкость хранилища
		int   SecSnapshotInterval,		   // переодичность сохранения в сек.
		int   MaxKeyLength,                // максимальный размер ключа
		int   MaxPayloadLength,            // максимальный размер данных
		const char  FileName[512]) {

		HTHANDLE* ht = new HTHANDLE(Capacity, SecSnapshotInterval, MaxKeyLength,
			MaxPayloadLength, FileName);

		HANDLE hfile = CreateFileA(FileName,
			GENERIC_READ | GENERIC_WRITE,
			0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

		if (hfile == INVALID_HANDLE_VALUE) {
			setError(ht, "INVALID HANDLE VALUE");
			delete ht;
			return NULL;
		}
		ht->File = hfile;

		// Размер файла = я не очень понимаю почему это так + служебные данные хз фиксированные
		DWORD filesize = Capacity * (MaxKeyLength + MaxPayloadLength + 32);
		ht->FileMapping = CreateFileMapping(hfile, NULL, PAGE_READWRITE, 0, filesize, NULL);

		if (ht->FileMapping == NULL) {
			strcpy_s(ht->LastErrorMessage, "Error: cannot create file mapping");
			CloseHandle(ht->File);
			delete ht;
			return NULL;
		}

		ht->Addr = MapViewOfFile(ht->FileMapping, FILE_MAP_ALL_ACCESS, 0, 0, 0);

		if (ht->Addr == NULL) {
			strcpy_s(ht->LastErrorMessage, "Error: cannot map view of file");
			CloseHandle(ht->FileMapping);
			CloseHandle(ht->File);
			delete ht;
			return NULL;
		}

		return ht;
	}

	HTHANDLE* Open(const char FileName[512]) {
		HTHANDLE* ht = new HTHANDLE();

		strncpy(ht->FileName, FileName, sizeof(ht->FileName));
		ht->FileName[sizeof(ht->FileName) - 1] = '\0';

		ht->File = CreateFileA(FileName, GENERIC_READ | GENERIC_WRITE,
			0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

		if (ht->File == INVALID_HANDLE_VALUE) {
			strcpy_s(ht->LastErrorMessage, "Error: cannot open file");
			delete ht;
			return NULL;
		}

		ht->FileMapping = CreateFileMappingA(ht->File, NULL, PAGE_READWRITE, 0, 0, NULL);
		if (ht->FileMapping == NULL) {
			strcpy_s(ht->LastErrorMessage, "Error: cannot create file mapping");
			CloseHandle(ht->File);
			delete ht;
			return NULL;
		}

		ht->Addr = MapViewOfFile(ht->FileMapping, FILE_MAP_ALL_ACCESS, 0, 0, 0);
		if (ht->Addr == NULL) {
			strcpy_s(ht->LastErrorMessage, "Error: cannot map view of file");
			CloseHandle(ht->FileMapping);
			CloseHandle(ht->File);
			delete ht;
			return NULL;
		}

		HTHANDLE* fromFile = (HTHANDLE*)ht->Addr;
		ht->Capacity = fromFile->Capacity;
		ht->SecSnapshotInterval = fromFile->SecSnapshotInterval;
		ht->MaxKeyLength = fromFile->MaxKeyLength;
		ht->MaxPayloadLength = fromFile->MaxPayloadLength;
		ht->lastsnaptime = fromFile->lastsnaptime;

		return ht;
	}

	BOOL Snap(const HTHANDLE* hthandle) {
		if (hthandle == nullptr || hthandle->Addr == nullptr) return FALSE;

		if (!FlushViewOfFile(hthandle->Addr, 0)) {
			return FALSE;
		}

		HTHANDLE* nonConstHt = const_cast<HTHANDLE*>(hthandle);
		nonConstHt->lastsnaptime = time(nullptr);
		memcpy(nonConstHt->Addr, nonConstHt, sizeof(HTHANDLE));

		return TRUE;
	}

	BOOL Close(const HTHANDLE* hthandle) {

		BOOL res = TRUE;

		if (hthandle == nullptr) return FALSE;

		if (!Snap(hthandle)) return FALSE;

		if (hthandle->Addr != nullptr) UnmapViewOfFile(hthandle->Addr);

		if (hthandle->FileMapping != nullptr) CloseHandle(hthandle->Addr);

		if (hthandle->File != INVALID_HANDLE_VALUE) CloseHandle(hthandle->Addr);

		delete hthandle;

		return res;
	}

	BOOL Insert(const HTHANDLE* hthandle,            // управление HT
		const Element* element) {
		if (hthandle == nullptr || hthandle->Addr == nullptr || 
			element == nullptr || element->key == nullptr) return FALSE;

		Element* existing = Get(hthandle, element);
		if (existing == nullptr) {
			delete existing;
			strncpy_s(const_cast<HTHANDLE*>(hthandle)->LastErrorMessage,
				"Insert: key length exceeds MaxKeyLength", sizeof(hthandle->LastErrorMessage));
			return FALSE;
		}

	}
}