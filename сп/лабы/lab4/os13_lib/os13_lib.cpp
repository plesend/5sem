//
#include "pch.h"
#include "os13_lib.h"
#include <atomic>

static IHT* g_pIHT = nullptr;
static std::atomic<LONG> g_comRefCount(0);

HRESULT GetIHTInterface(IHT** ppIHT)
{
    if (ppIHT == nullptr) return E_INVALIDARG;

    HRESULT hr = CoCreateInstance(CLSID_OS13, nullptr, CLSCTX_INPROC_SERVER, IID_IHT, (void**)ppIHT);
    if (FAILED(hr)) {
        std::cout << "Failed to create COM instance: 0x" << std::hex << hr << std::endl;
    }
    return hr;
}

 OS13_LIB_API HRESULT HTCreate(
    int Capacity,
    int SecSnapshotInterval,
    int MaxKeyLength,
    int MaxPayloadLength,
    const char* FileName,
    HTHANDLE** result)
{
    IHT* pIHT = nullptr;
    HRESULT hr = GetIHTInterface(&pIHT);
    if (FAILED(hr)) return hr;

    hr = pIHT->Create(Capacity, SecSnapshotInterval, MaxKeyLength, MaxPayloadLength, FileName, result);
    pIHT->Release();
    return hr;
}

 OS13_LIB_API HRESULT HTOpen(
    const char* FileName,
    HTHANDLE** result)
{
    IHT* pIHT = nullptr;
    HRESULT hr = GetIHTInterface(&pIHT);
    if (FAILED(hr)) return hr;

    hr = pIHT->Open(FileName, result);
    pIHT->Release();
    return hr;
}

 OS13_LIB_API HRESULT HTClose(
    HTHANDLE* ht)
{
    if (ht == nullptr) return E_INVALIDARG;

    IHT* pIHT = nullptr;
    HRESULT hr = GetIHTInterface(&pIHT);
    if (FAILED(hr)) return hr;

    hr = pIHT->Close(ht);
    pIHT->Release();
    return hr;
}

 OS13_LIB_API HRESULT HTInsert(
    HTHANDLE* ht,
    const Element* element)
{
    if (ht == nullptr || element == nullptr) return E_INVALIDARG;

    IHT* pIHT = nullptr;
    HRESULT hr = GetIHTInterface(&pIHT);
    if (FAILED(hr)) return hr;

    hr = pIHT->Insert(ht, element);
    pIHT->Release();
    return hr;
}

 OS13_LIB_API HRESULT HTDelete(
    HTHANDLE* ht,
    const Element* element)
{
    if (ht == nullptr || element == nullptr) return E_INVALIDARG;

    IHT* pIHT = nullptr;
    HRESULT hr = GetIHTInterface(&pIHT);
    if (FAILED(hr)) return hr;

    hr = pIHT->Delete(ht, element);
    pIHT->Release();
    return hr;
}

 OS13_LIB_API HRESULT HTUpdate(
    HTHANDLE* ht,
    const Element* oldelement,
    const void* newpayload,
    int newpayloadlength)
{
    if (ht == nullptr || oldelement == nullptr) return E_INVALIDARG;

    IHT* pIHT = nullptr;
    HRESULT hr = GetIHTInterface(&pIHT);
    if (FAILED(hr)) return hr;

    hr = pIHT->Update(ht, oldelement, newpayload, newpayloadlength);
    pIHT->Release();
    return hr;
}

 OS13_LIB_API HRESULT HTGet(
    HTHANDLE* ht,
    const Element* element,
    Element** result)
{
    if (ht == nullptr || element == nullptr || result == nullptr) return E_INVALIDARG;

    IHT* pIHT = nullptr;
    HRESULT hr = GetIHTInterface(&pIHT);
    if (FAILED(hr)) return hr;

    hr = pIHT->Get(ht, element, result);
    pIHT->Release();
    return hr;
}

 OS13_LIB_API HRESULT HTSnap(
    HTHANDLE* ht)
{
    if (ht == nullptr) return E_INVALIDARG;

    IHT* pIHT = nullptr;
    HRESULT hr = GetIHTInterface(&pIHT);
    if (FAILED(hr)) return hr;

    hr = pIHT->Snap(ht);
    pIHT->Release();
    return hr;
}

 OS13_LIB_API HRESULT HTStop(
    HTHANDLE* ht)
{
    if (ht == nullptr) return E_INVALIDARG;

    IHT* pIHT = nullptr;
    HRESULT hr = GetIHTInterface(&pIHT);
    if (FAILED(hr)) return hr;

    hr = pIHT->Stop(ht);
    pIHT->Release();
    return hr;
}

 OS13_LIB_API HRESULT HTStart(
    HTHANDLE* ht)
{
    if (ht == nullptr) return E_INVALIDARG;

    IHT* pIHT = nullptr;
    HRESULT hr = GetIHTInterface(&pIHT);
    if (FAILED(hr)) return hr;

    hr = pIHT->Start(ht);
    pIHT->Release();
    return hr;
}

 OS13_LIB_API HRESULT HTPrintInfo(
    HTHANDLE* ht)
{
    if (ht == nullptr) return E_INVALIDARG;

    IHT* pIHT = nullptr;
    HRESULT hr = GetIHTInterface(&pIHT);
    if (FAILED(hr)) return hr;

    hr = pIHT->PrintInfo(ht);
    pIHT->Release();
    return hr;
}

 OS13_LIB_API HRESULT HTIsStopped(
    HTHANDLE* ht,
    BOOL* stopped)
{
    if (ht == nullptr || stopped == nullptr) return E_INVALIDARG;

    IHT* pIHT = nullptr;
    HRESULT hr = GetIHTInterface(&pIHT);
    if (FAILED(hr)) return hr;

    hr = pIHT->IsStopped(ht, stopped);
    pIHT->Release();
    return hr;
}

 OS13_LIB_API HRESULT InitializeCOM()
{
    LONG count = g_comRefCount.fetch_add(1);
    if (count > 0) {
        return S_OK; 
    }

    HRESULT hr = CoInitialize(nullptr);
    if (FAILED(hr)) {
        g_comRefCount.fetch_sub(1);
        std::cout << "CoInitialize failed: 0x" << std::hex << hr << std::endl;
    }
    return hr;
}

 OS13_LIB_API HRESULT UninitializeCOM()
{
    LONG count = g_comRefCount.fetch_sub(1);
    if (count > 1) {
        return S_OK; 
    }

    CoUninitialize();
    return S_OK;
}

 OS13_LIB_API void HTFreeElement(Element* element)
{
    if (element == nullptr) return;

        if (element->key != nullptr) {
        delete[] static_cast<const BYTE*>(element->key);
    }
    if (element->payload != nullptr) {
        delete[] static_cast<const BYTE*>(element->payload);
    }

    delete element;
}
