// MFCApplication3View.h

#pragma once

class CMFCApplication3View : public CView
{
protected:
	CMFCApplication3View() noexcept;
	DECLARE_DYNCREATE(CMFCApplication3View)
public:
	CMFCApplication3Doc* GetDocument() const;

	int m_mode;
	COLORREF m_lineColor, m_fillColor, m_boundColor;
	BOOL m_filled; CPoint m_poly[6]; int m_nPoly;
	CPoint m_orig[5], m_cur[5]; int m_nVert; BOOL m_trans;
	CPoint m_pt1, m_pt2; BOOL m_drawing;
	CPoint m_bez[4]; int m_nBez;

public:
	virtual void OnDraw(CDC* pDC);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo*);
	virtual void OnBeginPrinting(CDC*, CPrintInfo*);
	virtual void OnEndPrinting(CDC*, CPrintInfo*);
public:
	virtual ~CMFCApplication3View();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	afx_msg void OnDrawLine();
	afx_msg void OnDrawRect();
	afx_msg void OnDrawEllipse();
	void DDALine(CDC*,int,int,int,int);
	void MidLine(CDC*,int,int,int,int);
	void BresLine(CDC*,int,int,int,int);
	afx_msg void OnDDA(); afx_msg void OnMidpoint(); afx_msg void OnBresenham();
	void MidCircle(CDC*,int,int,int); afx_msg void OnCircle();
	void DrawPoly(CDC*,CPoint*,int,COLORREF);
	void ScanFill(CDC*,CPoint); void EdgeFill(CDC*,CPoint,COLORREF,COLORREF);
	BOOL InsidePoly(CPoint*,int,CPoint); void PolyFill(CDC*,CPoint);
	afx_msg void OnFillScan(); afx_msg void OnFillEdge(); afx_msg void OnFillPoly();
	afx_msg void OnLButtonDown(UINT,CPoint); afx_msg void OnMouseMove(UINT,CPoint); afx_msg void OnLButtonUp(UINT,CPoint);
	CPoint Centroid(CPoint*,int); void ApplyMat(double[3][3]);
	afx_msg void OnTransTranslate(); afx_msg void OnTransScale();
	afx_msg void OnTransRotate(); afx_msg void OnTransReset();
	afx_msg void OnFreeLine(); afx_msg void OnFreeCircle();
	void BezierCurve(CDC*,CPoint*,int); afx_msg void OnBezier();
	afx_msg void OnColorLine(); afx_msg void OnColorFill();
	DECLARE_MESSAGE_MAP()
};
#ifndef _DEBUG
inline CMFCApplication3Doc* CMFCApplication3View::GetDocument() const
   { return reinterpret_cast<CMFCApplication3Doc*>(m_pDocument); }
#endif
