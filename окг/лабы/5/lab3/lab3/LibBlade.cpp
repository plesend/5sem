#include "stdafx.h"

CRectD::CRectD(double l, double t, double r, double b)
{
    left = l;
    top = t;
    right = r;
    bottom = b;
}

//------------------------------------------------------------------------------
void CRectD::SetRectD(double l, double t, double r, double b)
{
    left = l;
    top = t;
    right = r;
    bottom = b;
}

//------------------------------------------------------------------------------
CSizeD CRectD::SizeD()
{
    CSizeD cz;
    cz.cx = fabs(right - left); // Вычисление ширины прямоугольника
    cz.cy = fabs(top - bottom);  // Вычисление высоты прямоугольника
    return cz;
}

//------------------------------------------------------------------------------
CMatrix CreateTranslate2D(double dx, double dy)
// Создание матрицы для трансляции (перемещения) на dx по оси X и dy по оси Y
{
    CMatrix TM(3, 3);
    TM(0, 0) = 1; TM(0, 2) = dx;
    TM(1, 1) = 1; TM(1, 2) = dy;
    TM(2, 2) = 1;
    return TM;
}


//------------------------------------------------------------------------------
CMatrix CreateRotate2D(double fi)
// Создание матрицы для вращения на угол fi (градусы)
// Положительное fi означает вращение по часовой стрелке
{
    double fg = fmod(fi, 360.0);
    double ff = (fg / 180.0) * pi; // Преобразование в радианы
    CMatrix RM(3, 3);
    RM(0, 0) = cos(ff); RM(0, 1) = -sin(ff);
    RM(1, 0) = sin(ff); RM(1, 1) = cos(ff);
    RM(2, 2) = 1;
    return RM;
}

//------------------------------------------------------------------------------
CMatrix SpaceToWindow(CRectD& RS, CRect& RW)
// Преобразование пространства координат из реального в оконное
// RS - прямоугольник в реальных координатах (double)
// RW - прямоугольник в оконных координатах (int)
{
    CMatrix M(3, 3);
    CSize sz = RW.Size();    // Размер окна
    int dwx = sz.cx;         // Ширина
    int dwy = sz.cy;         // Высота
    CSizeD szd = RS.SizeD(); // Размер реального прямоугольника

    double dsx = szd.cx;     // Ширина реального прямоугольника
    double dsy = szd.cy;     // Высота реального прямоугольника

    double kx = (double)dwx / dsx;   // Коэффициент по x
    double ky = (double)dwy / dsy;   // Коэффициент по y

    M(0, 0) = kx;  M(0, 1) = 0;    M(0, 2) = (double)RW.left - kx * RS.left;
    M(1, 0) = 0;   M(1, 1) = -ky;  M(1, 2) = (double)RW.bottom + ky * RS.bottom;
    M(2, 0) = 0;   M(2, 1) = 0;    M(2, 2) = 1;
    return M;
}

//------------------------------------------------------------------------------
void SetMyMode(CDC& dc, CRectD& RS, CRect& RW)
// Установка режима отображения с использованием MM_ANISOTROPIC
// dc - контекст устройства CDC MFC
// RS - прямоугольник в реальных координатах (int)
// RW - прямоугольник в оконных координатах (int)
{
    double dsx = RS.right - RS.left;
    double dsy = RS.top - RS.bottom;
    double xsL = RS.left;
    double ysL = RS.bottom;

    int dwx = RW.right - RW.left;
    int dwy = RW.bottom - RW.top;
    int xwL = RW.left;
    int ywH = RW.bottom;

    dc.SetMapMode(MM_ANISOTROPIC);
    dc.SetWindowExt((int)dsx, (int)dsy);
    dc.SetViewportExt(dwx, -dwy);
    dc.SetWindowOrg((int)xsL, (int)ysL);
    dc.SetViewportOrg(xwL, ywH);
}

