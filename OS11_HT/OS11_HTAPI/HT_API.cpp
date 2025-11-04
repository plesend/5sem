#include "pch.h"
#include "HT.h"
#include <Windows.h>
#include <cstdio>
#include <cstdint>
#include <ctime>
#include <cstring>
#include <cstdlib>
#include <cstdarg>
#include <algorithm>
#include <unordered_map>
#include <mutex>
#include <iostream>
#include <string>           

using namespace std;

const char* GetLastErrorAsCP1251();

typedef unsigned int uint;

struct InternalState {
	HANDLE hMutex;
	HANDLE hShutdownEvent;
	InternalState() : hMutex(NULL) {}
	~InternalState() {
		if (hMutex) {
			CloseHandle(hMutex);
			hMutex = NULL;
		}
	}
};

static std::unordered_map<std::string, std::shared_ptr<InternalState>> g_internalStates;
static std::mutex g_internalStatesMutex;

struct ScopedInternalLock {
	std::shared_ptr<InternalState> s;
	bool locked;

	explicit ScopedInternalLock(std::shared_ptr<InternalState> s_)
		: s(std::move(s_)), locked(false)
	{
		if (s && s->hMutex != NULL) {
			DWORD r = WaitForSingleObject(s->hMutex, INFINITE);
			locked = (r == WAIT_OBJECT_0);
		}
	}

	~ScopedInternalLock()
	{
		if (locked && s && s->hMutex != NULL) {
			ReleaseMutex(s->hMutex);
		}
	}
};


enum  SlotMetaState : char
{
	EMPTY = 0,
	USED = 1,
	DELETED = 2
};

struct SlotMeta {
	SlotMetaState state;
	int keylen;
	int payloadlen;
};

static const uint HT_MAGIC = 0x48545448u;

static uint hashFunction(const void* key, int keyLength)
{
	int hash = 5381;


	const char* str = static_cast<const char*>(key);

	for (int i = 0; i < keyLength; ++i) {


		hash = ((hash << 5) + hash) + str[i];

	}

	cout << "--Hash: current Hash value: " << hash << endl;


	return hash;
}

static size_t compute_entry_size(int maxKeyLen, int maxPayloadLen) {
	return sizeof(SlotMeta) + (size_t)maxKeyLen + (size_t)maxPayloadLen;
}

static size_t compute_file_size(const DiskHeader* hdr) {
	return sizeof(DiskHeader) + compute_entry_size(hdr->maxKeyLength, hdr->maxPayloadLength) * hdr->capacity;
}

static void* slot_ptr(void* base, int index, const DiskHeader* hdr) {
	size_t entrySize = compute_entry_size(hdr->maxKeyLength, hdr->maxPayloadLength);
	return (void*)((char*)base + sizeof(DiskHeader) + (size_t)index * entrySize);
}

static SlotMeta* slot_meta(void* slotBase) {
	return (SlotMeta*)slotBase;
}

static void* slot_key_ptr(void* slotBase) {
	return (char*)slotBase + sizeof(SlotMeta);
}

static void* slot_payload_ptr(void* slotBase, const DiskHeader* hdr) {
	return (char*)slotBase + sizeof(SlotMeta) + hdr->maxKeyLength;
}

static void set_last_error(HTHANDLE* h, const char* fmt, ...) {
	if (!h) return;

	va_list ap;
	va_start(ap, fmt);
	vsnprintf(h->LastErrorMessage, sizeof(h->LastErrorMessage), fmt, ap);
	va_end(ap);
}

