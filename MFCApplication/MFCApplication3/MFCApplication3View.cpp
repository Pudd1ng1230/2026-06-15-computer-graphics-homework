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
#include <stack>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

class CTransDlg:public CDialog{public:int m_dx,m_dy;CTransDlg():CDialog(IDD_TRANSLATE_DLG),m_dx(100),m_dy(50){}protected:virtual void DoDataExchange(CDataExchange* pDX){CDialog::DoDataExchange(pDX);DDX_Text(pDX,IDC_EDIT_DX,m_dx);DDX_Text(pDX,IDC_EDIT_DY,m_dy);}};
class CScaleDlg2:public CDialog{public:int m_sx,m_sy;CScaleDlg2():CDialog(IDD_SCALE_DLG),m_sx(150),m_sy(150){}protected:virtual void DoDataExchange(CDataExchange* pDX){CDialog::DoDataExchange(pDX);DDX_Text(pDX,IDC_EDIT_SX,m_sx);DDX_Text(pDX,IDC_EDIT_SY,m_sy);}};
class CRotDlg2:public CDialog{public:int m_ang;CRotDlg2():CDialog(IDD_ROTATE_DLG),m_ang(45){}protected:virtual void DoDataExchange(CDataExchange* pDX){CDialog::DoDataExchange(pDX);DDX_Text(pDX,IDC_EDIT_ANGLE,m_ang);}};

IMPLEMENT_DYNCREATE(CMFCApplication3View,CView)

BEGIN_MESSAGE_MAP(CMFCApplication3View,CView)
ON_COMMAND(ID_FILE_PRINT,&CView::OnFilePrint)
ON_COMMAND(ID_FILE_PRINT_DIRECT,&CView::OnFilePrint)
ON_COMMAND(ID_FILE_PRINT_PREVIEW,&CView::OnFilePrintPreview)
ON_COMMAND(ID_DRAW_LINE,&CMFCApplication3View::OnDrawLine)
ON_COMMAND(ID_DRAW_RECT,&CMFCApplication3View::OnDrawRect)
ON_COMMAND(ID_DRAW_ELLIPSE,&CMFCApplication3View::OnDrawEllipse)
ON_COMMAND(ID_LINE_DDA,&CMFCApplication3View::OnDDA)
ON_COMMAND(ID_LINE_MID,&CMFCApplication3View::OnMidpoint)
ON_COMMAND(ID_LINE_BRES,&CMFCApplication3View::OnBresenham)
ON_COMMAND(ID_CIRCLE_MID,&CMFCApplication3View::OnCircle)
ON_COMMAND(ID_FILL_SCAN,&CMFCApplication3View::OnFillScan)
ON_COMMAND(ID_FILL_EDGE,&CMFCApplication3View::OnFillEdge)
ON_COMMAND(ID_FILL_POLY,&CMFCApplication3View::OnFillPoly)
ON_COMMAND(ID_TRANS_TRANSLATE,&CMFCApplication3View::OnTransTranslate)
ON_COMMAND(ID_TRANS_SCALE,&CMFCApplication3View::OnTransScale)
ON_COMMAND(ID_TRANS_ROTATE,&CMFCApplication3View::OnTransRotate)
ON_COMMAND(ID_TRANS_RESET,&CMFCApplication3View::OnTransReset)
ON_COMMAND(ID_FREE_LINE,&CMFCApplication3View::OnFreeLine)
ON_COMMAND(ID_FREE_CIRCLE,&CMFCApplication3View::OnFreeCircle)
ON_COMMAND(ID_BEZIER,&CMFCApplication3View::OnBezier)
ON_COMMAND(ID_COLOR_LINE,&CMFCApplication3View::OnColorLine)
ON_COMMAND(ID_COLOR_FILL,&CMFCApplication3View::OnColorFill)
ON_WM_LBUTTONDOWN()
ON_WM_MOUSEMOVE()
ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

CMFCApplication3View::CMFCApplication3View() noexcept
{
	m_mode=0; m_lineColor=RGB(255,0,0); m_fillColor=RGB(0,0,255); m_boundColor=RGB(0,0,0);
	m_filled=FALSE;
	m_poly[0]=CPoint(300,60); m_poly[1]=CPoint(520,220); m_poly[2]=CPoint(460,420);
	m_poly[3]=CPoint(140,420); m_poly[4]=CPoint(80,220); m_nPoly=5;
	m_orig[0]=CPoint(250,100); m_orig[1]=CPoint(400,100); m_orig[2]=CPoint(400,250);
	m_orig[3]=CPoint(325,320); m_orig[4]=CPoint(250,250); m_nVert=5; m_trans=FALSE;
	for(int i=0;i<m_nVert;i++) m_cur[i]=m_orig[i];
	m_pt1=m_pt2=CPoint(0,0); m_drawing=FALSE;
	m_bez[0]=CPoint(100,300); m_bez[1]=CPoint(250,50); m_bez[2]=CPoint(450,350); m_bez[3]=CPoint(550,200); m_nBez=4;
}
CMFCApplication3View::~CMFCApplication3View(){}
BOOL CMFCApplication3View::PreCreateWindow(CREATESTRUCT& cs){return CView::PreCreateWindow(cs);}

