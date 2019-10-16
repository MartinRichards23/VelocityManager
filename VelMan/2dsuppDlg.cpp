// 2dsupp.cpp : implementation file

#include "stdafx.h"
#include "velman.h"
#include "VelmanDoc.h"
#include "2dsuppDlg.h"
#include "mainfrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRMSImportSuppfileDlg dialog

CRMSImportSuppfileDlg::CRMSImportSuppfileDlg(CWnd* pParent /*=NULL*/)
: CDialog(CRMSImportSuppfileDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRMSImportSuppfileDlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CRMSImportSuppfileDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRMSImportSuppfileDlg)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CRMSImportSuppfileDlg, CDialog)
	//{{AFX_MSG_MAP(CRMSImportSuppfileDlg)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRMSImportSuppfileDlg message handlers

BOOL CRMSImportSuppfileDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// fill listbox with names of all available grids that contain shotpoint info
	CListBox *pLB=(CListBox *)GetDlgItem(IDC_RMS2DSUPP_GRIDLIST);

	// now create a list of ZIMS grid names from that list of DOS names
	CStringArray zimsnames;
	pDoc->seismicdata.MakeZimsNameList(zimsnames, pDoc->ListOfDOSnames, ZIMS_TYPE_LINE);

	for(int i=0;i<zimsnames.GetSize();i++)
		pLB->AddString((const char *)(zimsnames.GetAt(i)));

	pLB->SetCurSel(0);

	SetIcon (AfxGetApp()->LoadIcon(IDR_MAINFRAME), FALSE);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CRMSImportSuppfileDlg::OnOK()
{
	CListBox *pLB=(CListBox *)GetDlgItem(IDC_RMS2DSUPP_GRIDLIST);
	pLB->GetText(pLB->GetCurSel(), ChosenGrid);

	CDialog::OnOK();
}
