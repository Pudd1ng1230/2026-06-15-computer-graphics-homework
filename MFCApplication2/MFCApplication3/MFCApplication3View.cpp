
// MFCApplication3View.cpp: CMFCApplication3View 类的实现
//

#include "pch.h"
#include "framework.h"
#ifndef SHARED_HANDLERS
#include "MFCApplication3.h"
#endif

#include "MFCApplication3Doc.h"
#include "MFCApplication3View.h"
#include <cmath>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNCREATE(CMFCApplication3View, CView)

BEGIN_MESSAGE_MAP(CMFCApplication3View, CView)
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)

	// 实验2：算法菜单
	ON_COMMAND(ID_SUANFA_DDA, &CMFCApplication3View::OnDDA)
	ON_COMMAND(ID_SUANFA_ZHONGDIAN, &CMFCApplication3View::OnMidpoint)
	ON_COMMAND(ID_SUANFA_BRESENHAM, &CMFCApplication3View::OnBresenham)

	// 实验2：颜色设置
	ON_COMMAND(ID_LINE_COLOR, &CMFCApplication3View::OnLineColor)
END_MESSAGE_MAP()


CMFCApplication3View::CMFCApplication3View() noexcept
{
	lineColor = RGB(255, 0, 0);   // 默认红色
	m_algoType = 0;               // 初始不绘制
}

CMFCApplication3View::~CMFCApplication3View()
{
}

BOOL CMFCApplication3View::PreCreateWindow(CREATESTRUCT& cs)
{
	return CView::PreCreateWindow(cs);
}


// ============================================================
// OnDraw — 根据所选算法绘制直线（全部使用 SetPixel 逐像素绘制）
// ============================================================

void CMFCApplication3View::OnDraw(CDC* pDC)
{
	CMFCApplication3Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	CRect rect;
	GetClientRect(&rect);
	pDC->FillSolidRect(rect, RGB(255, 255, 255));  // 白色背景

	// 演示用线段: (100,100) → (600,400)，斜率 k≈0.6，在 [0,1] 区间
	int x1 = 100, y1 = 100;
	int x2 = 600, y2 = 400;

	// 在顶部显示当前算法名称（红色大字）
	CString algoName;
	switch (m_algoType)
	{
	case 1: algoName = _T("当前算法：DDA（数值微分法）"); break;
	case 2: algoName = _T("当前算法：中点画线算法"); break;
	case 3: algoName = _T("当前算法：Bresenham 算法"); break;
	default: algoName = _T("请点击菜单「算法」选择一种算法"); break;
	}
	pDC->SetTextColor(RGB(200, 0, 0));
	pDC->SetBkMode(TRANSPARENT);
	pDC->TextOutW(20, 10, algoName);

	// 标注线段端点坐标
	CString info;
	info.Format(_T("线段: (%d, %d) → (%d, %d)   斜率 k = %.2f"),
		x1, y1, x2, y2, float(y2 - y1) / float(x2 - x1));
	pDC->SetTextColor(RGB(0, 0, 180));
	pDC->TextOutW(20, 35, info);

	if (m_algoType == 0)
		return;

	// 在端点处画小圆标记
	CBrush brush(RGB(0, 0, 255));
	CBrush* pOldBrush = pDC->SelectObject(&brush);
	pDC->Ellipse(x1 - 4, y1 - 4, x1 + 4, y1 + 4);
	pDC->Ellipse(x2 - 4, y2 - 4, x2 + 4, y2 + 4);
	pDC->SelectObject(pOldBrush);

	switch (m_algoType)
	{
	case 1:
		DrawDDA(pDC, x1, y1, x2, y2);
		break;
	case 2:
		DrawMidpoint(pDC, x1, y1, x2, y2);
		break;
	case 3:
		DrawBresenham(pDC, x1, y1, x2, y2);
		break;
	}
}


// ============================================================
// DDA 算法（数值微分法）
// 原理：y = kx + b，增量思想。|k|≤1 时 x 每步±1、y 每步±k
//       |k|>1 时 y 每步±1、x 每步±1/k
// 每组迭代对坐标四舍五入后用 SetPixel 绘制
// ============================================================