void CMFCApplication3View::OnDraw(CDC* pDC)
{
	CMFCApplication3Doc* pDoc=GetDocument(); ASSERT_VALID(pDoc); if(!pDoc)return;
	CRect rc; GetClientRect(&rc); pDC->FillSolidRect(rc,RGB(255,255,255));
	pDC->SetBkMode(TRANSPARENT); pDC->SetTextColor(RGB(0,0,0));
	switch(m_mode){
	case 0:pDC->TextOutW(20,10,_T("请从菜单选择功能"));break;
	case 1:{CPen pen(PS_SOLID,2,m_lineColor);CPen* o=pDC->SelectObject(&pen);pDC->MoveTo(100,200);pDC->LineTo(500,400);pDC->SelectObject(o);pDC->TextOutW(20,10,_T("直线"));break;}
	case 2:{CPen pen(PS_SOLID,2,m_lineColor);CPen* o=pDC->SelectObject(&pen);pDC->Rectangle(100,100,500,400);pDC->SelectObject(o);pDC->TextOutW(20,10,_T("矩形"));break;}
	case 3:{CPen pen(PS_SOLID,2,m_lineColor);CPen* o=pDC->SelectObject(&pen);pDC->Ellipse(100,100,500,400);pDC->SelectObject(o);pDC->TextOutW(20,10,_T("椭圆"));break;}
	case 10:{DDALine(pDC,50,50,200,300);pDC->TextOutW(20,10,_T("DDA(50,50)->(200,300)"));break;}
	case 11:{MidLine(pDC,300,50,450,300);pDC->TextOutW(20,10,_T("中点(300,50)->(450,300)"));break;}
	case 12:{BresLine(pDC,550,50,700,300);pDC->TextOutW(20,10,_T("Bresenham(550,50)->(700,300)"));break;}
	case 20:{MidCircle(pDC,350,250,150);pDC->TextOutW(20,10,_T("中点画圆R=150"));break;}
	case 30:{DrawPoly(pDC,m_poly,m_nPoly,m_boundColor);pDC->TextOutW(20,10,m_filled?_T("扫描线填充完成"):_T("点击内部触发填充"));break;}
	case 31:{DrawPoly(pDC,m_poly,m_nPoly,m_boundColor);pDC->TextOutW(20,10,m_filled?_T("边线填充完成"):_T("点击内部触发填充"));break;}
	case 32:{DrawPoly(pDC,m_poly,m_nPoly,m_boundColor);pDC->TextOutW(20,10,m_filled?_T("多边形填充完成"):_T("点击内部(蓝种子/黑边界)"));break;}
	case 40:case 41:case 42:{CPen pen(PS_SOLID,1,RGB(180,180,180));CPen* o=pDC->SelectObject(&pen);pDC->MoveTo(m_orig[0]);for(int i=1;i<m_nVert;i++)pDC->LineTo(m_orig[i]);pDC->LineTo(m_orig[0]);pDC->SelectObject(o);if(m_trans){CPen p2(PS_SOLID,3,RGB(255,0,0));pDC->SelectObject(&p2);pDC->MoveTo(m_cur[0]);for(int i=1;i<m_nVert;i++)pDC->LineTo(m_cur[i]);pDC->LineTo(m_cur[0]);pDC->SelectObject(o);}pDC->TextOutW(20,10,_T("虚线=原始 实线=变换"));break;}
	case 50:{pDC->TextOutW(20,10,_T("手绘直线:拖动鼠标"));if(m_pt1!=CPoint(0,0)&&m_pt2!=CPoint(0,0)){CPen pen(PS_SOLID,2,m_lineColor);CPen* o=pDC->SelectObject(&pen);pDC->MoveTo(m_pt1);pDC->LineTo(m_pt2);pDC->SelectObject(o);}break;}
	case 51:{pDC->TextOutW(20,10,_T("手绘圆:圆心+半径"));if(m_pt1!=CPoint(0,0)&&m_pt2!=CPoint(0,0)){CPen pen(PS_SOLID,2,m_lineColor);CPen* o=pDC->SelectObject(&pen);int r=(int)sqrt((double)((m_pt2.x-m_pt1.x)*(m_pt2.x-m_pt1.x)+(m_pt2.y-m_pt1.y)*(m_pt2.y-m_pt1.y)));pDC->Ellipse(m_pt1.x-r,m_pt1.y-r,m_pt1.x+r,m_pt1.y+r);pDC->SelectObject(o);}break;}
	case 52:{BezierCurve(pDC,m_bez,m_nBez);pDC->TextOutW(20,10,_T("贝塞尔曲线"));break;}
	}
}

