#include <windows.h>
#include <cmath>
#include <iostream>
#include <vector>
#include <afxwin.h>

using namespace std;

struct CRectD {
	double left, right, top, bottom;
	double Width() const { return right - left; }
	double Height() const { return top - bottom; }
};

struct CRect {
	int left, right, top, bottom;
	int Width() const { return right - left; }
	int Height() const { return bottom - top; }
};

class CMatrix {
public: 
	std::vector<std::vector<double>> m;

	void RedimMatrix(int rows, int cols) {
		m.resize(rows);
		for (int i = 0; i < rows; i++)
			m[i].resize(cols, 0.0);
	}

	double& operator()(int i, int j) { return m[i][j]; }

	double Min() {
		double min = m[0][0];

		for (auto& row : m) {
			for (double value : row) {
				if (value < min) min = value;
			}
		}
		return min;
	}

	double Max() {
		double max = m[0][0];

		for (auto& row : m) {
			for (double value : row) {
				if (value > max) max = value;
			}
		}
		return max;
	}

};

CMatrix SpaceToWindow(CRectD& rs, CRect& rw) {
	double scaleW = static_cast<double>(rw.Width()) / rs.Width();
	double scaleH = static_cast<double>(rw.Height()) / rs.Height();

	scaleH = -scaleH;//потому что у нас окошко с у вниз

	double tx = rw.left - rs.left * scaleW;
	double ty = rw.bottom - rs.top * scaleH;

	CMatrix m;
	m.RedimMatrix(3, 3);
	m(0, 0) = scaleW; m(0, 1) = 0;  m(0, 2) = tx;
	m(1, 0) = 0;      m(1, 1) = scaleH; m(1, 2) = ty;
	m(2, 0) = 0;      m(2, 1) = 0;       m(2, 2) = 1;
	return m;
}

struct CMyPen {
	int PenStyle;
	int PenWidth;
	COLORREF PenColor;
	CMyPen() { 
		PenStyle = PS_SOLID; 
		PenWidth = 1; 
		PenColor = RGB(0, 0, 0); 
	};
	void Set(int PS, int PW, COLORREF PC) {
		PenStyle = PS; PenWidth = PW; PenColor = PC;
	};
};

class CPlot2D {
	CMatrix X;				// Аргумент
	CMatrix Y;				// Функция
	CMatrix K;				// Матрица пересчета коорднат
	CRect RW;				// Прямоугольник в окне
	CRectD RS;				// Прямоугольник области в МСК
	CMyPen PenLine;                         // Перо для линий
	CMyPen PenAxis;                        // Перо для осей
public:
	CPlot2D() { K.RedimMatrix(3, 3); };          //Конструктор по умолчанию

	void SetParams(CMatrix& XX, CMatrix& YY, CRect& RWX)  // Установка параметров графика
	{
		X = XX; Y = YY; RW = RWX;

		RS.left = X.Min();
		RS.right = X.Max();
		RS.bottom = Y.Min();
		RS.top = Y.Max();

		K = SpaceToWindow(RS, RW);
	}
	void SetWindowRect(CRect& RWX) { RW = RWX; K = SpaceToWindow(RS, RW); };	//Установка области в окне для отображения графика

	void GetWindowCoords(double xs, double ys, int& xw, int& yw) { //Пересчет координаты точки из МСК в оконную СК
		double x_new = K(0, 0) * xs + K(0, 1) * ys + K(0, 2);
		double y_new = K(1, 0) * xs + K(1, 1) * ys + K(1, 2);
		xw = static_cast<int>(x_new);
		yw = static_cast<int>(y_new);
	}; 

	void SetPenLine(CMyPen& PLine) { PenLine = PLine; };	// Перо для рисования графика

	void SetPenAxis(CMyPen& PAxis) { PenAxis = PAxis; };	// Перо для осей координат

	void Draw(CDC& dc, int Ind1, int Ind2)	// Рисование с самостоятельным пересчетом координат
	{
		CPen penLine(PenLine.PenStyle, PenLine.PenWidth, PenLine.PenColor);
		CPen penAxis(PenAxis.PenStyle, PenAxis.PenWidth, PenAxis.PenColor);

		CPen* oldPen = dc.SelectObject(&penAxis);

		int x0, y0, x1, y1;

		// Ось X
		GetWindowCoords(RS.left, 0, x0, y0);
		GetWindowCoords(RS.right, 0, x1, y1);
		dc.MoveTo(x0, y0);
		dc.LineTo(x1, y1);

		// Ось Y
		GetWindowCoords(0, RS.bottom, x0, y0);
		GetWindowCoords(0, RS.top, x1, y1);
		dc.MoveTo(x0, y0);
		dc.LineTo(x1, y1);

		// График
		dc.SelectObject(&penLine);
		int xw, yw;
		GetWindowCoords(X(Ind1, 0), Y(Ind1, 0), xw, yw);
		dc.MoveTo(xw, yw);
		for (int i = Ind1 + 1; i <= Ind2; i++) {
			GetWindowCoords(X(i, 0), Y(i, 0), xw, yw);
			dc.LineTo(xw, yw);
		}

		dc.SelectObject(oldPen);
	}

};