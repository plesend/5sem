// OS12_02.cpp - в проекте OS12_02
#include <iostream>
#include <windows.h>

#include "OS12_COM.h"

#define IERR(s)    std::cout << "error " << s << std::endl
#define IRES(s,r)  std::cout << s << r << std::endl

IAdder* pIAdder = nullptr;
IMultiplier* pMultiplier = nullptr;

int main()
{
    std::cout << "=== OS12_02 COM Client Started ===" << std::endl;

    IUnknown* pIUnknown = NULL;

    HRESULT hrInit = CoInitialize(NULL);
    if (FAILED(hrInit))
    {
        std::cout << "COM initialization failed: " << hrInit << std::endl;
        return -1;
    }
    std::cout << "COM initialized successfully" << std::endl;

    HRESULT hr0 = CoCreateInstance(CLSID_OS12, NULL, CLSCTX_INPROC_SERVER, IID_IUnknown, (void**)&pIUnknown);
    if (SUCCEEDED(hr0))
    {
        std::cout << "CoCreateInstance succeeded" << std::endl;

        std::cout << "\n--- Testing IAdder interface ---" << std::endl;
        if (SUCCEEDED(pIUnknown->QueryInterface(IID_IAdder, (void**)&pIAdder)))
        {
            {
                double z = 0.0;
                if (!SUCCEEDED(pIAdder->Add(2.0, 3.0, z)))
                    IERR("IAdder::Add");
                else
                    IRES("IAdder::Add(2, 3) = ", z);
            }

            {
                double z = 0.0;
                if (!SUCCEEDED(pIAdder->Sub(2.0, 3.0, z)))
                    IERR("IAdder::Sub");
                else
                    IRES("IAdder::Sub(2, 3) = ", z);
            }

            std::cout << "\n--- Testing IMultiplier interface ---" << std::endl;
            if (SUCCEEDED(pIAdder->QueryInterface(IID_IMultiplier, (void**)&pMultiplier)))
            {
                {
                    double z = 0.0;
                    if (!SUCCEEDED(pMultiplier->Mul(2.0, 3.0, z)))
                        IERR("IMultiplier::Mul");
                    else
                        IRES("Multiplier::Mul(2, 3) = ", z);
                }

                {
                    double z = 0.0;
                    if (!SUCCEEDED(pMultiplier->Div(2.0, 3.0, z)))
                        IERR("IMultiplier::Div");
                    else
                        IRES("IMultiplier::Div(2, 3) = ", z);
                }

                std::cout << "\n--- Testing return to IAdder ---" << std::endl;
                if (SUCCEEDED(pMultiplier->QueryInterface(IID_IAdder, (void**)&pIAdder)))
                {
                    double z = 0.0;
                    if (!SUCCEEDED(pIAdder->Add(5.0, 7.0, z)))
                        IERR("IAdder::Add");
                    else
                        IRES("IAdder::Add(5, 7) = ", z);
                    pIAdder->Release();
                }
                else
                {
                    IERR("IMultiplier->IAdder");
                }

                pMultiplier->Release();
            }
            else
            {
                IERR("IAdder->IMultiplier");
            }

            pIAdder->Release();
        }
        else
        {
            IERR("IAdder - QueryInterface failed");
        }
    }
    else
    {
        std::cout << "CoCreateInstance error: " << hr0 << std::endl;
    }

    if (pIUnknown != NULL)
    {
        pIUnknown->Release();
    }

    CoFreeUnusedLibraries();
    CoUninitialize();

    std::cout << "\n=== OS12_02 COM Client Finished ===" << std::endl;
    return 0;
}
