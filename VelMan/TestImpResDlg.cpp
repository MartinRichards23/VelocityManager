// TIResDlg.cpp : implementation file
//

#include "stdafx.h"
#include <afxcmn.h>
#include "velman.h"
#include "VelmanDoc.h"
#include "TestImpResDlg.h"
#include "mainfrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRMSTestImportResultsDlg dialog

CRMSTestImportResultsDlg::CRMSTestImportResultsDlg(CWnd* pParent /*=NULL*/)
: CDialog(CRMSTestImportResultsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRMSTestImportResultsDlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	initialised=FALSE;
}

void CRMSTestImportResultsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRMSTestImportResultsDlg)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CRMSTestImportResultsDlg, CDialog)
	//{{AFX_MSG_MAP(CRMSTestImportResultsDlg)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRMSTestImportResultsDlg message handlers

BOOL CRMSTestImportResultsDlg::OnInitDialog()
{
	smallFont.CreateFont(14, 0, 0, 0, 400, FALSE, FALSE, 0, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_SWISS, "Arial");

	CDialog::OnInitDialog();

	// fill list
	CListCtrl *pL=(CListCtrl *)GetDlgItem(IDC_TESTIMPORT_LIST);
	CDummyReadSeisdata *seisdata=&(pDoc->seismicdata.dummyseisdata);

	pL->SetFont(&smallFont);

	int i, j;

	// create the columns
	char *col_label[]={ "Line ID", "Pt No", "t", "v", "x", "y" };
	int col_width[]  ={  100   , 40      , 80, 80, 80, 80 };

	for(i=0;i<4;i++)
		pL->InsertColumn(i, col_label[i], LVCFMT_LEFT, col_width[i], i);
	if(seisdata->Filtertype=='1')
	{
		for(i=4;i<6;i++)
			pL->InsertColumn(i, col_label[i], LVCFMT_LEFT, col_width[i], i);
	}

	// fill first 4 columns in any case
	for(i=0;i<seisdata->GetSize();i++)
	{
		pL->InsertItem(i, (const char *)seisdata->GetLine(i));
		for(j=1;j<4;j++)
			pL->SetItem(i, j, LVIF_TEXT, (const char *)seisdata->GetItem(i, j), 0, 0, 0, 0);
	}

	// note about coordinates, maybe fill columns 5+6
	switch(seisdata->Filtertype)
	{
	case '1':	// no worries -- coords are in file anyway
		GetDlgItem(IDC_TESTIMPORT_COORDNOTE)->ShowWindow(FALSE);
		for(i=0;i<seisdata->GetSize();i++)
		{
			for(j=4;j<6;j++)
				pL->SetItem(i, j, LVIF_TEXT, (const char *)seisdata->GetItem(i, j), 0, 0, 0, 0);
		}
		break;
	case '2':
		SetDlgItemText(IDC_TESTIMPORT_COORDNOTE,
			"Coordinates will be computed later");
		break;
	case '3':
		SetDlgItemText(IDC_TESTIMPORT_COORDNOTE,
			"Coordinates will be read from another file");
		break;
	}

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
	GetDlgItem (IDC_TESTIMPORT_COORDNOTE)->GetWindowRect(&component);
	border_LR_IDC_TESTIMPORT_COORDNOTE=form.right-component.left;

	//BB
	GetDlgItem (IDC_TESTIMPORT_COORDNOTE)->GetWindowRect(&component);
	border_BB_IDC_TESTIMPORT_COORDNOTE=form.bottom-component.bottom;

	//RR
	GetDlgItem (IDC_TESTIMPORT_LIST)->GetWindowRect(&component);
	border_RR_IDC_TESTIMPORT_LIST=form.right-component.right;

	//BB
	GetDlgItem (IDC_TESTIMPORT_LIST)->GetWindowRect(&component);
	border_BB_IDC_TESTIMPORT_LIST=form.bottom-component.bottom;

	// cleanup code
	initialised=TRUE;

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CRMSTestImportResultsDlg::OnDestroy()
{
	CDialog::OnDestroy();
	smallFont.DeleteObject();
}

void CRMSTestImportResultsDlg::OnSize(UINT nType, int cx, int cy)
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
		left=form.right -form.left- border_LR_IDOK-4;
		top=component.top-form.top-CMainFrame::GetConfigTitleBarThickness();
		GetDlgItem(IDOK)->SetWindowPos(&wndTop, left, top, 0, 0, SWP_NOSIZE);

		//LR
		GetDlgItem(IDC_TESTIMPORT_COORDNOTE)->GetWindowRect(&component);
		left=form.right -form.left- border_LR_IDC_TESTIMPORT_COORDNOTE-4;
		top=component.top-form.top-CMainFrame::GetConfigTitleBarThickness();
		GetDlgItem(IDC_TESTIMPORT_COORDNOTE)->SetWindowPos(&wndTop, left, top, 0, 0, SWP_NOSIZE);

		//BB
		GetDlgItem(IDC_TESTIMPORT_COORDNOTE)->GetWindowRect(&component);
		height=form.bottom-border_BB_IDC_TESTIMPORT_COORDNOTE-component.top;
		width=component.right-component.left;
		GetDlgItem(IDC_TESTIMPORT_COORDNOTE)->SetWindowPos(&wndTop, 0, 0, width, height, SWP_NOMOVE);

		//RR
		GetDlgItem(IDC_TESTIMPORT_LIST)->GetWindowRect(&component);
		height=component.bottom-component.top;
		width=form.right-component.left-border_RR_IDC_TESTIMPORT_LIST;
		GetDlgItem(IDC_TESTIMPORT_LIST)->SetWindowPos(&wndTop, 0, 0, width, height, SWP_NOMOVE);

		//BB
		GetDlgItem(IDC_TESTIMPORT_LIST)->GetWindowRect(&component);
		height=form.bottom-border_BB_IDC_TESTIMPORT_LIST-component.top;
		width=component.right-component.left;
		GetDlgItem(IDC_TESTIMPORT_LIST)->SetWindowPos(&wndTop, 0, 0, width, height, SWP_NOMOVE);

		// cleanup variables and repaint form
		this->Invalidate();
	}

}