void CMFCApplication3View::OnDrawLine(){m_mode=1;m_filled=FALSE;Invalidate();}
void CMFCApplication3View::OnDrawRect(){m_mode=2;m_filled=FALSE;Invalidate();}
void CMFCApplication3View::OnDrawEllipse(){m_mode=3;m_filled=FALSE;Invalidate();}

void CMFCApplication3View::DDALine(CDC* pDC,int x1,int y1,int x2,int y2){float dx=(float)(x2-x1),dy=(float)(y2-y1);float steps=fabs(dx)>fabs(dy)?fabs(dx):fabs(dy);float xi=dx/steps,yi=dy/steps;float x=(float)x1,y=(float)y1;for(int i=0;i<=(int)steps;i++){pDC->SetPixel((int)(x+0.5f),(int)(y+0.5f),m_lineColor);x+=xi;y+=yi;}}
void CMFCApplication3View::MidLine(CDC* pDC,int x1,int y1,int x2,int y2){int a=y1-y2,b=x2-x1,d=2*a+b,d1=2*a,d2=2*(a+b),x=x1,y=y1;pDC->SetPixel(x,y,m_lineColor);while(x<x2){if(d<0){x++;y++;d+=d2;}else{x++;d+=d1;}pDC->SetPixel(x,y,m_lineColor);}}
void CMFCApplication3View::BresLine(CDC* pDC,int x1,int y1,int x2,int y2){int dx=x2-x1,dy=y2-y1,p=2*dy-dx,x=x1,y=y1;pDC->SetPixel(x,y,m_lineColor);while(x<x2){x++;if(p<0)p+=2*dy;else{y++;p+=2*(dy-dx);}pDC->SetPixel(x,y,m_lineColor);}}
void CMFCApplication3View::OnDDA(){m_mode=10;m_filled=FALSE;Invalidate();}
void CMFCApplication3View::OnMidpoint(){m_mode=11;m_filled=FALSE;Invalidate();}
void CMFCApplication3View::OnBresenham(){m_mode=12;m_filled=FALSE;Invalidate();}

void CMFCApplication3View::MidCircle(CDC* pDC,int cx,int cy,int r){int x=0,y=r,d=1-r;while(x<=y){pDC->SetPixel(cx+x,cy+y,m_lineColor);pDC->SetPixel(cx-x,cy+y,m_lineColor);pDC->SetPixel(cx+x,cy-y,m_lineColor);pDC->SetPixel(cx-x,cy-y,m_lineColor);pDC->SetPixel(cx+y,cy+x,m_lineColor);pDC->SetPixel(cx-y,cy+x,m_lineColor);pDC->SetPixel(cx+y,cy-x,m_lineColor);pDC->SetPixel(cx-y,cy-x,m_lineColor);if(d<0)d+=2*x+3;else{d+=2*(x-y)+5;y--;}x++;}}
void CMFCApplication3View::OnCircle(){m_mode=20;m_filled=FALSE;Invalidate();}

void CMFCApplication3View::DrawPoly(CDC* pDC,CPoint* pts,int n,COLORREF c){CPen pen(PS_SOLID,3,c);CPen* o=pDC->SelectObject(&pen);pDC->MoveTo(pts[0]);for(int i=1;i<n;i++)pDC->LineTo(pts[i]);pDC->LineTo(pts[0]);pDC->SelectObject(o);}

