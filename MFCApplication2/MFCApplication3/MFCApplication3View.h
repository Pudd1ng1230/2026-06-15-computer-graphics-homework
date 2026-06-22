
// MFCApplication3View.h: CMFCApplication3View 类的接口
//

#pragma once


class CMFCApplication3View : public CView
{
protected: // 仅从序列化创建
	CMFCApplication3View() noexcept;
	DECLARE_DYNCREATE(CMFCApplication3View)

// 特性
public:
	CMFCApplication3Doc* GetDocument() const;

// 操作
public:

// 重写
	COLORREF lineColor;       // 线条颜色
	int m_algoType;           // 当前算法: 0=无, 1=DDA, 2=中点, 3=Bresenham

public:
	virtual void OnDraw(CDC* pDC);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 实现
public:
	virtual ~CMFCApplication3View();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	// 三种直线扫描算法
	void DrawDDA(CDC* pDC, int x1, int y1, int x2, int y2);
	void DrawMidpoint(CDC* pDC, int x1, int y1, int x2, int y2);
	void DrawBresenham(CDC* pDC, int x1, int y1, int x2, int y2);

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDDA();
	afx_msg void OnMidpoint();
	afx_msg void OnBresenham();
	afx_msg void OnLineColor();
};

#ifndef _DEBUG
inline CMFCApplication3Doc* CMFCApplication3View::GetDocument() const
   { return reinterpret_cast<CMFCApplication3Doc*>(m_pDocument); }
#endif
