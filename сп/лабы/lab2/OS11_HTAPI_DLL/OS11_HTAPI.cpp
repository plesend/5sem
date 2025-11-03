#include "pch.h"  
#include "Header.h"
#include <iostream>
#include <future>
#include <Windows.h>
#include <cstring>
#include <thread>
#include <string>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

#define HEADERSIZE 20
using namespace std;

namespace HT
{
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

    bool WriteHeaderToFile(HTHANDLE* ht)
    {
        DWORD written;
        if (!WriteFile(ht->File, &ht->Capacity, sizeof(ht->Capacity), &written, NULL) ||
            !WriteFile(ht->File, &ht->SecSnapshotInterval, sizeof(ht->SecSnapshotInterval), &written, NULL) ||
            !WriteFile(ht->File, &ht->MaxKeyLength, sizeof(ht->MaxKeyLength), &written, NULL) ||
            !WriteFile(ht->File, &ht->MaxPayloadLength, sizeof(ht->MaxPayloadLength), &written, NULL) ||
            !WriteFile(ht->File, &ht->elementCount, sizeof(ht->elementCount), &written, NULL))
        {
            cout << "\ncreate: failed to write header\n";
            return false;
        }
        return true;
    }

    bool ReadHeaderFromFile(HTHANDLE* ht)
    {
        DWORD read;
        if (!ReadFile(ht->File, &ht->Capacity, sizeof(ht->Capacity), &read, NULL) ||
            !ReadFile(ht->File, &ht->SecSnapshotInterval, sizeof(ht->SecSnapshotInterval), &read, NULL) ||
            !ReadFile(ht->File, &ht->MaxKeyLength, sizeof(ht->MaxKeyLength), &read, NULL) ||
            !ReadFile(ht->File, &ht->MaxPayloadLength, sizeof(ht->MaxPayloadLength), &read, NULL) ||
            !ReadFile(ht->File, &ht->elementCount, sizeof(ht->elementCount), &read, NULL))
        {
            cout << "\nopen: failed to read header\n";
            return false;
        }
        return true;
    }

    HTHANDLE::HTHANDLE() :
        Capacity(0), SecSnapshotInterval(0), MaxKeyLength(0), MaxPayloadLength(0),
        File(NULL), FileMapping(NULL), Addr(NULL), lastsnaptime(0), elementCount(0),
        DataAddr(NULL), Mutex(NULL) {
    }

    HTHANDLE::HTHANDLE(int Capacity, int SecSnapshotInterval, int MaxKeyLength, int MaxPayloadLength, const char FileName[512], int elementCount) :
        Capacity(Capacity), SecSnapshotInterval(SecSnapshotInterval), MaxKeyLength(MaxKeyLength), MaxPayloadLength(MaxPayloadLength),
        File(NULL), FileMapping(NULL), Addr(NULL), elementCount(elementCount)
    {
        strcpy_s(this->FileName, 512, FileName);
    }

    Element::Element() : key(nullptr), keylength(0), payload(nullptr), payloadlength(0) {}
    Element::Element(const void* key, int keylength) : key(key), keylength(keylength), payload(nullptr), payloadlength(0) {}
    Element::Element(const void* key, int keylength, const void* payload, int payloadlength) :
        key(key), keylength(keylength), payload(payload), payloadlength(payloadlength) {
    }
    Element::Element(Element* oldelement, const void* newpayload, int newpayloadlength) :
        key(oldelement->key), keylength(oldelement->keylength), payload(newpayload), payloadlength(newpayloadlength) {
    }

    string MakeSafeMutexName(const char* filename)
    {
        std::string name(filename);
        for (auto& c : name) if (!isalnum(c)) c = '_';
        return "Global\\HTMutex_" + name;
    }

