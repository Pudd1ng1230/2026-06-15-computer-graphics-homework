// MFCApplication3View.cpp

#include "pch.h"
#include "framework.h"
#ifndef SHARED_HANDLERS
#include "MFCApplication3.h"
#endif
#include "MFCApplication3Doc.h"
#include "MFCApplication3View.h"
#include "resource.h"
#include <cmath>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

class CTranslateDlg : public CDialog
{
public:
	int m_dx, m_dy;
	CTranslateDlg() : CDialog(IDD_TRANSLATE_DLG), m_dx(100), m_dy(50) {}
protected:
	virtual void DoDataExchange(CDataExchange* pDX) {
		CDialog::DoDataExchange(pDX);
		DDX_Text(pDX, IDC_EDIT_DX, m_dx);
		DDX_Text(pDX, IDC_EDIT_DY, m_dy);
	}
};

class CScaleDlg : public CDialog
{
public:
	int m_sx, m_sy;
	CScaleDlg() : CDialog(IDD_SCALE_DLG), m_sx(150), m_sy(150) {}
protected:
	virtual void DoDataExchange(CDataExchange* pDX) {
		CDialog::DoDataExchange(pDX);
		DDX_Text(pDX, IDC_EDIT_SX, m_sx);
		DDX_Text(pDX, IDC_EDIT_SY, m_sy);
	}
};

class CRotateDlg : public CDialog
{
public:
	int m_angle;
	CRotateDlg() : CDialog(IDD_ROTATE_DLG), m_angle(45) {}
protected:
	virtual void DoDataExchange(CDataExchange* pDX) {
		CDialog::DoDataExchange(pDX);
		DDX_Text(pDX, IDC_EDIT_ANGLE, m_angle);
	}
};


IMPLEMENT_DYNCREATE(CMFCApplication3View, CView)

BEGIN_MESSAGE_MAP(CMFCApplication3View, CView)
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_COMMAND(ID_SUANFA_DDA, &CMFCApplication3View::OnTransformTranslate)
	ON_COMMAND(ID_SUANFA_ZHONGDIAN, &CMFCApplication3View::OnTransformScale)
	ON_COMMAND(ID_SUANFA_BRESENHAM, &CMFCApplication3View::OnTransformRotate)
	ON_COMMAND(ID_LINE_COLOR, &CMFCApplication3View::OnTransformReset)
END_MESSAGE_MAP()


CMFCApplication3View::CMFCApplication3View() noexcept
{
	m_original[0] = CPoint(250, 100); m_original[1] = CPoint(400, 100);
	m_original[2] = CPoint(400, 250); m_original[3] = CPoint(325, 320);
	m_original[4] = CPoint(250, 250); m_vertexCount = 5;
	for (int i = 0; i < m_vertexCount; i++) m_current[i] = m_original[i];
	m_transformed = FALSE;
}
CMFCApplication3View::~CMFCApplication3View() {}
BOOL CMFCApplication3View::PreCreateWindow(CREATESTRUCT& cs) { return CView::PreCreateWindow(cs); }

void CMFCApplication3View::OnDraw(CDC* pDC)
{
	CMFCApplication3Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc); if (!pDoc) return;
	CRect rect; GetClientRect(&rect);
	pDC->FillSolidRect(rect, RGB(255, 255, 255));
	pDC->SetTextColor(RGB(0,0,0)); pDC->SetBkMode(TRANSPARENT);
	if (m_transformed)
		pDC->TextOutW(20, 10, _T("灰色虚线=原始  红色实线=变换后  菜单[变换->重置]恢复"));
	else
		pDC->TextOutW(20, 10, _T("菜单[变换] 平移/缩放/旋转  弹出对话框输入参数"));
	DrawPolygon(pDC, m_original, RGB(180,180,180), 1);
	if (m_transformed) DrawPolygon(pDC, m_current, RGB(255,0,0), 3);
	CBrush brush(RGB(0,0,255));
	CBrush* pOldBrush = pDC->SelectObject(&brush);
	CPoint* pts = m_transformed ? m_current : m_original;
	for (int i = 0; i < m_vertexCount; i++)
		pDC->Ellipse(pts[i].x-3, pts[i].y-3, pts[i].x+3, pts[i].y+3);
	pDC->SelectObject(pOldBrush);
}

