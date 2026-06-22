
// MFCApplication3View.cpp: CMFCApplication3View 类的实现
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
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

	ON_COMMAND(ID_SUANFA_DDA, &CMFCApplication3View::OnSuanfaDda)

	ON_COMMAND(ID_LINE_COLOR, &CMFCApplication3View::OnLineColor)

	ON_COMMAND(ID_SUANFA_ZHONGDIAN, &CMFCApplication3View::OnSuanfaZhongdian)

	
	ON_COMMAND(ID_SUANFA_BRESENHAM, &CMFCApplication3View::OnSuanfaBresenham)

	ON_COMMAND(ID_SUANFA_ZHONGDIANYUAN, &CMFCApplication3View::OnSuanfaZhongdianyuan)

	


	ON_COMMAND(ID_SHOUHUI_JUXING, &CMFCApplication3View::OnShouhuiJuxing)
	ON_COMMAND(ID_SHOUHUI_YUANXING, &CMFCApplication3View::OnShouhuiYuanxing)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_COMMAND(ID_SHOUHUI_ZHIXIAN, &CMFCApplication3View::OnShouhuiZhixian)

	// 实验3：双缓冲动画
	ON_COMMAND(ID_TUXING_PLAY, &CMFCApplication3View::OnTuxingPlay)
	ON_WM_TIMER()
END_MESSAGE_MAP()

// CMFCApplication3View 构造/析构

CMFCApplication3View::CMFCApplication3View() noexcept
{
	// TODO: 在此处添加构造代码
	lineColor = RGB(255, 0, 255);
	drawtype = 0;
	beginPoint = CPoint(0, 0);
	movePoint = CPoint(0, 0);

	// 实验3：初始化动画参数
	m_ballCenter = CPoint(0, 0);     // 初始为(0,0)，在首次OnDraw中会根据客户区中心设置
	m_dx = 5;                        // X方向每10ms移动5像素
	m_dy = 4;                        // Y方向每10ms移动4像素
	m_isPlaying = FALSE;             // 初始停止状态
	m_nTimerID = 0;
}

CMFCApplication3View::~CMFCApplication3View()
{
}

BOOL CMFCApplication3View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

// CMFCApplication3View 绘图

void CMFCApplication3View::OnDraw(CDC*pDC)
{
	CMFCApplication3Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 在此处为本机数据添加绘制代码
		// 实验3：使用双缓冲机制绘制动画
	DoubleBuffer(pDC);
}


// CMFCApplication3View 打印

BOOL CMFCApplication3View::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CMFCApplication3View::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加额外的打印前进行的初始化过程
}

void CMFCApplication3View::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加打印后进行的清理过程
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

CMFCApplication3Doc* CMFCApplication3View::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMFCApplication3Doc)));
	return (CMFCApplication3Doc*)m_pDocument;
}
#endif //_DEBUG


// CMFCApplication3View 消息处理程序

void CMFCApplication3View::OnSuanfaDda()
{
	CDC* pDC = GetDC();
	int x1, y1, x2, y2;
	x1 = 100, y1 = 100, x2 = 500, y2 = 600;
	float dx, dy, k, max;
	dx = float(x2 - x1);
	dy = float(y2 - y1);
	k = dy / dx;
	max = dx > dy ? dx : dy;
	float x, y;
	x = float(x1);
	y = float(y1);
	for (int i = 0; i < max; i++)
	{
		if (k < 1) {
			x += 1;
			y += k;
		}
		if (k > 1) {
			x += 1 / k;
			y += 1;
		}
		pDC->SetPixel(int(x + 0.5), int(y + 0.5), lineColor);
	}

}

void CMFCApplication3View::OnLineColor()
{
	CColorDialog dlg;
	if (dlg.DoModal()) {
		lineColor = dlg.GetColor();
	}
}

