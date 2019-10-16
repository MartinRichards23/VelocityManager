// FT_QH1.cpp : implementation file
//

#include "stdafx.h"
#include "velman.h"
#include "VelmanDoc.h"
#include "finetune.h"
#include "ft_sheet.h"
#include "FT_QH1.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFinetuneQuickhelp1_Page property page

IMPLEMENT_DYNCREATE(CFinetuneQuickhelp1_Page, CPropertyPage)

CFinetuneQuickhelp1_Page::CFinetuneQuickhelp1_Page(CFinetuneSheet *mysheet) : CPropertyPage(CFinetuneQuickhelp1_Page::IDD)
{
	//{{AFX_DATA_INIT(CFinetuneQuickhelp1_Page)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	sheet=mysheet;
}

CFinetuneQuickhelp1_Page::CFinetuneQuickhelp1_Page()
{
}

CFinetuneQuickhelp1_Page::~CFinetuneQuickhelp1_Page()
{
}

void CFinetuneQuickhelp1_Page::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFinetuneQuickhelp1_Page)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CFinetuneQuickhelp1_Page, CPropertyPage)
	//{{AFX_MSG_MAP(CFinetuneQuickhelp1_Page)
	ON_BN_CLICKED(IDC_FINETUNE_HIDEME, OnFinetuneHideme)
	ON_BN_CLICKED(IDC_FINETUNE_SWITCHVIEWMODE, OnFinetuneSwitchviewmode)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFinetuneQuickhelp1_Page message handlers

void CFinetuneQuickhelp1_Page::OnOK()
{
	sheet->OnOK();
}

void CFinetuneQuickhelp1_Page::OnCancel()
{
	sheet->OnCancel();
}

void CFinetuneQuickhelp1_Page::OnFinetuneHideme()
{
	sheet->Hideme();
}

BOOL CFinetuneQuickhelp1_Page::OnInitDialog()
{
	CPropertyPage::OnInitDialog();
	SetDlgItemText(IDC_FINETUNE_SWITCHVIEWMODE, "Contour &view");
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CFinetuneQuickhelp1_Page::OnFinetuneSwitchviewmode()
{
	// flip viewmode
	(pDoc->pFTview)->ViewMode=3-(pDoc->pFTview)->ViewMode;
	Invalidate();
	(pDoc->pFTview)->ZoomDepth=0;
	(pDoc->pFTview)->Invalidate();	// enforce redraw
}

void CFinetuneQuickhelp1_Page::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	int mode=(pDoc->pFTview)->ViewMode;
	CButton *pB=(CButton *)GetDlgItem(IDC_FINETUNE_SWITCHVIEWMODE);
	if(mode==VIEWMODE_SCATTER)
		pB->SetWindowText("Contour &view");
	else
		pB->SetWindowText("Scatter &view");
}
