// ChooseAnyGrid.cpp : implementation file
//

#include "stdafx.h"
#include "velman.h"
#include "VelmanDoc.h"
#include "ChooseAGridDlg.h"
#include "mainfrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChooseAnyGrid dialog

CChooseAnyGrid::CChooseAnyGrid(CWnd* pParent /*=NULL*/)
: CDialog(CChooseAnyGrid::IDD, pParent)
{
	//{{AFX_DATA_INIT(CChooseAnyGrid)
	//}}AFX_DATA_INIT
}

void CChooseAnyGrid::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChooseAnyGrid)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CChooseAnyGrid, CDialog)
	//{{AFX_MSG_MAP(CChooseAnyGrid)
	ON_LBN_DBLCLK(IDC_CHOOSEGRID_LIST, OnOK)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChooseAnyGrid message handlers

BOOL CChooseAnyGrid::OnInitDialog()
{
	CDialog::OnInitDialog();

	CListBox *pLB=(CListBox *)GetDlgItem(IDC_CHOOSEGRID_LIST);

	if(pDoc->zimscube.FillListboxWithGridnames(pLB, entries)==0)
	{
		AfxMessageBox("There are no grids to display", MB_OK | MB_ICONINFORMATION);
		OnCancel();		// give it up if there are no grids around
	}

	pLB->SetCurSel(1);
	pLB->SetTabStops(15);
	SetWindowText(dlg_title);

	SetDlgItemText(IDC_CHOOSEGRID_WHATSORT, dlg_request);

	SetIcon (AfxGetApp()->LoadIcon(IDR_MAINFRAME), FALSE);

	::SetWindowPos(m_hWnd, HWND_TOPMOST,
		NULL, NULL,
		NULL, NULL, SWP_NOSIZE|SWP_NOMOVE);  // added this to force the dialog to be on top of the
	// contour windows

	return TRUE;
}

void CChooseAnyGrid::OnOK()
{
	CListBox *pLB=(CListBox *)GetDlgItem(IDC_CHOOSEGRID_LIST);
	int click, i;

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

void CChooseAnyGrid::SetDlgTexts(const char *title, const char *request)
{
	dlg_title=title;
	dlg_request=request;
}
