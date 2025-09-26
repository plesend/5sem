#include <iostream>
#include <cstring>
#include <ctime>
#include "ОССП_Лабораторная_работа_10_HT.h"
#pragma disable warning(4996)
//как генерируется хеш, как решать коллизии, где генерируется хеш
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

		strncpy_s(this->FileName, FileName, sizeof(this->FileName));
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

		strncpy_s(ht->FileName, FileName, sizeof(ht->FileName));
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

		if (hthandle->FileMapping != nullptr) CloseHandle(hthandle->FileMapping);

		if (hthandle->File != INVALID_HANDLE_VALUE) CloseHandle(hthandle->File);

		delete hthandle;

		return res;
	}

	BOOL Insert(const HTHANDLE* hthandle,            
		const Element* element) {
		if (hthandle == nullptr || hthandle->Addr == nullptr || 
			element == nullptr || element->key == nullptr) return FALSE;

		Element* existing = Get(hthandle, element);
		if (existing != nullptr) {
			delete existing;
			strncpy_s(const_cast<HTHANDLE*>(hthandle)->LastErrorMessage,
				"Insert: key length exceeds MaxKeyLength", sizeof(hthandle->LastErrorMessage));
			return FALSE;
		}

		char* dataStart = static_cast<char*>(hthandle->Addr) + sizeof(HTHANDLE);

		memcpy(dataStart, element, sizeof(Element));

		time_t currentTime = time(nullptr);
		if (difftime(currentTime, hthandle->lastsnaptime) >= hthandle->SecSnapshotInterval) 
			Snap(hthandle);

		return TRUE;
	}

	BOOL Delete(const HTHANDLE* hthandle,            
		const Element* element) {
		if (hthandle == nullptr || hthandle->Addr == nullptr ||
			element == nullptr || element->key == nullptr) return FALSE;

		Element* existing = Get(hthandle, element);
		if (existing == nullptr) {
			delete existing;
			strncpy_s(const_cast<HTHANDLE*>(hthandle)->LastErrorMessage,
				"Delete: key not found", sizeof(hthandle->LastErrorMessage));
			return FALSE;
		}

		char* dataStart = static_cast<char*>(hthandle->Addr) + sizeof(HTHANDLE);
		memset(dataStart, 0, sizeof(Element));

		delete existing;

		time_t currentTime = time(nullptr);
		if (difftime(currentTime, hthandle->lastsnaptime) >= hthandle->SecSnapshotInterval)
			Snap(hthandle);

		return TRUE;
	}

	Element* Get(const HTHANDLE* hthandle, const Element* element) {
		if (!hthandle || !hthandle->Addr || !element || !element->key)
			return nullptr;

		char* dataStart = static_cast<char*>(hthandle->Addr) + sizeof(HTHANDLE);
		Element* storedElement = reinterpret_cast<Element*>(dataStart);

		if (storedElement->key &&
			storedElement->keylength == element->keylength &&
			memcmp(storedElement->key, element->key, element->keylength) == 0) {

			Element* result = new Element();
			result->key = storedElement->key;
			result->keylength = storedElement->keylength;
			result->payload = storedElement->payload;
			result->payloadlength = storedElement->payloadlength;

			return result;
		}

		return nullptr;
	}

	BOOL Update(const HTHANDLE* hthandle,            // управление HT
		const Element* oldelement,          // старый элемент (ключ, размер ключа)
		const void* newpayload,          // новые данные  
		int newpayloadlength) {

		if (hthandle == nullptr || oldelement == nullptr || 
			newpayload == nullptr) return FALSE;

		Element* existing = Get(hthandle, oldelement);
		if (existing == nullptr) {
			strncpy_s(const_cast<HTHANDLE*>(hthandle)->LastErrorMessage,
				"Update: key not found", sizeof(hthandle->LastErrorMessage));
			return FALSE;
		}

		char* dataStart = static_cast<char*>(hthandle->Addr) + sizeof(HTHANDLE);
		Element* storedElement = reinterpret_cast<Element*>(dataStart);

		storedElement->payload = newpayload;
		storedElement->payloadlength = newpayloadlength;

		delete existing;

		time_t currentTime = time(nullptr);
		if (difftime(currentTime, hthandle->lastsnaptime) >= hthandle->SecSnapshotInterval) {
			Snap(hthandle);
		}

		return TRUE;
	}

	char* GetLastError(HTHANDLE* ht) {
		if (ht == nullptr) {
			char buffer[] = "no HTHANDLE";
			return buffer;
		}
		return ht->LastErrorMessage;
	}

	void print(const Element* element)
	{
		if (element == nullptr) {
			std::cout << "[Print] Element not found." << std::endl;
			return;
		}

		std::cout << "[Print] Key: ";
		if (element->key != nullptr && element->keylength > 0) {
			for (int i = 0; i < element->keylength; i++) {
				std::cout << static_cast<const char*>(element->key)[i];
			}
		}
		else {
			std::cout << "<empty>";
		}
		std::cout << std::endl;

		std::cout << "[Print] Payload: ";
		if (element->payload != nullptr && element->payloadlength > 0) {
			for (int i = 0; i < element->payloadlength; i++) {
				std::cout << static_cast<const char*>(element->payload)[i];
			}
		}
		else {
			std::cout << "<empty>";
		}
		std::cout << std::endl;
	}

}