void CMFCApplication3View::DrawDDA(CDC* pDC, int x1, int y1, int x2, int y2)
{
	float dx = float(x2 - x1);
	float dy = float(y2 - y1);

	// 取 dx 和 dy 中绝对值较大的作为步数
	float steps = fabs(dx) > fabs(dy) ? fabs(dx) : fabs(dy);

	float xIncrement = dx / steps;  // x 每步增量
	float yIncrement = dy / steps;  // y 每步增量

	float x = float(x1);
	float y = float(y1);

	for (int i = 0; i <= int(steps); i++)
	{
		pDC->SetPixel(int(x + 0.5f), int(y + 0.5f), lineColor);
		x += xIncrement;
		y += yIncrement;
	}
}


// ============================================================
// 中点画线算法
// 原理：直线一般式 F(x,y)=ax+by+c=0，其中 a=y1-y2, b=x2-x1
//       判断中点在直线上方还是下方来决定下一个像素
//       判别式 d = 2a + b，d1 = 2a，d2 = 2(a+b)
// ============================================================

void CMFCApplication3View::DrawMidpoint(CDC* pDC, int x1, int y1, int x2, int y2)
{
	int a = y1 - y2;
	int b = x2 - x1;
	int d = 2 * a + b;       // 初始判别式
	int d1 = 2 * a;          // 中点在直线上方时的增量
	int d2 = 2 * (a + b);    // 中点在直线下方时的增量

	int x = x1, y = y1;
	pDC->SetPixel(x, y, lineColor);

	while (x < x2)
	{
		if (d < 0)
		{
			// 中点在直线下方，取右上像素
			x++;
			y++;
			d += d2;
		}
		else
		{
			// 中点在直线上方，取右像素
			x++;
			d += d1;
		}
		pDC->SetPixel(x, y, lineColor);
	}
}


// ============================================================
// Bresenham 算法
// 原理：通过比较 d（交点与下方最近像素的距离）来进行决策
//       初始 p = 2dy - dx
//       p >= 0 时 y++，p += 2(dy - dx)
//       p < 0 时 y 不变，p += 2dy
// ============================================================

void CMFCApplication3View::DrawBresenham(CDC* pDC, int x1, int y1, int x2, int y2)
{
	int dx = x2 - x1;
	int dy = y2 - y1;
	int p = 2 * dy - dx;     // 初始决策参数

	int x = x1, y = y1;
	pDC->SetPixel(x, y, lineColor);

	while (x < x2)
	{
		x++;
		if (p < 0)
		{
			// 离下方像素更近，y 不变
			p += 2 * dy;
		}
		else
		{
			// 离上方像素更近，y 递增
			y++;
			p += 2 * (dy - dx);
		}
		pDC->SetPixel(x, y, lineColor);
	}
}


// ============================================================
// 打印
// ============================================================

BOOL CMFCApplication3View::OnPreparePrinting(CPrintInfo* pInfo)
{
	return DoPreparePrinting(pInfo);
}

void CMFCApplication3View::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/) {}
void CMFCApplication3View::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/) {}


// ============================================================
// 诊断
// ============================================================

#ifdef _DEBUG
void CMFCApplication3View::AssertValid() const { CView::AssertValid(); }
void CMFCApplication3View::Dump(CDumpContext& dc) const { CView::Dump(dc); }
CMFCApplication3Doc* CMFCApplication3View::GetDocument() const
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMFCApplication3Doc)));
	return (CMFCApplication3Doc*)m_pDocument;
}
#endif


// ============================================================
// 菜单响应函数
// ============================================================

// DDA 算法
void CMFCApplication3View::OnDDA()
{
	m_algoType = 1;
	Invalidate();
}

// 中点算法
void CMFCApplication3View::OnMidpoint()
{
	m_algoType = 2;
	Invalidate();
}

// Bresenham 算法
void CMFCApplication3View::OnBresenham()
{
	m_algoType = 3;
	Invalidate();
}

// 设置线条颜色
void CMFCApplication3View::OnLineColor()
{
	CColorDialog dlg;
	if (dlg.DoModal())
	{
		lineColor = dlg.GetColor();
		Invalidate();  // 用新颜色重绘
	}
}
