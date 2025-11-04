#pragma once
#include <Windows.h>
#include <future>
#include <string>

#ifdef HTAPI_EXPORTS
#define HTAPI __declspec(dllexport)
#else
#define HTAPI __declspec(dllimport)
#endif

namespace HT
{
    struct HTHANDLE
    {
        HTHANDLE();
        HTHANDLE(int Capacity, int SecSnapshotInterval, int MaxKeyLength, int MaxPayloadLength, const char FileName[512], int elementCount);
        int Capacity;
        int elementCount;
        int SecSnapshotInterval;
        int MaxKeyLength;
        int MaxPayloadLength;
        char FileName[512];
        HANDLE File;
        HANDLE FileMapping;
        LPVOID Addr;
        char LastErrorMessage[512];
        time_t lastsnaptime;
        BYTE* DataAddr;
        std::future<void> snapFuture;
        HANDLE Mutex;
    };

    struct Element
    {
        Element();
        Element(const void* key, int keylength);
        Element(const void* key, int keylength, const void* payload, int payloadlength);
        Element(Element* oldelement, const void* newpayload, int newpayloadlength);
        const void* key;
        int keylength;
        const void* payload;
        int payloadlength;
    };
    extern "C" HTAPI void CleanupHandle(HTHANDLE* ht);
    extern "C" HTAPI unsigned int HashFunction(const void* key, int keylength);
    extern "C" HTAPI HTHANDLE* Create(int Capacity, int SecSnapshotInterval, int MaxKeyLength, int MaxPayloadLength, const char FileName[512], int elementCount);
    extern "C" HTAPI HTHANDLE* Open(const char FileName[512]);
    extern "C" HTAPI BOOL Snap(HTHANDLE* hthandle);
    extern "C" HTAPI BOOL Close(HTHANDLE* hthandle);
    extern "C" HTAPI BOOL Insert(HTHANDLE* hthandle, const Element* element);
    extern "C" HTAPI BOOL Delete(HTHANDLE* hthandle, const Element* element);
    extern "C" HTAPI Element* Get(HTHANDLE* hthandle, const Element* element);
    extern "C" HTAPI BOOL Update(HTHANDLE* hthandle, const Element* oldelement, const void* newpayload, int newpayloadlength);
    extern "C" HTAPI void print(const Element* element);

}