CBlade::CBlade()
{
    double rS = 30; // Радиус солнечного диска
    double RoE = 10 * rS; // Радиус орбиты
    RS.SetRectD(-RoE, RoE, RoE, -RoE);
    RW.SetRect(0, 0, 600, 600);
    MainPoint.SetRect(-rS, rS, rS, -rS);

    BlueCoords1.RedimMatrix(3);
    BlueCoords2.RedimMatrix(3);
    BlueCoords3.RedimMatrix(3);
    BlueCoords4.RedimMatrix(3);

    RedBrush.CreateSolidBrush(RGB(255, 0, 0));
    BlueBrush.CreateSolidBrush(RGB(0, 0, 255));
    SunBrush.CreateSolidBrush(RGB(0, 128, 0));

    // Размер лопасти
    double bladeSize = 5;
    FirstTop.SetRect(-bladeSize, bladeSize, bladeSize, -bladeSize);
    SecondTop.SetRect(-bladeSize, bladeSize, bladeSize, -bladeSize);
    FirstBootom.SetRect(-bladeSize, bladeSize, bladeSize, -bladeSize);
    SecondBootom.SetRect(-bladeSize, bladeSize, bladeSize, -bladeSize);
    LeftBlade.SetRect(-bladeSize, bladeSize, bladeSize, -bladeSize);
    RightBlade.SetRect(-bladeSize, bladeSize, bladeSize, -bladeSize);

    EarthOrbit.SetRect(-RoE, RoE, RoE, -RoE);

    // Углы вращения
    fiFT = 35;   // Угол первой лопасти (вперед)
    fiST = 25;   // Угол второй лопасти (вперед)
    fiFB = 205;  // Угол первой лопасти (назад)
    fiSB = 215;  // Угол второй лопасти (назад)
    fiLeft = 180; // Угол левой лопасти
    fiRight = 0;  // Угол правой лопасти

    fiBlue1 = 5;    // Угол первой синей лопасти
    fiBlue2 = -5;   // Угол второй синей лопасти
    fiBlue3 = 175;  // Угол третьей синей лопасти
    fiBlue4 = 185;  // Угол четвертой синей лопасти

    wPoint = -10; // Скорость вращения
    dt = 1;

    // Инициализация координат
    FTCoords.RedimMatrix(3);
    STCoords.RedimMatrix(3);
    FBCoords.RedimMatrix(3);
    SBCoords.RedimMatrix(3);
    LBCoords.RedimMatrix(3);
    RBCoords.RedimMatrix(3);
}

