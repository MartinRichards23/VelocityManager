#include "stdafx.h"
#include "velman.h"
#include "NewDirDlg.h"
#include "mainfrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCreateNewDirDlg dialog

CCreateNewDirDlg::CCreateNewDirDlg(CWnd* pParent /*=NULL*/)
: CDialog(CCreateNewDirDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCreateNewDirDlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CCreateNewDirDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCreateNewDirDlg)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CCreateNewDirDlg, CDialog)
	//{{AFX_MSG_MAP(CCreateNewDirDlg)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCreateNewDirDlg message handlers

BOOL CCreateNewDirDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	char buf[512];

	sprintf(buf, "Created in %s", (const char *)BasePath);
	SetDlgItemText(IDC_CREATEDIR_BASEPATH, buf);

	SetIcon (AfxGetApp()->LoadIcon(IDR_MAINFRAME), FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CCreateNewDirDlg::OnOK()
{
	GetDlgItem(IDC_CREATEDIR_NAME)->GetWindowText(NewDir);

	CDialog::OnOK();
}
