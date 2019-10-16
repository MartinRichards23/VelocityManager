// DHorDlg.cpp : implementation file

#include "stdafx.h"
#include "velman.h"
#include "VelmanDoc.h"
#include "DefineHorDlg.h"
#include "mainfrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDefineHorizonsDlg dialog

CDefineHorizonsDlg::CDefineHorizonsDlg(CWnd* pParent /*=NULL*/)
: CDialog(CDefineHorizonsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDefineHorizonsDlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CDefineHorizonsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDefineHorizonsDlg)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDefineHorizonsDlg, CDialog)
	//{{AFX_MSG_MAP(CDefineHorizonsDlg)
	ON_BN_CLICKED(IDC_DEFHOR_REMOVE, OnDefhorRemove)
	ON_BN_CLICKED(IDC_DEFHOR_DONE, OnDone)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDefineHorizonsDlg message handlers

void CDefineHorizonsDlg::OnDone()
{
	// create "fake" horizons (almost empty except they have a name and set some standard values)
	// so that e.g. we can map time grids to horizons etc.

	CHorizon *horizon;
	CString name;
	int i, n;

	OnOK();		// works like ADD, if there is another fresh entry in the text box

	CListBox *pLB=(CListBox *)GetDlgItem(IDC_DEFHOR_LIST);

	n=pLB->GetCount();

	for(i=0;i<n;i++)
	{
		if(!(horizon = new CHorizon)) {
			pDoc->Error("Could not allocate new horizon", MSG_ERR);
			OnCancel();
		}
		pLB->GetText(i, name);
		horizon->WriteName(name);
		pDoc->horizonlist.Add(horizon);
		pDoc->Log("Defined horizon " + name, LOG_STD);
	}
	CDialog::OnOK();		// Done works like OK (but we hook up the real OK button so that
	// the "add" function can be invoked with the return key
}

void CDefineHorizonsDlg::OnOK()
{
	// OK works as the "ADD" button here

	CString newentry;

	CEdit *pE=(CEdit *)GetDlgItem(IDC_DEFHOR_NEWENTRY);
	pE->GetWindowText(newentry);

	if(newentry!="")
	{
		CListBox *pLB=(CListBox *)GetDlgItem(IDC_DEFHOR_LIST);
		pLB->AddString((const char *)newentry);

		pE->SetWindowText("");
	}
	GotoDlgCtrl(GetDlgItem(IDC_DEFHOR_NEWENTRY));
}

void CDefineHorizonsDlg::OnDefhorRemove()
{
	int i = 0;
	CListBox *pLB=(CListBox *)GetDlgItem(IDC_DEFHOR_LIST);
	if((i=pLB->GetCurSel())!=LB_ERR)
		pLB->DeleteString(i);
}

BOOL CDefineHorizonsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon (AfxGetApp()->LoadIcon(IDR_MAINFRAME), FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
