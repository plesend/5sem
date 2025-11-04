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
        if (!ht) return false;
        if (!ht->File) return false;

        if (ht->Mutex == NULL)
        {
            strcpy_s(ht->LastErrorMessage, "WriteHeaderToFile: Mutex is NULL");
            return false;
        }
        DWORD wait = WaitForSingleObject(ht->Mutex, INFINITE);
        if (wait != WAIT_OBJECT_0)
        {
            strcpy_s(ht->LastErrorMessage, "WriteHeaderToFile: WaitForSingleObject failed");
            return false;
        }

        SetFilePointer(ht->File, 0, NULL, FILE_BEGIN);

        DWORD written;
        BOOL ok =
            WriteFile(ht->File, &ht->Capacity, sizeof(ht->Capacity), &written, NULL) &&
            WriteFile(ht->File, &ht->SecSnapshotInterval, sizeof(ht->SecSnapshotInterval), &written, NULL) &&
            WriteFile(ht->File, &ht->MaxKeyLength, sizeof(ht->MaxKeyLength), &written, NULL) &&
            WriteFile(ht->File, &ht->MaxPayloadLength, sizeof(ht->MaxPayloadLength), &written, NULL) &&
            WriteFile(ht->File, &ht->elementCount, sizeof(ht->elementCount), &written, NULL);

        if (ok) FlushFileBuffers(ht->File);

        ReleaseMutex(ht->Mutex);

        if (!ok)
        {
            strcpy_s(ht->LastErrorMessage, "WriteHeaderToFile: failed to write header");
            return false;
        }
        return true;
    }

    bool ReadHeaderFromFile(HTHANDLE* ht)
    {
        if (!ht || !ht->File) return false;

        SetFilePointer(ht->File, 0, NULL, FILE_BEGIN);

        DWORD read;
        BOOL ok =
            ReadFile(ht->File, &ht->Capacity, sizeof(ht->Capacity), &read, NULL) && read == sizeof(ht->Capacity) &&
            ReadFile(ht->File, &ht->SecSnapshotInterval, sizeof(ht->SecSnapshotInterval), &read, NULL) && read == sizeof(ht->SecSnapshotInterval) &&
            ReadFile(ht->File, &ht->MaxKeyLength, sizeof(ht->MaxKeyLength), &read, NULL) && read == sizeof(ht->MaxKeyLength) &&
            ReadFile(ht->File, &ht->MaxPayloadLength, sizeof(ht->MaxPayloadLength), &read, NULL) && read == sizeof(ht->MaxPayloadLength) &&
            ReadFile(ht->File, &ht->elementCount, sizeof(ht->elementCount), &read, NULL) && read == sizeof(ht->elementCount);

        return ok;
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

        strcpy_s(ht->FileName, 512, FileName);

        ht->Mutex = CreateMutexA(NULL, FALSE, MakeSafeMutexName(FileName).c_str());
        if (!ht->Mutex)
        {
            strcpy_s(ht->LastErrorMessage, "Create: Cannot create mutex");
            CleanupHandle(ht);
            return NULL;
        }

        DWORD fileSize = GetFileSize(ht->File, NULL);
        if (fileSize < HEADERSIZE)
        {
            if (!WriteHeaderToFile(ht))
            {
                CleanupHandle(ht);
                return nullptr;
            }
        }

        int elementSize = 1 + ht->MaxKeyLength + ht->MaxPayloadLength;
        int totalSize = HEADERSIZE + ht->Capacity * elementSize;
        SetFilePointer(ht->File, totalSize - 1, NULL, FILE_BEGIN);
        SetEndOfFile(ht->File);

        ht->FileMapping = CreateFileMappingA(ht->File, NULL, PAGE_READWRITE, 0, totalSize, NULL);
        if (!ht->FileMapping || ht->FileMapping == INVALID_HANDLE_VALUE)
        {
            cout << "Create: FileMapping error\n";
            CleanupHandle(ht);
            return NULL;
        }

        ht->Addr = MapViewOfFile(ht->FileMapping, FILE_MAP_ALL_ACCESS, 0, 0, 0);
        if (!ht->Addr)
        {
            cout << "Create: MapViewOfFile error\n";
            CleanupHandle(ht);
            return NULL;
        }
        ht->DataAddr = (BYTE*)ht->Addr + HEADERSIZE;

        return ht;
    }

    HTHANDLE* Open(const char FileName[512])
    {
        HTHANDLE* ht = new HTHANDLE();

        ht->File = CreateFileA(FileName, GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (ht->File == INVALID_HANDLE_VALUE)
        {
            cout << "Open: file open failed. Error code: " << GetLastError() << "\n";
            CleanupHandle(ht);
            return nullptr;
        }
        cout << "Open: file opened\n";

        SetFilePointer(ht->File, 0, NULL, FILE_BEGIN);
        DWORD read;
        BOOL ok =
            ReadFile(ht->File, &ht->Capacity, sizeof(ht->Capacity), &read, NULL) && read == sizeof(ht->Capacity) &&
            ReadFile(ht->File, &ht->SecSnapshotInterval, sizeof(ht->SecSnapshotInterval), &read, NULL) && read == sizeof(ht->SecSnapshotInterval) &&
            ReadFile(ht->File, &ht->MaxKeyLength, sizeof(ht->MaxKeyLength), &read, NULL) && read == sizeof(ht->MaxKeyLength) &&
            ReadFile(ht->File, &ht->MaxPayloadLength, sizeof(ht->MaxPayloadLength), &read, NULL) && read == sizeof(ht->MaxPayloadLength) &&
            ReadFile(ht->File, &ht->elementCount, sizeof(ht->elementCount), &read, NULL) && read == sizeof(ht->elementCount);

        if (!ok)
        {
            cout << "Open: failed to read header\n";
            CleanupHandle(ht);
            return nullptr;
        }

        strcpy_s(ht->FileName, 512, FileName);

        ht->Mutex = OpenMutexA(SYNCHRONIZE, FALSE, MakeSafeMutexName(FileName).c_str());
        if (!ht->Mutex)
        {
            cout << "Open: mutex not found, creating a new one\n";
            ht->Mutex = CreateMutexA(NULL, FALSE, MakeSafeMutexName(FileName).c_str());
            if (!ht->Mutex)
            {
                cout << "Open: failed to create mutex\n";
                CleanupHandle(ht);
                return nullptr;
            }
        }

        int elementSize = 1 + ht->MaxKeyLength + ht->MaxPayloadLength;
        int totalSize = HEADERSIZE + ht->Capacity * elementSize;

        SetFilePointer(ht->File, totalSize - 1, NULL, FILE_BEGIN);
        SetEndOfFile(ht->File);

        ht->FileMapping = CreateFileMappingA(ht->File, NULL, PAGE_READWRITE, 0, totalSize, NULL);
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

        cout << "Open: HT-handle ready. Capacity=" << ht->Capacity
            << " MaxKeyLength=" << ht->MaxKeyLength
            << " MaxPayloadLength=" << ht->MaxPayloadLength
            << " elementCount=" << ht->elementCount << "\n";

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

    BOOL Close(HTHANDLE* hthandle)
    {
        if (hthandle == NULL)
        {
            cout << "\nClose: hthandle is null";
            return false;
        }

        if (hthandle->Mutex)
        {
            DWORD wait = WaitForSingleObject(hthandle->Mutex, INFINITE);
            if (wait == WAIT_OBJECT_0)
            {
                bool closeResult = true;

                if (hthandle->Addr != NULL)
                {
                    if (!UnmapViewOfFile(hthandle->Addr))
                    {
                        closeResult = false;
                    }
                    hthandle->Addr = NULL;
                }
                if (hthandle->FileMapping != NULL && hthandle->FileMapping != INVALID_HANDLE_VALUE)
                {
                    if (!CloseHandle(hthandle->FileMapping))
                    {
                        closeResult = false;
                    }
                    hthandle->FileMapping = NULL;
                }

                if (hthandle->File != NULL && hthandle->File != INVALID_HANDLE_VALUE)
                {
                    if (!CloseHandle(hthandle->File))
                    {
                        closeResult = false;
                    }
                    hthandle->File = NULL;
                }

                ReleaseMutex(hthandle->Mutex);

                cout << "\nClosed hthandle";
                return closeResult;
            }
            else
            {
                cout << "\nClose: failed to acquire mutex";
                return FALSE;
            }
        }
        else
        {
            bool closeResult = true;
            if (hthandle->Addr != NULL)
            {
                if (!UnmapViewOfFile(hthandle->Addr)) closeResult = false;
                hthandle->Addr = NULL;
            }
            if (hthandle->FileMapping != NULL && hthandle->FileMapping != INVALID_HANDLE_VALUE)
            {
                if (!CloseHandle(hthandle->FileMapping)) closeResult = false;
                hthandle->FileMapping = NULL;
            }
            if (hthandle->File != NULL && hthandle->File != INVALID_HANDLE_VALUE)
            {
                if (!CloseHandle(hthandle->File)) closeResult = false;
                hthandle->File = NULL;
            }
            cout << "\nClosed hthandle (no mutex)";
            return closeResult;
        }
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

        if (!hthandle->Mutex)
        {
            cout << "\nInsert: Mutex is null";
            return FALSE;
        }

        DWORD wait = WaitForSingleObject(hthandle->Mutex, INFINITE);
        if (wait != WAIT_OBJECT_0)
        {
            cout << "\nInsert: WaitForSingleObject failed";
            return FALSE;
        }

        do
        {
            if (hthandle->elementCount >= hthandle->Capacity)
            {
                cout << "\nInsert: HashTable full";
                break;
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

            // Если вставлено — обновляем заголовок файла (elementCount)
            if (inserted)
            {
                if (!WriteHeaderToFile(hthandle))
                {
                    cout << "\nInsert: warning - failed to write header after insert";
                }
            }

        } while (false);

        ReleaseMutex(hthandle->Mutex);

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

        if (!hthandle->Mutex)
        {
            cout << "\nDelete: Mutex is null";
            return FALSE;
        }

        bool removed = false;
        DWORD wait = WaitForSingleObject(hthandle->Mutex, INFINITE);
        if (wait != WAIT_OBJECT_0)
        {
            cout << "\nDelete: WaitForSingleObject failed";
            return FALSE;
        }

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
                removed = false;
                break;
            }

            if (memcmp(keyPtr, element->key, element->keylength) == 0)
            {
                isUsed = 0;
                memset(keyPtr, 0, hthandle->MaxKeyLength);
                memset(slot + 1 + hthandle->MaxKeyLength, 0, hthandle->MaxPayloadLength);

                hthandle->elementCount--;
                cout << "\nDelete: success at index " << idx;
                removed = true;
                break;
            }
        }

        if (removed)
        {
            if (!WriteHeaderToFile(hthandle))
            {
                cout << "\nDelete: warning - failed to write header after delete";
            }
            ReleaseMutex(hthandle->Mutex);
            Snap(hthandle);
            return TRUE;
        }

        ReleaseMutex(hthandle->Mutex);
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

        if (!hthandle->Mutex)
        {
            cout << "\nGet: Mutex is null";
            return nullptr;
        }

        DWORD wait = WaitForSingleObject(hthandle->Mutex, INFINITE);
        if (wait != WAIT_OBJECT_0)
        {
            cout << "\nGet: WaitForSingleObject failed";
            return nullptr;
        }

        int slotSize = 1 + hthandle->MaxKeyLength + hthandle->MaxPayloadLength;
        BYTE* dataStart = (BYTE*)hthandle->DataAddr;

        unsigned int hash = HashFunction(element->key, element->keylength);
        int capacity = hthandle->Capacity;

        Element* result = nullptr;

        for (int i = 0; i < capacity; i++)
        {
            int idx = (hash + i) % capacity;
            BYTE* slot = dataStart + idx * slotSize;

            BYTE& isUsed = slot[0];
            BYTE* keyPtr = slot + 1;
            BYTE* payloadPtr = slot + 1 + hthandle->MaxKeyLength;

            if (isUsed == 0)
            {
                // нет дальше
                break;
            }

            if (memcmp(keyPtr, element->key, element->keylength) == 0)
            {
                BYTE* keyCopy = new BYTE[element->keylength];
                memcpy(keyCopy, keyPtr, element->keylength);

                BYTE* payloadCopy = new BYTE[hthandle->MaxPayloadLength];
                memcpy(payloadCopy, payloadPtr, hthandle->MaxPayloadLength);

                result = new Element(keyCopy, element->keylength, payloadCopy, hthandle->MaxPayloadLength);
                break;
            }
        }

        ReleaseMutex(hthandle->Mutex);
        return result;
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

        if (!hthandle->Mutex)
        {
            cout << "\nUpdate: Mutex is null";
            return FALSE;
        }

        bool updated = false;
        DWORD wait = WaitForSingleObject(hthandle->Mutex, INFINITE);
        if (wait != WAIT_OBJECT_0)
        {
            cout << "\nUpdate: WaitForSingleObject failed";
            return FALSE;
        }

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
                updated = false;
                break;
            }

            if (memcmp(keyPtr, oldelement->key, oldelement->keylength) == 0)
            {
                memset(payloadPtr, 0, hthandle->MaxPayloadLength);
                if (newpayloadlength > 0 && newpayload != nullptr)
                    memcpy(payloadPtr, newpayload, newpayloadlength);

                cout << "\nUpdate: success at index " << idx;
                updated = true;
                break;
            }
        }

        if (updated)
        {
            if (!WriteHeaderToFile(hthandle))
            {
                cout << "\nUpdate: warning - failed to write header after update";
            }
            ReleaseMutex(hthandle->Mutex);
            Snap(hthandle);
            return TRUE;
        }

        ReleaseMutex(hthandle->Mutex);
        return FALSE;
    }
    void print(const Element* element)
    {
        cout << "----------------------------ELEMENT--------------------------" << endl;
        cout << "\nelement key: " << (char*)element->key << "\nelement payload: " << (char*)element->payload << endl;
        cout << "----------------------------GG--------------------------" << endl;
    }

}
