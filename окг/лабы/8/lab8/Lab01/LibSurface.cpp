#include "stdafx.h"

double Function1(double x, double y)
{
	return x * x + y * y;
};

double Function2(double x, double y)
{
	return x * x - y * y;
};

double Function3(double x, double y)
{
	if ((9 - x * x - y * y) >= 0)
		return sqrt(9 - x * x - y * y);
	else return 0;
};

CPlot3D::CPlot3D()///////////////
//конструктор класса 
{
	pFunc = NULL;
	ViewPoint.RedimMatrix(3);
	WinRect.SetRect(0, 0, 200, 200);
	ViewPoint(0) = 10, ViewPoint(1) = 30;
	ViewPoint(2) = 45;
};

void CPlot3D::SetFunction(pfunc2 pF, CRectD RS, double dx, double dy)//////////
//устанавливает функцию и создает матрицы для хранения координат точек графика.
{
	pFunc = pF;
	SpaceRect.SetRectD(RS.left, RS.top, RS.right, RS.bottom);
	MatrF.clear();		// очищаем матрицу для хранения координат точек поверхности в МСК
	MatrView.clear();	// очищаем матрицу для хранения к-т точек проекции поверхности на плоскость XY ВСК
	MatrWindow.clear();	// очищаем матрицу для хранения оконных координат точек изображения 

	CreateMatrF(dx, dy);// заполняем матрицу к-ми точек поверхности
	CreateMatrView();	// заполняем матрицу к-ми точек проекции поверхности на плоскость XY ВСК
	CreateMatrWindow(); // заполняем матрицу 
}

void CPlot3D::SetViewPoint(double r, double fi, double q)////////////
//метод для установки точки обзора трехмерного графика
{
	ViewPoint(0) = r, ViewPoint(1) = fi;
	ViewPoint(2) = q;
	MatrView.clear();
	CreateMatrView();
	MatrWindow.clear();
	CreateMatrWindow();
}

CMatrix CPlot3D::GetViewPoint()
//метод для получения текущих координат точки обзора.
{
	CMatrix P = ViewPoint;
	return P;
}

void CPlot3D::SetWinRect(CRect Rect)////////
//метод для установки прямоугольника окна графика.
{
	WinRect = Rect;
	MatrWindow.clear();
	CreateMatrWindow();
}
void CPlot3D::CreateMatrF(double dx, double dy)
{
	// Создание матрицы точек поверхности графика

	double xL = SpaceRect.left; // Нижняя граница по оси X
	double xH = SpaceRect.right; // Верхняя граница по оси X
	double yL = SpaceRect.bottom; // Нижняя граница по оси Y
	double yH = SpaceRect.top; // Верхняя граница по оси Y

	CVecMatrix VecMatrix; // Вектор матриц для хранения точек поверхности
	CMatrix V(4); // Вектор размерности 4 для хранения координат точки
	V(3) = 1; // Установка четвертой координаты точки в 1 (для матричных операций)

	for (double x = xL; x <= xH; x += dx)
	{
		VecMatrix.clear(); // Очистка вектора матриц перед заполнением новой строки точек

		for (double y = yL; y <= yH; y += dy)
		{
			V(0) = x; // Координата X точки
			V(1) = y; // Координата Y точки
			V(2) = pFunc(x, y); // Вычисление координаты Z точки с использованием переданной функции pFunc

			VecMatrix.push_back(V); // Добавление точки в вектор матрицы
		}

		MatrF.push_back(VecMatrix); // Добавление строки точек в матрицу
	}
}

void CPlot3D::CreateMatrView()
{
	// Создание матрицы точек проекции поверхности графика

	CMatrix MV = CreateViewCoord(ViewPoint(0), ViewPoint(1), ViewPoint(2)); // Матрица преобразования координат для точек проекции
	CVecMatrix VecMatrix; // Вектор матриц для хранения точек проекции
	CMatrix VX(4), V(3); // Векторы размерностей 4 и 3 для хранения координат точек
	V(2) = 1; // Установка третьей координаты точки в 1 (для матричных операций)

	double xmin = DBL_MAX; // Минимальное значение по оси X
	double xmax = DBL_MIN; // Максимальное значение по оси X
	double ymin = DBL_MAX; // Минимальное значение по оси Y
	double ymax = DBL_MIN; // Максимальное значение по оси Y

	for (int i = 0; i < MatrF.size(); i++) // перебирает по Х
	{
		VecMatrix.clear(); // Очистка вектора матриц перед заполнением новой строки точек проекции

		for (int j = 0; j < MatrF[i].size(); j++) // перебирает по у
		{
			VX = MatrF[i][j]; // Извлечение точки поверхности
			VX = MV * VX; // Преобразование координат точки с помощью матрицы преобразования
			V(0) = VX(0); // Координата X точки проекции
			V(1) = VX(1); // Координата Y точки проекции
			VecMatrix.push_back(V); // Добавление точки проекции в вектор матрицы

			double x = V(0); // Координата X точки проекции
			double y = V(1); // Координата Y точки проекции
			if (x < xmin)
				xmin = x; // Обновление минимального значения по оси X
			if (x > xmax)
				xmax = x; // Обновление максимального значения по оси X
			if (y < ymin)
				ymin = y; // Обновление минимального значения по оси Y
			if (y > ymax)
				ymax = y; // Обновление максимального значения по оси Y
		}

		MatrView.push_back(VecMatrix); // Добавление строки точек проекции в матрицу
	}

	ViewRect.SetRectD(xmin, ymax, xmax, ymin); // Установка прямоугольника представления графика
}
void CPlot3D::CreateMatrWindow()/////////////
// в оконные
{
	CMatrix MW = SpaceToWindow(ViewRect, WinRect);
	CVecPoint VecPoint;
	CMatrix  V(3);
	for (int i = 0; i < MatrView.size(); i++)
	{
		VecPoint.clear();
		for (int j = 0; j < MatrView[i].size(); j++)
		{
			V = MatrView[i][j];
			V = MW * V;
			CPoint P((int)V(0), (int)V(1));
			VecPoint.push_back(P);
		}
		MatrWindow.push_back(VecPoint);
	}
}

void CPlot3D::Draw(CDC& dc)//////
{
	if (MatrWindow.empty())
	{
		TCHAR* error = _T("Массив данных для рисования в окне пуст! ");
		MessageBox(NULL, error, _T("Ошибка"), MB_ICONSTOP);
		return;
	}
	
	CPoint pt[4];
	int k, d;
	CBrush brush(RGB(22, 224, 224));     
	CBrush* pOldBrush = dc.SelectObject(&brush);
	int nRows = MatrWindow.size();
	int nCols = MatrWindow[0].size();
	for (int i = 0; i < nRows - 1; i++)
		for (int j = 0; j < nCols - 1; j++)
		{
			pt[0] = MatrWindow[i][j];
			pt[1] = MatrWindow[i][j + 1];
			pt[2] = MatrWindow[i + 1][j + 1];
			pt[3] = MatrWindow[i + 1][j];
			dc.Polygon(pt, 4);
		}
	dc.SelectObject(pOldBrush);
}