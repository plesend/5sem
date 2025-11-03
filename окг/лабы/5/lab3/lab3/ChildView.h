
// ChildView.h: интерфейс класса CChildView
//


#pragma once


// Окно CChildView

class CChildView : public CWnd
{
	// Создание
public:
	CChildView();

	// Атрибуты
public:
	int Index;				// Флаг готовности данных
	int dT_Timer;			// Интервал времени для таймера
	CRect RW;				// Область в окне
	CRectD RS;				// Область в мировых координатах
	CBlade Blade;	// Объект (конструктор по умолчанию)
	// Операции
public:

	// Переопределение
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

	// Реализация
public:
	virtual ~CChildView();

	// Созданные функции схемы сообщений
protected:
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()
public:
	// действия при выборе пункта меню
	afx_msg void OnLabpBlade();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
};
