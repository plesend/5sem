// Registry.cpp
#include "OS12_COM.h"
#include <windows.h>
#include <objbase.h>
#include <assert.h>

// Вспомогательные функции для работы с реестром
BOOL setKeyAndValue(const WCHAR* szKey, const WCHAR* szSubkey, const WCHAR* szValue);
void CLSIDtochar(const CLSID& clsid, WCHAR* szCLSID, int length);
LONG recursiveDeleteKey(HKEY hKeyParent, const WCHAR* szKeyChild);

const int CLSID_STRING_SIZE = 39;

// Регистрация сервера
HRESULT RegisterServer(HMODULE hModule, const CLSID& clsid,
    const WCHAR* szFriendlyName, const WCHAR* szVerIndProgID,
    const WCHAR* szProgID)
{
    WCHAR szModule[512];
    DWORD dwResult = GetModuleFileName(hModule, szModule, sizeof(szModule) / sizeof(WCHAR));
    assert(dwResult != 0);

    // Конвертируем CLSID в строку
    WCHAR szCLSID[CLSID_STRING_SIZE];
    CLSIDtochar(clsid, szCLSID, sizeof(szCLSID));

    // Создаем ключ CLSID
    WCHAR szKey[264];
    wcscpy_s(szKey, L"CLSID\\");
    wcscat_s(szKey, szCLSID);

    // Записываем информацию в реестр
    setKeyAndValue(szKey, NULL, szFriendlyName);
    setKeyAndValue(szKey, L"InprocServer32", szModule);
    setKeyAndValue(szKey, L"ProgID", szProgID);
    setKeyAndValue(szKey, L"VersionIndependentProgID", szVerIndProgID);
    setKeyAndValue(szVerIndProgID, NULL, szFriendlyName);
    setKeyAndValue(szVerIndProgID, L"CLSID", szCLSID);
    setKeyAndValue(szVerIndProgID, L"CurVer", szProgID);
    setKeyAndValue(szProgID, NULL, szFriendlyName);
    setKeyAndValue(szProgID, L"CLSID", szCLSID);

    return S_OK;
}

// Удаление сервера из реестра
HRESULT UnregisterServer(const CLSID& clsid, const WCHAR* szVerIndProgID, const WCHAR* szProgID)
{
    WCHAR szCLSID[CLSID_STRING_SIZE];
    CLSIDtochar(clsid, szCLSID, sizeof(szCLSID));

    WCHAR szKey[264];
    wcscpy_s(szKey, L"CLSID\\");
    wcscat_s(szKey, szCLSID);

    // Рекурсивно удаляем ключи
    LONG lResult = recursiveDeleteKey(HKEY_CLASSES_ROOT, szKey);
    assert((lResult == ERROR_SUCCESS) || (lResult == ERROR_FILE_NOT_FOUND));
    lResult = recursiveDeleteKey(HKEY_CLASSES_ROOT, szVerIndProgID);
    assert((lResult == ERROR_SUCCESS) || (lResult == ERROR_FILE_NOT_FOUND));
    lResult = recursiveDeleteKey(HKEY_CLASSES_ROOT, szProgID);
    assert((lResult == ERROR_SUCCESS) || (lResult == ERROR_FILE_NOT_FOUND));

    return S_OK;
}

// Конвертация CLSID в строку
void CLSIDtochar(const CLSID& clsid, WCHAR* szCLSID, int length)
{
    assert(length >= CLSID_STRING_SIZE);
    LPOLESTR wszCLSID = NULL;
    HRESULT hr = StringFromCLSID(clsid, &wszCLSID);
    assert(SUCCEEDED(hr));
    wcscpy_s(szCLSID, CLSID_STRING_SIZE, wszCLSID);
    CoTaskMemFree(wszCLSID);
}

// Рекурсивное удаление ключа реестра
LONG recursiveDeleteKey(HKEY hKeyParent, const WCHAR* lpszKeyChild)
{
    HKEY hKeyChild;
    LONG lRes = RegOpenKeyEx(hKeyParent, lpszKeyChild, 0, KEY_ALL_ACCESS, &hKeyChild);
    if (lRes != ERROR_SUCCESS)
    {
        return lRes;
    }

    // Удаляем все подключа
    FILETIME time;
    WCHAR szBuffer[256];
    DWORD dwSize = 256;
    while (RegEnumKeyEx(hKeyChild, 0, szBuffer, &dwSize, NULL, NULL, NULL, &time) == S_OK)
    {
        lRes = recursiveDeleteKey(hKeyChild, szBuffer);
        if (lRes != ERROR_SUCCESS)
        {
            RegCloseKey(hKeyChild);
            return lRes;
        }
        dwSize = 256;
    }

    RegCloseKey(hKeyChild);
    return RegDeleteKey(hKeyParent, lpszKeyChild);
}

// Установка ключа и значения
BOOL setKeyAndValue(const WCHAR* szKey, const WCHAR* szSubkey, const WCHAR* szValue)
{
    HKEY hKey;
    WCHAR szKeyBuf[1024];

    wcscpy_s(szKeyBuf, szKey);

    if (szSubkey != NULL)
    {
        wcscat_s(szKeyBuf, L"\\");
        wcscat_s(szKeyBuf, szSubkey);
    }

    // Создаем/открываем ключ
    long lResult = RegCreateKeyEx(HKEY_CLASSES_ROOT, szKeyBuf, 0, NULL,
        REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS,
        NULL, &hKey, NULL);
    if (lResult != ERROR_SUCCESS)
    {
        return FALSE;
    }

    // Устанавливаем значение
    if (szValue != NULL)
    {
        RegSetValueEx(hKey, NULL, 0, REG_SZ, (BYTE*)szValue,
            2 * wcslen(szValue) + 1);
    }

    RegCloseKey(hKey);
    return TRUE;
}

// Точки входа для регистрации/удаления
STDAPI DllRegisterServer()
{
    SEQLOG("DllRegisterServer called");
    return RegisterServer(
        GetModuleHandle(L"OS12_COM.dll"),
        CLSID_OS12,
        L"OS12 COM Component",
        L"OS12.Component",
        L"OS12.Component.1");
}

STDAPI DllUnregisterServer()
{
    SEQLOG("DllUnregisterServer called");
    return UnregisterServer(
        CLSID_OS12,
        L"OS12.Component",
        L"OS12.Component.1");
}