#pragma once

#include <windows.h>
#include <unknwn.h>
#include <iostream>

#ifdef _DEBUG
#define SEQLOG(x) std::cout << x << std::endl
#else
#define SEQLOG(x) 
#endif

// {E1E53163-4E6E-4DCD-9F7D-3F2F01A0D6C1}
static const CLSID CLSID_OS13 =
{ 0xe1e53163, 0x4e6e, 0x4dcd, { 0x9f, 0x7d, 0x3f, 0x2f, 0x1, 0xa0, 0xd6, 0xc1 } };

static const IID IID_IHT =
{ 0xa1b2c3d4, 0xe5f6, 0x4a5b, { 0x8c, 0x9d, 0xe, 0x1f, 0x2a, 0x3b, 0x4c, 0x5d } };

#define MAX_KEY_LENGTH 256
#define MAX_VALUE_LENGTH 1024

#pragma pack(push, 1)
typedef struct HTHeader {
    DWORD capacity;
    DWORD maxKeyLength;
    DWORD maxDataLength;
    DWORD snapshotInterval;
    DWORD elementCount;
    LARGE_INTEGER lastSnapshotTime;
    LARGE_INTEGER lastHeartbeat;
    char fileName[MAX_PATH];
    BOOL isStorageStopped;  // Глобальный флаг остановки хранилища
    char reserved[100 - sizeof(BOOL)];
} HTHeader;

typedef struct HTElement {
    DWORD keyLength;
    DWORD dataLength;
    char key[MAX_KEY_LENGTH];
    char data[MAX_VALUE_LENGTH];
} HTElement;

typedef struct HTStorage {
    HANDLE hFile;
    HANDLE hFileMapping;
    HTHeader* pHeader;
    HTElement* pElements;
    HANDLE hMutex;
    BOOL isStarted;  // Локальный флаг для клиента
} HTStorage;
#pragma pack(pop)

class IHT : public IUnknown
{
public:
    virtual HRESULT STDMETHODCALLTYPE HTCreate(
        const char* fileName,
        DWORD capacity,
        DWORD maxKeyLength,
        DWORD maxDataLength,
        DWORD snapshotInterval,
        HTStorage** result) = 0;

    virtual HRESULT STDMETHODCALLTYPE HTOpen(
        const char* fileName,
        HTStorage** result) = 0;

    virtual HRESULT STDMETHODCALLTYPE HTStart(
        HTStorage* ht) = 0;

    virtual HRESULT STDMETHODCALLTYPE HTStop(
        HTStorage* ht) = 0;

    virtual HRESULT STDMETHODCALLTYPE HTClose(
        HTStorage* ht) = 0;

    virtual HRESULT STDMETHODCALLTYPE HTInsert(
        HTStorage* ht,
        const char* key,
        const char* data) = 0;

    virtual HRESULT STDMETHODCALLTYPE HTUpdate(
        HTStorage* ht,
        const char* key,
        const char* data) = 0;

    virtual HRESULT STDMETHODCALLTYPE HTDelete(
        HTStorage* ht,
        const char* key) = 0;

    virtual HRESULT STDMETHODCALLTYPE HTGet(
        HTStorage* ht,
        const char* key,
        char** result) = 0;

    virtual HRESULT STDMETHODCALLTYPE HTSnapshot(
        HTStorage* ht) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetLastErrorHT(
        DWORD* error) = 0;

    virtual HRESULT STDMETHODCALLTYPE PrintHTInfo(
        HTStorage* ht) = 0;
};