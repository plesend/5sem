
// lab3.h: основной файл заголовка для приложения lab3
//
#pragma once

#ifndef __AFXWIN_H__
	#error "включить pch.h до включения этого файла в PCH"
#endif

#include "resource.h"       // основные символы


// Clab3App:
// Сведения о реализации этого класса: lab3.cpp
//

class Clab3App : public CWinApp
{
public:
	Clab3App() noexcept;


// Переопределение
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Реализация

public:
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern Clab3App theApp;
