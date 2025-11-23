#include "OS12_COM.h"
#include <windows.h>

class OS12 : public IAdder, public IMultiplier
{
private:
    LONG m_refCount;

public:
    OS12() : m_refCount(1) {} //объект живёт, пока счётчик > 0 

    ~OS12() {}

    // IUnknown methods 
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject) override
    {
        if (ppvObject == nullptr) return E_POINTER;

        if (riid == IID_IUnknown) {
            *ppvObject = static_cast<IAdder*>(this);
        }
        else if (riid == IID_IAdder) {
            *ppvObject = static_cast<IAdder*>(this);
        }
        else if (riid == IID_IMultiplier) {
            *ppvObject = static_cast<IMultiplier*>(this);
        }
        else {
            *ppvObject = nullptr;
            return E_NOINTERFACE;
        }

        AddRef();//нужно увелисить счетчик при выдаче интерфейса
        return S_OK;
    }

    ULONG STDMETHODCALLTYPE AddRef() override
    {
        return InterlockedIncrement(&m_refCount);
    }

    ULONG STDMETHODCALLTYPE Release() override
    {
        ULONG refCount = InterlockedDecrement(&m_refCount);
        if (refCount == 0) {
            delete this;
        }
        return refCount;
    }

    // IAdder methods
    HRESULT STDMETHODCALLTYPE Add(double x, double y, double& result) override
    {
        result = x + y;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE Sub(double x, double y, double& result) override
    {
        result = x - y;
        return S_OK;
    }

    // IMultiplier methods
    HRESULT STDMETHODCALLTYPE Mul(double x, double y, double& result) override
    {
        result = x * y;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE Div(double x, double y, double& result) override
    {
        if (y == 0.0) return E_INVALIDARG;
        result = x / y;
        return S_OK;
    }
};

class OS12Factory : public IClassFactory//фабрика объектов
{
private:
    LONG m_refCount;

public:
    OS12Factory() : m_refCount(1) {}  // Начинаем с 1!

    ~OS12Factory() {}

    // IUnknown methods
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject) override
    {
        if (ppvObject == nullptr) return E_POINTER;

        if (riid == IID_IUnknown || riid == IID_IClassFactory) {
            *ppvObject = static_cast<IClassFactory*>(this);
        }
        else {
            *ppvObject = nullptr;
            return E_NOINTERFACE;
        }

        AddRef();
        return S_OK;
    }

    ULONG STDMETHODCALLTYPE AddRef() override
    {
        return InterlockedIncrement(&m_refCount);
    }

    ULONG STDMETHODCALLTYPE Release() override
    {
        ULONG refCount = InterlockedDecrement(&m_refCount);
        if (refCount == 0) {
            delete this;
        }
        return refCount;
    }

    // IClassFactory methods
    HRESULT STDMETHODCALLTYPE CreateInstance(IUnknown* pUnkOuter, REFIID riid, void** ppvObject) override
    {
        if (ppvObject == nullptr) return E_POINTER;
        *ppvObject = nullptr;

        if (pUnkOuter != nullptr) return CLASS_E_NOAGGREGATION;

        OS12* pOS12 = new (std::nothrow) OS12();
        if (pOS12 == nullptr) return E_OUTOFMEMORY;

        HRESULT hr = pOS12->QueryInterface(riid, ppvObject);
        pOS12->Release();
        return hr;
    }

    HRESULT STDMETHODCALLTYPE LockServer(BOOL fLock) override
    {
        return S_OK;
    }
};

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, void** ppv)
{//дай объект-фабрику вот с таким вот сидом (clsid)
    if (ppv == nullptr) return E_POINTER;
    *ppv = nullptr;

    if (!IsEqualCLSID(rclsid, CLSID_OS12)) return CLASS_E_CLASSNOTAVAILABLE;

    OS12Factory* pFactory = new (std::nothrow) OS12Factory();
    if (pFactory == nullptr) return E_OUTOFMEMORY;

    HRESULT hr = pFactory->QueryInterface(riid, ppv);
    pFactory->Release();
    return hr;
}

STDAPI DllCanUnloadNow()
{
    return S_FALSE;//нельзя выгружать
}

