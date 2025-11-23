#pragma once

#include <windows.h>
#include <unknwn.h>
#include <iostream>

// Макрос для логирования
#ifdef _DEBUG
#define SEQLOG(x) std::cout << x << std::endl
#else
#define SEQLOG(x) 
#endif
// {E1E53163-4E6E-4DCD-9F7D-3F2F01A0D6C1}
static const CLSID CLSID_OS12 =
{ 0xe1e53163, 0x4e6e, 0x4dcd, { 0x9f, 0x7d, 0x3f, 0x2f, 0x1, 0xa0, 0xd6, 0xc1 } };

// {A1B2C3D4-E5F6-4A5B-8C9D-0E1F2A3B4C5D}
static const IID IID_IAdder =
{ 0xa1b2c3d4, 0xe5f6, 0x4a5b, { 0x8c, 0x9d, 0xe, 0x1f, 0x2a, 0x3b, 0x4c, 0x5d } };

// {D4C3B2A1-F6E5-5B4A-9D8C-5D4C3B2A1F0E}
static const IID IID_IMultiplier =
{ 0xd4c3b2a1, 0xf6e5, 0x5b4a, { 0x9d, 0x8c, 0x5d, 0x4c, 0x3b, 0x2a, 0x1f, 0xe } };

class IAdder : public IUnknown
{
public:
    virtual HRESULT __stdcall Add(double x, double y, double& result) = 0;
    virtual HRESULT __stdcall Sub(double x, double y, double& result) = 0;
};

class IMultiplier : public IUnknown
{
public:
    virtual HRESULT __stdcall Mul(double x, double y, double& result) = 0;
    virtual HRESULT __stdcall Div(double x, double y, double& result) = 0;
};