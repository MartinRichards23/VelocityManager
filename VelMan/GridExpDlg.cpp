// ChooseExport.cpp : implementation file
//

#include "stdafx.h"
#include "velman.h"
#include "VelmanDoc.h"
#include "GridExpDlg.h"
#include "mainfrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChooseExport dialog

CGridExportDlg::CGridExportDlg(CWnd* pParent /*=NULL*/)
: CDialog(CGridExportDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CChooseExport)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CGridExportDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChooseExport)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CGridExportDlg, CDialog)
	//{{AFX_MSG_MAP(CChooseExport)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChooseExport message handlers

void CGridExportDlg::OnOK()
{
	CComboBox	*pCB = (CComboBox *)GetDlgItem(IDC_CHOOSEEXPORT_GRIDFILETYPE);
	CListBox	*pLB = (CListBox *)GetDlgItem(IDC_CHOOSEEXPORT_LIST);
	int click[128], i, j, l;

	numgrids=pLB->GetSelCount();
	chosengrids=new int[numgrids];
	if(chosengrids)
	{
		pLB->GetSelItems(128, click);

		l=0;
		for(j=0;j<numgrids;j++)
		{
			for(i=0;i<entries.GetSize();i++)
			{
				if( ((entries[i])&0xFF) == click[j])
				{
					chosengrids[l++]=entries[i]>>8;
					break;
				}
			}
		}
		numgrids=l;	// might be smaller than GetSelCount, if invalid items selected!
		if(l>0)
			CDialog::OnOK();
		else
		{
			delete chosengrids;
			chosengrids=NULL;
		}
	}

	OutputFileType = pCB->GetCurSel();

	CDialog::OnOK();
}

BOOL CGridExportDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CListBox *pLB=(CListBox *)GetDlgItem(IDC_CHOOSEEXPORT_LIST);
	CComboBox *pCB = (CComboBox *)GetDlgItem(IDC_CHOOSEEXPORT_GRIDFILETYPE);

	if(pDoc->zimscube.FillListboxWithGridnames(pLB, entries)==0)
	{
		AfxMessageBox("There are no grids to display", MB_OK | MB_ICONINFORMATION);
		OnCancel();		// give it up if there are no grids around
	}

	pLB->SetCurSel(1);
	pLB->SetTabStops(15);

	pCB->SetCurSel(pDoc->GridFileType);	//	set grid file type selection, remember we have ASCII as first!

	SetIcon (AfxGetApp()->LoadIcon(IDR_MAINFRAME), FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
