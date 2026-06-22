
// MFCApplication3View.h

#pragma once


class CMFCApplication3View : public CView
{
protected:
	CMFCApplication3View() noexcept;
	DECLARE_DYNCREATE(CMFCApplication3View)
public:
	CMFCApplication3Doc* GetDocument() const;
	CPoint m_original[5], m_current[5];
	int m_vertexCount;
	BOOL m_transformed;
public:
	virtual void OnDraw(CDC* pDC);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
public:
	virtual ~CMFCApplication3View();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
protected:
	void DrawPolygon(CDC* pDC, CPoint* pts, COLORREF color, int width);
	CPoint GetCentroid(CPoint* pts, int count);
	void ApplyMatrix(double m[3][3]);
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTransformTranslate();
	afx_msg void OnTransformScale();
	afx_msg void OnTransformRotate();
	afx_msg void OnTransformReset();
};

#ifndef _DEBUG
inline CMFCApplication3Doc* CMFCApplication3View::GetDocument() const
   { return reinterpret_cast<CMFCApplication3Doc*>(m_pDocument); }
#endif