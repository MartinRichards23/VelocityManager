#include "stdafx.h"
#include "velman.h"
#include "NewProj3Dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProjectScopeDlg dialog

CProjectScopeDlg::CProjectScopeDlg(CWnd* pParent /*=NULL*/)
: CDialog(CProjectScopeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CProjectScopeDlg)
	//}}AFX_DATA_INIT
}

void CProjectScopeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProjectScopeDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CProjectScopeDlg, CDialog)
	//{{AFX_MSG_MAP(CProjectScopeDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProjectScopeDlg message handlers

BOOL CProjectScopeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	((CButton *)GetDlgItem(IDC_SCOPDLG_RADIO1))->SetCheck(1);

	CFont *m_Font1 = new CFont;
	m_Font1->CreatePointFont(80, "Arial Bold");

	((CStatic *)GetDlgItem(IDC_STATIC5))->SetFont(m_Font1);
	((CStatic *)GetDlgItem(IDC_STATIC6))->SetFont(m_Font1);
	((CStatic *)GetDlgItem(IDC_STATIC7))->SetFont(m_Font1);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CProjectScopeDlg::OnOK()
{
	CDialog::OnOK();
	// work around problem with button data transfer under Unix
	if(((CButton *)GetDlgItem(IDC_SCOPDLG_RADIO1))->GetCheck())
		m_PrjScope=0;
	if(((CButton *)GetDlgItem(IDC_SCOPDLG_RADIO2))->GetCheck())
		m_PrjScope=1;
	if(((CButton *)GetDlgItem(IDC_SCOPDLG_RADIO3))->GetCheck())
		m_PrjScope=2;
}
