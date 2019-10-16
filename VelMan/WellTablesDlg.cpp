#include "stdafx.h"
#include "velman.h"
#include "VelmanDoc.h"
#include "WellTablesDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWellTablesDlg dialog

CWellTablesDlg::CWellTablesDlg(CWnd* pParent /*=NULL*/)
: CDialog(CWellTablesDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWellTablesDlg)
	m_chosentype = -1;
	//}}AFX_DATA_INIT
}

void CWellTablesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWellTablesDlg)
	DDX_Radio(pDX, IDC_RADIO1, m_chosentype);

	pDoc->TieTableChosenType=m_chosentype;
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CWellTablesDlg, CDialog)
	//{{AFX_MSG_MAP(CWellTablesDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWellTablesDlg message handlers

BOOL CWellTablesDlg::OnInitDialog()
{
	m_chosentype=0;
	CDialog::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
