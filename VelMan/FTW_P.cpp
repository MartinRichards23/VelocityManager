// FTW_P.cpp : implementation file
//

#include "stdafx.h"
#include "velman.h"
#include "VelmanDoc.h"
#include "finetune.h"
#include "ft_sheet.h"
#include "FTW_P.h"
#include "FTF_P.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//static variables to remember value choices
double CFinetuneWeight_Page::sx = 0.0;
double CFinetuneWeight_Page::sy = 0.0;
double CFinetuneWeight_Page::sr = 0.0;

/////////////////////////////////////////////////////////////////////////////
// CFinetuneWeight_Page property page

IMPLEMENT_DYNCREATE(CFinetuneWeight_Page, CPropertyPage)

CFinetuneWeight_Page::CFinetuneWeight_Page(CFinetuneSheet *mysheet) : CPropertyPage(CFinetuneWeight_Page::IDD)
{
	//{{AFX_DATA_INIT(CFinetuneWeight_Page)
	m_coi_rad = 0.0;
	m_coi_x = 0.0;
	m_coi_y = 0.0;
	//}}AFX_DATA_INIT
	sheet=mysheet;
}

CFinetuneWeight_Page::CFinetuneWeight_Page()
{
}

CFinetuneWeight_Page::~CFinetuneWeight_Page()
{
}

void CFinetuneWeight_Page::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFinetuneWeight_Page)
	DDX_Text(pDX, IDC_CHGWEIGHT_RAD, m_coi_rad);
	DDX_Text(pDX, IDC_CHGWEIGHT_X, m_coi_x);
	DDX_Text(pDX, IDC_CHGWEIGHT_Y, m_coi_y);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CFinetuneWeight_Page, CPropertyPage)
	//{{AFX_MSG_MAP(CFinetuneWeight_Page)
	ON_BN_CLICKED(IDC_FINETUNE_HIDEME, OnFinetuneHideme)
	ON_WM_HSCROLL()
	ON_WM_PAINT()
	ON_EN_CHANGE(IDC_CHGWEIGHT_RAD, OnChangeChgweightRad)
	ON_BN_CLICKED(IDC_CHGWEIGHT_DEFAULTS, OnChgweightDefaults)
	ON_CBN_SELCHANGE(IDC_CHGWEIGHT_METHOD2, OnSelchangeChgweightMethod2)
	ON_BN_CLICKED(IDC_FINETUNE_SWITCHVIEWMODE, OnFinetuneSwitchviewmode)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFinetuneWeight_Page message handlers

void CFinetuneWeight_Page::OnCancel()
{
	sheet->OnCancel();
}

void CFinetuneWeight_Page::OnOK()
{
	// this OK button only accepts the data and switches to another page
	UpdateData();

	// Remember chosen values
	CFinetuneWeight_Page::sx = m_coi_x;
	CFinetuneWeight_Page::sy = m_coi_y;
	CFinetuneWeight_Page::sr = m_coi_rad;

	if(m_coi_x==0 && m_coi_y==0)
		return; // quit seamlessly if nothing entered

	horizon->WeightCenterX=m_coi_x;
	horizon->WeightCenterY=m_coi_y;
	horizon->WeightCenterRad=m_coi_rad;
	CComboBox *pCB=(CComboBox *)GetDlgItem(IDC_CHGWEIGHT_METHOD2);
	m_method=pCB->GetCurSel();
	m_downto=0.01*downto_display;
	horizon->WeightDecreaseFunc=m_method;

	char buf[128];
	sprintf(buf, "Changed weights in layer %s in a %s decreasing manner:",
		(const char *)horizon->name, m_method==0 ? "linearly" : "quadratically");
	pDoc->Log(CString(buf), LOG_STD);
	sprintf(buf, " Region of interest: Center=(%f,%f), radius=%f", m_coi_x,
		m_coi_y, m_coi_rad);
	pDoc->Log(CString(buf), LOG_STD);
	sprintf(buf, " Weights are reduced from 1 down to %4.2f", m_downto);
	pDoc->Log(CString(buf), LOG_STD);
	horizon->SetHorizonWeights(WEIGHTMODEL_DECREASE, m_method,
		m_coi_x, m_coi_y, m_coi_rad, m_downto, pDoc);

	AfxGetApp()->WriteProfileInt("Well models", "WeightDecreaseFunc", m_method);
	// force update view (will also compute new model in the course of OnDraw() )
	pView->Invalidate();
	pDoc->horizonlist.FitLayer(layer, pDoc);
	pDoc->UpdateAllViews(NULL, DOC_CHANGE_FINETUNECHANGE);
	((CFinetuneFitting_Page *)(sheet->GetPage(0)))->ShowFittingParams();
}