void CMFCApplication3View::DrawPolygon(CDC* pDC, CPoint* pts, COLORREF color, int width)
{
	CPen pen(PS_SOLID, width, color);
	CPen* pOldPen = pDC->SelectObject(&pen);
	pDC->MoveTo(pts[0]);
	for (int i = 1; i < m_vertexCount; i++) pDC->LineTo(pts[i]);
	pDC->LineTo(pts[0]);
	pDC->SelectObject(pOldPen);
}

CPoint CMFCApplication3View::GetCentroid(CPoint* pts, int count)
{
	int cx=0, cy=0;
	for (int i=0; i<count; i++) { cx+=pts[i].x; cy+=pts[i].y; }
	return CPoint(cx/count, cy/count);
}

void CMFCApplication3View::ApplyMatrix(double m[3][3])
{
	for (int i=0; i<m_vertexCount; i++)
	{
		double x=(double)m_current[i].x, y=(double)m_current[i].y;
		m_current[i].x = (LONG)(x*m[0][0] + y*m[1][0] + m[2][0] + 0.5);
		m_current[i].y = (LONG)(x*m[0][1] + y*m[1][1] + m[2][1] + 0.5);
	}
}

void CMFCApplication3View::OnTransformTranslate()
{
	CTranslateDlg dlg;
	if (dlg.DoModal() == IDOK) {
		double m[3][3] = {{1,0,0},{0,1,0},{(double)dlg.m_dx,(double)dlg.m_dy,1}};
		ApplyMatrix(m); m_transformed = TRUE; Invalidate();
	}
}

void CMFCApplication3View::OnTransformScale()
{
	CScaleDlg dlg;
	if (dlg.DoModal() == IDOK) {
		double sx=dlg.m_sx/100.0, sy=dlg.m_sy/100.0;
		CPoint c = GetCentroid(m_current, m_vertexCount);
		double toO[3][3] = {{1,0,0},{0,1,0},{(double)-c.x,(double)-c.y,1}};
		double sc[3][3]  = {{sx,0,0},{0,sy,0},{0,0,1}};
		double back[3][3]= {{1,0,0},{0,1,0},{(double)c.x,(double)c.y,1}};
		ApplyMatrix(toO); ApplyMatrix(sc); ApplyMatrix(back);
		m_transformed = TRUE; Invalidate();
	}
}

void CMFCApplication3View::OnTransformRotate()
{
	CRotateDlg dlg;
	if (dlg.DoModal() == IDOK) {
		double rad = dlg.m_angle * M_PI / 180.0;
		double c = cos(rad), s = sin(rad);
		CPoint ct = GetCentroid(m_current, m_vertexCount);
		double toO[3][3] = {{1,0,0},{0,1,0},{(double)-ct.x,(double)-ct.y,1}};
		double rot[3][3] = {{c,s,0},{-s,c,0},{0,0,1}};
		double back[3][3]= {{1,0,0},{0,1,0},{(double)ct.x,(double)ct.y,1}};
		ApplyMatrix(toO); ApplyMatrix(rot); ApplyMatrix(back);
		m_transformed = TRUE; Invalidate();
	}
}

void CMFCApplication3View::OnTransformReset()
{
	for (int i=0; i<m_vertexCount; i++) m_current[i] = m_original[i];
	m_transformed = FALSE; Invalidate();
}

BOOL CMFCApplication3View::OnPreparePrinting(CPrintInfo* pInfo) { return DoPreparePrinting(pInfo); }
void CMFCApplication3View::OnBeginPrinting(CDC*, CPrintInfo*) {}
void CMFCApplication3View::OnEndPrinting(CDC*, CPrintInfo*) {}
#ifdef _DEBUG
void CMFCApplication3View::AssertValid() const { CView::AssertValid(); }
void CMFCApplication3View::Dump(CDumpContext& dc) const { CView::Dump(dc); }
CMFCApplication3Doc* CMFCApplication3View::GetDocument() const {
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMFCApplication3Doc)));
	return (CMFCApplication3Doc*)m_pDocument;
}
#endif
