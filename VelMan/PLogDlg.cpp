// PLogDlg.cpp : implementation file

#include "stdafx.h"
#include "velman.h"
#include "VelmanDoc.h"
#include "PLogDlg.h"
#include "mainfrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProjectLogDlg dialog

CProjectLogDlg::CProjectLogDlg(CWnd* pParent /*=NULL*/)
: CDialog(CProjectLogDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CProjectLogDlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	initialised=FALSE;
}

void CProjectLogDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProjectLogDlg)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CProjectLogDlg, CDialog)
	//{{AFX_MSG_MAP(CProjectLogDlg)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProjectLogDlg message handlers

BOOL CProjectLogDlg::OnInitDialog()
{
	CWaitCursor wait;
	CDialog::OnInitDialog();
	CString s;
	CString content;
	int numlines;

	CEdit *pE=(CEdit *)GetDlgItem(IDC_PROJECTLOG_TEXT);
	if(SkipFirstLine)
		logfile->ReadString(s);

	numlines=0;
	while(logfile->GetPosition()<logfile->GetLength())
	{
		logfile->ReadString(s);
		content+=s;
		content+="\r\n";
		numlines++;
	};

	listfont.CreatePointFont(80, "Courier New");
	pE->SetFont(&listfont);
	pE->SetWindowText((const char *)content);
	pE->LineScroll(numlines-10);

	SetIcon (AfxGetApp()->LoadIcon(IDR_MAINFRAME), FALSE);
	// this  is the first time that run so want to measure the distance constants
	RECT form;
	RECT component;
	this->GetWindowRect(&form);
	formWidth=form.right-form.left;
	// code to initialise variables

	//RR
	GetDlgItem (IDC_PROJECTLOG_TEXT)->GetWindowRect(&component);
	border_RR_IDC_PROJECTLOG_TEXT=form.right-component.right;

	//BB
	GetDlgItem (IDC_PROJECTLOG_TEXT)->GetWindowRect(&component);
	border_BB_IDC_PROJECTLOG_TEXT=form.bottom-component.bottom;

	//LR
	GetDlgItem (IDOK)->GetWindowRect(&component);
	border_LR_IDOK=form.right-component.left;

	//TB
	GetDlgItem (IDOK)->GetWindowRect(&component);
	border_TB_IDOK=form.bottom-component.top;

	// cleanup code
	initialised=TRUE;

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CProjectLogDlg::OnDestroy()
{
	CDialog::OnDestroy();
	listfont.DeleteObject();
}

void CProjectLogDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if (initialised)
	{
		RECT form;
		RECT component;

		int top, left, width, height;

		this->GetWindowRect(&form);
		// reposition/ size the components

		//RR
		GetDlgItem(IDC_PROJECTLOG_TEXT)->GetWindowRect(&component);
		height=component.bottom-component.top;
		width=form.right-component.left-border_RR_IDC_PROJECTLOG_TEXT;
		GetDlgItem(IDC_PROJECTLOG_TEXT)->SetWindowPos(&wndTop, 0, 0, width, height, SWP_NOMOVE);

		//BB
		GetDlgItem(IDC_PROJECTLOG_TEXT)->GetWindowRect(&component);
		height=form.bottom-border_BB_IDC_PROJECTLOG_TEXT-component.top;
		width=component.right-component.left;
		GetDlgItem(IDC_PROJECTLOG_TEXT)->SetWindowPos(&wndTop, 0, 0, width, height, SWP_NOMOVE);

		//LR
		GetDlgItem(IDOK)->GetWindowRect(&component);
		left=form.right -form.left- border_LR_IDOK-4;
		top=component.top-form.top-CMainFrame::GetConfigTitleBarThickness();
		GetDlgItem(IDOK)->SetWindowPos(&wndTop, left, top, 0, 0, SWP_NOSIZE);

		//TB
		GetDlgItem (IDOK)->GetWindowRect(&component);
		left=component.left-form.left-4;
		top=form.bottom-border_TB_IDOK-form.top-CMainFrame::GetConfigTitleBarThickness();
		GetDlgItem(IDOK)->SetWindowPos(&wndTop, left, top, 0, 0, SWP_NOSIZE);

		// cleanup variables and repaint form
		this->Invalidate();
	}
}
