
// MFCApplication3View.cpp: 实验三 — 扫描线种子填充算法
//

#include "pch.h"
#include "framework.h"
#ifndef SHARED_HANDLERS
#include "MFCApplication3.h"
#endif

#include "MFCApplication3Doc.h"
#include "MFCApplication3View.h"
#include <stack>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNCREATE(CMFCApplication3View, CView)

BEGIN_MESSAGE_MAP(CMFCApplication3View, CView)
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)

	ON_WM_LBUTTONDOWN()
	ON_COMMAND(ID_SHOUHUI_ZHIXIAN, &CMFCApplication3View::OnFillReset)
	ON_COMMAND(ID_LINE_COLOR, &CMFCApplication3View::OnBoundaryColor)
END_MESSAGE_MAP()


CMFCApplication3View::CMFCApplication3View() noexcept
{
	m_boundaryColor = RGB(0, 0, 255);    // 蓝色边界
	m_fillColor = RGB(255, 0, 0);        // 红色填充
	m_filled = FALSE;

	// 五边形顶点（顺时针）
	m_polygon[0] = CPoint(300, 60);
	m_polygon[1] = CPoint(520, 220);
	m_polygon[2] = CPoint(460, 420);
	m_polygon[3] = CPoint(140, 420);
	m_polygon[4] = CPoint(80, 220);
	m_vertexCount = 5;
}

CMFCApplication3View::~CMFCApplication3View() {}

BOOL CMFCApplication3View::PreCreateWindow(CREATESTRUCT& cs)
{
	return CView::PreCreateWindow(cs);
}


// ============================================================
// OnDraw — 绘制多边形边界（+ 提示文字）
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

	// 提示文字
	pDC->SetTextColor(RGB(0, 0, 0));
	pDC->SetBkMode(TRANSPARENT);
	CString tip;
	if (!m_filled)
		tip = _T("请在蓝色多边形内部点击鼠标左键，触发扫描线种子填充");
	else
		tip = _T("填充完成！点击菜单「填充→重置」可清除填充");
	pDC->TextOutW(20, 10, tip);

	// 显示边界和填充颜色
	CString colorInfo;
	colorInfo.Format(_T("边界: ■ RGB(%d,%d,%d)    填充: ■ RGB(%d,%d,%d)"),
		GetRValue(m_boundaryColor), GetGValue(m_boundaryColor), GetBValue(m_boundaryColor),
		GetRValue(m_fillColor), GetGValue(m_fillColor), GetBValue(m_fillColor));
	pDC->TextOutW(20, 35, colorInfo);

	// 绘制多边形边界
	DrawPolygon(pDC);
}


// ============================================================
// 绘制多边形边界（闭合五边形）
// ============================================================

void CMFCApplication3View::DrawPolygon(CDC* pDC)
{
	CPen pen(PS_SOLID, 3, m_boundaryColor);
	CPen* pOldPen = pDC->SelectObject(&pen);

	pDC->MoveTo(m_polygon[0]);
	for (int i = 1; i < m_vertexCount; i++)
		pDC->LineTo(m_polygon[i]);
	pDC->LineTo(m_polygon[0]);  // 闭合

	pDC->SelectObject(pOldPen);
}


// ============================================================
// 扫描线种子填充算法
// 流程: 种子压栈 → 出栈 → 沿扫描线左右填充至边界
//       → 确定[xl,xr] → 检查上下邻行 → 新种子压栈 → 循环
// ============================================================