static std::shared_ptr<InternalState> get_internal_state(HTHANDLE* h, bool forceCreate = false)
{
	if (h == nullptr) return nullptr;
	const char* filename = h->FileName;
	if (filename == nullptr || filename[0] == '\0') return nullptr;

	std::string key(filename);

	{
		std::lock_guard<std::mutex> lg(g_internalStatesMutex);
		auto it = g_internalStates.find(key);
		if (it != g_internalStates.end()) {
			return it->second;
		}
		if (!forceCreate) {
			return nullptr;
		}
	}

	auto ns = std::make_shared<InternalState>();

	//формируем имя mutex (32-bit хеш + префикс Global\)
	char mnamebuf[64];
	unsigned int hsh = hashFunction(filename, (int)strlen(filename));
	sprintf_s(mnamebuf, "Global\\HT_mutex_%08X", hsh);

	ns->hMutex = CreateMutexA(NULL, FALSE, mnamebuf);
	if (!ns->hMutex) {
		return nullptr;
	}

	char evnamebuf[64];
	sprintf_s(evnamebuf, "Global\\HT_shutdown_%08X", hsh);
	ns->hShutdownEvent = CreateEventA(NULL, TRUE, FALSE, evnamebuf);
	if (ns->hShutdownEvent == NULL) {
		// event creation failed; cleanup mutex and state
		CloseHandle(ns->hMutex);
		return nullptr;
	}

	{
		std::lock_guard<std::mutex> lg(g_internalStatesMutex);
		auto it = g_internalStates.find(key);
		if (it == g_internalStates.end()) {
			g_internalStates.emplace(key, ns);
			return ns;
		}
		else {
			return it->second;
		}
	}


}

static void erase_internal_state(HTHANDLE* h)
{
	if (h == nullptr) return;
	const char* filename = h->FileName;
	if (filename == nullptr || filename[0] == '\0') return;
	std::string key(filename);

	std::lock_guard<std::mutex> lg(g_internalStatesMutex);
	auto it = g_internalStates.find(key);
	if (it == g_internalStates.end()) return;
	g_internalStates.erase(it);
}

