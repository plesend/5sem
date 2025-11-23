#include "HT.h"


namespace HT
{
	HTHANDLE::HTHANDLE()
	{
		this->Capacity = 0;
		this->SecSnapshotInterval = 0;
		this->MaxKeyLength = 0;
		this->MaxPayloadLength = 0;
		ZeroMemory(this->FileName, sizeof(this->FileName));
		this->File = NULL;
		this->FileMapping = NULL;
		this->Addr = NULL;
		ZeroMemory(this->LastErrorMessage, sizeof(this->LastErrorMessage));
		this->LastSnaptime = 0;

		this->Count = 0;
	}

	HTHANDLE::HTHANDLE(int capacity, int secSnapshotInterval, int maxKeyLength, int maxPayloadLength, const wchar_t* fileName) : HTHANDLE()
	{
		this->Capacity = capacity;
		this->SecSnapshotInterval = secSnapshotInterval;
		this->MaxKeyLength = maxKeyLength;
		this->MaxPayloadLength = maxPayloadLength;
		memcpy(this->FileName, fileName, sizeof(this->FileName));
	}

	HTHANDLE* Create(
		int	  capacity,					// емкость хранилища
		int   secSnapshotInterval,		// периодичность сохранения в сек.
		int   maxKeyLength,             // максимальный размер ключа
		int   maxPayloadLength,			// максимальный размер данных
		const wchar_t* fileName)		// имя файла 
	{
		HANDLE hf = CreateFile(
			fileName,
			GENERIC_WRITE | GENERIC_READ,
			NULL,
			NULL,
			OPEN_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL);
		if (hf == INVALID_HANDLE_VALUE)
			throw "create or open file failed";

		int sizeMap = sizeof(HTHANDLE) + GetElementSize(maxKeyLength, maxPayloadLength) * capacity;
		HANDLE hm = CreateFileMapping(
			hf,
			NULL,
			PAGE_READWRITE,
			0, sizeMap,
			fileName);
		if (!hm)
			throw "create File Mapping failed";

		LPVOID lp = MapViewOfFile(
			hm,
			FILE_MAP_ALL_ACCESS,
			0, 0, 0);
		if (!lp)
			return NULL;

		ZeroMemory(lp, sizeMap);

		HTHANDLE* htHandle = new(lp) HTHANDLE(capacity, secSnapshotInterval, maxKeyLength, maxPayloadLength, fileName);
		htHandle->File = hf;
		htHandle->FileMapping = hm;
		htHandle->Addr = lp;
		htHandle->LastSnaptime = time(NULL);

		htHandle->SnapshotTimer = CreateWaitableTimer(0, FALSE, 0);
		LARGE_INTEGER Li{};
		Li.QuadPart = -(10000000 * htHandle->SecSnapshotInterval);
		SetWaitableTimer(htHandle->SnapshotTimer, &Li, htHandle->SecSnapshotInterval * 1000, snapAsync, htHandle, FALSE);

		return htHandle;
	}


	void CALLBACK snapAsync(LPVOID prm, DWORD, DWORD)
	{
		HTHANDLE* htHandle = (HTHANDLE*)prm;
		if (Snap(htHandle))
			std::cout << "-- spanshotAsync success" << std::endl;
	}

	HTHANDLE* Open
	(
		const wchar_t* fileName)
	{
		HTHANDLE* htHandle = OpenHTFromFile(fileName);
		if (htHandle == NULL)
		{
			htHandle = OpenHTFromMapName(fileName);
			if (htHandle == NULL)
				return NULL;
		}

		htHandle->SnapshotTimer = CreateWaitableTimer(0, FALSE, 0);
		LARGE_INTEGER Li{};
		Li.QuadPart = -(10000000 * htHandle->SecSnapshotInterval);
		SetWaitableTimer(htHandle->SnapshotTimer, &Li, htHandle->SecSnapshotInterval * 1000, snapAsync, htHandle, FALSE);

		return htHandle;
	}

	HTHANDLE* OpenHTFromFile(
		const wchar_t* fileName)
	{
		HANDLE hf = CreateFile(
			fileName,
			GENERIC_WRITE | GENERIC_READ,
			NULL,
			NULL,
			OPEN_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL);
		if (hf == INVALID_HANDLE_VALUE)
			return NULL;

		HANDLE hm = CreateFileMapping(
			hf,
			NULL,
			PAGE_READWRITE,
			0, 0,
			fileName);
		if (!hm)
			return NULL;

		LPVOID lp = MapViewOfFile(
			hm,
			FILE_MAP_ALL_ACCESS,
			0, 0, 0);
		if (!lp)
			return NULL;

		HTHANDLE* htHandle = (HTHANDLE*)lp;
		htHandle->File = hf;
		htHandle->FileMapping = hm;
		htHandle->Addr = lp;
		htHandle->LastSnaptime = time(NULL);

		return htHandle;
	}