void CMFCApplication3View::OnSuanfaZhongdian()
{
	CDC* pDC = GetDC();
	int x1 = 10, y1 = 10, x2 = 500, y2 = 420;
	int a, b, d, d1, d2;
	int x, y;
	a = y1 - y2, b = x2 - x1;
	d = 2 * a + b;
	d1 = 2 * a;
	d2 = 2 * (a + b);
	x = x1, y = y1;
	pDC->SetPixel(x, y, lineColor);
	while (x < x2) {
		if (d < 0) {
			x++;
			y++;
			d += d2;
		}
		else {
			x++;d += d1;
		}
		pDC->SetPixel(x, y, lineColor);
	}
}

void CMFCApplication3View::OnSuanfaBresenham()
{
	CDC* pDC = GetDC();
	int x1 = 200, y1 = 100, x2 = 600, y2 = 320;
	int dx, dy;
	dx = x2 - x1;
	dy = y2 - y1;
	int p = 2* dy - dx;
	int x, y;
	x = x1, y = y1;
	for (;x < x2;x++) {
		pDC->SetPixel(x, y, lineColor);

		if (p >= 0) {
			y++;
			p += x * (dy - dx);
		}
		else {
			p += 2 * dy;
		}
	}
}

void CMFCApplication3View::OnSuanfaZhongdianyuan()
{
	CDC* pDC = GetDC();
	CRect rect;

	GetClientRect(&rect);

	pDC->SetMapMode(MM_ANISOTROPIC);

	pDC->SetWindowExt(rect.Width(), rect.Height());

	pDC->SetViewportExt(rect.Width(), -rect.Height());

	pDC->SetViewportOrg(rect.Width() / 2, rect.Height() / 2);

	rect.OffsetRect(-rect.Width() / 2, -rect.Height() / 2);

	int r = 200;

	int x = 0, y = r;

	int d = 1 + r;

	while (x < y)

	{

		x++;

		if (d > 0) {

			d += 2 * (x - y) + 5;

			y--;

		}

		else

		{

			d += 2 * x + 3;

			y -= 0;

		}

		pDC->SetPixel(x, y, RGB(255, 0, 255));

		pDC->SetPixel(-x, y, RGB(255, 0, 255));

		pDC->SetPixel(x, -y, RGB(255, 0, 255));

		pDC->SetPixel(-x, -y, RGB(255, 0, 255));

		pDC->SetPixel(y, x, RGB(255, 0, 255));

		pDC->SetPixel(-y, x, RGB(255, 0, 255));

		pDC->SetPixel(y, -x, RGB(255, 0, 255));

		pDC->SetPixel(-y, -x, RGB(255, 0, 255));

		pDC->SetPixel(x, y, lineColor);
	}
}


void CMFCApplication3View::OnShouhuiJuxing()
{
	drawtype = 2;
}

void CMFCApplication3View::OnShouhuiYuanxing()
{
	// TODO: 在此添加命令处理程序代码
	drawtype = 3;
}



void CMFCApplication3View::OnLButtonDown(UINT nFlags, CPoint point)
{
	beginPoint = point;
	movePoint = point;

	CView::OnLButtonDown(nFlags, point);
}

void CMFCApplication3View::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CDC* pDC = GetDC();
	pDC->SetROP2(R2_NOTXORPEN);
	if (nFlags & MK_LBUTTON) {
		switch (drawtype) {
		case 1:
			pDC->MoveTo(beginPoint.x, beginPoint.y), pDC->LineTo(movePoint.x, movePoint.y);
			pDC->MoveTo(beginPoint.x, beginPoint.y), pDC->LineTo(point.x, point.y);
			break;
		case 2:
			pDC->Rectangle(beginPoint.x, beginPoint.y, movePoint.x, movePoint.y);
			pDC->Rectangle(beginPoint.x, beginPoint.y, point.x, point.y);
			break;
		case 3:
			pDC->Ellipse(beginPoint.x, beginPoint.y, movePoint.x, movePoint.y);
			pDC->Ellipse(beginPoint.x, beginPoint.y, point.x, point.y);
			break;
		}
	}
	movePoint = point;
	CView::OnMouseMove(nFlags, point);
}