void CFinetuneWeight_Page::OnFinetuneHideme()
{
	sheet->Hideme();
}

BOOL CFinetuneWeight_Page::OnInitDialog()
{
	pView=pDoc->pFTview;
	horizon=(CHorizon *)((pDoc->horizonlist).GetAt(layer));

	/*m_coi_x=horizon->WeightCenterX;
	m_coi_y=horizon->WeightCenterY;
	m_coi_rad=horizon->WeightCenterRad;*/

	//set values to choices we have remembered.
	m_coi_x = CFinetuneWeight_Page::sx;
	m_coi_y = CFinetuneWeight_Page::sy;
	m_coi_rad = CFinetuneWeight_Page::sr;

	if(m_coi_x == 0 && m_coi_y == 0)
	{
		// no values set already so use the centre of the grid.

		double *xlimits, *ylimits;
		double xRange, yRange, x, y;
		CZimsGrid *grid;
		grid=(CZimsGrid *)pDoc->zimscube.GetAt(0);

		xlimits = grid->GetXlimits();
		ylimits = grid->GetYlimits();

		xRange = xlimits[1] - xlimits[0];
		yRange = ylimits[1] - ylimits[0];

		x = xlimits[0] + (xRange/2);
		y = ylimits[0] + (yRange/2);

		m_coi_x =x;
		m_coi_y = y;
	}

	radius=m_coi_rad;

	CPropertyPage::OnInitDialog();
	SetDlgItemText(IDC_FINETUNE_SWITCHVIEWMODE, "Contour &view");

	CComboBox *pCB=(CComboBox *)GetDlgItem(IDC_CHGWEIGHT_METHOD2);
	// either use weight decrease function that is stored in horizon, of if there
	// is none (it's initialized with -1), read it from config file
	if(horizon->WeightDecreaseFunc==-1)
		m_method=AfxGetApp()->GetProfileInt("Well models", "WeightDecreaseFunc", 1);
	else
		m_method=horizon->WeightDecreaseFunc;
	pCB->SetCurSel(m_method);
	// take control of scrollbar & display initial value next to it
	CScrollBar *pSB=(CScrollBar *)GetDlgItem(IDC_CHGWGT_DOWNTOSCROLL);
	pSB->SetScrollRange(0, 99);
	pSB->SetScrollPos(10);
	downto_display=10;
	SetDlgItemText(IDC_CHGWGT_DOWNTONUMBER, "0.10");
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CFinetuneWeight_Page::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	int pos=pScrollBar->GetScrollPos();
	int oldpos=pos;
	BOOL UpdateView=TRUE;

	switch(nSBCode)
	{
	case SB_THUMBPOSITION:
		pScrollBar->SetScrollPos(nPos);
		oldpos=200;	// for some strange reason we have to reinforce the redraw
		break;
	case SB_THUMBTRACK:
		pScrollBar->SetScrollPos(nPos);
		UpdateView=FALSE;		// no updating while scrollbar is being moved (too slow)
		break;
	case SB_LINEUP:
		pScrollBar->SetScrollPos(max(pos-1, 0));
		break;
	case SB_LINEDOWN:
		pScrollBar->SetScrollPos(min(pos+1, 99));
		break;
	case SB_PAGELEFT:
		pScrollBar->SetScrollPos(max(pos-5, 0));
		break;
	case SB_PAGERIGHT:
		pScrollBar->SetScrollPos(min(pos+5, 99));
		break;
	}

	// update text in dialog box itself
	pos=pScrollBar->GetScrollPos();
	char buf[15];
	sprintf(buf, "%4.2f", 0.01*pos);
	SetDlgItemText(IDC_CHGWGT_DOWNTONUMBER, buf);
	downto_display=pos;

	if(UpdateView)  // do not always redraw to save time
		Invalidate();
}

