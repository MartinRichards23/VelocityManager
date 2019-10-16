#include "stdafx.h"
#include "velman.h"
#include "ImportErrorDlg.h"
#include "ACorrDlg.h"
#include "mainfrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTimeImportErrorDlg dialog

CTimeImportErrorDlg::CTimeImportErrorDlg(CWnd* pParent /*=NULL*/)
: CDialog(CTimeImportErrorDlg::IDD, pParent)
{
	answer=0;
	//{{AFX_DATA_INIT(CTimeImportErrorDlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CTimeImportErrorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTimeImportErrorDlg)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTimeImportErrorDlg, CDialog)
	//{{AFX_MSG_MAP(CTimeImportErrorDlg)
	ON_BN_CLICKED(IDC_AUTOCORRECT_BTN, OnAutocorrectBtn)
	ON_BN_CLICKED(IDC_RETRYBTN, OnRetrybtn)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTimeImportErrorDlg message handlers

void CTimeImportErrorDlg::SetTexts(CStringArray &msg, BOOL swap)
{
	msgs=&msg;
	shouldswap=swap;
}

void CTimeImportErrorDlg::OnAutocorrectBtn()
{
	CAutoCorrectOptionsDlg dlg;

	dlg.shouldswap=shouldswap;	 // tell dlg wether to enable the option of
	//ZNONing the conflicting points: do not do it if >90% wrong.
	if(dlg.DoModal()==IDOK)
	{
		answer=10+dlg.m_option;
		CDialog::OnOK();
	}
}

void CTimeImportErrorDlg::OnRetrybtn()
{
	answer=2;
	CDialog::OnOK();
}

BOOL CTimeImportErrorDlg::OnInitDialog()
{
	// make listbox font smaller
	smallFont.CreateFont(15, 0, 0, 0, 400, FALSE, FALSE, 0, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_SWISS, "Arial");

	CDialog::OnInitDialog();

	int i = 0;
	CListBox *pLB=(CListBox *)GetDlgItem(IDC_TIMPERR_LIST);
	pLB->SetFont(&smallFont);
	pLB->ResetContent();
	for(i=0;i<msgs->GetSize();i++)
		pLB->AddString((const char *)msgs->GetAt(i));

	if(shouldswap)
		SetDlgItemText(IDC_TIMPERR_RECC, "Recommendation: Retry the import, you have"
		" probably swapped two or more grids.");
	else
		SetDlgItemText(IDC_TIMPERR_RECC, "Recommendation: Use \"Correct Errors...\", \"AutoCorrect\" to remove the"
		" few errors that were found.");

	SetIcon (AfxGetApp()->LoadIcon(IDR_MAINFRAME), FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CTimeImportErrorDlg::OnDestroy()
{
	smallFont.DeleteObject();
	CDialog::OnDestroy();
}
