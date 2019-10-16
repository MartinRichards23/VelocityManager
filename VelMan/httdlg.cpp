// HTTDlg.cpp : implementation file
//

#include "stdafx.h"
#include "velman.h"
#include "HTTDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHowToTieDlg dialog

CHowToTieDlg::CHowToTieDlg(CWnd* pParent /*=NULL*/)
: CDialog(CHowToTieDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CHowToTieDlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CHowToTieDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHowToTieDlg)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CHowToTieDlg, CDialog)
	//{{AFX_MSG_MAP(CHowToTieDlg)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_HOWTOTIE_NORMGRAD, OnHowtotieNormgrad)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHowToTieDlg message handlers

BOOL CHowToTieDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CScrollBar *pSB=(CScrollBar *)GetDlgItem(IDC_HOWTOTIE_NORMGRAD_SCROLL);
	pSB->SetScrollRange(0, 100);
	pSB->SetScrollPos(50);
	SetDlgItemText(IDC_HOWTOTIE_NORMGRAD_SCROLLBUDDY, "50%");
	CheckDlgButton(IDC_HOWTOTIE_NORMGRAD, 1); // default "on" for normalisation
	GrayNormalisation();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CHowToTieDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	int pos=pScrollBar->GetScrollPos();

	switch(nSBCode)
	{
	case SB_THUMBPOSITION:
		pScrollBar->SetScrollPos(nPos);
		break;
	case SB_THUMBTRACK:
		pScrollBar->SetScrollPos(nPos);
		break;
	case SB_LINEUP:
		pScrollBar->SetScrollPos(max(pos-1, 0));
		break;
	case SB_LINEDOWN:
		pScrollBar->SetScrollPos(min(pos+1, 225));
		break;
	case SB_PAGELEFT:
		pScrollBar->SetScrollPos(max(pos-15, 0));
		break;
	case SB_PAGERIGHT:
		pScrollBar->SetScrollPos(min(pos+15, 225));
		break;
	}

	// update text in dialog box itself
	pos=pScrollBar->GetScrollPos();
	char buf[6];
	sprintf(buf, "%d%%", pos);
	SetDlgItemText(IDC_HOWTOTIE_NORMGRAD_SCROLLBUDDY, buf);
}

void CHowToTieDlg::OnHowtotieNormgrad()
{
	GrayNormalisation();
}

void CHowToTieDlg::GrayNormalisation()
{
	// the scrolling stuff will be grayed if the gradient normalisation is
	// disabled
	int enable=IsDlgButtonChecked(IDC_HOWTOTIE_NORMGRAD);
	GetDlgItem(IDC_HOWTOTIE_NORMGRAD_SCROLL)->EnableWindow(enable);
	GetDlgItem(IDC_HOWTOTIE_NORMGRAD_SCROLLBUDDY)->EnableWindow(enable);
	GetDlgItem(IDC_HOWTOTIE_NORMGRAD_SCROLLBLURP)->EnableWindow(enable);
}

void CHowToTieDlg::OnOK()
{
	// read gradient normalisation stuff
	NormGrad=IsDlgButtonChecked(IDC_HOWTOTIE_NORMGRAD);
	CScrollBar *pSB=(CScrollBar *)GetDlgItem(IDC_HOWTOTIE_NORMGRAD_SCROLL);
	NormGradFac=pSB->GetScrollPos();

	CDialog::OnOK();
}
