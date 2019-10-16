// SDCODlg.cpp : implementation file
//

#include "stdafx.h"
#include "velman.h"
#include "SDCODlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSecondaryDepthConvOptDlg dialog

CSecondaryDepthConvOptDlg::CSecondaryDepthConvOptDlg(CWnd* pParent /*=NULL*/)
: CDialog(CSecondaryDepthConvOptDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSecondaryDepthConvOptDlg)
	m_rebuild = FALSE;
	//}}AFX_DATA_INIT
	RebuildDoesNotApply=FALSE;
	TimeConversion=FALSE;
}

void CSecondaryDepthConvOptDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSecondaryDepthConvOptDlg)
	DDX_Check(pDX, IDC_DCOPT_REBUILD, m_rebuild);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSecondaryDepthConvOptDlg, CDialog)
	//{{AFX_MSG_MAP(CSecondaryDepthConvOptDlg)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_DCOPT_CUBIC, OnChangeSettings)
	ON_EN_CHANGE(IDC_DCOPT_DECREASEBY, OnChangeSettings)
	ON_BN_CLICKED(IDC_DCOPT_LINEAR, OnChangeSettings)
	ON_BN_CLICKED(IDC_DCOPT_CONST, OnChangeSettings)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSecondaryDepthConvOptDlg message handlers

BOOL CSecondaryDepthConvOptDlg::OnInitDialog()
{
	if(TimeConversion)
		SetWindowText("Options for time conversion");

	CDialog::OnInitDialog();

	// 7/02. The default now is for
	// the dialogue to appear with the linear option as the default. This is
	// a bit messy because the choice is controlled by whether may_apply_cubic
	// is true or not rather than using the value of the item. interp_method is
	// choice of the method got by ReadSettings().
	//if(may_apply_cubic)
	//	CheckRadioButton(IDC_DCOPT_LINEAR, IDC_DCOPT_CUBIC, IDC_DCOPT_CUBIC);
	//else
	//{
	//	CheckRadioButton(IDC_DCOPT_LINEAR, IDC_DCOPT_CUBIC, IDC_DCOPT_LINEAR);
	//	GetDlgItem(IDC_DCOPT_CUBIC)->EnableWindow(FALSE);
	//}
	CheckRadioButton(IDC_DCOPT_LINEAR, IDC_DCOPT_CUBIC, IDC_DCOPT_LINEAR);
	if ( ! may_apply_cubic )
		GetDlgItem(IDC_DCOPT_CUBIC)->EnableWindow(FALSE);
	// end of 7/02 changes.
	SetDlgItemText(IDC_DCOPT_DECREASEBY, "100");

	if(RebuildDoesNotApply)
		GetDlgItem(IDC_DCOPT_REBUILD)->ShowWindow(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CSecondaryDepthConvOptDlg::ReadSettings()
{
	interp_method=0; // linear
	if(IsDlgButtonChecked(IDC_DCOPT_CUBIC))
		interp_method=1;
	if(IsDlgButtonChecked(IDC_DCOPT_CONST))
		interp_method=2;
	percentage=max(0, min(200, GetDlgItemInt(IDC_DCOPT_DECREASEBY)));
}

void CSecondaryDepthConvOptDlg::OnOK()
{
	ReadSettings();
	CDialog::OnOK();
}

void CSecondaryDepthConvOptDlg::OnChangeSettings()
{
	ReadSettings();
	Invalidate();
}

void CSecondaryDepthConvOptDlg::OnPaint()
{
	int i = 0;
	CPaintDC(this);
	CWnd *pWnd=GetDlgItem(IDC_DCOPT_PREVIEW);
	CDC *pDC=pWnd->GetDC();

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
	pDC->TextOut(270, 480, "interval time");
	pDC->TextOut(20, 20, "v");

	// draw function
	// choose red pen
	CPen redpen(PS_SOLID, 1, RGB(255, 0, 0));
	CPen dotpen(PS_DOT, 1, RGB(0, 0, 0));
	CPen *pOldPen=pDC->SelectObject(&redpen);

	POINT support[5]={
		{ 70, 430 },
		{ 160, 300 },
		{ 250, 400 },
		{ 340, 200 },
		{ 400, 100}};

		if(!interp_method)
		{
			support[4].x=400+percentage/2;
			support[4].y=100+percentage/20;
		}
		else
		{
			support[4].x=400;
			support[4].y=100;
		}

		pDC->SelectStockObject(BLACK_PEN);
		switch(interp_method)
		{
		case 0:
			// sketch cubic spline
			pDC->Polyline(support, 4);
			break;
		case 1:
			// sketch linear interpolation
			pDC->PolyBezier(support, 4);
			break;
		case 2:
			// sketch constant vAvg
			for(i=0;i<3;i++)
			{
				pDC->MoveTo(support[i]);
				pDC->LineTo(support[i+1].x, support[i].y);
			}
			break;
		}
		pDC->SelectObject(&dotpen);
		pDC->MoveTo(support[3]);
		pDC->LineTo(support[4]);

		pDC->SelectObject(pOldFont);
		font.DeleteObject();
		pDC->SelectObject(pOldPen);
		pWnd->ReleaseDC(pDC);
}
