// ACorrDlg.cpp : implementation file

#include "stdafx.h"
#include "velman.h"
#include "ACorrDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAutoCorrectOptionsDlg dialog

CAutoCorrectOptionsDlg::CAutoCorrectOptionsDlg(CWnd* pParent /*=NULL*/)
: CDialog(CAutoCorrectOptionsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAutoCorrectOptionsDlg)
	//}}AFX_DATA_INIT
}

void CAutoCorrectOptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAutoCorrectOptionsDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAutoCorrectOptionsDlg, CDialog)
	//{{AFX_MSG_MAP(CAutoCorrectOptionsDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAutoCorrectOptionsDlg message handlers

BOOL CAutoCorrectOptionsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// if the recommendation is swapping the grids, we will only offer the
	// possibility of AutoCorrect, but not ZNONing the conflicting points,
	// as we do not want grids full of ZNONs

	((CButton *)GetDlgItem(IDC_HOWTOCORRECT_RADIO1))->SetCheck(1);
	if(shouldswap)
	{
		GetDlgItem(IDC_HOWTOCORRECT_RADIO2)->EnableWindow(FALSE);
		GetDlgItem(IDC_HOWTOCORRECT_RADIO3)->EnableWindow(FALSE);
		GetDlgItem(IDC_HOWTOCORRECT_RADIO4)->EnableWindow(FALSE);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CAutoCorrectOptionsDlg::OnOK()
{
	// the unix version does not read radio buttons so we do it by hand
	if(IsDlgButtonChecked(IDC_HOWTOCORRECT_RADIO1))
		m_option=0;
	if(IsDlgButtonChecked(IDC_HOWTOCORRECT_RADIO2))
		m_option=1;
	if(IsDlgButtonChecked(IDC_HOWTOCORRECT_RADIO3))
		m_option=2;
	if(IsDlgButtonChecked(IDC_HOWTOCORRECT_RADIO4))
		m_option=3;

	CDialog::OnOK();
}
