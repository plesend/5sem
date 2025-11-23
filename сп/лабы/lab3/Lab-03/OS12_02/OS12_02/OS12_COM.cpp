#include "OS12_COM.h"
#include <iostream>

#define SEQLOG(x) std::cout << x << std::endl

class OS12 : public IAdder, public IMultiplier
{
private:
    ULONG m_refCount;

public:
    OS12() : m_refCount(0)
    {
        SEQLOG("OS12::OS12()");
    }

    ~OS12()
    {
        SEQLOG("OS12::~OS12()");
    }

    // IUnknown methods
    HRESULT __stdcall QueryInterface(const IID& iid, void** ppv) override
    {
        SEQLOG("OS12::QueryInterface");

        if (iid == IID_IUnknown)
        {
            *ppv = static_cast<IAdder*>(this);
        }
        else if (iid == IID_IAdder)
        {
            *ppv = static_cast<IAdder*>(this);
        }
        else if (iid == IID_IMultiplier)
        {
            *ppv = static_cast<IMultiplier*>(this);
        }
        else
        {
            *ppv = NULL;
            return E_NOINTERFACE;
        }

        reinterpret_cast<IUnknown*>(*ppv)->AddRef();
        return S_OK;
    }

    ULONG __stdcall AddRef() override
    {
        SEQLOG("OS12::AddRef");
        return InterlockedIncrement(&m_refCount);
    }

    ULONG __stdcall Release() override
    {
        SEQLOG("OS12::Release");
        ULONG refCount = InterlockedDecrement(&m_refCount);
        if (refCount == 0)
        {
            delete this;
        }
        return refCount;
    }

    // IAdder methods
    HRESULT __stdcall Add(double x, double y, double& result) override
    {
        SEQLOG("OS12::Add");
        result = x + y;
        return S_OK;
    }

    HRESULT __stdcall Sub(double x, double y, double& result) override
    {
        SEQLOG("OS12::Sub");
        result = x - y;
        return S_OK;
    }

    // IMultiplier methods
    HRESULT __stdcall Mul(double x, double y, double& result) override
    {
        SEQLOG("OS12::Mul");
        result = x * y;
        return S_OK;
    }

    HRESULT __stdcall Div(double x, double y, double& result) override
    {
        SEQLOG("OS12::Div");
        if (y == 0.0)
        {
            return E_INVALIDARG;
        }
        result = x / y;
        return S_OK;
    }
};

class OS12Factory : public IClassFactory
{
private:
    ULONG m_refCount;

public:
    OS12Factory() : m_refCount(0)
    {
        SEQLOG("OS12Factory::OS12Factory()");
    }

    ~OS12Factory()
    {
        SEQLOG("OS12Factory::~OS12Factory()");
    }

    // IUnknown methods
    HRESULT __stdcall QueryInterface(const IID& iid, void** ppv) override
    {
        SEQLOG("OS12Factory::QueryInterface");

        if (iid == IID_IUnknown || iid == IID_IClassFactory)
        {
            *ppv = static_cast<IClassFactory*>(this);
        }
        else
        {
            *ppv = NULL;
            return E_NOINTERFACE;
        }

        reinterpret_cast<IUnknown*>(*ppv)->AddRef();
        return S_OK;
    }

    ULONG __stdcall AddRef() override
    {
        SEQLOG("OS12Factory::AddRef");
        return InterlockedIncrement(&m_refCount);
    }

    ULONG __stdcall Release() override
    {
        SEQLOG("OS12Factory::Release");
        ULONG refCount = InterlockedDecrement(&m_refCount);
        if (refCount == 0)
        {
            delete this;
        }
        return refCount;
    }

    // IClassFactory methods
    HRESULT __stdcall CreateInstance(IUnknown* pUnknownOuter, const IID& iid, void** ppv) override
    {
        SEQLOG("OS12Factory::CreateInstance");

        if (pUnknownOuter != NULL)
        {
            return CLASS_E_NOAGGREGATION;
        }

        OS12* pOS12 = new OS12();
        if (pOS12 == NULL)
        {
            return E_OUTOFMEMORY;
        }

        HRESULT hr = pOS12->QueryInterface(iid, ppv);
        pOS12->Release();

        return hr;
    }

    HRESULT __stdcall LockServer(BOOL bLock) override
    {
        SEQLOG("OS12Factory::LockServer");
        return S_OK;
    }
};

// DLL Exports
STDAPI DllGetClassObject(const CLSID& clsid, const IID& iid, void** ppv)
{
    SEQLOG("DllGetClassObject");

    if (clsid != CLSID_OS12)
    {
        return CLASS_E_CLASSNOTAVAILABLE;
    }

    OS12Factory* pFactory = new OS12Factory();
    if (pFactory == NULL)
    {
        return E_OUTOFMEMORY;
    }

    HRESULT hr = pFactory->QueryInterface(iid, ppv);
    pFactory->Release();

    return hr;
}

STDAPI DllCanUnloadNow()
{
    SEQLOG("DllCanUnloadNow");
    return S_OK;
}