	HTHANDLE* OpenHTFromMapName(
		const wchar_t* fileName)
	{
		HANDLE hm = OpenFileMapping(
			FILE_MAP_ALL_ACCESS,
			false,
			fileName);
		if (!hm)
			return NULL;

		LPVOID lp = MapViewOfFile(
			hm,
			FILE_MAP_ALL_ACCESS,
			0, 0, 0);
		if (!lp)
			return NULL;

		HTHANDLE* htHandle = new HTHANDLE();
		memcpy(htHandle, lp, sizeof(HTHANDLE));
		htHandle->File = NULL;
		htHandle->FileMapping = hm;
		htHandle->Addr = lp;
		htHandle->SnapshotTimer = NULL;

		return htHandle;
	}


	Element* Get     //  читать элемент из хранилища
	(
		HTHANDLE* htHandle,
		const Element* element)
	{
		int index = findIndex(htHandle, element);
		if (index < 0)
		{
			WriteLastError(htHandle, "-- not found element (GET)");
			return NULL;
		}

		Element* foundElement = new Element();
		readFromMemory(htHandle, foundElement, index);

		return foundElement;
	}

	BOOL Insert		// добавить элемент в хранилище
	(
		HTHANDLE* htHandle,
		const Element* element)
	{
		if (htHandle->Count >= htHandle->Capacity)
		{
			WriteLastError(htHandle, "-- not found free memory");
			return false;
		}

		int freeIndex = findFreeIndex(htHandle, element);

		if (freeIndex < 0)
		{
			WriteLastError(htHandle, "-- key already exists");
			return false;
		}

		writeToMemory(htHandle, element, freeIndex);
		incrementCount(htHandle);

		return true;
	}

	BOOL Update     //  изменить элемент в хранилище
	(
		HTHANDLE* htHandle,
		const Element* oldElement,
		const void* newPayload,
		int             newPayloadLength)
	{
		int index = findIndex(htHandle, oldElement);
		if (index < 0)
		{
			WriteLastError(htHandle, "-- not found element (UPDATE)");
			return false;
		}

		Element* updateElement = new Element(oldElement, newPayload, newPayloadLength);
		writeToMemory(htHandle, updateElement, index);
		return true;
	}

	BOOL Delete      // удалить элемент в хранилище
	(
		HTHANDLE* htHandle,
		const Element* element)
	{
		int index = findIndex(htHandle, element);
		if (index < 0)
		{
			WriteLastError(htHandle, "-- not found element (DELETE)");
			return false;
		}

		clearMemoryByIndex(htHandle, index);
		decrementCount(htHandle);

		return true;
	}

	BOOL Snap         // выполнить Snapshot
	(
		HTHANDLE* htHandle)           // управление HT (File, FileMapping)
	{
		if (!FlushViewOfFile(htHandle->Addr, NULL)) {
			WriteLastError(htHandle, "-- snapshot error");
			return false;
		}
		htHandle->LastSnaptime = time(NULL);
		return true;
	}

	void print                               // распечатать элемент 
	(
		const Element* element)              // элемент 
	{
		std::cout << "Element:" << std::endl;
		std::cout << "{" << std::endl;
		std::cout << "\t\"key\": \"" << (char*)element->key << "\"," << std::endl;
		std::cout << "\t\"keyLength\": " << element->keyLength << "," << std::endl;
		std::cout << "\t\"payload\": \"" << (char*)element->payload << "\"," << std::endl;
		std::cout << "\t\"payloadLength\": " << element->payloadLength << std::endl;
		std::cout << "}" << std::endl;
	}

	BOOL Close        // snap и закрыть HT  и  очистить htHandle
	(
		const HTHANDLE* htHandle)           // управление HT (File, FileMapping)
	{
		HANDLE hf = htHandle->File;
		HANDLE hfm = htHandle->FileMapping;

		if (!CancelWaitableTimer(htHandle->SnapshotTimer))
			throw "cancel snapshotTimer failed";
		if (!UnmapViewOfFile(htHandle->Addr))
			throw "unmapping view failed";
		if (!CloseHandle(hfm))
			throw "close File Mapping failed";
		if (hf != NULL)
			if (!CloseHandle(hf))
				throw "close File failed";

		return true;
	}


	Element::Element()
	{
		key = nullptr;
		keyLength = 0;
		payload = nullptr;
		payloadLength = 0;
	}

	Element::Element(const void* key, int keyLength) : Element()
	{
		this->key = key;
		this->keyLength = keyLength;
	}

