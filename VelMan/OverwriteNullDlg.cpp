// OverwriteNULL.cpp : implementation file

#include "stdafx.h"
#include "velman.h"
#include "VelmanDoc.h"
#include "OverwriteNullDlg.h"
#include "mainfrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COverwriteNULL dialog

COverwriteNULL::COverwriteNULL(CWnd* pParent /*=NULL*/)
: CDialog(COverwriteNULL::IDD, pParent)
{
	//{{AFX_DATA_INIT(COverwriteNULL)
	m_overwrite_value = 0.0;
	//}}AFX_DATA_INIT
}

void COverwriteNULL::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COverwriteNULL)
	DDX_Text(pDX, IDC_OVERWRITENULL_VALUE, m_overwrite_value);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(COverwriteNULL, CDialog)
	//{{AFX_MSG_MAP(COverwriteNULL)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COverwriteNULL message handlers

BOOL COverwriteNULL::OnInitDialog()
{
	CDialog::OnInitDialog();

	CListBox *pLB=(CListBox *)GetDlgItem(IDC_OVERWRITE_LIST);

	if(pDoc->zimscube.FillListboxWithGridnames(pLB, entries)==0)
	{
		AfxMessageBox("There are no grids to display", MB_OK | MB_ICONINFORMATION);
		OnCancel();		// give it up if there are no grids around
	}

	pLB->SetCurSel(1);
	pLB->SetTabStops(15);

	SetIcon (AfxGetApp()->LoadIcon(IDR_MAINFRAME), FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void COverwriteNULL::OnOK()
{
	CListBox *pLB=(CListBox *)GetDlgItem(IDC_OVERWRITE_LIST);
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
