// LGridDlg.cpp : implementation file

#include "stdafx.h"
#include "velman.h"
#include "VelmanDoc.h"
#include "LimitGridDlg.h"
#include "mainfrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLimitGridDlg dialog

CLimitGridDlg::CLimitGridDlg(CWnd* pParent /*=NULL*/)
: CDialog(CLimitGridDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLimitGridDlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CLimitGridDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLimitGridDlg)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CLimitGridDlg, CDialog)
	//{{AFX_MSG_MAP(CLimitGridDlg)
	ON_LBN_SELCHANGE(IDC_GRIDLIMIT_INPUT2, OnSelchangeGridlimitInput2)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLimitGridDlg message handlers

BOOL CLimitGridDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// fill the two listboxes
	CListBox *pLB=(CListBox *)GetDlgItem(IDC_GRIDLIMIT_INPUT1);
	if(pDoc->zimscube.FillListboxWithGridnames(pLB, entries)==0)
		OnCancel();		// give it up if there are no grids around
	pLB->SetCurSel(1);
	pLB->SetTabStops(15);

	pLB=(CListBox *)GetDlgItem(IDC_GRIDLIMIT_INPUT2);
	if(pDoc->zimscube.FillListboxWithGridnames(pLB, entries)==0)
		OnCancel();		// give it up if there are no grids around
	pLB->SetCurSel(1);
	pLB->SetTabStops(15);
	UpdateOutputName();

	SetIcon (AfxGetApp()->LoadIcon(IDR_MAINFRAME), FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CLimitGridDlg::UpdateOutputName()
{
	// write the name of the grid to be created into the space in the dialog
	CString buf;

	int i = 0;
	// retrieve name of depth grid to be limited
	CListBox *pLB=(CListBox *)GetDlgItem(IDC_GRIDLIMIT_INPUT2);
	pLB->GetText(pLB->GetCurSel(), buf);
	// remove some extra bits	(the "[+]" before the name)
	i=buf.Find("\t");
	if(i>-1)
		buf=buf.Mid(i+1);
	// append an "_L" and output it
	buf="Output grid: \""+buf+"_Limited\"";
	SetDlgItemText(IDC_GRIDLIMIT_OUTPUTNAME, buf);
}

void CLimitGridDlg::OnSelchangeGridlimitInput2()
{
	UpdateOutputName();
}

void CLimitGridDlg::OnOK()
{
	int click, i;
	grid1=-1;
	grid2=-1;

	CListBox *pLB=(CListBox *)GetDlgItem(IDC_GRIDLIMIT_INPUT1);
	click=pLB->GetCurSel();
	for(i=0;i<entries.GetSize();i++)
	{
		if( ((entries[i])&0xFF) == click)
		{
			grid1=entries[i]>>8;
			break;
		}
	}
	pLB=(CListBox *)GetDlgItem(IDC_GRIDLIMIT_INPUT2);
	click=pLB->GetCurSel();
	for(i=0;i<entries.GetSize();i++)
	{
		if( ((entries[i])&0xFF) == click)
		{
			grid2=entries[i]>>8;
			break;
		}
	}
	CDialog::OnOK();
}
