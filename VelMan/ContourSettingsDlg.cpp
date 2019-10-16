// ContSet.cpp : implementation file

#include "stdafx.h"
#include "velman.h"
#include "VelmanDoc.h"
#include "ContourSettingsDlg.h"
#include "global.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CContourSettingsDlg dialog

CContourSettingsDlg::CContourSettingsDlg(CWnd* pParent /*=NULL*/)
: CDialog(CContourSettingsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CContourSettingsDlg)
	m_numlabels = 1;
	m_numlines  = -1;
	m_showwells = 1;
	m_showfaults = 0;
	m_showshotpoints = 0;
	m_palette = 1;
	m_showlabels = FALSE;
	m_colorshading = FALSE;
	//}}AFX_DATA_INIT
}

void CContourSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CContourSettingsDlg)
	DDX_Radio(pDX, IDC_CONTSET_FEWLABELS, m_numlabels);
	DDX_Radio(pDX, IDC_CONTSET_NUMLEVS_NONE, m_numlines);
	DDX_Check(pDX, IDC_CONTSET_SHOWWELLS, m_showwells);
	DDX_Check(pDX, IDC_CONTSET_SHOWFAULTS, m_showfaults);
	DDX_Check(pDX, IDC_CONTSET_SHOWSHOTPOINTS, m_showshotpoints);
	DDX_Radio(pDX, IDC_CONTSET_PAL_RAIN, m_palette);
	DDX_Check(pDX, IDC_CONTSET_SHOWLABELS, m_showlabels);
	DDX_Check(pDX, IDC_CONTSET_USESHADING, m_colorshading);
	//}}AFX_DATA_MAP
}

#define theDerivedClass CContourSettingsDlg ::
BEGIN_MESSAGE_MAP(CContourSettingsDlg, CDialog)
	//{{AFX_MSG_MAP(CContourSettingsDlg)
	ON_BN_CLICKED(IDC_CONTSET_CHANGETEXTCOL, OnContsetChangetextcol)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_CONTSET_SHOWLABELS, OnContsetShowlabels)
	ON_BN_CLICKED(IDC_CONTSET_LABELSMALLER, OnContsetLabelsmaller)
	ON_BN_CLICKED(IDC_CONTSET_LABELLARGER, OnContsetLabellarger)
	ON_BN_CLICKED(IDC_CONTSET_NUMLEVS_NONE, OnContsetNumlevels)
	ON_BN_CLICKED(IDC_CONTSET_NUMLEVS_LOW, OnContsetNumlevsLow)
	ON_BN_CLICKED(IDC_CONTSET_NUMLEVS_NORM, OnContsetNumlevsNorm)
	ON_BN_CLICKED(IDC_CONTSET_NUMLEVS_HIGH, OnContsetNumlevsHigh)
	ON_WM_KILLFOCUS()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_CONTSET_SHOWFAULTS, OnContsetShowfaults)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#undef theDerivedClass

/////////////////////////////////////////////////////////////////////////////
// CContourSettingsDlg message handlers

void CContourSettingsDlg::OnContsetChangetextcol()
{
	CColorDialog dlg;
	dlg.m_cc.rgbResult=m_textcol;
	dlg.m_cc.Flags|=CC_RGBINIT;
	dlg.DoModal();
	m_textcol=dlg.m_cc.rgbResult;
	Invalidate();
}

