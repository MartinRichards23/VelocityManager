// rmsimpor.cpp : implementation file

#include "stdafx.h"
#include "velman.h"
#include "VelmanDoc.h"
#ifdef VELMAN_UNIX
#include <fstream.h>
#else
#include <fstream>
#endif
#include "RmsImportDlg.h"
#include "mainfrm.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////
// CRMSImportDlg dialog

CRMSImportDlg::CRMSImportDlg(CWnd* pParent /*=NULL*/)
: CDialog(CRMSImportDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRMSImportDlg)
	m_Unit = -1;
	//}}AFX_DATA_INIT
}

void CRMSImportDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRMSImportDlg)
	DDX_Radio(pDX, IDC_FILTERCHOICE_UNIT, m_Unit);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CRMSImportDlg, CDialog)
	//{{AFX_MSG_MAP(CRMSImportDlg)
	ON_LBN_DBLCLK(IDC_FILTERNAMELIST, OnOK)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRMSImportDlg message handlers

BOOL CRMSImportDlg::OnInitDialog()
{
	int err;
	CString name;
	ifstream filterfile;
	CString filterfilename, helperstring;

	Error=FALSE;
	CDialog::OnInitDialog();

	CListBox *pLB=(CListBox *)GetDlgItem(IDC_FILTERNAMELIST);

#ifdef VELMAN_UNIX
	filterfile.open(pDoc->GetFilterFileDir()+"/velman.flt");
#else
	filterfile.open(pDoc->GetFilterFileDir()+"\\VELMAN.FLT");
#endif
	do
	{
		name=pDoc->GetNextFiltername(err, &filterfile, &helperstring);
		if(!err)
		{
			pLB->AddString(name);
			AllTheTypes.Add(helperstring);
		}
	} while(err==0);

	filterfile.close();

	CString preSelect;	// preselect the filter that the user chose last time,
	// or the first filter if there was no "last time"
	preSelect=AfxGetApp()->GetProfileString("RMS Import", "PreferredFilter", "");
	if(pLB->SelectString(-1, preSelect)==LB_ERR)
	{
		if(pLB->SetCurSel(0)==LB_ERR)	// select first item if you don't know better
		{
			Error=TRUE;
			CDialog::OnCancel();	// if list is empty, some file error etc. occured, we give up
		}
	}

	SetIcon (AfxGetApp()->LoadIcon(IDR_MAINFRAME), FALSE);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CRMSImportDlg::OnOK()
{
	CListBox *pLB=(CListBox *)GetDlgItem(IDC_FILTERNAMELIST);

	// find out which filter was chosen in the list
	pLB->GetText(pLB->GetCurSel(), ChosenFilter);
	AfxGetApp()->WriteProfileString("RMS Import", "PreferredFilter", ChosenFilter);
	ChosenType=AllTheTypes.GetAt(pLB->GetCurSel());
	CDialog::OnOK();
}
