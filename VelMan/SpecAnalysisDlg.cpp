#include "stdafx.h"
#include "velman.h"
#include "VelmanDoc.h"
#include "SpecAnalysisDlg.h"
#include "mainfrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSpecAnalysisDlg dialog

CSpecAnalysisDlg::CSpecAnalysisDlg(CWnd* pParent /*=NULL*/)
: CDialog(CSpecAnalysisDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSpecAnalysisDlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	initialised=FALSE;
}

void CSpecAnalysisDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_PRIMARY, SetAsPrimary);
	DDX_Text(pDX, IDC_EDITFROM, userFrom);
	DDX_Text(pDX, IDC_EDITTO, userTo);
	//{{AFX_DATA_MAP(CSpecAnalysisDlg)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSpecAnalysisDlg, CDialog)
	//{{AFX_MSG_MAP(CSpecAnalysisDlg)
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1, &CSpecAnalysisDlg::OnBnClickedButton1)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSpecAnalysisDlg message handlers

BOOL CSpecAnalysisDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	int i, j;

	SetAsPrimary = grid->GetID()==0 && grid->GetHorizon()>=0;
	CheckDlgButton(IDC_PRIMARY, SetAsPrimary);

	CheckDlgButton(IDC_SPECANAL_REBUILD, 1);
	gmax=grid->CalcMaxVal();
	gmin=grid->CalcMinVal();

	slotfill=new int[NUM_SLOTS];	// last slot counts the ZNON values
	for(i=0;i<NUM_SLOTS;i++)
		slotfill[i]=0;

	slotsize=(gmax-gmin)/(NUM_SLOTS-1);
	if(slotsize==0.0)
		slotsize=1.0;	// also handle complete constant grids somehow...
	double x;

	double **data=grid->GetGrid();
	grid->Stamp();

	totalpts=0;
	for(i=0;i<grid->GetRows();i++)
	{
		for(j=0;j<grid->GetColumns();j++)
		{
			if(!(grid->GetZnon(data[i][j])))
			{
				x=(data[i][j]-gmin)/slotsize;
				slotfill[(int)x]++;
				totalpts++;
			}
		}
	}
	// select all slots
	first=0;
	last=NUM_SLOTS-1;
	// find highest and second highest filled slot
	maxfill=-1;
	secondfill=-1;
	for(i=0;i<NUM_SLOTS;i++)
	{
		if(slotfill[i]>maxfill)
		{
			secondfill=maxfill;
			maxfill=slotfill[i];
		}
	}

	// if highest slot is overly full, use 2nd highest one as guidancne on how to scale
	// (==> have highest slot cut off)
	// do not do so if there is only one slot filled anyway, i.e. if secondfill==-1 still
	if(secondfill>-1 && maxfill>5*secondfill)
	{
		// maxfill is suspiciously high
		maxfill=secondfill;
		// round up maxfill to nearest tenner
		if(maxfill % 10)
			maxfill=10*(maxfill/10+1);
		maxfill+=10;	// and add another ten just so
	}
	else
	{
		// round up maxfill to nearest tenner
		if(maxfill % 10)
			maxfill=10*(maxfill/10+1);
	}
	if(totalpts==0)
		totalpts=1;	// so we do not divide by zero

	SetDlgItemText(IDC_SPECANAL_NUMEXCL, "0");
	CString s;
	s="Spectral analysis of grid '"+grid->GetName()+"'";
	SetWindowText((const char *)s);
	PrintRange(gmin, gmax);

	haveCaptured=0;  // 1=capture in top region, 2=bottom region (two sliders)
	BlockRect.SetRect(0, 0, 4, 20);

	SetIcon (AfxGetApp()->LoadIcon(IDR_MAINFRAME), FALSE);
	// this  is the first time that run so want to measure the distance constants
	RECT form;
	RECT component;
	this->GetWindowRect(&form);
	formWidth=form.right-form.left;
	// code to initialise variables

	//LR
	GetDlgItem (IDOK)->GetWindowRect(&component);
	border_LR_IDOK=form.right-component.left;

	//LR
	GetDlgItem (IDCANCEL)->GetWindowRect(&component);
	border_LR_IDCANCEL=form.right-component.left;

	//RR
	GetDlgItem (IDC_SPECANAL_GRAPH)->GetWindowRect(&component);
	border_RR_IDC_SPECANAL_GRAPH=form.right-component.right;

	//BB
	GetDlgItem (IDC_SPECANAL_GRAPH)->GetWindowRect(&component);
	border_BB_IDC_SPECANAL_GRAPH=form.bottom-component.bottom;

	//RR
	GetDlgItem (IDC_SPECANAL_NUMEXCL)->GetWindowRect(&component);
	border_RR_IDC_SPECANAL_NUMEXCL=form.right-component.right;

	//TB
	GetDlgItem (IDC_SPECANAL_NUMEXCL)->GetWindowRect(&component);
	border_TB_IDC_SPECANAL_NUMEXCL=form.bottom - component.top;

	//TB
	GetDlgItem (IDC_STATIC1)->GetWindowRect(&component);
	border_TB_IDC_STATIC1=form.bottom - component.top;

	//TB
	GetDlgItem (IDC_STATIC2)->GetWindowRect(&component);
	border_TB_IDC_STATIC2=form.bottom - component.top;

	//TB
	GetDlgItem (IDC_SPECANAL_NULL)->GetWindowRect(&component);
	border_TB_IDC_RADIO2=form.bottom - component.top;

	//TB
	GetDlgItem (IDC_SPECANAL_REBUILD)->GetWindowRect(&component);
	border_TB_IDC_SPECANAL_REBUILD=form.bottom - component.top;

	// cleanup code
	initialised=TRUE;

	return TRUE;
}

