#pragma once
#include "os13_htcom.h"
#include <sstream>
#include <chrono>
#include <conio.h>
#define HEADERSIZE 20

using namespace std;
DWORD lastErrorHT = 0;

HANDLE startMutex = NULL;    
HANDLE operationMutex = NULL; 

bool InitMutexes()
{
    startMutex = CreateMutexA(NULL, FALSE, "ht_start_mutex");
    if (startMutex == NULL)
    {
        return false;
    }

    operationMutex = CreateMutexA(NULL, FALSE, NULL);
    if (operationMutex == NULL)
    {
        CloseHandle(startMutex);
        return false;
    }

    return true;
}

namespace HT_HELPERS {
    unsigned int HashFunction(const void* key, int keylength)
    {
        const unsigned char* data = (const unsigned char*)key;
        unsigned int hash = 2166136261u;
        for (int i = 0; i < keylength; i++)
        {
            hash ^= data[i];
            hash *= 16777619;
        }
        return hash;
    }

    BOOL WriteHeaderToFileImpl(HTHANDLE* ht)
    {
        DWORD written;

        SetFilePointer(ht->File, 0, NULL, FILE_BEGIN);

        if (!WriteFile(ht->File, &ht->Capacity, sizeof(ht->Capacity), &written, NULL) ||
            !WriteFile(ht->File, &ht->SecSnapshotInterval, sizeof(ht->SecSnapshotInterval), &written, NULL) ||
            !WriteFile(ht->File, &ht->MaxKeyLength, sizeof(ht->MaxKeyLength), &written, NULL) ||
            !WriteFile(ht->File, &ht->MaxPayloadLength, sizeof(ht->MaxPayloadLength), &written, NULL) ||
            !WriteFile(ht->File, &ht->elementCount, sizeof(ht->elementCount), &written, NULL)) {
            return FALSE;
        }

        FlushFileBuffers(ht->File);
        return TRUE;
    }

    BOOL ReadHeaderFromFileImpl(HTHANDLE* ht)
    {
        DWORD read;

        SetFilePointer(ht->File, 0, NULL, FILE_BEGIN);

        if (!ReadFile(ht->File, &ht->Capacity, sizeof(ht->Capacity), &read, NULL) ||
            !ReadFile(ht->File, &ht->SecSnapshotInterval, sizeof(ht->SecSnapshotInterval), &read, NULL) ||
            !ReadFile(ht->File, &ht->MaxKeyLength, sizeof(ht->MaxKeyLength), &read, NULL) ||
            !ReadFile(ht->File, &ht->MaxPayloadLength, sizeof(ht->MaxPayloadLength), &read, NULL) ||
            !ReadFile(ht->File, &ht->elementCount, sizeof(ht->elementCount), &read, NULL)) {
            return FALSE;
        }

        return TRUE;
    }

    BOOL InsertImpl(HTHANDLE* ht, const Element* element)
    {
        if (!ht || !element || !ht->DataAddr) return FALSE;
        if (element->keylength <= 0 || element->keylength > ht->MaxKeyLength) return FALSE;
        if (element->payloadlength < 0 || element->payloadlength > ht->MaxPayloadLength) return FALSE;

        if (ht->elementCount >= ht->Capacity)
        {
            return FALSE;
        }

        int slotSize = 1 + ht->MaxKeyLength + ht->MaxPayloadLength;
        BYTE* tablestart = (BYTE*)ht->DataAddr;

        unsigned int hash = HashFunction(element->key, element->keylength);
        int capacity = ht->Capacity;

        for (int i = 0; i < capacity; i++)
        {
            int idx = (hash + i) % capacity;
            BYTE* slot = tablestart + idx * slotSize;

            BYTE& isSlotUsedFlag = slot[0];
            BYTE* keyStart = slot + 1;
            BYTE* payloadPtr = slot + 1 + ht->MaxKeyLength;

            if (isSlotUsedFlag == 0)
            {
                isSlotUsedFlag = 1;
                memset(keyStart, 0, ht->MaxKeyLength);
                memcpy(keyStart, element->key, element->keylength);

                memset(payloadPtr, 0, ht->MaxPayloadLength);
                if (element->payloadlength > 0 && element->payload != nullptr)
                    memcpy(payloadPtr, element->payload, element->payloadlength);

                ht->elementCount++;
                return TRUE;
            }
            else if (memcmp(keyStart, element->key, element->keylength) == 0)
            {
                memset(payloadPtr, 0, ht->MaxPayloadLength);
                if (element->payloadlength > 0 && element->payload != nullptr)
                    memcpy(payloadPtr, element->payload, element->payloadlength);
                return TRUE;
            }
        }

        return FALSE;
    }

