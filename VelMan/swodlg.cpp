// SWODlg.cpp : implementation file
//

#include "stdafx.h"
#include "velman.h"
#include "VelmanDoc.h"
#include "SWODlg.h"
#include <afxcmn.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSmoothWndOptDlg dialog

CSmoothWndOptDlg::CSmoothWndOptDlg(CWnd* pParent /*=NULL*/)
: CDialog(CSmoothWndOptDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSmoothWndOptDlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CSmoothWndOptDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_PRIMARY, SetAsPrimary);
	//{{AFX_DATA_MAP(CSmoothWndOptDlg)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSmoothWndOptDlg, CDialog)
	//{{AFX_MSG_MAP(CSmoothWndOptDlg)
	ON_WM_HSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSmoothWndOptDlg message handlers

BOOL CSmoothWndOptDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetAsPrimary = TRUE;
	CheckDlgButton(IDC_PRIMARY, SetAsPrimary);

	CSliderCtrl *pSC=(CSliderCtrl *)GetDlgItem(IDC_WNDSMOOTH_SLIDER);
	minsize=max(3, AfxGetApp()->GetProfileInt("ArithmeticParams", "MovingWindowMinsize", 3));
	maxsize=max((int)(AfxGetApp()->GetProfileInt("ArithmeticParams", "MovingWindowMaxsize", 90)), minsize);
	int pos=AfxGetApp()->GetProfileInt("ArithmeticParams", "MovingWindowPos", 7);
	if(pos > maxsize) pos = maxsize;
	if(pos < minsize) pos = minsize;
	orgminsize=minsize;
	pos = (pos-orgminsize)/2;
	minsize /= 2;
	maxsize /= 2;
	pSC->SetRange(0, (maxsize-minsize));
	pSC->SetPos(pos);
	pos=pSC->GetPos();
	pos=orgminsize+2*pos;
	char buf[64];
	sprintf(buf, "%d * %d nodes", pos, pos);
	SetDlgItemText(IDC_WNDSMOOTH_SIZEPREVIEW, buf);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CSmoothWndOptDlg::OnOK()
{
	CSliderCtrl *pSC=(CSliderCtrl *)GetDlgItem(IDC_WNDSMOOTH_SLIDER);
	windowsize=orgminsize+2*pSC->GetPos();
	KeepHighFrequencies = IsDlgButtonChecked(IDC_CHECK1);
	CDialog::OnOK();
}

void CSmoothWndOptDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CSliderCtrl *pSC=(CSliderCtrl *)pScrollBar;

	int pos=pSC->GetPos();

	switch(nSBCode)
	{
	case TB_THUMBPOSITION:
		pSC->SetPos(nPos);
		break;
	case TB_THUMBTRACK:
		pSC->SetPos(nPos);
		break;
	case TB_LINEUP:
		pSC->SetPos(max(pos-1, 0));
		break;
	case TB_LINEDOWN:
		pSC->SetPos(min(pos+1, maxsize-minsize));
		break;
	case TB_PAGEUP:
		pSC->SetPos(max(pos-3, 0));
		break;
	case TB_PAGEDOWN:
		pSC->SetPos(min(pos+3, maxsize-minsize));
		break;
	}

	// update text in dialog box itself
	pos=pSC->GetPos();
	pos=orgminsize+2*pos;
	char buf[64];
	sprintf(buf, "%d * %d nodes", pos, pos);
	SetDlgItemText(IDC_WNDSMOOTH_SIZEPREVIEW, buf);
}