BOOL CContourSettingsDlg::OnInitDialog()
{
	char buf[128];

	if(showstuff)
	{
		m_showfaults=showstuff->showfaults;
		m_showwells=showstuff->showwells;
		m_showshotpoints=showstuff->showshotpoints;
	}

	CDialog::OnInitDialog();

	CheckDlgButton(IDC_CONTSET_USESHADING, m_colorshading);
	CheckDlgButton(IDC_CONTSET_SHOWLABELS, m_showlabels);
	GrayItems();

	// hide the zooming info if not required
	if(options & CNTDLG_OPT_NOZOOM)
		GetDlgItem(IDC_CONTSET_ZOOMNOTE)->ShowWindow(SW_HIDE);

	// hide all the checkboxes which are not required
	if(!(options & CNTDLG_SHOWFAULTS))
		GetDlgItem(IDC_CONTSET_SHOWFAULTS)->ShowWindow(SW_HIDE);
	if(!(options & CNTDLG_SHOWWELLS))
		GetDlgItem(IDC_CONTSET_SHOWWELLS)->ShowWindow(SW_HIDE);
	if(!(options & CNTDLG_SHOWSHOTPOINTS))
		GetDlgItem(IDC_CONTSET_SHOWSHOTPOINTS)->ShowWindow(SW_HIDE);

	// if there are no checkboxes at all, hide their frame
	if(!(options & (CNTDLG_SHOWWELLS | CNTDLG_SHOWFAULTS | CNTDLG_SHOWSHOTPOINTS)))
		GetDlgItem(IDC_OTHERELEMENTSFRAME)->ShowWindow(SW_HIDE);

	ShowLabelSize();

	// initialize color samples
	sprintf(buf, "(%5.3f)", minval);
	SetDlgItemText(IDC_CONTSET_MINVAL, buf);
	sprintf(buf, "(%5.3f)", maxval);
	SetDlgItemText(IDC_CONTSET_MAXVAL, buf);

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CContourSettingsDlg::sQuit()
{
	CDialog::OnCancel();
}

void CContourSettingsDlg::OnOK()
{
	UpdateData();
	m_showlabels=IsDlgButtonChecked(IDC_CONTSET_SHOWLABELS);
	m_labelnum=(m_numlabels == 0 ? 4 : (m_numlabels == 1 ? 2 : 1) );
	m_colorshading=IsDlgButtonChecked(IDC_CONTSET_USESHADING);

	//  Check to see if user has turned everything off by mistake
	if ( !m_colorshading && IsDlgButtonChecked(IDC_CONTSET_NUMLEVS_NONE) )
	{
		m_colorshading = 1;     // turn colour shading back on
		CheckDlgButton(IDC_CONTSET_USESHADING, m_colorshading);
		AfxMessageBox( "You have turned off both contouring and colour fill.\n"
			"As this would result in a blank plot, colour fill has been "
			"turned back on.", MB_ICONINFORMATION );
	}

	if(showstuff)
	{
		showstuff->showfaults=m_showfaults;
		showstuff->showwells=m_showwells;
		showstuff->showshotpoints=m_showshotpoints;
	}
	CDialog::OnOK();
}

void CContourSettingsDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CWnd *pS;
	CDC *pControlDC;
	CBrush tbrush(m_textcol);
	CBrush minbrush(mincol);
	CBrush maxbrush(maxcol);
	CBrush *oldbrush;

	pS=GetDlgItem(IDC_CONTSET_TEXTCOLPREVIEW);
	pControlDC = pS->GetDC();
	pS->Invalidate();
	pS->UpdateWindow();
	oldbrush=pControlDC->SelectObject(&tbrush);
	pControlDC->Rectangle(0, 0, 14, 14);
	pControlDC->SelectObject(oldbrush);
	pS->ReleaseDC(pControlDC);

	pS=GetDlgItem(IDC_CONTSET_MAXCOL);
	pControlDC = pS->GetDC();
	pS->Invalidate();
	pS->UpdateWindow();
	oldbrush=pControlDC->SelectObject(&maxbrush);
	pControlDC->Rectangle(0, 0, 14, 14);
	pControlDC->SelectObject(oldbrush);
	pS->ReleaseDC(pControlDC);

	pS=GetDlgItem(IDC_CONTSET_MINCOL);
	pControlDC = pS->GetDC();
	pS->Invalidate();
	pS->UpdateWindow();
	oldbrush=pControlDC->SelectObject(&minbrush);
	pControlDC->Rectangle(0, 0, 14, 14);
	pControlDC->SelectObject(oldbrush);
	pS->ReleaseDC(pControlDC);
}

void CContourSettingsDlg::OnContsetShowlabels()
{
	GrayItems();	// disable labelling options if labelling itself switched off
}

void CContourSettingsDlg::ShowLabelSize()
{
	char buf[32];
	sprintf(buf, "Font size: %d pts", m_labelsize);
	SetDlgItemText(IDC_CONTSET_LABELSIZEPREVIEW, buf);
}

void CContourSettingsDlg::GrayItems()
{   //  Controls greying out the contour line labelling options.
	//  All options are greyed out if 'None' contour is selected.
	//  Otherwise, only labelling options are disabled if 'Show labels'
	//  is not checked.

	BOOL enableall    = !IsDlgButtonChecked(IDC_CONTSET_NUMLEVS_NONE);
	BOOL enablelabels = IsDlgButtonChecked(IDC_CONTSET_SHOWLABELS);
	BOOL enable = enableall && enablelabels;

	GetDlgItem(IDC_CONTSET_SHOWLABELS)->EnableWindow(enableall);

	GetDlgItem(IDC_CONTSET_FEWLABELS)->EnableWindow(enable);
	GetDlgItem(IDC_CONTSET_NORMALLABELS)->EnableWindow(enable);
	GetDlgItem(IDC_CONTSET_MANYLABELS)->EnableWindow(enable);
}

void CContourSettingsDlg::OnContsetLabelsmaller()
{
	m_labelsize=max(6, m_labelsize-1);
	ShowLabelSize();
}

void CContourSettingsDlg::OnContsetLabellarger()
{
	m_labelsize=min(36, m_labelsize+1);
	ShowLabelSize();
}

void CContourSettingsDlg::OnContsetNumlevels()
{
	GrayItems();
}

void CContourSettingsDlg::OnContsetNumlevsLow()
{
	GrayItems();
}

void CContourSettingsDlg::OnContsetNumlevsNorm()
{
	GrayItems();
}

void CContourSettingsDlg::OnContsetNumlevsHigh()
{
	GrayItems();
}

void CContourSettingsDlg::OnKillFocus(CWnd* pNewWnd)
{
	CDialog::OnKillFocus(pNewWnd);
}

void CContourSettingsDlg::OnFinalRelease()
{
	// TODO: Add your specialized code here and/or call the base class

	CDialog::OnFinalRelease();
}

BOOL CContourSettingsDlg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// TODO: Add your specialized code here and/or call the base class

	return CDialog::OnNotify(wParam, lParam, pResult);
}

void CContourSettingsDlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default

	CDialog::OnClose();
}

void CContourSettingsDlg::OnContsetShowfaults()
{
	// TODO: Add your control notification handler code here

}