    BOOL DeleteImpl(HTHANDLE* ht, const Element* element)
    {
        if (!ht || !element || !ht->DataAddr) return FALSE;
        if (element->keylength <= 0 || element->keylength > ht->MaxKeyLength) return FALSE;

        int slotSize = 1 + ht->MaxKeyLength + ht->MaxPayloadLength;
        BYTE* base = (BYTE*)ht->DataAddr;

        unsigned int hash = HashFunction(element->key, element->keylength);
        int capacity = ht->Capacity;

        for (int i = 0; i < capacity; i++)
        {
            int idx = (hash + i) % capacity;
            BYTE* slot = base + idx * slotSize;

            BYTE& isUsed = slot[0];
            BYTE* keyPtr = slot + 1;

            if (isUsed == 0) break;

            if (memcmp(keyPtr, element->key, element->keylength) == 0)
            {
                isUsed = 0;
                memset(keyPtr, 0, ht->MaxKeyLength);
                memset(slot + 1 + ht->MaxKeyLength, 0, ht->MaxPayloadLength);
                ht->elementCount--;
                return TRUE;
            }
        }

        return FALSE;
    }

    Element* GetImpl(HTHANDLE* ht, const Element* element)
    {
        if (!ht || !element || !ht->DataAddr) return nullptr;
        if (element->keylength <= 0 || element->keylength > ht->MaxKeyLength) return nullptr;

        int slotSize = 1 + ht->MaxKeyLength + ht->MaxPayloadLength;
        BYTE* dataStart = (BYTE*)ht->DataAddr;

        unsigned int hash = HashFunction(element->key, element->keylength);
        int capacity = ht->Capacity;

        for (int i = 0; i < capacity; i++)
        {
            int idx = (hash + i) % capacity;
            BYTE* slot = dataStart + idx * slotSize;

            BYTE& isUsed = slot[0];
            BYTE* keyPtr = slot + 1;
            BYTE* payloadPtr = slot + 1 + ht->MaxKeyLength;

            if (isUsed == 0) break;

            if (memcmp(keyPtr, element->key, element->keylength) == 0)
            {
                BYTE* keyCopy = new BYTE[element->keylength];
                memcpy(keyCopy, keyPtr, element->keylength);

                BYTE* payloadCopy = new BYTE[ht->MaxPayloadLength];
                memcpy(payloadCopy, payloadPtr, ht->MaxPayloadLength);

                return new Element{ keyCopy, element->keylength, payloadCopy, ht->MaxPayloadLength };
            }
        }

        return nullptr;
    }

    BOOL UpdateImpl(HTHANDLE* ht, const Element* oldelement, const void* newpayload, int newpayloadlength)
    {
        if (!ht || !oldelement || !ht->DataAddr) return FALSE;
        if (oldelement->keylength <= 0 || oldelement->keylength > ht->MaxKeyLength) return FALSE;
        if (newpayloadlength < 0 || newpayloadlength > ht->MaxPayloadLength) return FALSE;

        int slotSize = 1 + ht->MaxKeyLength + ht->MaxPayloadLength;
        BYTE* base = (BYTE*)ht->DataAddr;

        unsigned int hash = HashFunction(oldelement->key, oldelement->keylength);
        int capacity = ht->Capacity;

        for (int i = 0; i < capacity; i++)
        {
            int idx = (hash + i) % capacity;
            BYTE* slot = base + idx * slotSize;

            BYTE& isUsed = slot[0];
            BYTE* keyPtr = slot + 1;
            BYTE* payloadPtr = slot + 1 + ht->MaxKeyLength;

            if (isUsed == 0) break;

            if (memcmp(keyPtr, oldelement->key, oldelement->keylength) == 0)
            {
                memset(payloadPtr, 0, ht->MaxPayloadLength);
                if (newpayloadlength > 0 && newpayload != nullptr)
                    memcpy(payloadPtr, newpayload, newpayloadlength);
                return TRUE;
            }
        }

        return FALSE;
    }