void CMFCApplication3View::ScanFill(CDC* pDC,CPoint seed){std::stack<CPoint> stk;stk.push(seed);while(!stk.empty()){CPoint pt=stk.top();stk.pop();int y=pt.y,x=pt.x;COLORREF cur=pDC->GetPixel(x,y);if(cur==m_boundColor||cur==m_fillColor)continue;int xl=x;while(pDC->GetPixel(xl,y)!=m_boundColor){pDC->SetPixel(xl,y,m_fillColor);xl--;}xl++;int xr=x+1;while(pDC->GetPixel(xr,y)!=m_boundColor){pDC->SetPixel(xr,y,m_fillColor);xr++;}xr--;for(int ny=y-1;ny<=y+1;ny+=2){int sx=xl;while(sx<=xr){while(sx<=xr){COLORREF c=pDC->GetPixel(sx,ny);if(c!=m_boundColor&&c!=m_fillColor)break;sx++;}if(sx>xr)break;int rs=sx;while(sx<=xr){COLORREF c=pDC->GetPixel(sx,ny);if(c==m_boundColor||c==m_fillColor)break;rs=sx;sx++;}stk.push(CPoint(rs,ny));}}}}
void CMFCApplication3View::EdgeFill(CDC* pDC,CPoint seed,COLORREF bc,COLORREF fc){std::stack<CPoint> stk;stk.push(seed);while(!stk.empty()){CPoint pt=stk.top();stk.pop();COLORREF c=pDC->GetPixel(pt.x,pt.y);if(c==bc||c==fc)continue;pDC->SetPixel(pt.x,pt.y,fc);stk.push(CPoint(pt.x-1,pt.y));stk.push(CPoint(pt.x,pt.y-1));stk.push(CPoint(pt.x+1,pt.y));stk.push(CPoint(pt.x,pt.y+1));}}
BOOL CMFCApplication3View::InsidePoly(CPoint* pts,int n,CPoint p){int i,j,c=0;for(i=0,j=n-1;i<n;j=i++){if(((pts[i].y>p.y)!=(pts[j].y>p.y))&&(p.x<(pts[j].x-pts[i].x)*(p.y-pts[i].y)/(pts[j].y-pts[i].y)+pts[i].x))c=!c;}return c;}
void CMFCApplication3View::PolyFill(CDC* pDC,CPoint seed){CRect rc;GetClientRect(&rc);if(seed.x<=rc.left||seed.x>=rc.right||seed.y<=rc.top||seed.y>=rc.bottom){AfxMessageBox(_T("种子不在图形之内!"));return;}if(!InsidePoly(m_poly,m_nPoly,seed)){AfxMessageBox(_T("种子不在图形之内!"));return;}std::stack<CPoint> stk;stk.push(seed);while(!stk.empty()){CPoint pt=stk.top();stk.pop();COLORREF c=pDC->GetPixel(pt.x,pt.y);if(c==m_boundColor||c==m_fillColor)continue;pDC->SetPixel(pt.x,pt.y,m_fillColor);stk.push(CPoint(pt.x-1,pt.y));stk.push(CPoint(pt.x,pt.y-1));stk.push(CPoint(pt.x+1,pt.y));stk.push(CPoint(pt.x,pt.y+1));}}
void CMFCApplication3View::OnFillScan(){m_mode=30;m_filled=FALSE;Invalidate();}
void CMFCApplication3View::OnFillEdge(){m_mode=31;m_filled=FALSE;Invalidate();}
void CMFCApplication3View::OnFillPoly(){m_mode=32;m_filled=FALSE;Invalidate();}

void CMFCApplication3View::OnLButtonDown(UINT nFlags,CPoint point){if(m_mode>=30&&m_mode<=32){CDC* pDC=GetDC();if(m_mode==30)ScanFill(pDC,point);else if(m_mode==31)EdgeFill(pDC,point,m_boundColor,m_fillColor);else PolyFill(pDC,point);ReleaseDC(pDC);m_filled=TRUE;Invalidate();}else if(m_mode==50||m_mode==51){m_pt1=point;m_pt2=point;m_drawing=TRUE;SetCapture();}else if(m_mode==52){if(m_nBez<4){m_bez[m_nBez++]=point;Invalidate();}}CView::OnLButtonDown(nFlags,point);}
void CMFCApplication3View::OnMouseMove(UINT nFlags,CPoint point){if(m_drawing&&(m_mode==50||m_mode==51)){m_pt2=point;Invalidate();}CView::OnMouseMove(nFlags,point);}
void CMFCApplication3View::OnLButtonUp(UINT nFlags,CPoint point){if(m_drawing){m_pt2=point;m_drawing=FALSE;ReleaseCapture();Invalidate();}CView::OnLButtonUp(nFlags,point);}

