#include "CPyramid.h"
#include <iostream>
#define pi 3.14159265;

CPyramid::CPyramid()
{
	this->Vertices.RedimMatrix(4, 6);	// создание матрицы с координатами точек пирамиды

	this->Vertices(0, 0) = 6;		
	this->Vertices(0, 3) = 2;		
	this->Vertices(2, 3) = 6;		
	this->Vertices(1, 1) = -6;		
	this->Vertices(1, 4) = -2;		
	this->Vertices(2, 4) = 6;		
	this->Vertices(2, 5) = 6;		

	for (int i = 0; i < 6; i++)
		this->Vertices(3, i) = 1;
}

CMatrix CreateViewCoord(double r, double fi, double q)			// МСК -> ВСК
{
	// сферические координаты точки наблюдения
	double fi_sphere = (fi / 180) * pi;		// азимут
	double q_sphere = (q / 180) * pi;		// зенит

	CMatrix K(4, 4);
	K(0, 0) = -sin(fi_sphere);
	K(0, 1) = cos(fi_sphere);
	K(1, 0) = -cos(q_sphere) * cos(fi_sphere);
	K(1, 1) = -cos(q_sphere) * sin(fi_sphere);
	K(1, 2) = sin(q_sphere);
	K(2, 0) = -sin(q_sphere) * cos(fi_sphere);
	K(2, 1) = -sin(q_sphere) * sin(fi_sphere);
	K(2, 2) = -cos(q_sphere);
	K(2, 3) = r;
	K(3, 3) = 1;
	return K;
}

void CPyramid::Draw1(CDC& dc, CMatrix& viewport, CRect& rectWindow)	// нарисовать пирамиду без невидимых граней
{
	// Переводим сферические координаты камеры в декартовы 
	CMatrix viewportCoords = sphericalToCartesian(viewport);

	// Находим матрицу перевода из МСК в видовую СК
	CMatrix converterWorldToView = CreateViewCoord(viewport(0), viewport(1), viewport(2));

	char buf[50] = "";
	sprintf(buf, "%.*f", 0, viewport(1));
	dc.TextOut(10, 10, buf);
	sprintf(buf, "%.*f", 0, viewport(2));
	dc.TextOut(10, 30, buf);

	// Находим координаты вертикалей в ВСК
	CMatrix verticesView = converterWorldToView * this->Vertices;

	CRectD rectView;
	GetRect(this->Vertices, rectView);

	// Находим матрицу перевода из ВСК в оконную СК
	CMatrix converterWorldToWindow = SpaceToWindow(rectView, rectWindow);

	CPoint vertices[6];
	CMatrix verticesCoords(3);
	verticesCoords(2) = 1;

	for (int i = 0; i < 6; i++)		// задание точек
	{
		verticesCoords(0) = verticesView(0, i);
		verticesCoords(1) = verticesView(1, i);

		verticesCoords = converterWorldToWindow * verticesCoords;
		vertices[i].x = (int)verticesCoords(0);
		vertices[i].y = (int)verticesCoords(1);
	}

	CPen brushEdge(PS_SOLID, 2, RGB(0, 0, 255));
	CPen* pOldPen = dc.SelectObject(&brushEdge);

	// Кисть для нижней грани
	CBrush brushBottom(RGB(255, 0, 0));
	CBrush* pOldBrush = dc.SelectObject(&brushBottom);

	CMatrix R1(3), R2(3), normalOuter(3);
	double sm;

	for (int i = 0; i < 3; i++) // закрашивание граней
	{
		// Получаем вертикаль грани
		CMatrix VE = this->Vertices.GetCol(i + 3, 0, 2);

		int k = i == 2 ? 0 : i + 1;

		// Получаем точки ребра у основания
		R1 = this->Vertices.GetCol(i, 0, 2);
		R2 = this->Vertices.GetCol(k, 0, 2);

		// Получаем длину ребра у основания
		CMatrix edgeBase = R2 - R1;

		// Получаем длину ребра вертикали
		CMatrix edgeVertex = VE - R1;

		// Находим внешний вектор нормали
		normalOuter = vectorProduct(edgeVertex, edgeBase);

		// Находим скалярное произведение
		sm = scalarProduct(normalOuter, viewportCoords);

		if (sm >= 0)
		{
			dc.MoveTo(vertices[i]);
			dc.LineTo(vertices[k]);
			dc.LineTo(vertices[k + 3]);
			dc.LineTo(vertices[i + 3]);
			dc.LineTo(vertices[i]);
		}
	}
	if (viewportCoords(2) < 0)
	{
		dc.Polygon(vertices, 3);
	}
	else
	{
		CBrush brushTop(RGB(0, 255, 0));	// задаем в перо зеленого цвета для основания
		dc.SelectObject(brushTop);
		dc.Polygon(vertices + 3, 3);
	}
	dc.SelectObject(pOldPen);
	dc.SelectObject(pOldBrush);
}