void CBlade::SetNewCoords() // обновление положений лопастей во времени
{
    double RoV = (EarthOrbit.right - EarthOrbit.left) / 2;//Берёт радиус орбиты.
    double ff = (fiFT / 90.0) * pi;
    //Для каждой лопасти считает текущие координаты по формуле:
    double x = RoV * cos(ff);
    double y = RoV * sin(ff);
    FTCoords(0) = x;
    FTCoords(1) = y;
    FTCoords(2) = 1;
    //Увеличивает угол fi на wPoint * dt — имитируя вращение.
    fiFT += wPoint * dt;
    //Применяет матрицу вращения CreateRotate2D(fi).
    CMatrix P = CreateRotate2D(fiFT);
    FTCoords = P * FTCoords;

    RoV = (EarthOrbit.right - EarthOrbit.left) / 2;
    ff = (fiST / 90.0) * pi;
    x = RoV * cos(ff);
    y = RoV * sin(ff);
    STCoords(0) = x;
    STCoords(1) = y;
    STCoords(2) = 1;
    fiST += wPoint * dt;
    P = CreateRotate2D(fiST);
    STCoords = P * STCoords;

    RoV = (EarthOrbit.right - EarthOrbit.left) / 2;
    ff = (fiFB / 90.0) * pi;
    x = RoV * cos(ff);
    y = RoV * sin(ff);
    FBCoords(0) = x;
    FBCoords(1) = y;
    FBCoords(2) = 1;
    fiFB += wPoint * dt;
    P = CreateRotate2D(fiFB);
    FBCoords = P * FBCoords;

    RoV = (EarthOrbit.right - EarthOrbit.left) / 2;
    ff = (fiSB / 90.0) * pi;
    x = RoV * cos(ff);
    y = RoV * sin(ff);
    SBCoords(0) = x;
    SBCoords(1) = y;
    SBCoords(2) = 1;
    fiSB += wPoint * dt;
    P = CreateRotate2D(fiSB);
    SBCoords = P * SBCoords;

    // Обновление координат синих лопастей
    ff = (fiBlue1 / 90.0) * pi;
    BlueCoords1(0) = RoV * cos(ff);
    BlueCoords1(1) = RoV * sin(ff);
    BlueCoords1(2) = 1;
    fiBlue1 += wPoint * dt;
    P = CreateRotate2D(fiBlue1);
    BlueCoords1 = P * BlueCoords1;

    ff = (fiBlue2 / 90.0) * pi;
    BlueCoords2(0) = RoV * cos(ff);
    BlueCoords2(1) = RoV * sin(ff);
    BlueCoords2(2) = 1;
    fiBlue2 += wPoint * dt;
    P = CreateRotate2D(fiBlue2);
    BlueCoords2 = P * BlueCoords2;

    ff = (fiBlue3 / 90.0) * pi;
    BlueCoords3(0) = RoV * cos(ff);
    BlueCoords3(1) = RoV * sin(ff);
    BlueCoords3(2) = 1;
    fiBlue3 += wPoint * dt;
    P = CreateRotate2D(fiBlue3);
    BlueCoords3 = P * BlueCoords3;

    ff = (fiBlue4 / 90.0) * pi;
    BlueCoords4(0) = RoV * cos(ff);
    BlueCoords4(1) = RoV * sin(ff);
    BlueCoords4(2) = 1;
    fiBlue4 += wPoint * dt;
    P = CreateRotate2D(fiBlue4);
    BlueCoords4 = P * BlueCoords4;
}

void CBlade::Draw(CDC& dc) //рисует всю сцену
{
    // Применение преобразования к координатам
    CMatrix transformMatrix = SpaceToWindow(RS, RW);

    // Установка пера для рисования
    CPen blackPen(PS_SOLID, 1, RGB(0, 0, 0));
    CPen* pOldPen = dc.SelectObject(&blackPen);

    // Рисование треугольников
    DrawTriangleWithBorder(FTCoords, STCoords, dc, RGB(255, 0, 0), transformMatrix);
    DrawTriangleWithBorder(FBCoords, SBCoords, dc, RGB(255, 0, 0), transformMatrix);
    DrawTriangleWithBorder(BlueCoords1, BlueCoords2, dc, RGB(0, 0, 255), transformMatrix);
    DrawTriangleWithBorder(BlueCoords3, BlueCoords4, dc, RGB(0, 0, 255), transformMatrix);

    // Рисование солнечного диска
    CBrush* pOldBrush = dc.SelectObject(&SunBrush);
    dc.Ellipse(MainPoint);
    dc.SelectObject(pOldBrush);

    // Восстановление пера
    dc.SelectObject(pOldPen);
}

void CBlade::DrawTriangleWithBorder(CMatrix FCoords, CMatrix SCoords, CDC& dc, COLORREF fillColor, CMatrix& transformMatrix)
{//Отрисовывает треугольник (лопасть):
    // Преобразование координат треугольника
    CMatrix p1 = transformMatrix * FCoords;
    CMatrix p2 = transformMatrix * SCoords;

    // Установка кисти и пера
    CBrush brush(fillColor);
    CPen blackPen(PS_SOLID, 1, RGB(0, 0, 0));
    CPen* pOldPen = dc.SelectObject(&blackPen);
    CBrush* pOldBrush = dc.SelectObject(&brush);

    // Рисование треугольника
    POINT points[3] = {
        {0, 0},
        {static_cast<int>(FCoords(0)), static_cast<int>(FCoords(1))},
        {static_cast<int>(SCoords(0)), static_cast<int>(SCoords(1))}
    };
    dc.Polygon(points, 3);

    // Восстановление кисти и пера
    dc.SelectObject(pOldBrush);
    dc.SelectObject(pOldPen);
}

