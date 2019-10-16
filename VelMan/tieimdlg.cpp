// TieImDlg.cpp : implementation file
//

#include "stdafx.h"
#include "velman.h"
#include "VelmanDoc.h"
#include "TieImDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTieAfterImportDlg dialog

CTieAfterImportDlg::CTieAfterImportDlg(CWnd* pParent /*=NULL*/)
: CDialog(CTieAfterImportDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTieAfterImportDlg)
	m_how = -1;
	m_tieGrids = FALSE;
	//}}AFX_DATA_INIT
}

void CTieAfterImportDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTieAfterImportDlg)
	DDX_Radio(pDX, IDC_AFTERTIE1, m_how);
	DDX_Check(pDX, IDC_TIE, m_tieGrids);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTieAfterImportDlg, CDialog)
	//{{AFX_MSG_MAP(CTieAfterImportDlg)
	ON_BN_CLICKED(IDC_TIE, OnTie)
	ON_BN_CLICKED(IDC_WELLTIME_NORMGRAD, OnWelltimeNormgrad)
	ON_WM_HSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTieAfterImportDlg message handlers

void CTieAfterImportDlg::OnTie()
{
	GrayNormalisation();
}

void CTieAfterImportDlg::OnWelltimeNormgrad()
{
	GrayNormalisation();
}

void CTieAfterImportDlg::GrayNormalisation()
{
	// the gradient normalisation options are only required if the "tie grids"
	// option has been checked in the first place (and if this option CAN be
	// checked, i.e. if the button is ON and NOT DISABLED. It is disabled if we
	// do not have well data)

	CButton *pB=(CButton *)GetDlgItem(IDC_TIE);
	int enable=(  (pB->GetCheck()==1 && pB->IsWindowEnabled()) ? 1 : 0);
	GetDlgItem(IDC_WELLTIME_NORMGRAD)->EnableWindow(enable);
	// the "how to retain zero isochronochore regions" will be grayed s well
	GetDlgItem(IDC_STATIC_RETAINFRAME)->EnableWindow(enable);
	GetDlgItem(IDC_STATIC_RETAINBLURB)->EnableWindow(enable);
	GetDlgItem(IDC_AFTERTIE1)->EnableWindow(enable);
	GetDlgItem(IDC_AFTERTIE2)->EnableWindow(enable);
	GetDlgItem(IDC_AFTERTIE3)->EnableWindow(enable);
	GetDlgItem(IDC_AFTERTIE4)->EnableWindow(enable);
	// the scrolling stuff will also be grayed if the gradient normalisation is
	// disabled
	pB=(CButton *)GetDlgItem(IDC_WELLTIME_NORMGRAD);
	if(pB->GetCheck()==0)
		enable=FALSE;
	GetDlgItem(IDC_WELLTIME_NORMGRAD_SCROLL)->EnableWindow(enable);
	GetDlgItem(IDC_WELLTIME_NORMGRAD_SCROLLBUDDY)->EnableWindow(enable);
	GetDlgItem(IDC_WELLTIME_NORMGRAD_SCROLLBLURP)->EnableWindow(enable);

}

void CTieAfterImportDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
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
		pScrollBar->SetScrollPos(min(pos+1, 100));
		break;
	case SB_PAGELEFT:
		pScrollBar->SetScrollPos(max(pos-5, 0));
		break;
	case SB_PAGERIGHT:
		pScrollBar->SetScrollPos(min(pos+5, 100));
		break;
	}

	// update text in dialog box itself
	pos=pScrollBar->GetScrollPos();
	char buf[6];
	sprintf(buf, "%d%%", pos);
	SetDlgItemText(IDC_WELLTIME_NORMGRAD_SCROLLBUDDY, buf);
}

BOOL CTieAfterImportDlg::OnInitDialog()
{
	m_how=1;
	CDialog::OnInitDialog();

	//if using pseudo velocities, tieing makes no sense, so disable it
	if(pDoc->UsePseudoVelocities == Pseudo_All)
	{
		AfxMessageBox("You are using pseudo velocities, therefore tieing has been disabled");

		CButton* pBtn = (CButton*) GetDlgItem(IDC_TIE);
		pBtn->SetCheck(0);// uncheck it
		pBtn->EnableWindow(0);// disable it
	}

	CheckDlgButton(IDC_WELLTIME_NORMGRAD, 1); // default "on" for normalisation
	GrayNormalisation();  // enable or disable the gradient normalisation stuff
	// take control of the scrollbar, if it will ever be enabled
	if(pDoc->ProjectScope!=SCOPE_TIMERMS)
	{
		CScrollBar *pSB=(CScrollBar *)GetDlgItem(IDC_WELLTIME_NORMGRAD_SCROLL);
		pSB->SetScrollRange(0, 100);
		pSB->SetScrollPos(50);
	}
	SetDlgItemText(IDC_WELLTIME_NORMGRAD_SCROLLBUDDY, "50%");

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CTieAfterImportDlg::OnOK()
{
	// read gradient normalisation stuff
	NormGrad=IsDlgButtonChecked(IDC_WELLTIME_NORMGRAD);
	CScrollBar *pSB=(CScrollBar *)GetDlgItem(IDC_WELLTIME_NORMGRAD_SCROLL);
	NormGradFac=pSB->GetScrollPos();

	CDialog::OnOK();
}
