// projguid.cpp : implementation file
//

#include "stdafx.h"
#include "velman.h"
#include "ProjGuideDlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProjGuideDlg dialog

CProjGuideDlg::CProjGuideDlg(CWnd* pParent /*=NULL*/)
: CDialog(CProjGuideDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CProjGuideDlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CProjGuideDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProjGuideDlg)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CProjGuideDlg, CDialog)
	//{{AFX_MSG_MAP(CProjGuideDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProjGuideDlg message handlers

BOOL CProjGuideDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CWnd *pW=GetDlgItem(IDC_WHATNOW_TEXT);
	pW->SetWindowText(HelpText);

	return TRUE;  // return TRUE  unless you set the focus to a control
}
