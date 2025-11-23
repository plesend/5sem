#include "OS13_HTCOM_LIB.h"
#include <iostream>
#include <string>

IHT* g_pHT = nullptr;
DWORD lastErrorHT = 0;

BOOL InitializeHTCOM()
{
    HRESULT hr = CoInitialize(NULL);
    if (FAILED(hr)) {
        lastErrorHT = hr;
        return FALSE;
    }

    IClassFactory* pFactory = nullptr;
    hr = CoGetClassObject(CLSID_OS13, CLSCTX_INPROC_SERVER, NULL, IID_IClassFactory, (void**)&pFactory);
    if (FAILED(hr)) {
        lastErrorHT = hr;
        CoUninitialize();
        return FALSE;
    }

    hr = pFactory->CreateInstance(NULL, IID_IHT, (void**)&g_pHT);
    pFactory->Release();

    if (FAILED(hr)) {
        lastErrorHT = hr;
        CoUninitialize();
        return FALSE;
    }

    return TRUE;
}

void UninitializeHTCOM()
{
    if (g_pHT) {
        g_pHT->Release();
        g_pHT = nullptr;
    }
    CoUninitialize();
}

// Реализация функций HT API через COM компонент
OS13HTCOMLIB_API HTStorage* HTCreate(const char* fileName, DWORD capacity, DWORD maxKeyLength, DWORD maxDataLength, DWORD snapshotInterval)
{
    if (!g_pHT) {
        lastErrorHT = ERROR_INVALID_HANDLE;
        return NULL;
    }

    HTStorage* result = nullptr;
    HRESULT hr = g_pHT->HTCreate(fileName, capacity, maxKeyLength, maxDataLength, snapshotInterval, &result);

    if (FAILED(hr)) {
        lastErrorHT = hr;
        return NULL;
    }

    return result;
}

OS13HTCOMLIB_API HTStorage* HTOpen(const char* fileName)
{
    if (!g_pHT) {
        lastErrorHT = ERROR_INVALID_HANDLE;
        return NULL;
    }

    HTStorage* result = nullptr;
    HRESULT hr = g_pHT->HTOpen(fileName, &result);

    if (FAILED(hr)) {
        lastErrorHT = hr;
        return NULL;
    }

    return result;
}

OS13HTCOMLIB_API BOOL HTStart(HTStorage* ht)
{
    if (!g_pHT) {
        lastErrorHT = ERROR_INVALID_HANDLE;
        return FALSE;
    }

    HRESULT hr = g_pHT->HTStart(ht);
    if (FAILED(hr)) {
        lastErrorHT = hr;
        return FALSE;
    }

    return TRUE;
}

OS13HTCOMLIB_API BOOL HTStop(HTStorage* ht)
{
    if (!g_pHT) {
        lastErrorHT = ERROR_INVALID_HANDLE;
        return FALSE;
    }

    HRESULT hr = g_pHT->HTStop(ht);
    if (FAILED(hr)) {
        lastErrorHT = hr;
        return FALSE;
    }

    return TRUE;
}

OS13HTCOMLIB_API BOOL HTClose(HTStorage* ht)
{
    if (!g_pHT) {
        lastErrorHT = ERROR_INVALID_HANDLE;
        return FALSE;
    }

    HRESULT hr = g_pHT->HTClose(ht);
    if (FAILED(hr)) {
        lastErrorHT = hr;
        return FALSE;
    }

    return TRUE;
}

OS13HTCOMLIB_API BOOL HTInsert(HTStorage* ht, const char* key, const char* data)
{
    if (!g_pHT) {
        lastErrorHT = ERROR_INVALID_HANDLE;
        return FALSE;
    }

    HRESULT hr = g_pHT->HTInsert(ht, key, data);
    if (FAILED(hr)) {
        lastErrorHT = hr;
        return FALSE;
    }

    return TRUE;
}

OS13HTCOMLIB_API BOOL HTUpdate(HTStorage* ht, const char* key, const char* data)
{
    if (!g_pHT) {
        lastErrorHT = ERROR_INVALID_HANDLE;
        return FALSE;
    }

    HRESULT hr = g_pHT->HTUpdate(ht, key, data);
    if (FAILED(hr)) {
        lastErrorHT = hr;
        return FALSE;
    }

    return TRUE;
}

OS13HTCOMLIB_API BOOL HTDelete(HTStorage* ht, const char* key)
{
    if (!g_pHT) {
        lastErrorHT = ERROR_INVALID_HANDLE;
        return FALSE;
    }

    HRESULT hr = g_pHT->HTDelete(ht, key);
    if (FAILED(hr)) {
        lastErrorHT = hr;
        return FALSE;
    }

    return TRUE;
}

OS13HTCOMLIB_API char* HTGet(HTStorage* ht, const char* key)
{
    if (!g_pHT) {
        lastErrorHT = ERROR_INVALID_HANDLE;
        return NULL;
    }

    char* result = nullptr;
    HRESULT hr = g_pHT->HTGet(ht, key, &result);
    if (FAILED(hr)) {
        lastErrorHT = hr;
        return NULL;
    }

    return result;
}

OS13HTCOMLIB_API BOOL HTSnapshot(HTStorage* ht)
{
    if (!g_pHT) {
        lastErrorHT = ERROR_INVALID_HANDLE;
        return FALSE;
    }

    HRESULT hr = g_pHT->HTSnapshot(ht);
    if (FAILED(hr)) {
        lastErrorHT = hr;
        return FALSE;
    }

    return TRUE;
}

OS13HTCOMLIB_API DWORD GetLastErrorHT()
{
    return lastErrorHT;
}

OS13HTCOMLIB_API void PrintHTInfo(HTStorage* ht)
{
    if (!g_pHT) {
        return;
    }

    g_pHT->PrintHTInfo(ht);
}