HTHANDLE* Create(int Capacity, int SecSnapshotInterval, int MaxKeyLength, int MaxPayloadLength, const char FileName[512])
{
	if (FileName == nullptr || Capacity <= 0 || MaxKeyLength <= 0 || MaxPayloadLength <= 0) {
		return NULL;
	}

	HTHANDLE* h = new HTHANDLE();
	ZeroMemory(h, sizeof(HTHANDLE));

	h->Capacity = Capacity;
	h->SecSnapshotInterval = SecSnapshotInterval;
	h->MaxKeyLength = MaxKeyLength;
	h->MaxPayloadLength = MaxPayloadLength;
	strncpy(h->FileName, FileName, sizeof(h->FileName) - 1);
	h->FileName[sizeof(h->FileName) - 1] = '\0';

	auto fail = [&](const char* msg, const char* error = GetLastErrorAsCP1251()) -> HTHANDLE*
		{
			set_last_error(h, "%s: %s", msg, error);

			if (h->Addr) UnmapViewOfFile(h->Addr);
			if (h->FileMapping) CloseHandle(h->FileMapping);
			if (h->File) CloseHandle(h->File);
			delete h;

			return NULL;
		};

	h->File = CreateFileA(
		FileName,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);
	if (h->File == INVALID_HANDLE_VALUE) {
		return fail("CreateFile failed");
	}

	DiskHeader hdr;
	ZeroMemory(&hdr, sizeof(hdr));
	hdr.magic = HT_MAGIC;
	hdr.capacity = Capacity;
	hdr.secSnapshotInterval = SecSnapshotInterval;
	hdr.maxKeyLength = MaxKeyLength;
	hdr.maxPayloadLength = MaxPayloadLength;
	hdr.lastsnaptime = time(nullptr);

	size_t fileSize = compute_file_size(&hdr);
	LARGE_INTEGER liSize;
	liSize.QuadPart = (long long)fileSize;

	if (!SetFilePointerEx(h->File, liSize, NULL, FILE_BEGIN)) {
		return fail("SetFilePointerEx failed");
	}

	if (!SetEndOfFile(h->File)) {
		return fail("SetEndOfFile failed");
	}

	h->FileMapping = CreateFileMappingA(h->File, NULL, PAGE_READWRITE, 0, 0, NULL);
	if (h->FileMapping == NULL) {
		return fail("CreateFileMapping failed");
	}

	h->Addr = MapViewOfFile(h->FileMapping, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	if (h->Addr == NULL) {
		return fail("MapViewOfFile failed");
	}

	memcpy(h->Addr, &hdr, sizeof(DiskHeader));
	char* base = (char*)h->Addr;
	size_t rem = fileSize - sizeof(DiskHeader);
	if (rem > 0) {
		ZeroMemory(base + sizeof(DiskHeader), rem);
	}

	auto s = get_internal_state(h, true);
	(void)s;

	return h;
}


HTHANDLE* Open(const char FileName[512])
{
	if (FileName == nullptr) {
		return NULL;
	}

	HTHANDLE* h = new HTHANDLE();
	ZeroMemory(h, sizeof(HTHANDLE));

	strncpy(h->FileName, FileName, sizeof(h->FileName) - 1);
	h->FileName[sizeof(h->FileName) - 1] = '\0';

	auto fail = [&](const char* msg, const char* error = GetLastErrorAsCP1251()) -> HTHANDLE*
		{
			set_last_error(h, "%s: %s", msg, error);

			if (h->Addr) UnmapViewOfFile(h->Addr);
			if (h->FileMapping) CloseHandle(h->FileMapping);
			if (h->File) CloseHandle(h->File);
			delete h;

			return NULL;
		};

	h->File = CreateFileA(
		FileName,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (h->File == INVALID_HANDLE_VALUE) {
		return fail("CreateFile(Open) failed");
	}

	h->FileMapping = CreateFileMappingA(h->File, NULL, PAGE_READWRITE, 0, 0, FileName);
	if (h->FileMapping == NULL) {
		return fail("CreateFileMapping(Open) failed");
	}

	h->Addr = MapViewOfFile(h->FileMapping, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	if (h->Addr == NULL) {
		return fail("MapViewOfFile(Open) failed");
	}

	DiskHeader* hdr = (DiskHeader*)h->Addr;
	if (hdr->magic != HT_MAGIC) {
		set_last_error(h, "Bad magic or not an HT file");
		return fail("Bad magic");
	}

	h->Capacity = hdr->capacity;
	h->SecSnapshotInterval = hdr->secSnapshotInterval;
	h->MaxKeyLength = hdr->maxKeyLength;
	h->MaxPayloadLength = hdr->maxPayloadLength;
	h->lastsnaptime = hdr->lastsnaptime;

	auto s = get_internal_state(h, true);
	(void)s;

	return h;
}

const char* CreateSnapshotFileName(HTHANDLE* handle) {
	static char buffer[100];
	char time_buffer[80];
	tm time_info;

	localtime_s(&time_info, &handle->lastsnaptime);
	strftime(time_buffer, sizeof(time_buffer), "%Y%m%d_%H%M%S", &time_info);
	snprintf(buffer, sizeof(buffer), "Snapshot-%s.htsnap", time_buffer);

	return buffer;

}

BOOL Snap(const HTHANDLE* hthandle)
{
	std::cout << std::endl << "----------Snap----------" << std::endl;

	if (!hthandle) {
		std::cout << "--Snap: Failed to open the handle-- Error: invalid argument" << std::endl;
		return FALSE;
	}

	
	HTHANDLE* h = (HTHANDLE*)hthandle;
	if (h->Addr == NULL) {
		std::cout << "--Snap: Failed (Addr is NULL)" << std::endl;
		set_last_error(h, "Invalid mapping (Addr is NULL)");
		return FALSE;
	}

	
	auto s = get_internal_state(h, false);
	ScopedInternalLock guard(s);

	DiskHeader* hdr = (DiskHeader*)h->Addr;
	if (hdr == nullptr) {
		std::cout << "--Snap: Failed (header is NULL)" << std::endl;
		set_last_error(h, "Invalid mapping (header is NULL)");
		return FALSE;
	}

	
	hdr->lastsnaptime = (uint32_t)time(nullptr);
	h->lastsnaptime = hdr->lastsnaptime;

	size_t fileSize = compute_file_size(hdr);
	std::cout << "--Snap: computed file size: " << fileSize << " bytes" << std::endl;

	
	if (fileSize > 0xFFFFFFFFULL) {
		set_last_error(h, "Snapshot size too large: %zu", fileSize);
		std::cout << "--Snap: Failed (file too large to snapshot)" << std::endl;
		return FALSE;
	}

	const char* snapName = CreateSnapshotFileName(h);
	std::cout << "--Snap: snapshot file name: " << snapName << std::endl;

	HANDLE HTSnapshot = CreateFileA(
		snapName,
		GENERIC_WRITE | GENERIC_READ,
		0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);

	if (HTSnapshot == INVALID_HANDLE_VALUE) {
		DWORD err = GetLastError();
		const char* errstr = GetLastErrorAsCP1251();
		set_last_error(h, "CreateFileA(snapshot) failed: %s", errstr);
		std::cout << "--Snap: Failed to create snapshot file -- Error: " << err << " (" << errstr << ")" << std::endl;
		return FALSE;
	}
	else {
		std::cout << "--Snap: Snapshot file created: " << snapName << std::endl;
	}

	DWORD bytesToWrite = (DWORD)fileSize;
	DWORD bytesWritten = 0;

	BOOL writeResult = WriteFile(
		HTSnapshot,
		h->Addr,
		bytesToWrite,
		&bytesWritten,
		NULL
	);

	if (!writeResult) {
		DWORD writeErr = GetLastError();
		const char* errstr = GetLastErrorAsCP1251();
		set_last_error(h, "WriteFile(snapshot) failed: %s", errstr);
		std::cout << "--Snap: Failed to write snapshot (WriteFile error) -- Error: " << writeErr << " (" << errstr << ")" << std::endl;
		CloseHandle(HTSnapshot);
		return FALSE;
	}

	if (bytesWritten != bytesToWrite) {
		const char* errstr = GetLastErrorAsCP1251();
		set_last_error(h, "WriteFile wrote fewer bytes than expected: wrote=%u expect=%u", bytesWritten, bytesToWrite);
		std::cout << "--Snap: Failed to write snapshot (incomplete write) -- wrote: " << bytesWritten << " expected: " << bytesToWrite << std::endl;
		CloseHandle(HTSnapshot);
		return FALSE;
	}

	std::cout << "--Snap: Snapshot Executed successfully (" << bytesWritten << " bytes written)--" << std::endl;

	// Для большей надёжности можно вызвать FlushFileBuffers на HTSnapshot:
	if (!FlushFileBuffers(HTSnapshot)) {
		const char* errstr = GetLastErrorAsCP1251();
		std::cout << "--Snap: Warning: FlushFileBuffers(snapshot) failed: " << errstr << std::endl;
		// не делаем ошибку окончательной — файл уже записан, но логируем сообщение
	}

	CloseHandle(HTSnapshot);

	std::cout << "----------End----------" << std::endl;
	return TRUE;
}

BOOL Close(const HTHANDLE* hh)
{
	HTHANDLE* h = (HTHANDLE*)hh;
	if (h == nullptr) {
		return FALSE;
	}

	auto s = get_internal_state(h, false);

	if (s != nullptr) {
		ScopedInternalLock guard(s);
		Snap(h);
	}
	else {
		Snap(h);
	}

	if (h->Addr != NULL) {
		UnmapViewOfFile(h->Addr);
		h->Addr = NULL;
	}
	if (h->FileMapping != NULL) {
		CloseHandle(h->FileMapping);
		h->FileMapping = NULL;
	}
	if (h->File != NULL) {
		CloseHandle(h->File);
		h->File = NULL;
	}

	erase_internal_state(h);
	delete h;

	return TRUE;
}

BOOL Insert(const HTHANDLE* hh, const Element* element)
{
	if (hh == nullptr || element == nullptr || element->key == nullptr || element->keylength <= 0) {
		return FALSE;
	}

	HTHANDLE* h = (HTHANDLE*)hh;

	if (element->keylength > h->MaxKeyLength) {
		set_last_error(h, "Key length exceeds maximum");
		return FALSE;
	}

	if (element->payload != nullptr && element->payloadlength > h->MaxPayloadLength) {
		set_last_error(h, "Payload length exceeds maximum");
		return FALSE;
	}

	auto s = get_internal_state(h, true);
	ScopedInternalLock guard(s);

	DiskHeader* hdr = (DiskHeader*)h->Addr;
	if (hdr == nullptr) {
		set_last_error(h, "Invalid mapping");
		return FALSE;
	}

	uint hash = hashFunction(element->key, element->keylength);
	int idx = (int)(hash % (uint)h->Capacity);

	int firstDeleted = -1;

	for (int i = 0; i < h->Capacity; ++i) {
		int pos = (idx + i) % h->Capacity;

		void* slot = slot_ptr(h->Addr, pos, hdr);
		SlotMeta* sm = slot_meta(slot);

		if (sm->state == SlotMetaState::EMPTY) {
			int target = (firstDeleted != -1) ? firstDeleted : pos;
			void* tslot = slot_ptr(h->Addr, target, hdr);
			SlotMeta* tsm = slot_meta(tslot);

			tsm->state = SlotMetaState::USED;
			tsm->keylen = element->keylength;
			tsm->payloadlen = (element->payload != nullptr) ? element->payloadlength : 0;

			void* kptr = slot_key_ptr(tslot);
			memcpy(kptr, element->key, element->keylength);

			if (tsm->payloadlen > 0) {
				void* pptr = slot_payload_ptr(tslot, hdr);
				memcpy(pptr, element->payload, tsm->payloadlen);
			}
			return TRUE;
		}
		else if (sm->state == SlotMetaState::DELETED) {
			if (firstDeleted == -1) {
				firstDeleted = pos;
			}
		}
		else {
			if (sm->keylen == element->keylength) {
				void* kptr = slot_key_ptr(slot);
				if (memcmp(kptr, element->key, element->keylength) == 0) {
					set_last_error(h, "Key already exists");
					return FALSE;
				}
			}
		}
	}

	if (firstDeleted != -1) {
		void* tslot = slot_ptr(h->Addr, firstDeleted, hdr);
		SlotMeta* tsm = slot_meta(tslot);
		tsm->state = SlotMetaState::USED;
		tsm->keylen = element->keylength;
		tsm->payloadlen = (element->payload != nullptr) ? element->payloadlength : 0;
		void* kptr = slot_key_ptr(tslot);
		memcpy(kptr, element->key, element->keylength);
		if (tsm->payloadlen > 0) {
			void* pptr = slot_payload_ptr(tslot, hdr);
			memcpy(pptr, element->payload, tsm->payloadlen);
		}
		return TRUE;
	}

	set_last_error(h, "Hash table full");
	return FALSE;
}

BOOL Delete(const HTHANDLE* hh, const Element* element)
{
	if (hh == nullptr || element == nullptr || element->key == nullptr) {
		return FALSE;
	}

	HTHANDLE* h = (HTHANDLE*)hh;
	auto s = get_internal_state(h, true);
	ScopedInternalLock guard(s);

	DiskHeader* hdr = (DiskHeader*)h->Addr;
	if (hdr == nullptr) {
		set_last_error(h, "Invalid mapping");
		return FALSE;
	}

	uint hash = hashFunction(element->key, element->keylength);
	int idx = (int)(hash % (uint)h->Capacity);

	for (int i = 0; i < h->Capacity; ++i) {
		int pos = (idx + i) % h->Capacity;
		void* slot = slot_ptr(h->Addr, pos, hdr);
		SlotMeta* sm = slot_meta(slot);

		if (sm->state == SlotMetaState::EMPTY) {
			set_last_error(h, "Key not found (delete)");
			return FALSE;
		}

		if (sm->state == SlotMetaState::USED && sm->keylen == element->keylength) {
			void* kptr = slot_key_ptr(slot);
			if (memcmp(kptr, element->key, element->keylength) == 0) {
				sm->state = SlotMetaState::DELETED;
				sm->keylen = 0;
				sm->payloadlen = 0;
				return TRUE;
			}
		}
	}

	set_last_error(h, "Key not found (delete)");
	return FALSE;
}

Element* Get(const HTHANDLE* hh, const Element* element)
{
	if (hh == nullptr || element == nullptr || element->key == nullptr) {
		return NULL;
	}

	HTHANDLE* h = (HTHANDLE*)hh;

	auto s = get_internal_state(h, true);
	ScopedInternalLock guard(s);

	DiskHeader* hdr = (DiskHeader*)h->Addr;
	if (hdr == nullptr) {
		return NULL;
	}

	uint hash = hashFunction(element->key, element->keylength);
	int idx = (int)(hash % (uint)h->Capacity);

	for (int i = 0; i < h->Capacity; ++i) {
		int pos = (idx + i) % h->Capacity;
		void* slot = slot_ptr(h->Addr, pos, hdr);
		SlotMeta* sm = slot_meta(slot);

		if (sm->state == SlotMetaState::EMPTY) {
			return NULL;
		}

		if (sm->state == SlotMetaState::USED && sm->keylen == element->keylength) {
			void* kptr = slot_key_ptr(slot);
			if (memcmp(kptr, element->key, element->keylength) == 0) {
				Element* out = new Element();
				if (out == nullptr) {
					set_last_error(h, "new(Element) failed");
					return NULL;
				}

				out->keylength = sm->keylen;
				out->payloadlength = sm->payloadlen;
				out->key = NULL;
				out->payload = NULL;

				if (out->keylength > 0) {
					unsigned char* kbuf = new unsigned char[out->keylength];
					if (!kbuf) {
						delete out;
						set_last_error(h, "new(key) failed");
						return NULL;
					}
					memcpy(kbuf, kptr, out->keylength);
					out->key = (const void*)kbuf;
				}

				if (out->payloadlength > 0) {
					void* pptr = slot_payload_ptr(slot, hdr);
					unsigned char* pbuf = new unsigned char[out->payloadlength];
					if (!pbuf) {
						if (out->key != NULL) {
							delete[](unsigned char*)out->key;
						}
						delete out;
						set_last_error(h, "new(payload) failed");
						return NULL;
					}
					memcpy(pbuf, pptr, out->payloadlength);
					out->payload = (const void*)pbuf;
				}

				return out;
			}
		}
	}

	return NULL;
}

BOOL Update(const HTHANDLE* hh, const Element* oldelement, const void* newpayload, int newpayloadlength)
{
	if (hh == nullptr || oldelement == nullptr || oldelement->key == nullptr) {
		return FALSE;
	}

	HTHANDLE* h = (HTHANDLE*)hh;

	if (newpayload != nullptr && newpayloadlength > h->MaxPayloadLength) {
		set_last_error(h, "new payload too large");
		return FALSE;
	}

	auto s = get_internal_state(h, true);
	ScopedInternalLock guard(s);

	DiskHeader* hdr = (DiskHeader*)h->Addr;
	if (hdr == nullptr) {
		set_last_error(h, "Invalid mapping");
		return FALSE;
	}

	uint hash = hashFunction(oldelement->key, oldelement->keylength);
	int idx = (int)(hash % (uint)h->Capacity);

	for (int i = 0; i < h->Capacity; ++i) {
		int pos = (idx + i) % h->Capacity;
		void* slot = slot_ptr(h->Addr, pos, hdr);
		SlotMeta* sm = slot_meta(slot);

		if (sm->state == SlotMetaState::EMPTY) {
			set_last_error(h, "Key not found (update)");
			return FALSE;
		}

		if (sm->state == SlotMetaState::USED && sm->keylen == oldelement->keylength) {
			void* kptr = slot_key_ptr(slot);
			if (memcmp(kptr, oldelement->key, oldelement->keylength) == 0) {
				if (newpayload == nullptr) {
					sm->payloadlen = 0;
				}
				else {
					sm->payloadlen = newpayloadlength;
					void* pptr = slot_payload_ptr(slot, hdr);
					memcpy(pptr, newpayload, newpayloadlength);
				}
				return TRUE;
			}
		}
	}

	set_last_error(h, "Key not found (update)");
	return FALSE;
}

const char* HTGetLastError(HTHANDLE* ht)
{
	if (ht == nullptr) {
		return GetLastErrorAsCP1251();
	}
	return ht->LastErrorMessage;
}

const char* GetLastErrorAsCP1251()
{
	static thread_local std::string message; // живёт до конца потока
	message.clear();

	DWORD code = ::GetLastError();
	if (code == 0) {
		message = "";
		return message.c_str();
	}

	LPSTR buf = nullptr;
	DWORD size = FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr,
		code,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPSTR)&buf,
		0,
		nullptr);

	if (size == 0 || !buf) {
		message = "Не удалось получить текст ошибки";
		return message.c_str();
	}

	message.assign(buf, size);
	LocalFree(buf);

	while (!message.empty() && (message.back() == '\r' || message.back() == '\n'))
		message.pop_back();

	return message.c_str();
}


void HTPrint(const Element* element)
{
	if (element == nullptr) {
		printf("(null)\n");
		return;
	}

	printf("Key(len=%d): ", element->keylength);
	if (element->key != NULL && element->keylength > 0) {
		fwrite(element->key, 1, element->keylength, stdout);
	}
	printf("\n");

	printf("Payload(len=%d): ", element->payloadlength);
	if (element->payload != NULL && element->payloadlength > 0) {
		fwrite(element->payload, 1, element->payloadlength, stdout);
	}
	printf("\n");
}
