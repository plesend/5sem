#include "Header.h"
#include <iostream>
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
            cout << "\ncreate: failed to wwrite header\n";
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

    std::mutex mutex1;
    HTHANDLE::HTHANDLE() :
        Capacity(0),
        SecSnapshotInterval(0),
        MaxKeyLength(0),
        MaxPayloadLength(0),
        File(NULL),
        FileMapping(NULL),
        Addr(NULL),
        lastsnaptime(0),
        elementCount(0),
        DataAddr(NULL)
    {

    }
    HTHANDLE::HTHANDLE(int Capacity, int SecSnapshotInterval, int MaxKeyLength, int MaxPayloadLength, const char FileName[512], int elementCount) :
        Capacity(Capacity), SecSnapshotInterval(SecSnapshotInterval), MaxKeyLength(MaxKeyLength), MaxPayloadLength(MaxPayloadLength),
        File(NULL), FileMapping(NULL), Addr(NULL), elementCount(elementCount)
    {
        strcpy_s(this->FileName, 512, FileName);
    }

    Element::Element() :
        key(nullptr),
        keylength(0),
        payload(nullptr),
        payloadlength(0)
    {}
    Element::Element(const void* key, int keylength) :
        key(key),
        keylength(keylength),
        payload(nullptr),
        payloadlength(0)
    {}
    Element::Element(const void* key, int keylength, const void* payload, int payloadlength) :
        key(key),
        keylength(keylength),
        payload(payload),
        payloadlength(payloadlength)
    {}
    Element::Element(Element* oldelement, const void* newpayload, int newpayloadlength) :
        key(oldelement->key),
        keylength(oldelement->keylength),
        payload(newpayload),
        payloadlength(newpayloadlength)
    {}

    HTHANDLE* Create(int Capacity, int SecSnapshotInterval, int MaxKeyLength, int MaxPayloadLength, const char FileName[512], int elementCount)
    {
        lock_guard<mutex>lock(mutex1);
        HTHANDLE* ht = new HTHANDLE(Capacity, SecSnapshotInterval, MaxKeyLength, MaxPayloadLength, FileName, elementCount);


        ht->File = CreateFileA(FileName, GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (ht->File == INVALID_HANDLE_VALUE)
        {
            cout << "Create: file creation failed. " << "\n";
            delete ht;
            return NULL;
        }

        cout << "Create: file created" << "\n";

        if (!WriteHeaderToFile(ht))
        {
            CloseHandle(ht->File);
            delete ht;
            return 0;
        }

        int elementSize = 1 + ht->MaxKeyLength + ht->MaxPayloadLength;
        int dwMaxSizeLow = elementSize * ht->Capacity + HEADERSIZE;

        ht->FileMapping = CreateFileMappingA(ht->File, NULL,
            PAGE_READWRITE, 0, dwMaxSizeLow, NULL);

        if (ht->FileMapping == NULL || ht->FileMapping == INVALID_HANDLE_VALUE)
        {
            cout << "Create: FileMapping error " << "\n";
            delete ht;
            return NULL;
        }

        ht->Addr = MapViewOfFile(ht->FileMapping, FILE_MAP_ALL_ACCESS, 0, 0, 0);

        if (ht->Addr == NULL || ht->Addr == INVALID_HANDLE_VALUE)
        {
            cout << "Create: MapViewOfFile error " << "\n";
            delete ht;
            return NULL;
        }
        ht->DataAddr = (BYTE*)ht->Addr + HEADERSIZE;
        return ht;
    }
    HTHANDLE* Open(const char FileName[512])
    {
        std::lock_guard<std::mutex> lock(mutex1);

        HTHANDLE* ht = new HTHANDLE();

        ht->File = CreateFileA(FileName, GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (ht->File == INVALID_HANDLE_VALUE)
        {
            std::cout << "Open: file open failed.\n";
            delete ht;
            return nullptr;
        }
        std::cout << "Open: file opened\n";

        if (!ReadHeaderFromFile(ht))
        {
            CloseHandle(ht->File);
            delete ht;
            return nullptr;
        }

        int elementSize = 1 + ht->MaxKeyLength + ht->MaxPayloadLength;
        int dwMaxSizeLow = elementSize * ht->Capacity + HEADERSIZE;

        ht->FileMapping = CreateFileMappingA(ht->File, NULL,
            PAGE_READWRITE, 0, dwMaxSizeLow, NULL);

        if (ht->FileMapping == NULL || ht->FileMapping == INVALID_HANDLE_VALUE)
        {
            std::cout << "Open: FileMapping error\n";
            CloseHandle(ht->File);
            delete ht;
            return nullptr;
        }

        ht->Addr = MapViewOfFile(ht->FileMapping, FILE_MAP_ALL_ACCESS, 0, 0, 0);

        if (ht->Addr == NULL)
        {
            std::cout << "Open: MapViewOfFile error\n";
            CloseHandle(ht->FileMapping);
            CloseHandle(ht->File);
            delete ht;
            return nullptr;
        }

        ht->DataAddr = (BYTE*)ht->Addr + HEADERSIZE;
        return ht;
    }
    BOOL Snap(HTHANDLE* hthandle)
    {
        if (hthandle == NULL)
        {
            cout << "Snap: hthandle is null\n";
            return FALSE;
        }

        std::lock_guard<std::mutex> lock(mutex1);

        hthandle->lastsnaptime = time(0);

        auto now = std::chrono::system_clock::now();
        std::time_t now_c = std::chrono::system_clock::to_time_t(now);
        std::tm local_tm;
        localtime_s(&local_tm, &now_c);

        std::ostringstream filenameStream;
        filenameStream << "snapshotHELP_";
        filenameStream << std::put_time(&local_tm, "%Y-%m-%d_%H-%M-%S");
        filenameStream << ".htSNAP";
        std::string filename = filenameStream.str();

        HANDLE snapshot = CreateFileA(
            filename.c_str(),
            GENERIC_WRITE,
            0,
            NULL,
            CREATE_ALWAYS,
            FILE_ATTRIBUTE_NORMAL,
            NULL);

        if (snapshot == INVALID_HANDLE_VALUE)
        {
            cout << "\nSnap: CreateFileA error, code: " << GetLastError() << "\n";
            return FALSE;
        }

        int slotSize = 1 + hthandle->MaxKeyLength + hthandle->MaxPayloadLength;

        int totalSize = hthandle->Capacity * slotSize + HEADERSIZE;

        DWORD writtenBytesAmount = 0;
        BOOL writeResult = WriteFile(
            snapshot,
            hthandle->Addr,
            (DWORD)totalSize,
            &writtenBytesAmount,
            NULL);

        CloseHandle(snapshot);

        if (!writeResult || writtenBytesAmount != (DWORD)totalSize)
        {
            cout << "\nSnap: write ERROR. Written bytes: " << writtenBytesAmount << ", expected: " << totalSize << "\n";
            return FALSE;
        }

        cout << "\nSnapshot created: " << filename << ", size: " << writtenBytesAmount << " bytes\n";

        return TRUE;
    }
    BOOL Close(HTHANDLE* hthandle)
    {
        if (hthandle == NULL)
        {
            cout << "\nClose: hthandle is null";
            return false;
        }
        lock_guard<mutex>lock(mutex1);

        bool closeResult = true;

        if (hthandle->Addr != NULL)
        {
            if (UnmapViewOfFile(hthandle->Addr) == false)
            {
                closeResult = false;
            }
            hthandle->Addr = NULL;
        }
        if (hthandle->FileMapping != NULL && hthandle->FileMapping != INVALID_HANDLE_VALUE)
        {
            if (CloseHandle(hthandle->FileMapping) == false)
            {
                closeResult = false;
            }
            hthandle->FileMapping = NULL;
        }

        if (hthandle->File != NULL && hthandle->File != INVALID_HANDLE_VALUE)
        {
            if (CloseHandle(hthandle->File) == false)
            {
                closeResult = false;
            }
            hthandle->File = NULL;
        }
        cout << "\nClosed hthandle";
        return closeResult;
    }

    BOOL Insert(HTHANDLE* hthandle, const Element* element)
    {
        bool inserted = false;
        if (!hthandle || !element || !hthandle->DataAddr)
        {
            cout << "\nInsert: Invalid parameters";
            return FALSE;
        }
        if (element->keylength <= 0 || element->keylength > hthandle->MaxKeyLength)
        {
            cout << "\nInsert: Invalid key length";
            return FALSE;
        }
        if (element->payloadlength < 0 || element->payloadlength > hthandle->MaxPayloadLength)
        {
            cout << "\nInsert: Invalid payload length";
            return FALSE;
        }

        {
            std::lock_guard<std::mutex> lock(mutex1);

            if (hthandle->elementCount >= hthandle->Capacity)
            {
                cout << "\nInsert: HashTable full";
                return FALSE;
            }

            int slotSize = 1 + hthandle->MaxKeyLength + hthandle->MaxPayloadLength;
            BYTE* tablestart = (BYTE*)hthandle->DataAddr;

            unsigned int hash = HashFunction(element->key, element->keylength);
            int capacity = hthandle->Capacity;

            for (int i = 0; i < capacity; i++)
            {
                int idx = (hash + i) % capacity;
                BYTE* slot = tablestart + idx * slotSize;

                BYTE& isSlotUsedFlag = slot[0];
                BYTE* keyStart = slot + 1;
                BYTE* payloadPtr = slot + 1 + hthandle->MaxKeyLength;

                if (isSlotUsedFlag == 0)
                {
                    isSlotUsedFlag = 1;
                    memset(keyStart, 0, hthandle->MaxKeyLength);
                    memcpy(keyStart, element->key, element->keylength);

                    memset(payloadPtr, 0, hthandle->MaxPayloadLength);
                    if (element->payloadlength > 0 && element->payload != nullptr)
                        memcpy(payloadPtr, element->payload, element->payloadlength);

                    hthandle->elementCount++;
                    cout << "\nInsert: success at index " << idx;
                    inserted = true;
                    break;
                }
                else
                {
                    if (memcmp(keyStart, element->key, element->keylength) == 0)
                    {
                        memset(payloadPtr, 0, hthandle->MaxPayloadLength);
                        if (element->payloadlength > 0 && element->payload != nullptr)
                            memcpy(payloadPtr, element->payload, element->payloadlength);

                        cout << "\nInsert: key exists, payload updated at index " << idx;
                        inserted = true;
                        break;
                    }
                }
            }
        }
        if (inserted)
        {
            Snap(hthandle);
            return TRUE;
        }
        cout << "\nInsert: no free slot found";
        return FALSE;
    }
    BOOL Delete(HTHANDLE* hthandle, const Element* element)
    {
        if (!hthandle || !element || !hthandle->DataAddr)
        {
            cout << "\nDelete: Invalid parameters";
            return FALSE;
        }
        if (element->keylength <= 0 || element->keylength > hthandle->MaxKeyLength)
        {
            cout << "\nDelete: Invalid key length";
            return FALSE;
        }


        bool inserted = false;
        {
            std::lock_guard<std::mutex> lock(mutex1);
            int slotSize = 1 + hthandle->MaxKeyLength + hthandle->MaxPayloadLength;
            BYTE* base = (BYTE*)hthandle->DataAddr;

            unsigned int hash = HashFunction(element->key, element->keylength);
            int capacity = hthandle->Capacity;

            for (int i = 0; i < capacity; i++)
            {
                int idx = (hash + i) % capacity;
                BYTE* slot = base + idx * slotSize;

                BYTE& isUsed = slot[0];
                BYTE* keyPtr = slot + 1;

                if (isUsed == 0)
                {
                    inserted = false;
                    break;
                }

                if (memcmp(keyPtr, element->key, element->keylength) == 0)
                {
                    isUsed = 0;
                    memset(keyPtr, 0, hthandle->MaxKeyLength);
                    memset(slot + 1 + hthandle->MaxKeyLength, 0, hthandle->MaxPayloadLength);

                    hthandle->elementCount--;
                    cout << "\nDelete: success at index " << idx;
                    inserted = true;
                    break;
                }
            }
        }
        if (inserted)
        {
            Snap(hthandle);
            return TRUE;
        }

        return FALSE;
    }
    Element* Get(HTHANDLE* hthandle, const Element* element)
    {
        if (!hthandle || !element || !hthandle->DataAddr)
        {
            cout << "\nGet: Invalid parameters";
            return nullptr;
        }
        if (element->keylength <= 0 || element->keylength > hthandle->MaxKeyLength)
        {
            cout << "\nGet: Invalid key length";
            return nullptr;
        }

        std::lock_guard<std::mutex> lock(mutex1);

        int slotSize = 1 + hthandle->MaxKeyLength + hthandle->MaxPayloadLength;
        BYTE* dataStart = (BYTE*)hthandle->DataAddr;

        unsigned int hash = HashFunction(element->key, element->keylength);
        int capacity = hthandle->Capacity;

        for (int i = 0; i < capacity; i++)
        {
            int idx = (hash + i) % capacity;
            BYTE* slot = dataStart + idx * slotSize;

            BYTE& isUsed = slot[0];
            BYTE* keyPtr = slot + 1;
            BYTE* payloadPtr = slot + 1 + hthandle->MaxKeyLength;

            if (isUsed == 0)
                return nullptr;

            if (memcmp(keyPtr, element->key, element->keylength) == 0)
            {
                BYTE* keyCopy = new BYTE[element->keylength];
                memcpy(keyCopy, keyPtr, element->keylength);

                BYTE* payloadCopy = new BYTE[hthandle->MaxPayloadLength];
                memcpy(payloadCopy, payloadPtr, hthandle->MaxPayloadLength);

                return new Element(keyCopy, element->keylength, payloadCopy, hthandle->MaxPayloadLength);
            }
        }

        return nullptr;
    }
    BOOL Update(HTHANDLE* hthandle, const Element* oldelement, const void* newpayload, int newpayloadlength)
    {
        if (!hthandle || !oldelement || !hthandle->DataAddr)
        {
            cout << "\nUpdate: Invalid parameters";
            return FALSE;
        }
        if (oldelement->keylength <= 0 || oldelement->keylength > hthandle->MaxKeyLength)
        {
            cout << "\nUpdate: Invalid old key length";
            return FALSE;
        }
        if (newpayload == nullptr && newpayloadlength != 0)
        {
            cout << "\nUpdate: New payload pointer is null";
            return FALSE;
        }
        if (newpayloadlength < 0 || newpayloadlength > hthandle->MaxPayloadLength)
        {
            cout << "\nUpdate: Invalid new payload length";
            return FALSE;
        }

        bool inserted = false;
        {
            std::lock_guard<std::mutex> lock(mutex1);

            int slotSize = 1 + hthandle->MaxKeyLength + hthandle->MaxPayloadLength;
            BYTE* base = (BYTE*)hthandle->DataAddr;

            unsigned int hash = HashFunction(oldelement->key, oldelement->keylength);
            int capacity = hthandle->Capacity;

            for (int i = 0; i < capacity; i++)
            {
                int idx = (hash + i) % capacity;
                BYTE* slot = base + idx * slotSize;

                BYTE& isUsed = slot[0];
                BYTE* keyPtr = slot + 1;
                BYTE* payloadPtr = slot + 1 + hthandle->MaxKeyLength;

                if (isUsed == 0)
                {
                    inserted = false;
                    break;
                }

                if (memcmp(keyPtr, oldelement->key, oldelement->keylength) == 0)
                {
                    memset(payloadPtr, 0, hthandle->MaxPayloadLength);
                    if (newpayloadlength > 0 && newpayload != nullptr)
                        memcpy(payloadPtr, newpayload, newpayloadlength);

                    cout << "\nUpdate: success at index " << idx;
                    inserted = true;
                    break;
                }
            }
        }
        if (inserted)
        {
            Snap(hthandle);
            return TRUE;
        }

        return FALSE;
    }
    void print(const Element* element)
    {
        cout << "----------------------------ELEMENT--------------------------" << endl;
        cout << "\nelement key: " << (char*)element->key << "\nelement payload: " << (char*)element->payload << endl;
        cout << "----------------------------GG--------------------------" << endl;
    }
}