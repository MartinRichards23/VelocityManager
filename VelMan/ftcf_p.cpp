// FTCF_P.cpp : implementation file
//

#include "stdafx.h"
#include "VelmanDoc.h"
#include "finetune.h"
#include "ft_sheet.h"
#include "velman.h"
#include "FTCF_P.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFinetuneCustomfit_Page property page

IMPLEMENT_DYNCREATE(CFinetuneCustomfit_Page, CPropertyPage)

CFinetuneCustomfit_Page::CFinetuneCustomfit_Page(CFinetuneSheet *mysheet) : CPropertyPage(CFinetuneCustomfit_Page::IDD)
{
	//{{AFX_DATA_INIT(CFinetuneCustomfit_Page)
	m_customfita = 0.0;
	m_customfitb = 0.0;
	//}}AFX_DATA_INIT
	sheet=mysheet;
}

CFinetuneCustomfit_Page::CFinetuneCustomfit_Page()
{
}

CFinetuneCustomfit_Page::~CFinetuneCustomfit_Page()
{
}

void CFinetuneCustomfit_Page::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFinetuneCustomfit_Page)
	DDX_Text(pDX, IDC_FINETUNE_CUSTOMFITA, m_customfita);
	DDX_Text(pDX, IDC_FINETUNE_CUSTOMFITB, m_customfitb);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CFinetuneCustomfit_Page, CPropertyPage)
	//{{AFX_MSG_MAP(CFinetuneCustomfit_Page)
	ON_BN_CLICKED(IDC_FINETUNE_HIDEME, OnFinetuneHideme)
	ON_BN_CLICKED(IDC_FINETUNE_SWITCHVIEWMODE, OnFinetuneSwitchviewmode)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_FINETUNE_USECUSTOMFIT, OnFinetuneDisplaycustomfit)
	ON_EN_KILLFOCUS(IDC_FINETUNE_CUSTOMFITA, OnKillfocusFinetuneCustomfit)
	ON_BN_CLICKED(IDC_FINETUNE_DISPLAYCUSTOMFIT, OnFinetuneDisplaycustomfit)
	ON_EN_KILLFOCUS(IDC_FINETUNE_CUSTOMFITB, OnKillfocusFinetuneCustomfit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFinetuneCustomfit_Page message handlers

void CFinetuneCustomfit_Page::OnCancel()
{
	sheet->OnCancel();
}

void CFinetuneCustomfit_Page::OnOK()
{
	sheet->OnOK();
}

void CFinetuneCustomfit_Page::OnFinetuneHideme()
{
	sheet->Hideme();
}

BOOL CFinetuneCustomfit_Page::OnInitDialog()
{
	int model;

	pView=pDoc->pFTview;
	horizon=(CHorizon *)((pDoc->horizonlist).GetAt(layer));
	model=horizon->ModelNumber;

	// 3/1/00
	// these are full precision now
	//m_customfita=ceil(10000.0*horizon->FitA)/10000.0;
	//m_customfitb=ceil(10000.0*horizon->FitB)/10000.0;
	if ( model == 0 )       // only the first model uses a non-linear fit
	{
		m_customfita = ab2V(horizon->FitA, horizon->FitB);
		m_customfitb = ab2K(horizon->FitA, horizon->FitB);
	}
	else
	{
		m_customfita = horizon->FitA;
		m_customfitb = horizon->FitB;
	}

	CPropertyPage::OnInitDialog();

	SetDlgItemText(IDC_FINETUNE_CUSTOMFITANAME, pDoc->well_model_lib[model]->parnameA);
	SetDlgItemText(IDC_FINETUNE_CUSTOMFITBNAME, pDoc->well_model_lib[model]->parnameB);
	CheckDlgButton(IDC_FINETUNE_USECUSTOMFIT, horizon->UseCustomFit);
	SetDlgItemText(IDC_FINETUNE_SWITCHVIEWMODE, "Contour &view");

	// if the model is "contoured intvels", we hide/disable quite a few dlg elements
	if(model==10)
	{
		GetDlgItem(IDC_FINETUNE_CUSTOMFITANAME)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_FINETUNE_CUSTOMFITBNAME)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_FINETUNE_CUSTOMFITA)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_FINETUNE_CUSTOMFITB)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_FINETUNE_USECUSTOMFIT)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_FINETUNE_DISPLAYCUSTOMFIT)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CUSTOMFITFRAME)->ShowWindow(SW_HIDE);
		SetDlgItemText(IDC_FINETUNE_CUSTOMFITBLURB, "Custom fit feature not"
			" available for this model.");
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CFinetuneCustomfit_Page::OnFinetuneSwitchviewmode()
{
	// flip viewmode
	(pDoc->pFTview)->ViewMode=3-(pDoc->pFTview)->ViewMode;
	Invalidate();
	(pDoc->pFTview)->ZoomDepth=0;
	(pDoc->pFTview)->Invalidate();	// enforce redraw
}

void CFinetuneCustomfit_Page::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	int mode=(pDoc->pFTview)->ViewMode;
	CButton *pB=(CButton *)GetDlgItem(IDC_FINETUNE_SWITCHVIEWMODE);
	if(mode==VIEWMODE_SCATTER)
		pB->SetWindowText("Contour &view");
	else
		pB->SetWindowText("Scatter &view");
}

void CFinetuneCustomfit_Page::OnFinetuneDisplaycustomfit()
{
	int model=horizon->ModelNumber;

	UpdateData();
	horizon->UseCustomFit=IsDlgButtonChecked(IDC_FINETUNE_USECUSTOMFIT);

	if ( model == 0 )
	{
		horizon->CustomFitA = VK2a(m_customfita, m_customfitb);
		horizon->CustomFitB = VK2b(m_customfita, m_customfitb);
	}
	else
	{
		horizon->CustomFitA=m_customfita;
		horizon->CustomFitB=m_customfitb;
	}
	pView->Invalidate();
	pDoc->horizonlist.FitLayer(layer, pDoc);
	pDoc->UpdateAllViews(NULL, DOC_CHANGE_FINETUNECHANGE);
	sheet->ShowFittingParams();
}

void CFinetuneCustomfit_Page::OnKillfocusFinetuneCustomfit()
{
	int model=horizon->ModelNumber;

	UpdateData();
	if ( model == 0 )
	{
		horizon->CustomFitA = VK2a(m_customfita, m_customfitb);
		horizon->CustomFitB = VK2b(m_customfita, m_customfitb);
	}
	else
	{
		horizon->CustomFitA=m_customfita;
		horizon->CustomFitB=m_customfitb;
	}
	horizon->UseCustomFit=IsDlgButtonChecked(IDC_FINETUNE_USECUSTOMFIT);
}