void CBlade::DrawBladePair(CDC& dc, CMatrix& coords1, CMatrix& coords2, COLORREF color, CMatrix& transformMatrix)
{//Это вспомогательные функции, чтобы нарисовать по две лопасти:
    CRect R;
    int d = 5; // Размер маркера

    // Установка кисти и пера
    CBrush brush(color);
    CPen pen(PS_SOLID, 1, color);
    CPen* pOldPen = dc.SelectObject(&pen);
    CBrush* pOldBrush = dc.SelectObject(&brush);

    // Рисование маркеров
    R.SetRect(
        static_cast<int>(coords1(0) - d),
        static_cast<int>(coords1(1) + d),
        static_cast<int>(coords1(0) + d),
        static_cast<int>(coords1(1) - d)
    );
    dc.Ellipse(R);

    R.SetRect(
        static_cast<int>(coords2(0) - d),
        static_cast<int>(coords2(1) + d),
        static_cast<int>(coords2(0) + d),
        static_cast<int>(coords2(1) - d)
    );
    dc.Ellipse(R);

    // Рисование линий к маркерам
    dc.MoveTo(0, 0);
    dc.LineTo(static_cast<int>(coords1(0)), static_cast<int>(coords1(1)));
    dc.MoveTo(0, 0);
    dc.LineTo(static_cast<int>(coords2(0)), static_cast<int>(coords2(1)));

    // Рисование треугольника
    DrawTriangleWithBorder(coords1, coords2, dc, color, transformMatrix);

    // Восстановление кисти и пера
    dc.SelectObject(pOldBrush);
    dc.SelectObject(pOldPen);
}

void CBlade::DrawBlade(CDC& dc, CMatrix& coords1, CMatrix& coords2, COLORREF color) //Это вспомогательные функции, чтобы нарисовать по две лопасти :
{
    // Установка кисти и пера
    CBrush brush(color);
    CPen pen(PS_SOLID, 1, color);
    CPen* pOldPen = dc.SelectObject(&pen);
    CBrush* pOldBrush = dc.SelectObject(&brush);

    // Рисование лопастей
    int bladeSize = 5;
    CRect rect1(
        static_cast<int>(coords1(0) - bladeSize),
        static_cast<int>(coords1(1) + bladeSize),
        static_cast<int>(coords1(0) + bladeSize),
        static_cast<int>(coords1(1) - bladeSize)
    );
    dc.Ellipse(rect1);

    CRect rect2(
        static_cast<int>(coords2(0) - bladeSize),
        static_cast<int>(coords2(1) + bladeSize),
        static_cast<int>(coords2(0) + bladeSize),
        static_cast<int>(coords2(1) - bladeSize)
    );
    dc.Ellipse(rect2);

    // Рисование линий к лопастям
    dc.MoveTo(0, 0);
    dc.LineTo(static_cast<int>(coords1(0)), static_cast<int>(coords1(1)));
    dc.MoveTo(0, 0);
    dc.LineTo(static_cast<int>(coords2(0)), static_cast<int>(coords2(1)));

    // Рисование треугольника
    POINT triangle[3] = {
        {0, 0},
        {static_cast<int>(coords1(0)), static_cast<int>(coords1(1))},
        {static_cast<int>(coords2(0)), static_cast<int>(coords2(1))}
    };
    dc.Polygon(triangle, 3);

    // Восстановление кисти и пера
    dc.SelectObject(pOldBrush);
    dc.SelectObject(pOldPen);
}

void CBlade::GetRS(CRectD& RSX) //Возвращает прямоугольник реальных координат — просто копирует RS в RSX.
// RS - Прямоугольник, который будет заполнен текущими координатами
{
    RSX.left = RS.left;
    RSX.top = RS.top;
    RSX.right = RS.right;
    RSX.bottom = RS.bottom;
}