void CSpecAnalysisDlg::OnBnClickedButton1()
{
	UpdateData();

	//check input value is within bounds
	if(userFrom > gmax)
		userFrom = gmax;
	else if(userFrom < gmin)
		userFrom = gmin;

	if(userTo > gmax)
		userTo = gmax;
	else if(userTo < gmin)
		userTo = gmin;

	//calculate first and last slots
	first = int((userFrom - gmin) / slotsize);
	last = int((userTo - gmin) / slotsize);

	PrintRange(userFrom, userTo);

	OnPaint();
}

void CSpecAnalysisDlg::PrintRange(double from, double to)
{
	// prints range of numbers that is selected into dialog box, together with
	// units
	char buf[64];
	int i, excl;

	//set textbox values
	sprintf(buf, "%6.1f", from);
	SetDlgItemText(IDC_EDITFROM, buf);
	sprintf(buf, "%6.1f", to);
	SetDlgItemText(IDC_EDITTO, buf);

	//set units
	sprintf(buf, grid->GetUnitName());
	SetDlgItemText(IDC_STATIC8, buf);
	SetDlgItemText(IDC_STATIC9, buf);

	SetDlgItemText(IDC_SPECANAL_INCL, buf);
	for(i=first, excl=0;i<=last;i++)
		excl+=slotfill[i];
	excl=totalpts-excl;
	sprintf(buf, "%d = %d%%", excl, (100*excl)/totalpts);
	SetDlgItemText(IDC_SPECANAL_NUMEXCL, buf);
}

void CSpecAnalysisDlg::OnPaint()
{
	CPaintDC(this);
	CWnd *pWnd=GetDlgItem(IDC_SPECANAL_GRAPH);
	CDC *pDC=pWnd->GetDC();
	CRect clientRect;
	CPoint pt[3];

	CFont axisfont;
	axisfont.CreateFont(70, 0, 0, 0, 400, FALSE, FALSE, 0, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_SWISS, "Arial");
	CFont *pOldFont=pDC->SelectObject(&axisfont);
	int i, hgt;

	pWnd->GetClientRect(clientRect);
	pDC->SetMapMode(MM_ANISOTROPIC);
	pDC->SetWindowExt(NUM_SLOTS+40, 1080);
	pDC->SetViewportExt(clientRect.right, clientRect.bottom);

	if(haveCaptured)
	{
		if(haveCaptured==1)
		{
			pt[0]=BlockRect.TopLeft();
			pt[1]=pt[0]+CSize(0, 80);
			pt[2]=pt[0]+CSize(10, 40);
			pDC->FillSolidRect(CRect(0, 0, NUM_SLOTS, 85), RGB(255, 255, 255));
		}
		if(haveCaptured==2)
		{
			pt[0]=BlockRect.BottomRight();
			pt[1]=pt[0]+CSize(0,-80);
			pt[2]=pt[0]+CSize(-10,-40);
			pDC->FillSolidRect(0, 1000, NUM_SLOTS, 85, RGB(255, 255, 255));
		}
		pDC->SelectStockObject(BLACK_BRUSH);
		pDC->Polygon(pt, 3);
		pWnd->ReleaseDC(pDC);
		return;
	}

	pWnd->Invalidate();
	pWnd->UpdateWindow();	 // prevent Windows from overwriting our work later

	pt[0]=CPoint(first, 0);
	pt[1]=pt[0]+CSize(0, 80);
	pt[2]=pt[0]+CSize(10, 40);
	pDC->FillSolidRect(CRect(0, 0, NUM_SLOTS, 80), RGB(255, 255, 255));
	pDC->SelectStockObject(BLACK_BRUSH);
	pDC->Polygon(pt, 3);
	pt[0]=CPoint(last, 1080);//-CSize(1, 1);
	pt[1]=pt[0]+CSize(0,-80);
	pt[2]=pt[0]+CSize(-10,-40);
	pDC->FillSolidRect(0, 1000, NUM_SLOTS, 90, RGB(255, 255, 255));
	pDC->Polygon(pt, 3);

	pDC->FillSolidRect(0, 80, NUM_SLOTS, 920, RGB(255, 255, 255));	// histogram area white
	pDC->SetBkColor(RGB(192, 192, 192));	// lt gray background elsewhere
	// draw y axis
	int nticks;
	char buf[25];

	nticks=maxfill/10;
	if(nticks>10)
		nticks=10;
	if(nticks<1)
		nticks=1;

	pDC->SetTextAlign(TA_LEFT | TA_BOTTOM);
	pDC->SelectStockObject(LTGRAY_BRUSH);
	for(i=0;i<=nticks;i++)
	{
		pDC->MoveTo(NUM_SLOTS, 970-(i*820)/nticks);
		pDC->LineTo(NUM_SLOTS+15, 970-(i*820)/nticks);
		sprintf(buf, "%d", (i*maxfill)/nticks);
		pDC->TextOut(NUM_SLOTS+1, 970-(i*820)/nticks, buf, strlen(buf));
	}

	// draw histogram
	for(i=0;i<NUM_SLOTS;i++)
	{
		if(slotfill[i]>maxfill)
			hgt=820;
		else
			hgt=(820*slotfill[i])/maxfill;
		pDC->FillSolidRect(i, 970-hgt, 1, hgt, (  (i < first || i>last) ? RGB(255, 0, 0) :
			RGB(22, 205, 233)));
	}

	pDC->SelectObject(pOldFont);
	axisfont.DeleteObject();
	pWnd->ReleaseDC(pDC);
}

