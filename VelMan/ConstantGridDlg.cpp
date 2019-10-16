// CGridDlg.cpp : implementation file

#include "stdafx.h"
#include "velman.h"
#include "ConstantGridDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCreateConstantGridDlg dialog

CCreateConstantGridDlg::CCreateConstantGridDlg(CWnd* pParent /*=NULL*/)
: CDialog(CCreateConstantGridDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCreateConstantGridDlg)
	m_value = 0.0;
	m_listType = 0;
	//}}AFX_DATA_INIT
}

void CCreateConstantGridDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCreateConstantGridDlg)
	DDX_Text(pDX, IDC_CONSTGRID_VALUE, m_value);
	DDX_CBIndex(pDX, IDC_GRIDIMP_GRIDTYPE, m_listType);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CCreateConstantGridDlg, CDialog)
	//{{AFX_MSG_MAP(CCreateConstantGridDlg)
	// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CCreateConstantGridDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	return TRUE;
}

void CCreateConstantGridDlg::OnOK()
{
	// TODO: Add extra validation here

	CDialog::OnOK();
}

/////////////////////////////////////////////////////////////////////////////
// CCreateConstantGridDlg message handlers