void CMFCApplication3View::OnShouhuiZhixian()
{
	// TODO: 在此添加命令处理程序代码
	drawtype = 1;
}


// =====================================================
// 实验3：双缓冲动画实现
// =====================================================

// 双缓冲绘制函数
void CMFCApplication3View::DoubleBuffer(CDC* pDC)
{
	CRect rect;
	GetClientRect(&rect);

	// 首次调用时，将球心设置为客户区中心
	if (m_ballCenter == CPoint(0, 0))
	{
		m_ballCenter = rect.CenterPoint();
	}

	// 创建内存DC与内存位图（双缓冲核心）
	CDC memDC;
	memDC.CreateCompatibleDC(pDC);

	CBitmap memBitmap;
	memBitmap.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height());
	CBitmap* pOldBitmap = memDC.SelectObject(&memBitmap);

	// 用黑色填充背景
	memDC.FillSolidRect(rect, RGB(0, 0, 0));

	// 检测边界碰撞
	BorderTest(rect);

	// 在内存DC上绘制小球
	DrawObject(&memDC, rect);

	// 将内存DC的内容一次性复制到屏幕DC（消除闪烁）
	pDC->BitBlt(0, 0, rect.Width(), rect.Height(), &memDC, 0, 0, SRCCOPY);

	// 恢复并释放资源
	memDC.SelectObject(pOldBitmap);
	memBitmap.DeleteObject();
	memDC.DeleteDC();
}

// 绘制红色小球
void CMFCApplication3View::DrawObject(CDC* pDC, const CRect& rect)
{
	int r = rect.Width() / 10;  // 半径为1/10客户区宽度

	// 创建红色画刷
	CBrush brush(RGB(255, 0, 0));
	CBrush* pOldBrush = pDC->SelectObject(&brush);

	// 绘制红色实心圆
	pDC->Ellipse(
		m_ballCenter.x - r,
		m_ballCenter.y - r,
		m_ballCenter.x + r,
		m_ballCenter.y + r
	);

	pDC->SelectObject(pOldBrush);
	brush.DeleteObject();
}

// 边界碰撞检测
void CMFCApplication3View::BorderTest(const CRect& rect)
{
	int r = rect.Width() / 10;  // 球半径

	// 左边界碰撞
	if (m_ballCenter.x - r <= rect.left)
	{
		m_dx = abs(m_dx);       // 反转X方向为正（向右）
		m_ballCenter.x = rect.left + r;  // 修正位置防止卡边界
	}

	// 右边界碰撞
	if (m_ballCenter.x + r >= rect.right)
	{
		m_dx = -abs(m_dx);      // 反转X方向为负（向左）
		m_ballCenter.x = rect.right - r;
	}

	// 上边界碰撞
	if (m_ballCenter.y - r <= rect.top)
	{
		m_dy = abs(m_dy);       // 反转Y方向为正（向下）
		m_ballCenter.y = rect.top + r;
	}

	// 下边界碰撞
	if (m_ballCenter.y + r >= rect.bottom)
	{
		m_dy = -abs(m_dy);      // 反转Y方向为负（向上）
		m_ballCenter.y = rect.bottom - r;
	}
}

// 菜单响应：播放/停止动画
void CMFCApplication3View::OnTuxingPlay()
{
	if (m_isPlaying)
	{
		// 停止动画
		KillTimer(m_nTimerID);
		m_isPlaying = FALSE;
	}
	else
	{
		// 开始播放：设置定时器，每10ms触发一次
		m_nTimerID = SetTimer(1, 10, NULL);
		m_isPlaying = TRUE;
	}
}

// WM_TIMER消息处理：更新小球位置
void CMFCApplication3View::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == m_nTimerID && m_isPlaying)
	{
		// 根据运动方向更新球心位置
		m_ballCenter.x += m_dx;
		m_ballCenter.y += m_dy;

		// 触发重绘
		Invalidate(FALSE);
	}

	CView::OnTimer(nIDEvent);
}
