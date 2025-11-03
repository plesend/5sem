#ifndef LIBPLANETS
#define LIBPLANETS 1
const double pi = 3.14159;


struct CSizeD
{
	double cx;
	double cy;
};
//-------------------------------------------------------------------------------
struct CRectD
{
	double left;
	double top;
	double right;
	double bottom;
	CRectD() { left = top = right = bottom = 0; };
	CRectD(double l, double t, double r, double b);
	void SetRectD(double l, double t, double r, double b);
	CSizeD SizeD();		// ���������� �������(������, ������) �������������� 
};
//-------------------------------------------------------------------------------

CMatrix CreateTranslate2D(double dx, double dy);
CMatrix CreateRotate2D(double fi);
CMatrix SpaceToWindow(CRectD& rs, CRect& rw);
void SetMyMode(CDC& dc, CRectD& RS, CRect& RW);



class CBlade
{
	CRect MainPoint;
	CRect FirstTop;
	CRect SecondTop;
	CRect FirstBootom;
	CRect SecondBootom;
	CRect EarthOrbit;
	CMatrix FTCoords;
	CMatrix STCoords;
	CMatrix FBCoords;
	CMatrix SBCoords;
	CRect RW;		   // ������������� � ����
	CRectD RS;		   // ������������� ������� � ���
	double wPoint = 10;		//������� ��������
	//���� ��� 4 ����� (������ ��������)
	double fiSB;
	double fiFB;
	double fiST;
	double fiFT;

	double dt;		   // �������� �������������, ���.
public:
	CBlade();
	//void DrawTriangle(CMatrix FTCoords, CMatrix STCoords, CDC& dc);
	void SetDT(double dtx) { dt = dtx; };	// ��������� ��������� �������������
	void SetNewCoords();					// ��������� ����� ���������� ������
	void GetRS(CRectD& RSX);				// ���������� ������� ������� � ������� ��
	CRect GetRW() { return RW; };			// ���������� ������� ������� � ����	
	void Draw(CDC& dc);						// ��������� ��� ���������������� ��������� ���������
	void DrawBladePair(CDC& dc, CMatrix& coords1, CMatrix& coords2, COLORREF color, CMatrix& transformMatrix);
	void DrawTriangleWithBorder(CMatrix FCoords, CMatrix SCoords, CDC& dc, COLORREF color, CMatrix& transformMatrix);
	void DrawBlade(CDC& dc, CMatrix& coords1, CMatrix& coords2, COLORREF color);

private:
	CBrush RedBrush;
	CBrush BlueBrush;
	CBrush SunBrush;
	CMatrix BlueCoords1;  // ������ �����
	CMatrix BlueCoords2;  // ����� �����
	CMatrix BlueCoords3;  // ������� �����
	CMatrix BlueCoords4;  // ������ �����
	double fiLeft = 180;    // ����� (�����)
	double fiRight = 0;     // ������ (�����)
	double fiBlue1 = 0;     // ������
	double fiBlue2 = 180;   // �����
	double fiBlue3 = 90;    // ������� (��������������)
	double fiBlue4 = 270;   // ������ (��������������)
	CRect LeftBlade;        // ����� �������
	CRect RightBlade;       // ������ �������
	CMatrix LBCoords;       // ���������� ����� �������
	CMatrix RBCoords;       // ���������� ������ �������
};


#endif
