#include "stdafx.h"
#include "velman.h"
#include "WarnoModDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWarnOtherModelDlg dialog

CWarnOtherModelDlg::CWarnOtherModelDlg(CWnd* pParent /*=NULL*/)
: CDialog(CWarnOtherModelDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWarnOtherModelDlg)
	m_RetainWeights = TRUE;
	//}}AFX_DATA_INIT
}

void CWarnOtherModelDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWarnOtherModelDlg)
	DDX_Check(pDX, IDC_WARNOTHERMODEL_RETAINWEIGHTS, m_RetainWeights);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CWarnOtherModelDlg, CDialog)
	//{{AFX_MSG_MAP(CWarnOtherModelDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWarnOtherModelDlg message handlers
