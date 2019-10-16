#include "stdafx.h"
#include "velman.h"
#include "ProgressDlg.h"
#include <afxcmn.h>

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProgressMonit dialog

CProgressMonit::CProgressMonit(CWnd* pParent /*=NULL*/)
: CDialog(CProgressMonit::IDD, pParent)
{
	//{{AFX_DATA_INIT(CProgressMonit)
	//}}AFX_DATA_INIT
	m_pDocument=NULL;
}

CProgressMonit::CProgressMonit(CDocument *pDocument) : CDialog()
{
	m_pDocument=pDocument;
}

BOOL CProgressMonit::Create()
{
	return CDialog::Create(CProgressMonit::IDD);
}

void CProgressMonit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProgressMonit)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CProgressMonit, CDialog)
	//{{AFX_MSG_MAP(CProgressMonit)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CProgressMonit::ChangeText(const char *newtext)
{
	CWnd *pW=GetDlgItem(IDC_RMSIMPORT_CURRENT);
	pW->SetWindowText(newtext);
}

void CProgressMonit::ChangeText2(const char *newtext)
{
	CWnd *pW=GetDlgItem(IDC_RMSIMPORT_CURRENT2);
	pW->SetWindowText(newtext);
}

void CProgressMonit::ChangeGeneralDescription(const char *newtext)
{
	CWnd *pW=GetDlgItem(IDC_RMSIMPORT_GENDESCR);
	pW->SetWindowText(newtext);
}

void CProgressMonit::InitializePercentage(BOOL enable)
{
	CProgressCtrl	*pC=(CProgressCtrl	*)GetDlgItem(IDC_RMSIMPORT_PROGBAR);
	pC->ShowWindow(enable ? SW_SHOW : SW_HIDE);
	currentpercentage=-1;
}

void CProgressMonit::ChangePercentage(int value)
{
	char buf[128];

	if(value!=currentpercentage)
	{
		sprintf(buf, "%d%% completed", value);
		SetDlgItemText(IDC_RMSIMPORT_CURRENT2, buf);
		CProgressCtrl	*pC=(CProgressCtrl	*)GetDlgItem(IDC_RMSIMPORT_PROGBAR);
		pC->SetPos(value);
		currentpercentage=value;
	}
}
/////////////////////////////////////////////////////////////////////////////
// CProgressMonit message handlers

BOOL CProgressMonit::OnInitDialog()
{
	CDialog::OnInitDialog();

	CProgressCtrl	*pC=(CProgressCtrl	*)GetDlgItem(IDC_RMSIMPORT_PROGBAR);
	pC->ShowWindow(SW_HIDE);
	AfxGetApp()->DoWaitCursor(1);
	return TRUE;  // return TRUE unless you set the focus to a control
}

void CProgressMonit::OnDestroy()
{
	CDialog::OnDestroy();

	AfxGetApp()->DoWaitCursor(-1);
}
