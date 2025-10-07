
// ChildView.cpp: реализация класса CChildView
//

#include "stdafx.h"
#include "lab3.h"
#include "ChildView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CChildView

CChildView::CChildView()
{
}

CChildView::~CChildView()
{
}


BEGIN_MESSAGE_MAP(CChildView, CWnd)
	ON_WM_PAINT()
	ON_COMMAND(ID_TESTS_F1, &CChildView::OnTestsF1)
	ON_COMMAND(ID_TESTS_F2, &CChildView::OnTestsF2)
	ON_COMMAND(ID_TESTS_F12, &CChildView::OnTestsF3)
END_MESSAGE_MAP()



// Обработчики сообщений CChildView

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs)
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS,
		::LoadCursor(nullptr, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1), nullptr);

	return TRUE;
}

void CChildView::OnPaint()
{
	CPaintDC dc(this); // контекст устройства для рисования
	if (Index == 1)
	{
		Graph.Draw(dc, 0, 1);
	}

	if (Index == 2)
	{
		Graph.GetRS(RS);
		SetMyMode(dc, CRectD(1, 10, 5, 10), CRect(1, 1, 2, 2));	    // Устанавливает режим отображения MM_ANISOTROPIC 
		Graph.Draw1(dc, 1, 1);
		dc.SetMapMode(MM_TEXT);										// Устанавливает режим отображения MM_TEXT
	}
	if (Index == 3)
	{
		CPen penFigure(PS_SOLID, 3, RGB(255, 0, 0));
		dc.SelectObject(&penFigure);

		CPen penCircle(PS_SOLID, 2, RGB(0, 0, 255));

		CRect rect;
		GetClientRect(&rect);
		const int squareSize = min(rect.Width(), rect.Height());

		const double radius = squareSize / 2.0;
		const double angle = 45.0;
		const double centerX = (rect.left + rect.right) / 2.0;
		const double centerY = (rect.top + rect.bottom) / 2.0;
		const double circleRadius = radius;

		const double circleLeft = centerX - circleRadius;
		const double circleTop = centerY - circleRadius;
		const double circleRight = centerX + circleRadius;
		const double circleBottom = centerY + circleRadius;

		dc.SelectObject(&penCircle);
		dc.Ellipse(circleLeft, circleTop, circleRight, circleBottom);

		const int sides = 8;
		const double angleRadians = angle * 3.14159 / 180.0;
		const double startX = centerX + radius;
		const double startY = centerY;
		double currentX = startX;
		double currentY = startY;

		for (int i = 1; i < sides; i++)
		{
			double nextX = centerX + radius * cos(angleRadians * i);
			double nextY = centerY - radius * sin(angleRadians * i);

			// Установка стиля линии для каждой стороны восьмиугольника
			if (i % 2 == 0)
				dc.SelectObject(&penFigure);
			else
				dc.SelectObject(&penFigure);

			dc.MoveTo((int)currentX, (int)currentY);
			dc.LineTo((int)nextX, (int)nextY);

			currentX = nextX;
			currentY = nextY;
		}

		// Отображение последней стороны восьмиугольника
		dc.SelectObject(&penFigure);
		dc.MoveTo((int)currentX, (int)currentY);
		dc.LineTo((int)startX, (int)startY);
	}
}

double CChildView::MyF1(double x)
{
	double y = sin(x) / x;
	return y;
}

double CChildView::MyF2(double x)
{
	double y = sqrt(abs(x)) * sin(x);
	return y;
}

void CChildView::OnTestsF1()	// MM_TEXT
{
	Invalidate();
	CPaintDC dc(this);
	double xL = -3 * pi;
	double xH = -xL;
	double dx = pi / 36;		//шаг
	int N = (xH - xL) / dx;
	X.RedimMatrix(N + 1);	//изменяет размер матрицы 
	Y.RedimMatrix(N + 1);
	for (int i = 0; i <= N; i++)//заполняем матрицы координатами точек для отрисовки
	{
		X(i) = xL + i * dx;
		Y(i) = MyF1(X(i));
	}
	PenLine.Set(PS_SOLID, 1, RGB(255, 0, 0));
	PenAxis.Set(PS_SOLID, 2, RGB(0, 0, 255));
	RW.SetRect(800, 300, 1400, 900);
	Graph.SetParams(X, Y, RW);
	Graph.SetPenLine(PenLine);
	Graph.SetPenAxis(PenAxis);
	Index = 1;
	Graph.Draw(dc, 1, 1);
}


void CChildView::OnTestsF2()
{
	Invalidate();
	CPaintDC dc(this);
	double xH = 4 * pi;
	double xL = -xH;
	double dx = pi / 36;
	int N = (xH - xL) / dx;
	X.RedimMatrix(N + 1);
	Y.RedimMatrix(N + 1);
	for (int i = 0; i <= N; i++)
	{
		X(i) = xL + i * dx;
		Y(i) = MyF2(X(i));
	}
	PenLine.Set(PS_DASH, 3, RGB(255, 0, 0));
	PenAxis.Set(PS_SOLID, 2, RGB(0, 0, 0));
	RW.SetRect(800, 300, 1400, 900);
	Graph.SetParams(X, Y, RW);
	Graph.SetPenLine(PenLine);
	Graph.SetPenAxis(PenAxis);
	Index = 2;
	Graph.Draw(dc, 1, 1);
}



void CChildView::OnTestsF3()
{
	Index = 3;

	Invalidate();
	
}