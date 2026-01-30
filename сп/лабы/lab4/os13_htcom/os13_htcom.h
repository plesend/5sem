#pragma once
#pragma warning( disable : 4996)
#include <Windows.h>
#include <unknwn.h>
#include <iostream>
#include <string>
#include <chrono>
#include <sstream>

#ifdef _DEBUG
#define SEQLOG(x) std::cout << x << std::endl
#else
#define SEQLOG(x) 
#endif

// {6FF75E28-A80C-4E9D-A7EF-D53A32909594}
static const GUID CLSID_OS13 =
{ 0x6ff75e28, 0xa80c, 0x4e9d, { 0xa7, 0xef, 0xd5, 0x3a, 0x32, 0x90, 0x95, 0x94 } };
// {41A1A79A-DC66-48DD-BC5F-AF7774919A74}
static const GUID IID_IHT =
{ 0x41a1a79a, 0xdc66, 0x48dd, { 0xbc, 0x5f, 0xaf, 0x77, 0x74, 0x91, 0x9a, 0x74 } };

#pragma pack(push, 1)
typedef struct HTHANDLE {
    int Capacity;
    int SecSnapshotInterval;
    int MaxKeyLength;
    int MaxPayloadLength;
    char FileName[512];
    HANDLE File;
    HANDLE FileMapping;
    LPVOID Addr;
    time_t lastsnaptime;
    int elementCount;
    BYTE* DataAddr;
    BOOL isStopped;
    HANDLE hMutex;
} HTHANDLE;

typedef struct Element {
    const void* key;
    int keylength;
    const void* payload;
    int payloadlength;
} Element;
#pragma pack(pop)

class IHT : public IUnknown
{
public:
    virtual HRESULT STDMETHODCALLTYPE WriteHeaderToFile(HTHANDLE* ht) = 0;
    virtual HRESULT STDMETHODCALLTYPE ReadHeaderFromFile(HTHANDLE* ht) = 0;
    virtual HRESULT STDMETHODCALLTYPE Create(
        int Capacity,
        int SecSnapshotInterval,
        int MaxKeyLength,
        int MaxPayloadLength,
        const char* FileName,
        HTHANDLE** result) = 0;

    virtual HRESULT STDMETHODCALLTYPE Open(
        const char* FileName,
        HTHANDLE** result) = 0;

    virtual HRESULT STDMETHODCALLTYPE Close(HTHANDLE* ht) = 0;
    virtual HRESULT STDMETHODCALLTYPE Insert(HTHANDLE* ht, const Element* element) = 0;
    virtual HRESULT STDMETHODCALLTYPE Delete(HTHANDLE* ht, const Element* element) = 0;
    virtual HRESULT STDMETHODCALLTYPE Update(
        HTHANDLE* ht,
        const Element* oldelement,
        const void* newpayload,
        int newpayloadlength) = 0;

    virtual HRESULT STDMETHODCALLTYPE Get(
        HTHANDLE* ht,
        const Element* element,
        Element** result) = 0;

    virtual HRESULT STDMETHODCALLTYPE Snap(HTHANDLE* ht) = 0;
    virtual HRESULT STDMETHODCALLTYPE Stop(HTHANDLE* ht) = 0;
    virtual HRESULT STDMETHODCALLTYPE Start(HTHANDLE* ht) = 0;
    virtual HRESULT STDMETHODCALLTYPE PrintInfo(HTHANDLE* ht) = 0;
    virtual HRESULT STDMETHODCALLTYPE IsStopped(HTHANDLE* ht, BOOL* stopped) = 0;
   
};