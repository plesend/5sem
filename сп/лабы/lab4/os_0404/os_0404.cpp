#include <iostream>
#include <windows.h>
#include <csignal>
#include "os13_lib.h"

using namespace std;

bool active = true;

void sigintHandler(int sig)
{
    cout << "exiting... " << endl;
    active = false;
}

int main(int argc, char** argv)
{
    char* filepath;
    if (argc != 2)
    {
        cout << "usage: os0402.exe {filename}" << endl;
        return 1;
    }
    filepath = argv[1];
    srand(time(0));
    signal(SIGINT, sigintHandler);

    HRESULT hr = InitializeCOM();
    if (FAILED(hr)) {
        cerr << "Failed to initialize COM. HRESULT: 0x" << hex << hr << endl;
        return 1;
    }

    IHT* pIHT = nullptr;
    hr = GetIHTInterface(&pIHT);
    if (FAILED(hr) || pIHT == nullptr) {
        cerr << "Failed to get IHT interface. HRESULT: 0x" << hex << hr << endl;
        UninitializeCOM();
        return 1;
    }

    HANDLE m = OpenMutexA(SYNCHRONIZE, FALSE, "ht_mutex");
    if (!m)
    {
        cerr << "os11Open is not running" << endl;
        pIHT->Release();
        UninitializeCOM();
        system("pause");
        return 1;
    }

    DWORD mutexCatch = WaitForSingleObject(m, 3000);
    if (mutexCatch == WAIT_OBJECT_0)
    {
        cerr << "no mutex to wait" << endl;
        CloseHandle(m);
        pIHT->Release();
        UninitializeCOM();
        system("pause");
        return 1;
    }
    ReleaseMutex(m);

    int i = 0;

    HTHANDLE* ht = nullptr;
    hr = pIHT->Open(filepath, &ht);
    if (FAILED(hr) || ht == nullptr)
    {
        cerr << "Failed to open HT storage via COM. HRESULT: 0x" << hex << hr << endl;
        CloseHandle(m);
        pIHT->Release();
        UninitializeCOM();
        return 1;
    }
    int maxKeys = ht->Capacity;
    cout << maxKeys << endl;
    cout << "Successfully opened HT storage. Starting insert operations..." << endl;

    while (active)
    {
        string key = "key" + to_string((rand() + maxKeys) % maxKeys);

        Element* e = new Element();
        e->key = key.c_str();
        e->keylength = (int)key.length();
        e->payload = &i;
        e->payloadlength = sizeof(int);

        if (SUCCEEDED(pIHT->Delete(ht, e)))
        {
            cout << "deleted " << i << " element with key: " << key << endl;
        }
        else
        {
            cerr << "failed to delete element " << i << "with key: " << key << endl;
        }

        delete e;
        i++;
        Sleep(1000);
    }

    hr = pIHT->Close(ht);
    if (FAILED(hr)) {
        cerr << "Warning: Failed to close HT storage properly. HRESULT: 0x" << hex << hr << endl;
    }

    CloseHandle(m);
    pIHT->Release();

    hr = UninitializeCOM();
    if (FAILED(hr)) {
        cerr << "Warning: Failed to uninitialize COM properly. HRESULT: 0x" << hex << hr << endl;
    }

    cout << "Application finished successfully" << endl;
    return 0;
}