	Element::Element(const void* key, int keyLength, const void* payload, int  payloadLength) : Element()
	{
		this->key = key;
		this->keyLength = keyLength;
		this->payload = payload;
		this->payloadLength = payloadLength;
	}

	Element::Element(const Element* oldElement, const void* newPayload, int  newPayloadLength) : Element()
	{
		this->key = oldElement->key;
		this->keyLength = oldElement->keyLength;
		this->payload = newPayload;
		this->payloadLength = newPayloadLength;

		delete oldElement;
	}

	int GetElementSize(int   maxKeyLength, int   maxPayloadLength)
	{
		return maxKeyLength + maxPayloadLength + sizeof(int) * 2;
	}

	int hashFunction(const char* key, int capacity)
	{
		unsigned long i = 0;
		for (int j = 0; key[j]; j++)
			i += key[j];
		return i % capacity;
	}

	int nextHash(int currentHash, const char* key, int capacity)
	{
		return ++currentHash;
	}

	int findFreeIndex(
		const HTHANDLE* htHandle,           // управление HT
		const Element* element)				// элемент
	{
		int index = hashFunction((char*)element->key, htHandle->Capacity);

		Element* foundElement = new Element();
		do
		{
			readFromMemory(htHandle, foundElement, index);
			if (index >= htHandle->Capacity || foundElement->key != NULL &&
				memcmp(foundElement->key, element->key, element->keyLength) == NULL)
			{
				index = -1;
				break;
			}


			index = nextHash(index, (char*)element->key, htHandle->Capacity);
		} while (
			foundElement->keyLength != NULL &&
			foundElement->payloadLength != NULL);

		delete foundElement;
		return index - 1;
	}

	int findIndex(
		const HTHANDLE* htHandle,           // управление HT
		const Element* element)				// элемент
	{
		int index = hashFunction((char*)element->key, htHandle->Capacity);

		Element* foundElement = new Element();
		do
		{
			if (index >= htHandle->Capacity)
			{
				index = -1;
				break;
			}

			readFromMemory(htHandle, foundElement, index);
			index = nextHash(index, (char*)element->key, htHandle->Capacity);
		} while (
			memcmp(foundElement->key, element->key, element->keyLength) != NULL);

		delete foundElement;
		return index - 1;
	}

	BOOL writeToMemory(const HTHANDLE* const htHandle, const Element* const element, int index)
	{
		LPVOID lp = htHandle->Addr;

		lp = (HTHANDLE*)lp + 1;
		lp = (byte*)lp + GetElementSize(htHandle->MaxKeyLength, htHandle->MaxPayloadLength) * index;

		memcpy(lp, element->key, element->keyLength);
		lp = (byte*)lp + htHandle->MaxKeyLength;
		memcpy(lp, &element->keyLength, sizeof(int));
		lp = (int*)lp + 1;
		memcpy(lp, element->payload, element->payloadLength);
		lp = (byte*)lp + htHandle->MaxPayloadLength;
		memcpy(lp, &element->payloadLength, sizeof(int));

		return true;
	}

	int incrementCount(HTHANDLE* const htHandle)
	{
		return ++htHandle->Count;
	}

	Element* readFromMemory(const HTHANDLE* const htHandle, Element* const element, int index)
	{
		LPVOID lp = htHandle->Addr;

		lp = (HTHANDLE*)lp + 1;
		lp = (byte*)lp + GetElementSize(htHandle->MaxKeyLength, htHandle->MaxPayloadLength) * index;

		element->key = lp;
		lp = (byte*)lp + htHandle->MaxKeyLength;
		element->keyLength = *(int*)lp;
		lp = (int*)lp + 1;
		element->payload = lp;
		lp = (byte*)lp + htHandle->MaxPayloadLength;
		element->payloadLength = *(int*)lp;

		return element;
	}

	BOOL clearMemoryByIndex(const HTHANDLE* const htHandle, int index)
	{
		LPVOID lp = htHandle->Addr;
		int sizeElement = GetElementSize(htHandle->MaxKeyLength, htHandle->MaxPayloadLength);

		lp = (HTHANDLE*)lp + 1;
		lp = (byte*)lp + sizeElement * index;

		ZeroMemory(lp, sizeElement);

		return true;
	}

	int decrementCount(HTHANDLE* const htHandle)
	{
		return --htHandle->Count;
	}

	const char* WriteLastError(HTHANDLE* const htHandle, const char* msg)
	{
		memcpy(htHandle->LastErrorMessage, msg, sizeof(htHandle->LastErrorMessage));
		return htHandle->LastErrorMessage;
	}

	const char* GetLastError
	(
		const HTHANDLE* htHandle)
	{
		return htHandle->LastErrorMessage;
	}
}
