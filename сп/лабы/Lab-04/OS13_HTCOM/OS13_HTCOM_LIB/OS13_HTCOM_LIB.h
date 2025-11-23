#pragma once

#ifdef OS13HTCOMLIB_EXPORTS
#define OS13HTCOMLIB_API 
#else
#define OS13HTCOMLIB_API 
#endif

#include "OS13_HTCOM.h"

// Глобальная переменная для COM компонента
extern IHT* g_pHT;

OS13HTCOMLIB_API BOOL InitializeHTCOM();
OS13HTCOMLIB_API void UninitializeHTCOM();

OS13HTCOMLIB_API HTStorage* HTCreate(const char* fileName, DWORD capacity, DWORD maxKeyLength, DWORD maxDataLength, DWORD snapshotInterval);
OS13HTCOMLIB_API HTStorage* HTOpen(const char* fileName);
OS13HTCOMLIB_API BOOL HTStart(HTStorage* ht);
OS13HTCOMLIB_API BOOL HTStop(HTStorage* ht);
OS13HTCOMLIB_API BOOL HTClose(HTStorage* ht);
OS13HTCOMLIB_API BOOL HTInsert(HTStorage* ht, const char* key, const char* data);
OS13HTCOMLIB_API BOOL HTUpdate(HTStorage* ht, const char* key, const char* data);
OS13HTCOMLIB_API BOOL HTDelete(HTStorage* ht, const char* key);
OS13HTCOMLIB_API char* HTGet(HTStorage* ht, const char* key);
OS13HTCOMLIB_API BOOL HTSnapshot(HTStorage* ht);
OS13HTCOMLIB_API DWORD GetLastErrorHT();
OS13HTCOMLIB_API void PrintHTInfo(HTStorage* ht);