CPoint CMFCApplication3View::Centroid(CPoint* pts,int n){int cx=0,cy=0;for(int i=0;i<n;i++){cx+=pts[i].x;cy+=pts[i].y;}return CPoint(cx/n,cy/n);}
void CMFCApplication3View::ApplyMat(double m[3][3]){for(int i=0;i<m_nVert;i++){double x=(double)m_cur[i].x,y=(double)m_cur[i].y;m_cur[i].x=(LONG)(x*m[0][0]+y*m[1][0]+m[2][0]+0.5);m_cur[i].y=(LONG)(x*m[0][1]+y*m[1][1]+m[2][1]+0.5);}}
void CMFCApplication3View::OnTransTranslate(){CTransDlg d;if(d.DoModal()==IDOK){double m[3][3]={{1,0,0},{0,1,0},{(double)d.m_dx,(double)d.m_dy,1}};ApplyMat(m);m_trans=TRUE;m_mode=40;Invalidate();}}
void CMFCApplication3View::OnTransScale(){CScaleDlg2 d;if(d.DoModal()==IDOK){double sx=d.m_sx/100.0,sy=d.m_sy/100.0;CPoint c=Centroid(m_cur,m_nVert);double to[3][3]={{1,0,0},{0,1,0},{(double)-c.x,(double)-c.y,1}};double sc[3][3]={{sx,0,0},{0,sy,0},{0,0,1}};double bk[3][3]={{1,0,0},{0,1,0},{(double)c.x,(double)c.y,1}};ApplyMat(to);ApplyMat(sc);ApplyMat(bk);m_trans=TRUE;m_mode=41;Invalidate();}}
void CMFCApplication3View::OnTransRotate(){CRotDlg2 d;if(d.DoModal()==IDOK){double rad=d.m_ang*M_PI/180.0,c2=cos(rad),s=sin(rad);CPoint ct=Centroid(m_cur,m_nVert);double to[3][3]={{1,0,0},{0,1,0},{(double)-ct.x,(double)-ct.y,1}};double rt[3][3]={{c2,s,0},{-s,c2,0},{0,0,1}};double bk[3][3]={{1,0,0},{0,1,0},{(double)ct.x,(double)ct.y,1}};ApplyMat(to);ApplyMat(rt);ApplyMat(bk);m_trans=TRUE;m_mode=42;Invalidate();}}
void CMFCApplication3View::OnTransReset(){for(int i=0;i<m_nVert;i++)m_cur[i]=m_orig[i];m_trans=FALSE;m_mode=0;Invalidate();}
void CMFCApplication3View::OnFreeLine(){m_mode=50;m_pt1=m_pt2=CPoint(0,0);m_filled=FALSE;Invalidate();}
void CMFCApplication3View::OnFreeCircle(){m_mode=51;m_pt1=m_pt2=CPoint(0,0);m_filled=FALSE;Invalidate();}
void CMFCApplication3View::BezierCurve(CDC* pDC,CPoint* pts,int n){CPen pen(PS_SOLID,1,RGB(0,0,255));CPen* o=pDC->SelectObject(&pen);for(int i=0;i<n-1;i++){pDC->MoveTo(pts[i]);pDC->LineTo(pts[i+1]);}CPen pen2(PS_SOLID,3,m_lineColor);pDC->SelectObject(&pen2);for(double t=0;t<=1.0;t+=0.001){double x=pow(1-t,3)*pts[0].x+3*t*pow(1-t,2)*pts[1].x+3*t*t*(1-t)*pts[2].x+t*t*t*pts[3].x;double y=pow(1-t,3)*pts[0].y+3*t*pow(1-t,2)*pts[1].y+3*t*t*(1-t)*pts[2].y+t*t*t*pts[3].y;pDC->SetPixel((int)(x+0.5),(int)(y+0.5),m_lineColor);}pDC->SelectObject(o);}
void CMFCApplication3View::OnBezier(){m_mode=52;m_nBez=0;m_filled=FALSE;Invalidate();}
void CMFCApplication3View::OnColorLine(){CColorDialog d(m_lineColor);if(d.DoModal()==IDOK){m_lineColor=d.GetColor();Invalidate();}}
void CMFCApplication3View::OnColorFill(){CColorDialog d(m_fillColor);if(d.DoModal()==IDOK){m_fillColor=d.GetColor();Invalidate();}}

BOOL CMFCApplication3View::OnPreparePrinting(CPrintInfo* pI){return DoPreparePrinting(pI);}
void CMFCApplication3View::OnBeginPrinting(CDC*,CPrintInfo*){}
void CMFCApplication3View::OnEndPrinting(CDC*,CPrintInfo*){}
#ifdef _DEBUG
void CMFCApplication3View::AssertValid()const{CView::AssertValid();}
void CMFCApplication3View::Dump(CDumpContext& dc)const{CView::Dump(dc);}
CMFCApplication3Doc* CMFCApplication3View::GetDocument()const{ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMFCApplication3Doc)));return(CMFCApplication3Doc*)m_pDocument;}
#endif
