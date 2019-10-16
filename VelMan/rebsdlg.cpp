// RebSDlg.cpp : implementation file
//

#include "stdafx.h"
#include "velman.h"
#include "RebSDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRebuildSettingsDlg dialog

CRebuildSettingsDlg::CRebuildSettingsDlg(CWnd* pParent /*=NULL*/)
: CDialog(CRebuildSettingsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRebuildSettingsDlg)
	m_doall = FALSE;
	SetAsPrimary = TRUE;
	m_number = 0;
	//}}AFX_DATA_INIT
}

void CRebuildSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRebuildSettingsDlg)
	DDX_Check(pDX, IDC_REBUILDSETTINGS_DOALL, m_doall);
	DDX_Check(pDX, IDC_PRIMARY, SetAsPrimary);
	DDX_Text(pDX, IDC_REBUILDSETTINGS_NUMBER, m_number);
	DDV_MinMaxInt(pDX, m_number, 0, 250);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CRebuildSettingsDlg, CDialog)
	//{{AFX_MSG_MAP(CRebuildSettingsDlg)
	ON_BN_CLICKED(IDC_REBUILDSETTINGS_DOALL, OnRebuildsettingsDoall)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRebuildSettingsDlg message handlers

BOOL CRebuildSettingsDlg::OnInitDialog()
{
	m_number=15;
	m_doall=FALSE;
	CheckDlgButton(IDC_PRIMARY, SetAsPrimary);

	CDialog::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CRebuildSettingsDlg::OnRebuildsettingsDoall()
{
	// cannot enter a number if "Rebuild all" is checked
	if(IsDlgButtonChecked(IDC_REBUILDSETTINGS_DOALL))
		GetDlgItem(IDC_REBUILDSETTINGS_NUMBER)->EnableWindow(FALSE);
	else
		GetDlgItem(IDC_REBUILDSETTINGS_NUMBER)->EnableWindow(TRUE);
}