    HTHANDLE* Create(int Capacity, int SecSnapshotInterval, int MaxKeyLength, int MaxPayloadLength, const char FileName[512], int elementCount)
    {
        HTHANDLE* ht = new HTHANDLE(Capacity, SecSnapshotInterval, MaxKeyLength, MaxPayloadLength, FileName, elementCount);

        ht->File = CreateFileA(FileName, GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (ht->File == INVALID_HANDLE_VALUE)
        {
            cout << "Create: file creation failed.\n";
            CleanupHandle(ht);
            return NULL;
        }

        cout << "Create: file created\n";

        if (!WriteHeaderToFile(ht))
        {
            CleanupHandle(ht);
            return NULL;
        }

        int elementSize = 1 + ht->MaxKeyLength + ht->MaxPayloadLength;
        int dwMaxSizeLow = elementSize * ht->Capacity + HEADERSIZE;

        ht->FileMapping = CreateFileMappingA(ht->File, NULL, PAGE_READWRITE, 0, dwMaxSizeLow, NULL);
        if (!ht->FileMapping || ht->FileMapping == INVALID_HANDLE_VALUE)
        {
            cout << "Create: FileMapping error\n";
            CleanupHandle(ht);
            return NULL;
        }

        ht->Addr = MapViewOfFile(ht->FileMapping, FILE_MAP_ALL_ACCESS, 0, 0, 0);
        if (!ht->Addr || ht->Addr == INVALID_HANDLE_VALUE)
        {
            cout << "Create: MapViewOfFile error\n";
            CleanupHandle(ht);
            return NULL;
        }
        ht->DataAddr = (BYTE*)ht->Addr + HEADERSIZE;

        ht->Mutex = CreateMutexA(NULL, FALSE, MakeSafeMutexName(FileName).c_str());
        if (!ht->Mutex)
        {
            strcpy_s(ht->LastErrorMessage, "Create: Cannot create mutex");
            CleanupHandle(ht);
            return NULL;
        }

        return ht;
    }

    HTHANDLE* Open(const char FileName[512])
    {
        HTHANDLE* ht = new HTHANDLE();

        ht->File = CreateFileA(FileName, GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (ht->File == INVALID_HANDLE_VALUE)
        {
            cout << "Open: file open failed.\n";
            CleanupHandle(ht);
            return nullptr;
        }
        cout << "Open: file opened\n";

        if (!ReadHeaderFromFile(ht))
        {
            CleanupHandle(ht);
            return nullptr;
        }

        int elementSize = 1 + ht->MaxKeyLength + ht->MaxPayloadLength;
        int dwMaxSizeLow = elementSize * ht->Capacity + HEADERSIZE;

        ht->FileMapping = CreateFileMappingA(ht->File, NULL, PAGE_READWRITE, 0, dwMaxSizeLow, NULL);
        if (!ht->FileMapping || ht->FileMapping == INVALID_HANDLE_VALUE)
        {
            cout << "Open: FileMapping error\n";
            CleanupHandle(ht);
            return nullptr;
        }

        ht->Addr = MapViewOfFile(ht->FileMapping, FILE_MAP_ALL_ACCESS, 0, 0, 0);
        if (!ht->Addr)
        {
            cout << "Open: MapViewOfFile error\n";
            CleanupHandle(ht);
            return nullptr;
        }
        ht->DataAddr = (BYTE*)ht->Addr + HEADERSIZE;

        ht->Mutex = OpenMutexA(SYNCHRONIZE, FALSE, MakeSafeMutexName(FileName).c_str());
        if (!ht->Mutex)
        {
            strcpy_s(ht->LastErrorMessage, "Open: Cannot open mutex");
            CleanupHandle(ht);
            return nullptr;
        }

        return ht;
    }

    BOOL Snap(HTHANDLE* hthandle)
    {
        if (!hthandle) return FALSE;

        if (hthandle->snapFuture.valid()) hthandle->snapFuture.wait();

        hthandle->snapFuture = std::async(std::launch::async, [hthandle]() {
            WaitForSingleObject(hthandle->Mutex, INFINITE);

            int slotSize = 1 + hthandle->MaxKeyLength + hthandle->MaxPayloadLength;
            int totalSize = hthandle->Capacity * slotSize + HEADERSIZE;

            std::vector<BYTE> buffer(totalSize);
            memcpy(buffer.data(), hthandle->Addr, totalSize);

            ReleaseMutex(hthandle->Mutex);

            auto now = std::chrono::system_clock::now();
            std::time_t now_c = std::chrono::system_clock::to_time_t(now);
            std::tm local_tm;
            localtime_s(&local_tm, &now_c);

            std::ostringstream filenameStream;
            filenameStream << "snapshot_" << std::put_time(&local_tm, "%Y-%m-%d_%H-%M-%S") << ".htSNAP";
            std::string filename = filenameStream.str();

            HANDLE snapshot = CreateFileA(filename.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
            if (snapshot != INVALID_HANDLE_VALUE)
            {
                DWORD written;
                WriteFile(snapshot, buffer.data(), (DWORD)totalSize, &written, NULL);
                CloseHandle(snapshot);
                std::cout << "Snap: snapshot created: " << filename << "\n";
            }
            else
            {
                std::cout << "Snap: CreateFileA error, code: " << GetLastError() << "\n";
            }
            });

        std::cout << "Snap started asynchronously...\n";
        return TRUE;
    }

    void CleanupHandle(HTHANDLE* ht)
    {
        if (!ht) return;
        if (ht->Addr) UnmapViewOfFile(ht->Addr);
        if (ht->FileMapping && ht->FileMapping != INVALID_HANDLE_VALUE) CloseHandle(ht->FileMapping);
        if (ht->File && ht->File != INVALID_HANDLE_VALUE) CloseHandle(ht->File);
        if (ht->Mutex) CloseHandle(ht->Mutex);
        delete ht;
    }

}
