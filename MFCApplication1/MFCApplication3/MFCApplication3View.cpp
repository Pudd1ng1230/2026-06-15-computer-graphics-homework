
// MFCApplication3View.cpp: CMFCApplication3View 类的实现
//

#include "pch.h"
#include "framework.h"
#ifndef SHARED_HANDLERS
#include "MFCApplication3.h"
#endif

#include "MFCApplication3Doc.h"
#include "MFCApplication3View.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMFCApplication3View

IMPLEMENT_DYNCREATE(CMFCApplication3View, CView)

BEGIN_MESSAGE_MAP(CMFCApplication3View, CView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)

	// 实验1：绘图菜单
	ON_COMMAND(ID_SHOUHUI_ZHIXIAN, &CMFCApplication3View::OnDrawLine)
	ON_COMMAND(ID_SHOUHUI_JUXING, &CMFCApplication3View::OnDrawRect)
	ON_COMMAND(ID_SHOUHUI_YUANXING, &CMFCApplication3View::OnDrawEllipse)
END_MESSAGE_MAP()

// CMFCApplication3View 构造/析构

CMFCApplication3View::CMFCApplication3View() noexcept
{
	lineColor = RGB(255, 0, 0);   // 默认红色线条
	m_drawType = 0;               // 初始不绘制
}

CMFCApplication3View::~CMFCApplication3View()
{
}

BOOL CMFCApplication3View::PreCreateWindow(CREATESTRUCT& cs)
{
	return CView::PreCreateWindow(cs);
}

// CMFCApplication3View 绘图

void CMFCApplication3View::OnDraw(CDC* pDC)
{
	CMFCApplication3Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// 实验1：根据菜单选择绘制对应图形
	// 使用白色背景
	CRect rect;
	GetClientRect(&rect);
	pDC->FillSolidRect(rect, RGB(255, 255, 255));

	// 创建红色画笔
	CPen pen(PS_SOLID, 2, lineColor);
	CPen* pOldPen = pDC->SelectObject(&pen);

	switch (m_drawType)
	{
	case 1:  // 绘制直线: MoveTo + LineTo
		pDC->MoveTo(100, 200);
		pDC->LineTo(500, 400);
		break;

	case 2:  // 绘制直角矩形: Rectangle
		pDC->Rectangle(100, 100, 500, 400);
		break;

	case 3:  // 绘制椭圆: Ellipse
		pDC->Ellipse(100, 100, 500, 400);
		break;

	default:
		break;
	}

	pDC->SelectObject(pOldPen);
}


// CMFCApplication3View 打印

BOOL CMFCApplication3View::OnPreparePrinting(CPrintInfo* pInfo)
{
	return DoPreparePrinting(pInfo);
}

void CMFCApplication3View::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
}

void CMFCApplication3View::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
}


// CMFCApplication3View 诊断

#ifdef _DEBUG
void CMFCApplication3View::AssertValid() const
{
	CView::AssertValid();
}

void CMFCApplication3View::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMFCApplication3Doc* CMFCApplication3View::GetDocument() const
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMFCApplication3Doc)));
	return (CMFCApplication3Doc*)m_pDocument;
}
#endif //_DEBUG


// ============================================================
// 实验1：菜单响应函数 — 分别绘制直线、矩形、椭圆
// ============================================================

// 绘制直线 — 使用 MoveTo / LineTo
void CMFCApplication3View::OnDrawLine()
{
	m_drawType = 1;
	Invalidate();  // 触发 OnDraw 重绘
}

// 绘制矩形 — 使用 Rectangle
void CMFCApplication3View::OnDrawRect()
{
	m_drawType = 2;
	Invalidate();
}

// 绘制椭圆 — 使用 Ellipse
void CMFCApplication3View::OnDrawEllipse()
{
	m_drawType = 3;
	Invalidate();
}
