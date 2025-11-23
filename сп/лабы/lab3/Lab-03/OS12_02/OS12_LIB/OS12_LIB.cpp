#include "OS12.h"
#include "../OS12_COM/OS12_COM.h"
#include <windows.h>
#include <stdexcept>

// Внутренняя структура для хранения COM интерфейсов
struct OS12Context
{
    IUnknown* pUnknown;
    IAdder* pAdder;
    IMultiplier* pMultiplier;
    bool comInitialized;

    OS12Context() : pUnknown(nullptr), pAdder(nullptr), pMultiplier(nullptr), comInitialized(false) {}
};

namespace OS12
{
    OS12HANDEL Init()
    {
        OS12Context* context = new OS12Context();

        try
        {
            // Инициализация COM
            HRESULT hr = CoInitialize(NULL);
            if (FAILED(hr))
            {
                throw static_cast<int>(hr);
            }
            context->comInitialized = true;

            // Создание COM объекта
            hr = CoCreateInstance(CLSID_OS12, nullptr, CLSCTX_INPROC_SERVER, IID_IUnknown, (void**)&context->pUnknown);
            if (FAILED(hr))
            {
                throw static_cast<int>(hr);
            }

            // Получение интерфейса IAdder
            hr = context->pUnknown->QueryInterface(IID_IAdder, (void**)&context->pAdder);
            if (FAILED(hr))
            {
                throw static_cast<int>(hr);
            }

            // Получение интерфейса IMultiplier
            hr = context->pUnknown->QueryInterface(IID_IMultiplier, (void**)&context->pMultiplier);
            if (FAILED(hr))
            {
                throw static_cast<int>(hr);
            }

            return static_cast<OS12HANDEL>(context);
        }
        catch (...)
        {
            // Очистка в случае ошибки
            if (context->pAdder != nullptr)
            {
                context->pAdder->Release();
            }
            if (context->pMultiplier != nullptr)
            {
                context->pMultiplier->Release();
            }
            if (context->pUnknown != nullptr)
            {
                context->pUnknown->Release();
            }
            if (context->comInitialized)
            {
                CoUninitialize();
            }
            delete context;
            throw;
        }
    }

    namespace Adder
    {
        double Add(OS12HANDEL h, double x, double y)
        {
            if (h == nullptr)
            {
                throw static_cast<int>(E_INVALIDARG);
            }

            OS12Context* context = static_cast<OS12Context*>(h);
            if (context->pAdder == nullptr)
            {
                throw static_cast<int>(E_NOINTERFACE);
            }

            double result = 0.0;
            HRESULT hr = context->pAdder->Add(x, y, result);
            if (FAILED(hr))
            {
                throw static_cast<int>(hr);
            }

            return result;
        }

        double Sub(OS12HANDEL h, double x, double y)
        {
            if (h == nullptr)
            {
                throw static_cast<int>(E_INVALIDARG);
            }

            OS12Context* context = static_cast<OS12Context*>(h);
            if (context->pAdder == nullptr)
            {
                throw static_cast<int>(E_NOINTERFACE);
            }

            double result = 0.0;
            HRESULT hr = context->pAdder->Sub(x, y, result);
            if (FAILED(hr))
            {
                throw static_cast<int>(hr);
            }

            return result;
        }
    }

    namespace Multiplier
    {
        double Mul(OS12HANDEL h, double x, double y)
        {
            if (h == nullptr)
            {
                throw static_cast<int>(E_INVALIDARG);
            }

            OS12Context* context = static_cast<OS12Context*>(h);
            if (context->pMultiplier == nullptr)
            {
                throw static_cast<int>(E_NOINTERFACE);
            }

            double result = 0.0;
            HRESULT hr = context->pMultiplier->Mul(x, y, result);
            if (FAILED(hr))
            {
                throw static_cast<int>(hr);
            }

            return result;
        }

        double Div(OS12HANDEL h, double x, double y)
        {
            if (h == nullptr)
            {
                throw static_cast<int>(E_INVALIDARG);
            }

            OS12Context* context = static_cast<OS12Context*>(h);
            if (context->pMultiplier == nullptr)
            {
                throw static_cast<int>(E_NOINTERFACE);
            }

            double result = 0.0;
            HRESULT hr = context->pMultiplier->Div(x, y, result);
            if (FAILED(hr))
            {
                throw static_cast<int>(hr);
            }

            return result;
        }
    }

    void Dispose(OS12HANDEL h)
    {
        if (h == nullptr) return;

        OS12Context* context = static_cast<OS12Context*>(h);

        // Освобождение COM интерфейсов в правильном порядке
        if (context->pAdder != nullptr)
        {
            context->pAdder->Release();
            context->pAdder = nullptr;
        }

        if (context->pMultiplier != nullptr)
        {
            context->pMultiplier->Release();
            context->pMultiplier = nullptr;
        }

        if (context->pUnknown != nullptr)
        {
            context->pUnknown->Release();
            context->pUnknown = nullptr;
        }

        // Деинициализация COM
        if (context->comInitialized)
        {
            CoUninitialize();
            context->comInitialized = false;
        }

        delete context;
    }
}