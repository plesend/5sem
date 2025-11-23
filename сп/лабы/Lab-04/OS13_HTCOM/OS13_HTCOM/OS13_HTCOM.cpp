#include "OS13_HTCOM.h"
#include <windows.h>
#include <iostream>
#include <string>

DWORD lastErrorHT = 0;

class OS13 : public IHT
{
private:
    LONG m_refCount;

public:
    OS13() : m_refCount(1) {}
    ~OS13() {}

    // IUnknown methods
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject) override
    {
        if (ppvObject == nullptr) return E_POINTER;

        if (riid == IID_IUnknown || riid == IID_IHT) {
            *ppvObject = static_cast<IHT*>(this);
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

    // Вспомогательная функция для проверки глобального состояния
    BOOL IsStorageStopped(HTStorage* ht)
    {
        if (ht == nullptr || ht->pHeader == nullptr) {
            return TRUE;
        }

        WaitForSingleObject(ht->hMutex, INFINITE);
        BOOL isStopped = ht->pHeader->isStorageStopped;
        ReleaseMutex(ht->hMutex);

        return isStopped;
    }

    // Вспомогательная функция для установки глобального состояния
    HRESULT SetStorageStopped(HTStorage* ht, BOOL isStopped)
    {
        if (ht == nullptr || ht->pHeader == nullptr) {
            return E_FAIL;
        }

        WaitForSingleObject(ht->hMutex, INFINITE);
        ht->pHeader->isStorageStopped = isStopped;
        FlushViewOfFile(ht->pHeader, sizeof(HTHeader)); // Принудительно записываем на диск
        ReleaseMutex(ht->hMutex);

        return S_OK;
    }

    // IHT methods
    HRESULT STDMETHODCALLTYPE HTCreate(
        const char* fileName,
        DWORD capacity,
        DWORD maxKeyLength,
        DWORD maxDataLength,
        DWORD snapshotInterval,
        HTStorage** result) override
    {
        HTStorage* ht = new HTStorage();
        ht->isStarted = FALSE;

        ht->hFile = CreateFileA(
            fileName,
            GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL,
            CREATE_ALWAYS,
            FILE_ATTRIBUTE_NORMAL,
            NULL
        );

        if (ht->hFile == INVALID_HANDLE_VALUE) {
            lastErrorHT = GetLastError();
            delete ht;
            return E_FAIL;
        }

        DWORD fileSize = sizeof(HTHeader) + capacity * sizeof(HTElement);

        SetFilePointer(ht->hFile, fileSize, NULL, FILE_BEGIN);
        SetEndOfFile(ht->hFile);

        ht->hFileMapping = CreateFileMapping(
            ht->hFile,
            NULL,
            PAGE_READWRITE,
            0,
            fileSize,
            NULL
        );

        if (ht->hFileMapping == NULL) {
            lastErrorHT = GetLastError();
            CloseHandle(ht->hFile);
            delete ht;
            return E_FAIL;
        }

        ht->pHeader = (HTHeader*)MapViewOfFile(
            ht->hFileMapping,
            FILE_MAP_ALL_ACCESS,
            0,
            0,
            fileSize
        );

        if (ht->pHeader == NULL) {
            lastErrorHT = GetLastError();
            CloseHandle(ht->hFileMapping);
            CloseHandle(ht->hFile);
            delete ht;
            return E_FAIL;
        }

        // Инициализация заголовка
        ht->pHeader->capacity = capacity;
        ht->pHeader->maxKeyLength = maxKeyLength;
        ht->pHeader->maxDataLength = maxDataLength;
        ht->pHeader->snapshotInterval = snapshotInterval;
        ht->pHeader->elementCount = 0;
        ht->pHeader->isStorageStopped = FALSE; // По умолчанию хранилище запущено
        QueryPerformanceCounter(&ht->pHeader->lastSnapshotTime);
        QueryPerformanceCounter(&ht->pHeader->lastHeartbeat);
        memset(ht->pHeader->reserved, 0, sizeof(ht->pHeader->reserved));
        strncpy_s(ht->pHeader->fileName, MAX_PATH, fileName, _TRUNCATE);

        ht->pElements = (HTElement*)((BYTE*)ht->pHeader + sizeof(HTHeader));

        for (DWORD i = 0; i < capacity; i++) {
            ht->pElements[i].keyLength = 0;
            ht->pElements[i].dataLength = 0;
            memset(ht->pElements[i].key, 0, MAX_KEY_LENGTH);
            memset(ht->pElements[i].data, 0, MAX_VALUE_LENGTH);
        }

        const char* mutexName = "HT_STORAGE_MUTEX";
        ht->hMutex = CreateMutexA(NULL, FALSE, mutexName);

        if (ht->hMutex == NULL) {
            lastErrorHT = GetLastError();
            UnmapViewOfFile(ht->pHeader);
            CloseHandle(ht->hFileMapping);
            CloseHandle(ht->hFile);
            delete ht;
            return E_FAIL;
        }

        FlushViewOfFile(ht->pHeader, fileSize);
        FlushFileBuffers(ht->hFile);

        *result = ht;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE HTOpen(
        const char* fileName,
        HTStorage** result) override
    {
        HTStorage* ht = new HTStorage();
        ht->isStarted = FALSE;

        ht->hFile = CreateFileA(
            fileName,
            GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            NULL
        );

        if (ht->hFile == INVALID_HANDLE_VALUE) {
            lastErrorHT = GetLastError();
            delete ht;
            return E_FAIL;
        }

        DWORD fileSize = GetFileSize(ht->hFile, NULL);

        ht->hFileMapping = CreateFileMapping(
            ht->hFile,
            NULL,
            PAGE_READWRITE,
            0,
            fileSize,
            NULL
        );

        if (ht->hFileMapping == NULL) {
            lastErrorHT = GetLastError();
            CloseHandle(ht->hFile);
            delete ht;
            return E_FAIL;
        }

        ht->pHeader = (HTHeader*)MapViewOfFile(
            ht->hFileMapping,
            FILE_MAP_ALL_ACCESS,
            0,
            0,
            fileSize
        );

        if (ht->pHeader == NULL) {
            lastErrorHT = GetLastError();
            CloseHandle(ht->hFileMapping);
            CloseHandle(ht->hFile);
            delete ht;
            return E_FAIL;
        }

        ht->pElements = (HTElement*)((BYTE*)ht->pHeader + sizeof(HTHeader));

        const char* mutexName = "HT_STORAGE_MUTEX";
        ht->hMutex = CreateMutexA(NULL, FALSE, mutexName);

        if (ht->hMutex == NULL) {
            lastErrorHT = GetLastError();
            UnmapViewOfFile(ht->pHeader);
            CloseHandle(ht->hFileMapping);
            CloseHandle(ht->hFile);
            delete ht;
            return E_FAIL;
        }

        *result = ht;
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE HTStart(HTStorage* ht) override
    {
        if (ht == NULL) {
            return E_FAIL;
        }

        WaitForSingleObject(ht->hMutex, INFINITE);
        ht->pHeader->isStorageStopped = FALSE;  // Запускаем хранилище ГЛОБАЛЬНО
        FlushViewOfFile(ht->pHeader, sizeof(HTHeader));
        ReleaseMutex(ht->hMutex);

        std::cout << "Storage STARTED globally" << std::endl;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE HTStop(HTStorage* ht) override
    {
        if (ht == NULL) {
            return E_FAIL;
        }

        HTSnapshot(ht);

        WaitForSingleObject(ht->hMutex, INFINITE);
        ht->pHeader->isStorageStopped = TRUE;  // Останавливаем хранилище ГЛОБАЛЬНО
        FlushViewOfFile(ht->pHeader, sizeof(HTHeader));
        ReleaseMutex(ht->hMutex);

        std::cout << "Storage STOPPED globally" << std::endl;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE HTClose(HTStorage* ht) override
    {
        if (ht == NULL) {
            return E_FAIL;
        }

        if (ht->isStarted) {
            HTStop(ht);
        }

        if (ht->pHeader != NULL) {
            UnmapViewOfFile(ht->pHeader);
        }

        if (ht->hFileMapping != NULL) {
            CloseHandle(ht->hFileMapping);
        }

        if (ht->hFile != INVALID_HANDLE_VALUE) {
            CloseHandle(ht->hFile);
        }

        if (ht->hMutex != NULL) {
            CloseHandle(ht->hMutex);
        }

        delete ht;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE HTInsert(
        HTStorage* ht,
        const char* key,
        const char* data) override
    {
        if (ht == NULL || key == NULL || data == NULL) {
            return E_FAIL;
        }
        if (ht->pHeader != NULL) {
            WaitForSingleObject(ht->hMutex, INFINITE);
            BOOL isStopped = ht->pHeader->isStorageStopped;
            ReleaseMutex(ht->hMutex);

            if (isStopped) {
                std::cout << "ACCESS DENIED: Storage is stopped globally" << std::endl;
                return E_ACCESSDENIED;
            }
        }

    

        WaitForSingleObject(ht->hMutex, INFINITE);

        DWORD keyLen = (DWORD)strlen(key);
        if (keyLen == 0 || keyLen > ht->pHeader->maxKeyLength) {
            ReleaseMutex(ht->hMutex);
            return E_FAIL;
        }

        DWORD dataLen = (DWORD)strlen(data);
        if (dataLen > ht->pHeader->maxDataLength) {
            ReleaseMutex(ht->hMutex);
            return E_FAIL;
        }

        if (ht->pHeader->elementCount >= ht->pHeader->capacity) {
            ReleaseMutex(ht->hMutex);
            return E_FAIL;
        }

        for (DWORD i = 0; i < ht->pHeader->capacity; i++) {
            if (ht->pElements[i].keyLength > 0 && strcmp(ht->pElements[i].key, key) == 0) {
                ReleaseMutex(ht->hMutex);
                return E_FAIL;
            }
        }

        DWORD index = 0;
        for (; index < ht->pHeader->capacity; index++) {
            if (ht->pElements[index].keyLength == 0) {
                break;
            }
        }

        if (index == ht->pHeader->capacity) {
            ReleaseMutex(ht->hMutex);
            return E_FAIL;
        }

        strcpy_s(ht->pElements[index].key, MAX_KEY_LENGTH, key);
        strcpy_s(ht->pElements[index].data, MAX_VALUE_LENGTH, data);
        ht->pElements[index].keyLength = keyLen + 1;
        ht->pElements[index].dataLength = dataLen + 1;
        ht->pHeader->elementCount++;

        ReleaseMutex(ht->hMutex);
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE HTUpdate(
        HTStorage* ht,
        const char* key,
        const char* data) override
    {
        if (ht == NULL || key == NULL || data == NULL) {
            return E_FAIL;
        }

        if (ht->pHeader != NULL) {
            WaitForSingleObject(ht->hMutex, INFINITE);
            BOOL isStopped = ht->pHeader->isStorageStopped;
            ReleaseMutex(ht->hMutex);

            if (isStopped) {
                std::cout << "ACCESS DENIED: Storage is stopped globally" << std::endl;
                return E_ACCESSDENIED;
            }
        }


        WaitForSingleObject(ht->hMutex, INFINITE);

        for (DWORD i = 0; i < ht->pHeader->capacity; i++) {
            if (ht->pElements[i].keyLength > 0 && strcmp(ht->pElements[i].key, key) == 0) {
                strncpy_s(ht->pElements[i].data, MAX_VALUE_LENGTH, data, _TRUNCATE);
                ht->pElements[i].dataLength = (DWORD)strlen(data) + 1;
                ReleaseMutex(ht->hMutex);
                return S_OK;
            }
        }

        ReleaseMutex(ht->hMutex);
        return E_FAIL;
    }

    HRESULT STDMETHODCALLTYPE HTDelete(
        HTStorage* ht,
        const char* key) override
    {
        if (ht == NULL || key == NULL) {
            return E_FAIL;
        }

        if (ht->pHeader != NULL) {
            WaitForSingleObject(ht->hMutex, INFINITE);
            BOOL isStopped = ht->pHeader->isStorageStopped;
            ReleaseMutex(ht->hMutex);

            if (isStopped) {
                std::cout << "ACCESS DENIED: Storage is stopped globally" << std::endl;
                return E_ACCESSDENIED;
            }
        }


        WaitForSingleObject(ht->hMutex, INFINITE);

        for (DWORD i = 0; i < ht->pHeader->capacity; i++) {
            if (ht->pElements[i].keyLength > 0 && strcmp(ht->pElements[i].key, key) == 0) {
                ht->pElements[i].keyLength = 0;
                ht->pElements[i].dataLength = 0;
                memset(ht->pElements[i].key, 0, MAX_KEY_LENGTH);
                memset(ht->pElements[i].data, 0, MAX_VALUE_LENGTH);
                ht->pHeader->elementCount--;
                ReleaseMutex(ht->hMutex);
                return S_OK;
            }
        }

        ReleaseMutex(ht->hMutex);
        return E_FAIL;
    }

    HRESULT STDMETHODCALLTYPE HTGet(
        HTStorage* ht,
        const char* key,
        char** result) override
    {
        if (ht == NULL || key == NULL) {
            return E_FAIL;
        }

        if (ht->pHeader != NULL) {
            WaitForSingleObject(ht->hMutex, INFINITE);
            BOOL isStopped = ht->pHeader->isStorageStopped;
            ReleaseMutex(ht->hMutex);

            if (isStopped) {
                std::cout << "ACCESS DENIED: Storage is stopped globally" << std::endl;
                return E_ACCESSDENIED;
            }
        }


        WaitForSingleObject(ht->hMutex, INFINITE);

        for (DWORD i = 0; i < ht->pHeader->capacity; i++) {
            if (ht->pElements[i].keyLength > 0 && strcmp(ht->pElements[i].key, key) == 0) {
                *result = new char[MAX_VALUE_LENGTH];
                strcpy_s(*result, MAX_VALUE_LENGTH, ht->pElements[i].data);
                ReleaseMutex(ht->hMutex);
                return S_OK;
            }
        }

        ReleaseMutex(ht->hMutex);
        return E_FAIL;
    }

    HRESULT STDMETHODCALLTYPE HTSnapshot(HTStorage* ht) override
    {
        std::cout << "[COM HTSnapshot] Function called" << std::endl;

        if (ht == NULL) {
            std::cout << "[COM HTSnapshot] ERROR: ht is NULL" << std::endl;
            return E_FAIL;
        }

        if (ht->pHeader == NULL) {
            std::cout << "[COM HTSnapshot] ERROR: ht->pHeader is NULL" << std::endl;
            return E_FAIL;
        }

        std::cout << "[COM HTSnapshot] Creating snapshot for: " << ht->pHeader->fileName << std::endl;

        WaitForSingleObject(ht->hMutex, INFINITE);
        std::cout << "[COM HTSnapshot] Mutex acquired" << std::endl;

        char snapshotFile[MAX_PATH];
        sprintf_s(snapshotFile, "%s.snapshot", ht->pHeader->fileName);
        std::cout << "[COM HTSnapshot] Snapshot file: " << snapshotFile << std::endl;

        HANDLE hSnapshot = CreateFileA(
            snapshotFile,
            GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ,
            NULL,
            CREATE_ALWAYS,
            FILE_ATTRIBUTE_NORMAL,
            NULL
        );

        if (hSnapshot == INVALID_HANDLE_VALUE) {
            DWORD error = GetLastError();
            std::cout << "[COM HTSnapshot] ERROR: Failed to create snapshot file. Error: " << error << std::endl;
            ReleaseMutex(ht->hMutex);
            return E_FAIL;
        }

        std::cout << "[COM HTSnapshot] Snapshot file created successfully" << std::endl;

        DWORD fileSize = sizeof(HTHeader) + ht->pHeader->capacity * sizeof(HTElement);
        std::cout << "[COM HTSnapshot] File size: " << fileSize << " bytes" << std::endl;

        DWORD bytesWritten;

        // Записываем заголовок
        BOOL result1 = WriteFile(hSnapshot, ht->pHeader, sizeof(HTHeader), &bytesWritten, NULL);
        std::cout << "[COM HTSnapshot] Header written: " << result1 << ", bytes: " << bytesWritten << std::endl;

        // Записываем элементы
        BOOL result2 = WriteFile(hSnapshot, ht->pElements, ht->pHeader->capacity * sizeof(HTElement), &bytesWritten, NULL);
        std::cout << "[COM HTSnapshot] Elements written: " << result2 << ", bytes: " << bytesWritten << std::endl;

        CloseHandle(hSnapshot);
        std::cout << "[COM HTSnapshot] File closed" << std::endl;

        FlushViewOfFile(ht->pHeader, fileSize);
        FlushFileBuffers(ht->hFile);
        std::cout << "[COM HTSnapshot] Data flushed to disk" << std::endl;

        QueryPerformanceCounter(&ht->pHeader->lastSnapshotTime);
        QueryPerformanceCounter(&ht->pHeader->lastHeartbeat);

        std::cout << "[COM HTSnapshot] Snapshot completed successfully: " << snapshotFile << std::endl;

        ReleaseMutex(ht->hMutex);
        std::cout << "[COM HTSnapshot] Mutex released" << std::endl;

        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE GetLastErrorHT(DWORD* error) override
    {
        if (error == NULL) {
            return E_FAIL;
        }
        *error = lastErrorHT;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE PrintHTInfo(HTStorage* ht) override
    {
        if (ht == NULL) {
            return E_FAIL;
        }

        std::cout << "HT-Storage Info:" << std::endl;
        std::cout << "  Capacity: " << ht->pHeader->capacity << std::endl;
        std::cout << "  Max Key Length: " << ht->pHeader->maxKeyLength << std::endl;
        std::cout << "  Max Data Length: " << ht->pHeader->maxDataLength << std::endl;
        std::cout << "  Snapshot Interval: " << ht->pHeader->snapshotInterval << std::endl;
        std::cout << "  Element Count: " << ht->pHeader->elementCount << std::endl;
        std::cout << "  Storage Stopped: " << (ht->pHeader->isStorageStopped ? "YES" : "NO") << std::endl;

        return S_OK;
    }
};

class OS13Factory : public IClassFactory
{
private:
    LONG m_refCount;

public:
    OS13Factory() : m_refCount(1) {}
    ~OS13Factory() {}

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

        OS13* pOS13 = new (std::nothrow) OS13();
        if (pOS13 == nullptr) return E_OUTOFMEMORY;

        HRESULT hr = pOS13->QueryInterface(riid, ppvObject);
        pOS13->Release();
        return hr;
    }

    HRESULT STDMETHODCALLTYPE LockServer(BOOL fLock) override
    {
        return S_OK;
    }
};

// Экспортируемые функции
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, void** ppv)
{
    if (ppv == nullptr) return E_POINTER;
    *ppv = nullptr;

    if (!IsEqualCLSID(rclsid, CLSID_OS13)) return CLASS_E_CLASSNOTAVAILABLE;

    OS13Factory* pFactory = new (std::nothrow) OS13Factory();
    if (pFactory == nullptr) return E_OUTOFMEMORY;

    HRESULT hr = pFactory->QueryInterface(riid, ppv);
    pFactory->Release();
    return hr;
}

STDAPI DllCanUnloadNow()
{
    return S_FALSE;
}