// ChooseManyGridsDlg.cpp : implementation file

#include "stdafx.h"
#include "velman.h"
#include "VelmanDoc.h"
#include "ChooseManyGridsDlg.h"
#include "mainfrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChooseManyGridsDlg dialog

CChooseManyGridsDlg::CChooseManyGridsDlg(CWnd* pParent /*=NULL*/)
: CDialog(CChooseManyGridsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CChooseManyGridsDlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	chosengrids=NULL;
	OnlyTimeGrids=FALSE;
}

void CChooseManyGridsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChooseManyGridsDlg)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CChooseManyGridsDlg, CDialog)
	//{{AFX_MSG_MAP(CChooseManyGridsDlg)
	ON_LBN_DBLCLK(IDC_CHOOSEGRID_LIST, OnOK)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChooseManyGridsDlg message handlers

BOOL CChooseManyGridsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CListBox *pLB=(CListBox *)GetDlgItem(IDC_CHOOSEGRID_LIST);

	if(pDoc->zimscube.FillListboxWithGridnames(pLB, entries, OnlyTimeGrids)==0)
	{
		AfxMessageBox("There are no grids to display", MB_OK | MB_ICONINFORMATION);
		OnCancel();		// give it up if there are no grids around
	}

	//pLB->SetSel(1);
	pLB->SetTabStops(15);
	SetWindowText(dlg_title);
	SetDlgItemText(IDC_CHOOSEGRID_WHATSORT, dlg_request);
	numgrids=0;

	SetIcon (AfxGetApp()->LoadIcon(IDR_MAINFRAME), FALSE);

	return TRUE;
}

void CChooseManyGridsDlg::SetDlgTexts(const char *title, const char *request)
{
	dlg_title=title;
	dlg_request=request;
}

void CChooseManyGridsDlg::OnOK()
{
	CListBox *pLB=(CListBox *)GetDlgItem(IDC_CHOOSEGRID_LIST);
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
}