void CMFCApplication3View::ScanLineFill(CDC* pDC, CPoint seed)
{
	std::stack<CPoint> stk;          // 1. 初始化堆栈
	stk.push(seed);                   //    种子点压栈

	while (!stk.empty())              // 2. 如果堆栈为空则结束
	{
		CPoint pt = stk.top();        //    取栈顶元素作为种子点
		stk.pop();                    //    出栈

		int y = pt.y;
		int x = pt.x;

		// 如果当前像素已是边界或已填充，跳过
		COLORREF curColor = pDC->GetPixel(x, y);
		if (curColor == m_boundaryColor || curColor == m_fillColor)
			continue;

		// 3. 沿扫描线向左填充，直到遇到边界
		int xl = x;
		while (pDC->GetPixel(xl, y) != m_boundaryColor)
		{
			pDC->SetPixel(xl, y, m_fillColor);
			xl--;
		}
		xl++;  // xl 回到最后一个填充像素

		//    沿扫描线向右填充，直到遇到边界
		int xr = x + 1;
		while (pDC->GetPixel(xr, y) != m_boundaryColor)
		{
			pDC->SetPixel(xr, y, m_fillColor);
			xr++;
		}
		xr--;  // xr 回到最后一个填充像素

		// 4. xl 和 xr 为当前扫描线上填充区段的两端

		// 5. 检查上方和下方邻行，寻找新种子点
		FindAndPushSeeds(pDC, xl, xr, y - 1, stk);
		FindAndPushSeeds(pDC, xl, xr, y + 1, stk);
	}
}


// ============================================================
// 扫描邻行，在 [xl, xr] 范围内找未填充的非边界像素段
// 每段的右端点作为新种子压入堆栈
// ============================================================

void CMFCApplication3View::FindAndPushSeeds(CDC* pDC, int xl, int xr, int y,
	std::stack<CPoint>& stk)
{
	int x = xl;
	while (x <= xr)
	{
		// 跳过已填充或边界像素
		while (x <= xr)
		{
			COLORREF c = pDC->GetPixel(x, y);
			if (c != m_boundaryColor && c != m_fillColor)
				break;
			x++;
		}

		if (x > xr) break;  // 该行扫描完毕

		// 找到一个未填充的非边界像素段
		// 将该段的右端点作为新种子压栈
		int seedX = x;
		while (x <= xr)
		{
			COLORREF c = pDC->GetPixel(x, y);
			if (c == m_boundaryColor || c == m_fillColor)
				break;
			seedX = x;
			x++;
		}

		stk.push(CPoint(seedX, y));  // 右端点压栈
	}
}


// ============================================================
// 鼠标左键 — 点击多边形内部触发填充
// ============================================================

void CMFCApplication3View::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (!m_filled)
	{
		CDC* pDC = GetDC();
		ScanLineFill(pDC, point);
		ReleaseDC(pDC);
		m_filled = TRUE;
		Invalidate();  // 刷新提示文字
	}

	CView::OnLButtonDown(nFlags, point);
}


// ============================================================
// 菜单响应
// ============================================================

// 重置：清除填充，仅保留边界
void CMFCApplication3View::OnFillReset()
{
	m_filled = FALSE;
	Invalidate();
}

// 设置边界颜色
void CMFCApplication3View::OnBoundaryColor()
{
	CColorDialog dlg(m_boundaryColor);
	if (dlg.DoModal())
	{
		m_boundaryColor = dlg.GetColor();
		m_filled = FALSE;   // 换颜色后需重新填充
		Invalidate();
	}
}


// ============================================================
// 打印 / 诊断
// ============================================================

BOOL CMFCApplication3View::OnPreparePrinting(CPrintInfo* pInfo) { return DoPreparePrinting(pInfo); }
void CMFCApplication3View::OnBeginPrinting(CDC*, CPrintInfo*) {}
void CMFCApplication3View::OnEndPrinting(CDC*, CPrintInfo*) {}

#ifdef _DEBUG
void CMFCApplication3View::AssertValid() const { CView::AssertValid(); }
void CMFCApplication3View::Dump(CDumpContext& dc) const { CView::Dump(dc); }
CMFCApplication3Doc* CMFCApplication3View::GetDocument() const
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMFCApplication3Doc)));
	return (CMFCApplication3Doc*)m_pDocument;
}
#endif
