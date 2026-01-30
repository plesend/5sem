#pragma once
#include "os13_htcom.h"
#include <Windows.h>

#ifdef OS13_LIB_EXPORTS
#define OS13_LIB_API
#else
#define OS13_LIB_API 
#endif
extern IHT* g_pHT;


 OS13_LIB_API HRESULT HTCreate(
    int Capacity,
    int SecSnapshotInterval,
    int MaxKeyLength,
    int MaxPayloadLength,
    const char* FileName,
    HTHANDLE** result
);

 OS13_LIB_API HRESULT HTOpen(
    const char* FileName,
    HTHANDLE** result
);

 OS13_LIB_API HRESULT HTClose(
    HTHANDLE* ht
);

 OS13_LIB_API HRESULT HTInsert(
    HTHANDLE* ht,
    const Element* element
);

 OS13_LIB_API HRESULT HTDelete(
    HTHANDLE* ht,
    const Element* element
);

 OS13_LIB_API HRESULT HTUpdate(
    HTHANDLE* ht,
    const Element* oldelement,
    const void* newpayload,
    int newpayloadlength
);

 OS13_LIB_API HRESULT HTGet(
    HTHANDLE* ht,
    const Element* element,
    Element** result
);
 OS13_LIB_API HRESULT HTSnap(
    HTHANDLE* ht
);

 OS13_LIB_API HRESULT HTStop(
    HTHANDLE* ht
);

 OS13_LIB_API HRESULT HTStart(
    HTHANDLE* ht
);

 OS13_LIB_API HRESULT HTPrintInfo(
    HTHANDLE* ht
);

 OS13_LIB_API HRESULT HTIsStopped(
    HTHANDLE* ht,
    BOOL* stopped
);

 OS13_LIB_API HRESULT InitializeCOM();
 OS13_LIB_API HRESULT UninitializeCOM();

 OS13_LIB_API void HTFreeElement(Element* element);
 OS13_LIB_API HRESULT GetIHTInterface(IHT** ppIHT);