    BOOL SnapImpl(HTHANDLE* ht)
    {
        if (!ht) return FALSE;

        ht->lastsnaptime = time(0);

        auto now = std::chrono::system_clock::now();
        std::time_t now_c = std::chrono::system_clock::to_time_t(now);
        std::tm local_tm;
        localtime_s(&local_tm, &now_c);

        std::ostringstream filenameStream;
        filenameStream << "snapshot_1.htSNAP";
        std::string filename = filenameStream.str();

        HANDLE snapshot = CreateFileA(filename.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (snapshot == INVALID_HANDLE_VALUE)
        {
            return FALSE;
        }

        int slotSize = 1 + ht->MaxKeyLength + ht->MaxPayloadLength;
        int totalSize = ht->Capacity * slotSize + HEADERSIZE;

        DWORD writtenBytesAmount = 0;
        BOOL writeResult = WriteFile(snapshot, ht->Addr, (DWORD)totalSize, &writtenBytesAmount, NULL);
        CloseHandle(snapshot);

        return writeResult && writtenBytesAmount == (DWORD)totalSize;
    }

    BOOL CloseImpl(HTHANDLE* ht)
    {
        if (!ht) return FALSE;

        bool closeResult = true;

        if (ht->Addr != NULL)
        {
            if (!UnmapViewOfFile(ht->Addr)) closeResult = false;
            ht->Addr = NULL;
        }
        if (ht->FileMapping != NULL && ht->FileMapping != INVALID_HANDLE_VALUE)
        {
            if (!CloseHandle(ht->FileMapping)) closeResult = false;
            ht->FileMapping = NULL;
        }
        if (ht->File != NULL && ht->File != INVALID_HANDLE_VALUE)
        {
            if (!CloseHandle(ht->File)) closeResult = false;
            ht->File = NULL;
        }

        return closeResult;
    }
}

class os13 : public IHT
{
private:
    LONG m_refCount;

public:
    os13() : m_refCount(1)
    {
        if (!startMutex || !operationMutex) {
            InitMutexes();
        }
    }

    ~os13() {}
    
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject) override
    {
        if (ppvObject == nullptr) return E_POINTER;
        if (riid == IID_IUnknown || riid == IID_IHT) {
            *ppvObject = static_cast<IHT*>(this);
            AddRef();
            return S_OK;
        }
        *ppvObject = nullptr;
        return E_NOINTERFACE;
    }

    ULONG STDMETHODCALLTYPE AddRef() override
    {
        return InterlockedIncrement(&m_refCount);
    }

    ULONG STDMETHODCALLTYPE Release() override
    {
        ULONG refCount = InterlockedDecrement(&m_refCount);
        if (refCount == 0) delete this;
        return refCount;
    }

    BOOL IsStopped(HTHANDLE* ht)
    {
        if (!ht) return TRUE;
        WaitForSingleObject(operationMutex, INFINITE);
        BOOL isStopped = ht->isStopped;
        ReleaseMutex(operationMutex);
        return isStopped;
    }