void CPyramid::Draw(CDC& dc, CMatrix& viewport, CRect& rectWindow)
{
	// Находим матрицу перевода из МСК в видовую СК
	CMatrix converterWorldToView = CreateViewCoord(viewport(0), viewport(1), viewport(2));

	char buf[50] = "";
	sprintf(buf, "%.*f", 0, viewport(1));
	dc.TextOut(10, 10, buf);
	sprintf(buf, "%.*f", 0, viewport(2));
	dc.TextOut(10, 30, buf);

	// Находим координаты вертикалей в ВСК
	CMatrix verticesView = converterWorldToView * this->Vertices;

	CRectD rectView;
	GetRect(this->Vertices, rectView);

	// Находим матрицу перевода из ВСК в оконную СК
	CMatrix converterWorldToWindow = SpaceToWindow(rectView, rectWindow);

	CPoint vertices[6];
	CMatrix verticesCoords(3);
	verticesCoords(2) = 1;

	for (int i = 0; i < 6; i++)		// задание точек
	{
		verticesCoords(0) = verticesView(0, i);
		verticesCoords(1) = verticesView(1, i);

		verticesCoords = converterWorldToWindow * verticesCoords;
		vertices[i].x = (int)verticesCoords(0);
		vertices[i].y = (int)verticesCoords(1);
	}

	CPen brushEdge(PS_SOLID, 2, RGB(0, 0, 255)); //задаем в перо синего цвета для краев(линий)
	CPen* pOldPen = dc.SelectObject(&brushEdge);

	dc.MoveTo(vertices[2]); // рисуем грани
	for (int i = 0; i < 3; i++)
	{
		dc.LineTo(vertices[i]);
	}
	dc.MoveTo(vertices[5]);
	for (int i = 3; i < 6; i++)
	{
		dc.LineTo(vertices[i]);
	}
	for (int i = 0; i < 3; i++)
	{
		dc.MoveTo(vertices[i]);
		dc.LineTo(vertices[i + 3]);
	}

	dc.SelectObject(pOldPen);
}

void  CPyramid::GetRect(CMatrix& Vert, CRectD&RectView)
{
	RectView.top = Vert.GetRow(2).MinElement();
	RectView.bottom = Vert.GetRow(2).MaxElement();
	RectView.left = Vert.GetRow(0).MinElement();
	RectView.right = Vert.GetRow(0).MaxElement();
}

CMatrix VectorMult(CMatrix& V1, CMatrix& V2)
{
	if (V1.rows() != V2.rows() || V1.cols() > 1 || V2.cols() > 1) // Число столбцов больше одного 
	{
		char* error = "CMatrix VectorMult(CMatrix& V1, CMatrix& V2) объект не вектор - число столбцов больше 1 ";
		MessageBoxA(NULL, error, "Ошибка", MB_ICONSTOP);
		exit(1);
	}

	CMatrix Temp = V1;
	Temp(0) = V1(1)*V2(2) - V1(2)*V2(1);
	Temp(1) = V1(2)*V2(0) - V1(0)*V2(2);
	Temp(2) = V1(0)*V2(1) - V1(1)*V2(0);

	return Temp;
}

double ScalarMult(CMatrix& V1, CMatrix& V2)
{
	if (V1.rows() != V2.rows() || V1.cols() > 1 || V2.cols() > 1) // Число столбцов больше одного 
	{
		char* error = "double ScalarMult(CMatrix& V1, CMatrix& V2) объект не вектор - число столбцов больше 1 ";
		MessageBoxA(NULL, error, "Ошибка", MB_ICONSTOP);
		exit(1);
	}

	return V1(0)*V2(0) + V1(1)*V2(1) + V1(2)*V2(2);
}