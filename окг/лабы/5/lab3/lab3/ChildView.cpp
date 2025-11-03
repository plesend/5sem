
// ChildView.cpp: реализация класса CChildView
//

#include "stdafx.h"
#include "Lab03.h"
#include "ChildView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CChildView

CChildView::CChildView()
{
	Index = 0;
}

CChildView::~CChildView()
{
}

// Реализация карты сообщений
BEGIN_MESSAGE_MAP(CChildView, CWnd)
	ON_WM_PAINT()
	// сообщения меню выбора
	ON_COMMAND(ID_LAB_BLADE, &CChildView::OnLabpBlade)
	ON_WM_TIMER()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDBLCLK()
END_MESSAGE_MAP()



// Обработчики сообщений CChildView

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs)
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.style &= ~WS_THICKFRAME; // нельзя меня окно мышью.
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS,
		::LoadCursor(nullptr, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1), nullptr);

	return TRUE;
}

void CChildView::OnPaint()
{
	CPaintDC dc(this); // контекст устройства для рисования

	if (Index == 1)
	{
		Blade.GetRS(RS);
		RW = Blade.GetRW();
		SetMyMode(dc, RS, RW);			// Устанавливает режим отображения MM_ANISOTROPIC
		Blade.Draw(dc);
		dc.SetMapMode(MM_TEXT);			// Устанавливает режим отображения MM_TEXT

	}
}

void CChildView::OnLabpBlade()
{
	Blade.SetDT(0);				// Для начального состояния
	Blade.SetNewCoords();
	Blade.SetDT(0.5);
	dT_Timer = 30;					// Миллисекунд
	Index = 1;
	Invalidate();

}

void CChildView::OnTimer(UINT_PTR nIDEvent)
{
	Blade.SetNewCoords();
	Invalidate();
	CWnd::OnTimer(nIDEvent);

}


void CChildView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	SetTimer(1, dT_Timer, NULL);
	CWnd::OnLButtonDblClk(nFlags, point);
}

void CChildView::OnRButtonDblClk(UINT nFlags, CPoint point)
{
	KillTimer(1);
	CWnd::OnRButtonDblClk(nFlags, point);
}
