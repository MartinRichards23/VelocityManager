// maperror.cpp : implementation file
//

#include "stdafx.h"
#include "velman.h"
#include "MapErrorDlg.h"
#include "mainfrm.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMapErrorDlg dialog

CMapErrorDlg::CMapErrorDlg(CWnd* pParent /*=NULL*/)
: CDialog(CMapErrorDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMapErrorDlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	initialised=FALSE;
}

void CMapErrorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMapErrorDlg)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMapErrorDlg, CDialog)
	//{{AFX_MSG_MAP(CMapErrorDlg)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMapErrorDlg message handlers

BOOL CMapErrorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Fill listbox with all the errors that the check in CWellhorzTimegridMapDlg::OnOK
	// told us about

	CListBox *pLB=(CListBox *)GetDlgItem(IDC_MAPERR_LIST);
	
	for(int i=0;i<errlistptr->GetSize();i++)
	{
		pLB->AddString(errlistptr->GetAt(i));
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

	//RR
	GetDlgItem (IDC_MAPERR_LIST)->GetWindowRect(&component);
	border_RR_IDC_MAPERR_LIST=form.right-component.right;

	//BB
	GetDlgItem (IDC_MAPERR_LIST)->GetWindowRect(&component);
	border_BB_IDC_MAPERR_LIST=form.bottom-component.bottom;

	initialised=TRUE;

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMapErrorDlg::OnSize(UINT nType, int cx, int cy)
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

		//RR
		GetDlgItem(IDC_MAPERR_LIST)->GetWindowRect(&component);
		height=component.bottom - component.top;
		width=form.right-component.left-border_RR_IDC_MAPERR_LIST;
		GetDlgItem(IDC_MAPERR_LIST)->SetWindowPos(&wndTop, 0, 0, width, height, SWP_NOMOVE);

		//BB
		GetDlgItem(IDC_MAPERR_LIST)->GetWindowRect(&component);
		height=form.bottom-border_BB_IDC_MAPERR_LIST-component.top;
		width=component.right - component.left;
		GetDlgItem(IDC_MAPERR_LIST)->SetWindowPos(&wndTop, 0, 0, width, height, SWP_NOMOVE);

		this->Invalidate();
	}

}
