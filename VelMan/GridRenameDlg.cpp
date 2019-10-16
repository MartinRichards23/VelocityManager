// GRenDlg.cpp : implementation file
//

#include "stdafx.h"
#include "velman.h"
#include "GridRenameDlg.h"
#include "mainfrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRenameGridDlg dialog

CRenameGridDlg::CRenameGridDlg(CWnd* pParent /*=NULL*/)
: CDialog(CRenameGridDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRenameGridDlg)
	m_gridname = _T("");
	m_listType = -1;
	//}}AFX_DATA_INIT
}

void CRenameGridDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRenameGridDlg)
	DDX_Text(pDX, IDC_GRIDRENAME_NAME, m_gridname);
	DDX_CBIndex(pDX, IDC_GRIDIMP_GRIDTYPE, m_listType);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CRenameGridDlg, CDialog)
	//{{AFX_MSG_MAP(CRenameGridDlg)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRenameGridDlg message handlers

BOOL CRenameGridDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetWindowText(oldname);

	SetIcon (AfxGetApp()->LoadIcon(IDR_MAINFRAME), FALSE);

	return TRUE;
}

void CRenameGridDlg::OnOK()
{
	// TODO: Add extra validation here

	CDialog::OnOK();
}