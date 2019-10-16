// DesGrid.cpp : implementation file
#include "stdafx.h"
#include "velman.h"
#include "VelmanDoc.h"
#include "DesignateGridDlg.h"
#include "mainfrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDesignateGridDlg dialog

CDesignateGridDlg::CDesignateGridDlg(CWnd* pParent /*=NULL*/)
: CDialog(CDesignateGridDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDesignateGridDlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

}

void CDesignateGridDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDesignateGridDlg)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDesignateGridDlg, CDialog)
	//{{AFX_MSG_MAP(CDesignateGridDlg)
	ON_LBN_SELCHANGE(IDC_DESGRID_LIST, OnSelchangeDesgridList)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDesignateGridDlg message handlers

BOOL CDesignateGridDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CListBox *pLB=(CListBox *)GetDlgItem(IDC_DESGRID_LIST);

	if(pDoc->zimscube.FillListboxWithGridnames(pLB, entries)==0)
	{
		AfxMessageBox("There are no grids to display", MB_OK | MB_ICONINFORMATION);
		OnCancel();		// give it up if there are no grids around
	}

	pLB->SetTabStops(15);

	CComboBox *pCB=(CComboBox *)GetDlgItem(IDC_DESGRID_HORS);

	for(int i=0;i<pDoc->horizonlist.GetSize();i++)
		pCB->AddString( ((CHorizon *)pDoc->horizonlist.GetAt(i))->name);

	pLB->SetCurSel(1);
	OnSelchangeDesgridList();

	SetIcon (AfxGetApp()->LoadIcon(IDR_MAINFRAME), FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDesignateGridDlg::OnOK()
{
	CListBox *pLB=(CListBox *)GetDlgItem(IDC_DESGRID_LIST);
	CComboBox *pCB=(CComboBox *)GetDlgItem(IDC_DESGRID_HORS);
	int click, i;

	newhorizon=pCB->GetCurSel();

	click=pLB->GetCurSel();

	for(i=0;i<entries.GetSize();i++)
	{
		if( ((entries[i])&0xFF) == click)
		{
			chosengrid=entries[i]>>8;
			CDialog::OnOK();
			break;
		}
	}
}

void CDesignateGridDlg::OnSelchangeDesgridList()
{
	CListBox *pLB=(CListBox *)GetDlgItem(IDC_DESGRID_LIST);
	CComboBox *pCB=(CComboBox *)GetDlgItem(IDC_DESGRID_HORS);
	int thegrid, click, i;

	click=pLB->GetCurSel();

	thegrid=-1;
	for(i=0;i<entries.GetSize();i++)
	{
		if( ((entries[i])&0xFF) == click)
		{
			thegrid=entries[i]>>8;
			break;
		}
	}
	if(thegrid>=0)
		pCB->SetCurSel(((CZimsGrid *)pDoc->zimscube.GetAt(thegrid))->GetHorizon());
}