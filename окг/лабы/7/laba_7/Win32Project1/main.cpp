#include "CMatrix.h"
#include "CPyramid.h"
#include <afxwin.h>
#include <windows.h>
#include <fstream>
#include <string>
#include <iostream>

#define ID_Dr1 2002
#define ID_Dr 2003
#define ID_32773 32773
#define ID_32774 32774

class CMainWin : public CFrameWnd
{
public:
	CMainWin()
	{
		Create(NULL, L"laba_7");
		Index = 0;
		PView.RedimMatrix(3);
		menu = nullptr;
	}
	CMenu* menu;
	CRect WinRect;
	CPyramid PIR;
	CMatrix PView;
	int Index;
	afx_msg int OnCreate(LPCREATESTRUCT);
	afx_msg void OnPaint();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	void Dr1();
	void Dr();
	void CurrentCameraPosition();
	void SetDefaultCameraPosition();
	DECLARE_MESSAGE_MAP();
};
class CMyApp : public CWinApp
{
public:
	CMyApp() {};
	virtual BOOL InitInstance()
	{
		m_pMainWnd = new CMainWin();
		m_pMainWnd->ShowWindow(SW_SHOW);
		return TRUE;
	}
};
BEGIN_MESSAGE_MAP(CMainWin, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_COMMAND(ID_Dr1, Dr1)
	ON_COMMAND(ID_Dr, Dr)
	ON_COMMAND(ID_32773, CurrentCameraPosition)
	ON_COMMAND(ID_32774, SetDefaultCameraPosition)
	ON_WM_KEYDOWN()
	ON_WM_SIZE()
END_MESSAGE_MAP()

afx_msg void CMainWin::OnPaint()
{
	CPaintDC dc(this);
	if (Index == 1) PIR.Draw(dc, PView, WinRect);
	if (Index == 2) PIR.Draw1(dc, PView, WinRect);

}

afx_msg int CMainWin::OnCreate(LPCREATESTRUCT)
{
	menu = new CMenu();
	menu->CreateMenu();
	this->menu->AppendMenu(MF_STRING, ID_Dr, _T("Visible border"));
	this->menu->AppendMenu(MF_STRING, ID_Dr1, _T("All border"));
	this->menu->AppendMenu(MF_STRING, ID_32773, _T("File position cords"));
	this->menu->AppendMenu(MF_STRING, ID_32774, _T("Defoult position cords"));
	SetMenu(menu);
	return 0;
}

void CMainWin::Dr1()
{
	PView(0) = 10;
	PView(1) = 315;
	PView(2) = 45;
	Index = 1;
	Invalidate();
}

void CMainWin::Dr()
{

	PView(0) = 10;
	PView(1) = 315;
	PView(2) = 45;
	Index = 2;
	Invalidate();
}
afx_msg void CMainWin::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	double step = 10;
	if (Index == 1 || Index == 2)
	{
		switch (nChar)
		{
		case VK_UP:
		{
			double d = PView(2) - step;
			if (d >= 0)
			{
				PView(2) = d;
			}
			break;
		}
		case VK_DOWN:
		{
			double d = PView(2) + step;
			if (d <= 180)
			{
				PView(2) = d;
			}
			break;
		}
		case VK_LEFT:
		{
			double d = PView(1) - step;
			if (d >= 0)
			{
				PView(1) = d;
			}
			else
			{
				PView(1) = 360 + d;
			}
			break;
		}
		case VK_RIGHT:
		{
			double d = PView(1) + step;
			if (d <= 360)
			{
				PView(1) = d;
			}
			else
			{
				PView(1) = d - 360;
			}
			break;
		}
		}
		Invalidate();
	}
}

afx_msg void CMainWin::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);
	WinRect.SetRect(cx * 0.5, -cy * 0.3, cx * 0.9, cy * 0.1);
}
void CMainWin::CurrentCameraPosition()
{
	std::string line;

	int r = 0;
	int fi = 0;
	int teta = 0;

	std::ifstream in(".//coords.txt");

	if (in.is_open())
	{
		std::getline(in, line);
		r = stoi(line);

		std::getline(in, line);
		fi = stoi(line);

		std::getline(in, line);
		teta = stoi(line);
	}
	in.close();

	PView(0) = r;
	PView(1) = fi;
	PView(2) = teta;

	Invalidate();
}


void CMainWin::SetDefaultCameraPosition()
{
	if (Index == 1)
	{
		CMainWin::Dr1();
		return;
	}
	CMainWin::Dr();
}

CMyApp theApp;