
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
	COLORREF lineColor;

	int drawtype;
	CPoint beginPoint, movePoint;

	// 实验3：双缓冲动画
	CPoint m_ballCenter;      // 小球圆心
	int m_dx;                 // X方向运动速度
	int m_dy;                 // Y方向运动速度
	BOOL m_isPlaying;         // 播放状态
	UINT_PTR m_nTimerID;      // 定时器ID

public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
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

	// 实验3：双缓冲动画辅助函数
	void DoubleBuffer(CDC* pDC);
	void DrawObject(CDC* pDC, const CRect& rect);
	void BorderTest(const CRect& rect);

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSuanfaDda();
	afx_msg void OnLineColor();
	afx_msg void OnSuanfaZhongdian();
	afx_msg void OnSuanfaBresenham();
	afx_msg void OnSuanfaZhongdianyuan();
	afx_msg void OnShouhuiJuxing();
	afx_msg void OnShouhuiYuanxing();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnShouhuiZhixian();
	afx_msg void OnTuxingPlay();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};

#ifndef _DEBUG  // MFCApplication3View.cpp 中的调试版本
inline CMFCApplication3Doc* CMFCApplication3View::GetDocument() const
   { return reinterpret_cast<CMFCApplication3Doc*>(m_pDocument); }
#endif