void CSpecAnalysisDlg::OnDestroy()
{
	delete slotfill;
	CDialog::OnDestroy();
}

void CSpecAnalysisDlg::OnOK()
{
	DoRebuild= IsDlgButtonChecked(IDC_SPECANAL_REBUILD);
	UseMinMax = IsDlgButtonChecked(IDC_SPECANAL_MINMAX);

	CDialog::OnOK();
}

void CSpecAnalysisDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// map mouse coordinate into within the histogram window
	POINT orgpt;
	orgpt.x=point.x;
	orgpt.y=point.y;
	MapWindowPoints(GetDlgItem(IDC_SPECANAL_GRAPH), &orgpt, 1);
	// now map into the NUM_SLOTS+25, 1000
	CPaintDC(this);
	CWnd *pWnd=GetDlgItem(IDC_SPECANAL_GRAPH);
	CDC *pDC=pWnd->GetDC();
	CRect clientRect;
	pWnd->GetClientRect(clientRect);
	pDC->SetMapMode(MM_ANISOTROPIC);
	pDC->SetWindowExt(NUM_SLOTS+25, 1080);
	pDC->SetViewportExt(clientRect.right, clientRect.bottom);
	pDC->DPtoLP(&orgpt);
	if(orgpt.y<80)
		haveCaptured=1;
	if(orgpt.y>1000)
		haveCaptured=2;

	pWnd->ReleaseDC(pDC);

	if(haveCaptured)
		SetCapture();
	MousePos=point;
}

void CSpecAnalysisDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	if(haveCaptured)
	{
		ReleaseCapture();
		haveCaptured=0;
		Invalidate();
		UpdateWindow();
	}
}

void CSpecAnalysisDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	if(!haveCaptured)
		return;

	// map mouse coordinate into within the histogram window
	POINT orgpt;
	orgpt.x=point.x;
	orgpt.y=point.y;
	MapWindowPoints(GetDlgItem(IDC_SPECANAL_GRAPH), &orgpt, 1);
	// now map into the NUM_SLOTS+25, 1000
	CPaintDC(this);
	CWnd *pWnd=GetDlgItem(IDC_SPECANAL_GRAPH);
	CDC *pDC=pWnd->GetDC();
	CRect clientRect;
	pWnd->UpdateWindow();	 // prevent Windows from overwriting our work later
	pWnd->GetClientRect(clientRect);
	pDC->SetMapMode(MM_ANISOTROPIC);
	pDC->SetWindowExt(NUM_SLOTS+25, 1080);
	pDC->SetViewportExt(clientRect.right, clientRect.bottom);
	pDC->DPtoLP(&orgpt);
	CRect OurRect;
	if(haveCaptured==1)
		OurRect.SetRect(0, 0, NUM_SLOTS-4, 50);
	else
		OurRect.SetRect(0, 970, NUM_SLOTS, 1080);
	if(OurRect.PtInRect(orgpt))
	{
		// make Windows coords out of OurRect and invalidate that bit
		InvalidateRect(CRect(1, 1, 2, 2), FALSE);
		MousePos=orgpt;
		if(haveCaptured==1)
		{
			first=MousePos.x;
			BlockRect.SetRect(MousePos.x, 0, MousePos.x+4, 80);
		}
		else
		{
			last=MousePos.x;
			BlockRect.SetRect(MousePos.x, 1000, MousePos.x+4, 1080);
		}
		PrintRange(gmin+slotsize*first, gmin+slotsize*last);
	}
	pWnd->ReleaseDC(pDC);
}

void CSpecAnalysisDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if (initialised)
	{
		RECT form;
		RECT component;

		int top, left, width, height;

		this->GetWindowRect(&form);
		// reposition/ size the components

		//LR
		GetDlgItem(IDOK)->GetWindowRect(&component);
		left=form.right - form.left- border_LR_IDOK-4;
		top=component.top-form.top-CMainFrame::GetConfigTitleBarThickness();
		GetDlgItem(IDOK)->SetWindowPos(&wndTop, left, top, 0, 0, SWP_NOSIZE);

		//LR
		GetDlgItem(IDCANCEL)->GetWindowRect(&component);
		left=form.right - form.left- border_LR_IDCANCEL-4;
		top=component.top-form.top-CMainFrame::GetConfigTitleBarThickness();
		GetDlgItem(IDCANCEL)->SetWindowPos(&wndTop, left, top, 0, 0, SWP_NOSIZE);

		//RR
		GetDlgItem(IDC_SPECANAL_GRAPH)->GetWindowRect(&component);
		height=component.bottom - component.top;
		width=form.right-component.left-border_RR_IDC_SPECANAL_GRAPH;
		GetDlgItem(IDC_SPECANAL_GRAPH)->SetWindowPos(&wndTop, 0, 0, width, height, SWP_NOMOVE);

		//BB
		GetDlgItem(IDC_SPECANAL_GRAPH)->GetWindowRect(&component);
		height=form.bottom-border_BB_IDC_SPECANAL_GRAPH-component.top;
		width=component.right - component.left;
		GetDlgItem(IDC_SPECANAL_GRAPH)->SetWindowPos(&wndTop, 0, 0, width, height, SWP_NOMOVE);

		//RR
		GetDlgItem(IDC_SPECANAL_NUMEXCL)->GetWindowRect(&component);
		height=component.bottom - component.top;
		width=form.right-component.left-border_RR_IDC_SPECANAL_NUMEXCL;
		GetDlgItem(IDC_SPECANAL_NUMEXCL)->SetWindowPos(&wndTop, 0, 0, width, height, SWP_NOMOVE);

		//TB
		GetDlgItem (IDC_SPECANAL_NUMEXCL)->GetWindowRect(&component);
		left=component.left - form.left-4;
		top=form.bottom-border_TB_IDC_SPECANAL_NUMEXCL-form.top-CMainFrame::GetConfigTitleBarThickness();
		GetDlgItem(IDC_SPECANAL_NUMEXCL)->SetWindowPos(&wndTop, left, top, 0, 0, SWP_NOSIZE);

		//TB
		GetDlgItem (IDC_STATIC1)->GetWindowRect(&component);
		left=component.left - form.left-4;
		top=form.bottom-border_TB_IDC_STATIC1-form.top-CMainFrame::GetConfigTitleBarThickness();
		GetDlgItem(IDC_STATIC1)->SetWindowPos(&wndTop, left, top, 0, 0, SWP_NOSIZE);

		//TB
		GetDlgItem (IDC_STATIC2)->GetWindowRect(&component);
		left=component.left - form.left-4;
		top=form.bottom-border_TB_IDC_STATIC2-form.top - CMainFrame::GetConfigTitleBarThickness();
		GetDlgItem(IDC_STATIC2)->SetWindowPos(&wndTop, left, top, 0, 0, SWP_NOSIZE);

		//TB
		GetDlgItem (IDC_SPECANAL_NULL)->GetWindowRect(&component);
		left=component.left - form.left-4;
		top=form.bottom-border_TB_IDC_RADIO2-form.top - CMainFrame::GetConfigTitleBarThickness();
		GetDlgItem(IDC_RADIO2)->SetWindowPos(&wndTop, left, top, 0, 0, SWP_NOSIZE);

		//TB
		GetDlgItem (IDC_SPECANAL_REBUILD)->GetWindowRect(&component);
		left=component.left - form.left-4;
		top=form.bottom-border_TB_IDC_SPECANAL_REBUILD-form.top - CMainFrame::GetConfigTitleBarThickness();
		GetDlgItem(IDC_SPECANAL_REBUILD)->SetWindowPos(&wndTop, left, top, 0, 0, SWP_NOSIZE);

		// cleanup variables and repaint form
		this->Invalidate();
	}
}