void CFinetuneWeight_Page::OnPaint()
{
	CPaintDC(this);
	CWnd *pWnd=GetDlgItem(IDC_CHGWGT_PREVIEW);
	CDC *pDC=pWnd->GetDC();

	// button texts
	int mode=(pDoc->pFTview)->ViewMode;
	CButton *pB=(CButton *)GetDlgItem(IDC_FINETUNE_SWITCHVIEWMODE);
	if(mode==VIEWMODE_SCATTER)
		pB->SetWindowText("Contour &view");
	else
		pB->SetWindowText("Scatter &view");

	// obtain radius
	char buf[64];
	GetDlgItemText(IDC_CHGWEIGHT_RAD, buf, 64);
	radius=atof(buf);

	// set up drawing
	pWnd->Invalidate();
	pWnd->UpdateWindow();
	CRect clientRect;
	pWnd->GetClientRect(clientRect);
	pDC->SetMapMode(MM_ANISOTROPIC);
	pDC->SetWindowExt(500, 600);
	pDC->SetViewportExt(clientRect.right, clientRect.bottom);
	pDC->SetViewportOrg(0, 0);

#ifdef VELMAN_UNIX
	// draw a white background (is gray in UNIX otherwise)
	CBrush WhiteBrush(RGB(255, 255, 255));
	CRect DrawingArea(0, 0, 500, 600);
	pDC->FillRect(&DrawingArea, &WhiteBrush);
#endif

	// draw coordinate system
	// arrow of y axis
	pDC->MoveTo(40, 67);
	pDC->LineTo(50, 50);
	pDC->LineTo(60, 67);
	// y axis
	pDC->MoveTo(50, 50);
	pDC->LineTo(50, 450);
	// x axis
	pDC->LineTo(450, 450);
	// arrow of x axis
	pDC->LineTo(435, 433);
	pDC->MoveTo(450, 450);
	pDC->LineTo(435, 467);

	// label for axis
	CFont font;
	font.CreateFont(-72, 0, 0, 0, 400, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Arial");
	CFont *pOldFont=(CFont *)pDC->SelectObject(&font);
	pDC->TextOut(420, 480, "dist");
	pDC->TextOut(10, 50, "1");
	pDC->TextOut(10, 420, "0");
	pDC->TextOut(40, 470, "0");

	// draw function
	// choose red pen
	CPen redpen(PS_SOLID, 1, RGB(255, 0, 0));
	CPen dotpen(PS_DOT, 1, RGB(0, 0, 0));
	CPen *pOldPen=pDC->SelectObject(&redpen);

	// draw weight=1 region, if any
	int funcwidth, startx;
	if(radius>0.0)
	{
		funcwidth=300;
		startx=120;
		pDC->TextOut(startx-15, 470, "R");
		pDC->MoveTo(50, 100);
		pDC->LineTo(startx, 100);
		pDC->SelectObject(&dotpen);
		pDC->LineTo(startx, 450);
		pDC->SelectObject(&redpen);
	}
	else
	{
		funcwidth=420;
		startx=50;
	}
	// sketch graph of function itslef
	int end_y=100+((100-downto_display)*7)/2;
	if(m_method==0)
	{
		pDC->MoveTo(startx, 100);
		pDC->LineTo(450, end_y);
	}
	else
	{
		double a=(downto_display/100.0-1)/((450-startx)*(450-startx));
		double b=-2*a*startx;
		double c=1+a*startx*startx;
		for(int x=startx;x<450;x+=3)
			pDC->SetPixel(x, 100+(int)(350*(1-(a*x*x+b*x+c))), RGB(255, 0, 0));
	}

	pDC->SelectObject(pOldFont);
	font.DeleteObject();
	pDC->SelectObject(pOldPen);
	pWnd->ReleaseDC(pDC);
}

void CFinetuneWeight_Page::OnChangeChgweightRad()
{
	double newradius;
	char buf[64];
	GetDlgItemText(IDC_CHGWEIGHT_RAD, buf, 64);
	newradius=atof(buf);
	// only redraw preview if radius changed from or to zero
	if((newradius!=0 && radius==0) || (newradius==0 && radius!=0))
		Invalidate();
}

void CFinetuneWeight_Page::OnSelchangeChgweightMethod2()
{
	m_method=((CComboBox *)GetDlgItem(IDC_CHGWEIGHT_METHOD2))->GetCurSel();
	Invalidate();
}

void CFinetuneWeight_Page::OnChgweightDefaults()
{
	pDoc->Log("Reset all weights of layer "+horizon->name+" to 1.0", LOG_STD);
	horizon->SetHorizonWeights(WEIGHTMODEL_DEFAULTS);
	// force update view (will also compute new model in the course of OnDraw() )
	pDoc->horizonlist.FitLayer(layer, pDoc);
	pView->Invalidate();
	pDoc->UpdateAllViews(NULL, DOC_CHANGE_FINETUNECHANGE);
	((CFinetuneFitting_Page *)(sheet->GetPage(0)))->ShowFittingParams();
}

void CFinetuneWeight_Page::OnFinetuneSwitchviewmode()
{
	// flip viewmode
	(pDoc->pFTview)->ViewMode=3-(pDoc->pFTview)->ViewMode;
	Invalidate();
	(pDoc->pFTview)->ZoomDepth=0;
	(pDoc->pFTview)->Invalidate();	// enforce redraw
}
