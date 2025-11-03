
// lab3.h: основной файл заголовка для приложения lab3
//
#pragma once

#ifndef __AFXWIN_H__
#error "включить stdafx.h до включения этого файла в PCH"
#endif

#include "resource.h"       // основные символы


// CLab01App:
// Сведения о реализации этого класса: Lab01.cpp
//

class CLab01App : public CWinApp
{
public:
	CLab01App() noexcept;

	// Переопределение
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	// Реализация

public:
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CLab01App theApp;