    HRESULT SetStopped(HTHANDLE* ht, BOOL isStopped)
    {
        if (!ht) return E_FAIL;
        WaitForSingleObject(operationMutex, INFINITE);
        ht->isStopped = isStopped;
        ReleaseMutex(operationMutex);
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE WriteHeaderToFile(HTHANDLE* ht) override
    {
        if (!ht) return E_INVALIDARG;
        WaitForSingleObject(operationMutex, INFINITE);
        HRESULT hr = HT_HELPERS::WriteHeaderToFileImpl(ht) ? S_OK : E_FAIL;
        ReleaseMutex(operationMutex);
        return hr;
    }

    HRESULT STDMETHODCALLTYPE ReadHeaderFromFile(HTHANDLE* ht) override
    {
        if (!ht) return E_INVALIDARG;
        WaitForSingleObject(operationMutex, INFINITE);
        HRESULT hr = HT_HELPERS::ReadHeaderFromFileImpl(ht) ? S_OK : E_FAIL;
        ReleaseMutex(operationMutex);
        return hr;
    }

    HRESULT STDMETHODCALLTYPE Create(
        int Capacity, int SecSnapshotInterval, int MaxKeyLength, int MaxPayloadLength,
        const char* FileName, HTHANDLE** result) override
    {
        if (!FileName || !result) return E_INVALIDARG;

        WaitForSingleObject(operationMutex, INFINITE);

        HTHANDLE* ht = new HTHANDLE();
        ht->Capacity = Capacity;
        ht->SecSnapshotInterval = SecSnapshotInterval;
        ht->MaxKeyLength = MaxKeyLength;
        ht->MaxPayloadLength = MaxPayloadLength;
        ht->elementCount = 0;
        strcpy_s(ht->FileName, FileName);
        ht->isStopped = FALSE;
        ht->lastsnaptime = 0;

        int elementSize = 1 + ht->MaxKeyLength + ht->MaxPayloadLength;
        int fileSize = elementSize * ht->Capacity + HEADERSIZE;

        ht->File = CreateFileA(FileName, GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (ht->File == INVALID_HANDLE_VALUE) {
            lastErrorHT = GetLastError();
            delete ht;
            ReleaseMutex(operationMutex);
            return E_FAIL;
        }

        SetFilePointer(ht->File, fileSize, NULL, FILE_BEGIN);
        SetEndOfFile(ht->File);
        SetFilePointer(ht->File, 0, NULL, FILE_BEGIN);

        if (!HT_HELPERS::WriteHeaderToFileImpl(ht)) {
            lastErrorHT = GetLastError();
            CloseHandle(ht->File);
            delete ht;
            ReleaseMutex(operationMutex);
            return E_FAIL;
        }

        FlushFileBuffers(ht->File);

        ht->FileMapping = CreateFileMappingA(ht->File, NULL, PAGE_READWRITE, 0, fileSize, NULL);
        if (ht->FileMapping == NULL) {
            lastErrorHT = GetLastError();
            CloseHandle(ht->File);
            delete ht;
            ReleaseMutex(operationMutex);
            return E_FAIL;
        }

        ht->Addr = MapViewOfFile(ht->FileMapping, FILE_MAP_ALL_ACCESS, 0, 0, 0);
        if (ht->Addr == NULL) {
            lastErrorHT = GetLastError();
            CloseHandle(ht->FileMapping);
            CloseHandle(ht->File);
            delete ht;
            ReleaseMutex(operationMutex);
            return E_FAIL;
        }

        ht->DataAddr = (BYTE*)ht->Addr + HEADERSIZE;

        memset(ht->DataAddr, 0, elementSize * ht->Capacity);

        *result = ht;
        ReleaseMutex(operationMutex);
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE Open(const char* FileName, HTHANDLE** result) override
    {
        if (!FileName || !result) return E_INVALIDARG;

        WaitForSingleObject(operationMutex, INFINITE);

        HTHANDLE* ht = new HTHANDLE();
        ht->isStopped = FALSE;

        ht->File = CreateFileA(FileName, GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (ht->File == INVALID_HANDLE_VALUE) {
            lastErrorHT = GetLastError();
            delete ht;
            ReleaseMutex(operationMutex);
            return E_FAIL;
        }

        if (!HT_HELPERS::ReadHeaderFromFileImpl(ht)) {
            lastErrorHT = GetLastError();
            CloseHandle(ht->File);
            delete ht;
            ReleaseMutex(operationMutex);
            return E_FAIL;
        }

        int elementSize = 1 + ht->MaxKeyLength + ht->MaxPayloadLength;
        int fileSize = elementSize * ht->Capacity + HEADERSIZE;

        ht->FileMapping = CreateFileMappingA(ht->File, NULL, PAGE_READWRITE, 0, fileSize, NULL);
        if (ht->FileMapping == NULL) {
            lastErrorHT = GetLastError();
            CloseHandle(ht->File);
            delete ht;
            ReleaseMutex(operationMutex);
            return E_FAIL;
        }

        ht->Addr = MapViewOfFile(ht->FileMapping, FILE_MAP_ALL_ACCESS, 0, 0, 0);
        if (ht->Addr == NULL) {
            lastErrorHT = GetLastError();
            CloseHandle(ht->FileMapping);
            CloseHandle(ht->File);
            delete ht;
            ReleaseMutex(operationMutex);
            return E_FAIL;
        }

        ht->DataAddr = (BYTE*)ht->Addr + HEADERSIZE;

        *result = ht;
        ReleaseMutex(operationMutex);
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE Close(HTHANDLE* ht) override
    {
        if (!ht) return E_INVALIDARG;

        WaitForSingleObject(operationMutex, INFINITE);
        BOOL result = HT_HELPERS::CloseImpl(ht);
        delete ht;
        ReleaseMutex(operationMutex);
        return result ? S_OK : E_FAIL;
    }

    HRESULT STDMETHODCALLTYPE Insert(HTHANDLE* ht, const Element* element) override
    {
        if (!ht || !element) return E_INVALIDARG;

        WaitForSingleObject(operationMutex, INFINITE);
        if (ht->isStopped) {
            ReleaseMutex(operationMutex);
            return E_ACCESSDENIED; 
        }
        HANDLE stopMutex = OpenMutexA(SYNCHRONIZE, FALSE, "ht_stop_mutex");
        if (stopMutex) {
            DWORD result = WaitForSingleObject(stopMutex, 0);
            if (result == WAIT_TIMEOUT) {
                CloseHandle(stopMutex);
                return E_ACCESSDENIED;
            }
            else if (result == WAIT_OBJECT_0) {
                ReleaseMutex(stopMutex);
                CloseHandle(stopMutex);
            }
        }
        HRESULT hr = HT_HELPERS::InsertImpl(ht, element) ? S_OK : E_FAIL;
        ReleaseMutex(operationMutex);
        return hr;
    }

    HRESULT STDMETHODCALLTYPE Delete(HTHANDLE* ht, const Element* element) override
    {
        if (!ht || !element) return E_INVALIDARG;

        WaitForSingleObject(operationMutex, INFINITE);
        if (ht->isStopped) {
            ReleaseMutex(operationMutex);
            return E_ACCESSDENIED;
        }
        HANDLE stopMutex = OpenMutexA(SYNCHRONIZE, FALSE, "ht_stop_mutex");
        if (stopMutex) {
            DWORD result = WaitForSingleObject(stopMutex, 0);
            if (result == WAIT_TIMEOUT) {
                CloseHandle(stopMutex);
                return E_ACCESSDENIED;
            }
            else if (result == WAIT_OBJECT_0) {
                ReleaseMutex(stopMutex);
                CloseHandle(stopMutex);
            }
        }
        HRESULT hr = HT_HELPERS::DeleteImpl(ht, element) ? S_OK : E_FAIL;
        ReleaseMutex(operationMutex);
        return hr;
    }

    HRESULT STDMETHODCALLTYPE Update(HTHANDLE* ht, const Element* oldelement,
        const void* newpayload, int newpayloadlength) override
    {
        if (!ht || !oldelement) return E_INVALIDARG;

        WaitForSingleObject(operationMutex, INFINITE);
        if (ht->isStopped) {
            ReleaseMutex(operationMutex);
            return E_ACCESSDENIED;
        }
        HANDLE stopMutex = OpenMutexA(SYNCHRONIZE, FALSE, "ht_stop_mutex");
        if (stopMutex) {
            DWORD result = WaitForSingleObject(stopMutex, 0);
            if (result == WAIT_TIMEOUT) {
                CloseHandle(stopMutex);
                return E_ACCESSDENIED;
            }
            else if (result == WAIT_OBJECT_0) {
                ReleaseMutex(stopMutex);
                CloseHandle(stopMutex);
            }
        }
        HRESULT hr = HT_HELPERS::UpdateImpl(ht, oldelement, newpayload, newpayloadlength) ? S_OK : E_FAIL;
        ReleaseMutex(operationMutex);
        return hr;
    }

    HRESULT STDMETHODCALLTYPE Get(HTHANDLE* ht, const Element* element, Element** result) override
    {
        if (!ht || !element || !result) return E_INVALIDARG;

        WaitForSingleObject(operationMutex, INFINITE);
        if (ht->isStopped) {
            ReleaseMutex(operationMutex);
            return E_ACCESSDENIED;
        }
        HANDLE stopMutex = OpenMutexA(SYNCHRONIZE, FALSE, "ht_stop_mutex");
        if (stopMutex) {
            DWORD result = WaitForSingleObject(stopMutex, 0);
            if (result == WAIT_TIMEOUT) {
                CloseHandle(stopMutex);
                return E_ACCESSDENIED;
            }
            else if (result == WAIT_OBJECT_0) {
                ReleaseMutex(stopMutex);
                CloseHandle(stopMutex);
            }
        }
        *result = HT_HELPERS::GetImpl(ht, element);
        ReleaseMutex(operationMutex);
        return *result ? S_OK : E_FAIL;

    }

    HRESULT STDMETHODCALLTYPE Snap(HTHANDLE* ht) override
    {
        if (!ht) return E_INVALIDARG;

        WaitForSingleObject(operationMutex, INFINITE);
        HRESULT hr = HT_HELPERS::SnapImpl(ht) ? S_OK : E_FAIL;
        ReleaseMutex(operationMutex);
        return hr;
    }

    HRESULT STDMETHODCALLTYPE Stop(HTHANDLE* ht) override
    {
        if (!ht) return E_INVALIDARG;

        WaitForSingleObject(operationMutex, INFINITE);
        ht->isStopped = TRUE; 
        ReleaseMutex(operationMutex);

        Snap(ht); 
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE Start(HTHANDLE* ht) override
    {
        if (!ht) return E_INVALIDARG;

        WaitForSingleObject(operationMutex, INFINITE);
        ht->isStopped = FALSE; 
        ReleaseMutex(operationMutex);
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE PrintInfo(HTHANDLE* ht) override
    {
        if (!ht) return E_INVALIDARG;

        WaitForSingleObject(operationMutex, INFINITE);

        std::stringstream ss;
        ss << "ht Info:" << std::endl;
        ss << "  Capacity: " << ht->Capacity << std::endl;
        ss << "  Snapshot Interval: " << ht->SecSnapshotInterval << std::endl;
        ss << "  Max Key Length: " << ht->MaxKeyLength << std::endl;
        ss << "  Max Payload Length: " << ht->MaxPayloadLength << std::endl;
        ss << "  File Name: " << ht->FileName << std::endl;
        ss << "  Element Count: " << ht->elementCount << std::endl;
        ss << "  Is Stopped: " << (ht->isStopped ? "YES" : "NO") << std::endl;
        ss << "  Last Snapshot: " << ctime(&ht->lastsnaptime);

        std::cout << ss.str();

        ReleaseMutex(operationMutex);
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE IsStopped(HTHANDLE* ht, BOOL* stopped) override
    {
        if (!ht || !stopped) return E_INVALIDARG;
        *stopped = IsStopped(ht);
        return S_OK;
    }
};

class HashTableFactory : public IClassFactory
{
private:
    LONG m_refCount;

public:
    HashTableFactory() : m_refCount(1)
    {
        if (!startMutex || !operationMutex) {
            InitMutexes();
        }
    }

    ~HashTableFactory() {}

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject) override
    {
        if (ppvObject == nullptr) return E_POINTER;

        if (riid == IID_IUnknown || riid == IID_IClassFactory) {
            *ppvObject = static_cast<IClassFactory*>(this);
        }
        else {
            *ppvObject = nullptr;
            return E_NOINTERFACE;
        }

        AddRef();
        return S_OK;
    }

    ULONG STDMETHODCALLTYPE AddRef() override
    {
        return InterlockedIncrement(&m_refCount);
    }

    ULONG STDMETHODCALLTYPE Release() override
    {
        ULONG refCount = InterlockedDecrement(&m_refCount);
        if (refCount == 0) {
            delete this;
        }
        return refCount;
    }

    HRESULT STDMETHODCALLTYPE CreateInstance(IUnknown* pUnkOuter, REFIID riid, void** ppvObject) override
    {
        if (ppvObject == nullptr) return E_POINTER;
        *ppvObject = nullptr;

        if (pUnkOuter != nullptr) return CLASS_E_NOAGGREGATION;

        os13* pHashTable = new (std::nothrow) os13();
        if (pHashTable == nullptr) return E_OUTOFMEMORY;

        HRESULT hr = pHashTable->QueryInterface(riid, ppvObject);
        pHashTable->Release();
        return hr;
    }

    HRESULT STDMETHODCALLTYPE LockServer(BOOL fLock) override
    {
        return S_OK;
    }
};

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, void** ppv)
{
    if (ppv == nullptr) return E_POINTER;
    *ppv = nullptr;

    if (!IsEqualCLSID(rclsid, CLSID_OS13)) return CLASS_E_CLASSNOTAVAILABLE;

    HashTableFactory* pFactory = new (std::nothrow) HashTableFactory();
    if (pFactory == nullptr) return E_OUTOFMEMORY;

    HRESULT hr = pFactory->QueryInterface(riid, ppv);
    pFactory->Release();
    return hr;
}

STDAPI DllCanUnloadNow()
{
    return S_FALSE;
}