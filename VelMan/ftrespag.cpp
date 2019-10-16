// FTResPag.cpp : implementation file
//

#include "stdafx.h"
#include "VelmanDoc.h"
#include "finetune.h"
#include "ft_sheet.h"
#include "velman.h"
#include "FTResPag.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFinetuneResidualsPage dialog

CFinetuneResidualsPage::CFinetuneResidualsPage(CFinetuneSheet *mysheet)
: CPropertyPage(CFinetuneResidualsPage::IDD)
{
	//{{AFX_DATA_INIT(CFinetuneResidualsPage)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	sheet=mysheet;
}

CFinetuneResidualsPage::CFinetuneResidualsPage()
{
}

void CFinetuneResidualsPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFinetuneResidualsPage)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CFinetuneResidualsPage, CDialog)
	//{{AFX_MSG_MAP(CFinetuneResidualsPage)
	ON_BN_CLICKED(IDC_FINETUNE_HIDEME, OnFinetuneHideme)
	ON_BN_CLICKED(IDC_FINETUNE_SWITCHVIEWMODE, OnFinetuneSwitchviewmode)
	ON_WM_PAINT()
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_FINETUNE_NORMGRAD, OnFinetuneNormgrad)
	ON_BN_CLICKED(IDC_RESIDUALS_INZONLY, GrayStuff)
	ON_BN_CLICKED(IDC_RESIDUALS_TOTALDESELECTION, OnResidualsTotaldeselection)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFinetuneResidualsPage message handlers

void CFinetuneResidualsPage::OnCancel()
{
	sheet->OnCancel();
}

void CFinetuneResidualsPage::OnFinetuneHideme()
{
	sheet->Hideme();
}

void CFinetuneResidualsPage::OnOK()
{
	sheet->OnOK();
}

BOOL CFinetuneResidualsPage::OnInitDialog()
{
	sheet->HaveDisplayedResidualsPage=TRUE;
	pView=pDoc->pFTview;
	horizon=(CHorizon *)((pDoc->horizonlist).GetAt(layer));

	// find out about how to set the switches
	ResidualHandling=horizon->HowToHandleWellResiduals;
	CheckDlgButton(IDC_RESIDUALS_TOTALDESELECTION, ResidualHandling & WELL_MODEL_NORESIDUALSIFDESELECTED);
	CheckDlgButton(IDC_RESIDUALS_INZONLY, ResidualHandling & WELL_MODEL_NORESIDUALSATALL);

	//set which radio button checked
	if(ResidualHandling & WELL_MODEL_MULTIPLICATIVERESIDUALS)
		CheckRadioButton(IDC_RESIDUAL_ADDRESID, IDC_RESIDUAL_MULTRESID, IDC_RESIDUAL_MULTRESID);
	else if(ResidualHandling & 	WELL_MODEL_ADDITIVERESIDUALS)
		CheckRadioButton(IDC_RESIDUAL_ADDRESID, IDC_RESIDUAL_MULTRESID, IDC_RESIDUAL_ADDRESID);
	else
	{
		//CheckRadioButton(IDC_RESIDUAL_ADDRESID, IDC_RESIDUAL_HYBRID, IDC_RESIDUAL_HYBRID);
		CheckRadioButton(IDC_RESIDUAL_ADDRESID, IDC_RESIDUAL_MULTRESID, IDC_RESIDUAL_MULTRESID);
	}

	CPropertyPage::OnInitDialog();
	SetDlgItemText(IDC_FINETUNE_SWITCHVIEWMODE, "Scatter &view");

	// gradient normalisation stuff
	CScrollBar *pSB=(CScrollBar *)GetDlgItem(IDC_FINETUNE_NORMGRAD_SCROLL);
	pSB->SetScrollRange(0, 100);
	pSB->SetScrollPos((int)(horizon->NormGradFactor));
	char buf[16];
	sprintf(buf, "%d%%", pSB->GetScrollPos());
	SetDlgItemText(IDC_RESIDUAL_NORMGRAD_SCROLLBUDDY, buf);
	CheckDlgButton(IDC_FINETUNE_NORMGRAD, horizon->NormalizeGradients);
	GrayStuff();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CFinetuneResidualsPage::OnFinetuneSwitchviewmode()
{
	// flip viewmode
	(pDoc->pFTview)->ViewMode=3-(pDoc->pFTview)->ViewMode;
	Invalidate();
	(pDoc->pFTview)->ZoomDepth=0;
	(pDoc->pFTview)->Invalidate();	// enforce redraw
}

void CFinetuneResidualsPage::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	int mode=(pDoc->pFTview)->ViewMode;
	CButton *pB=(CButton *)GetDlgItem(IDC_FINETUNE_SWITCHVIEWMODE);
	if(mode==VIEWMODE_SCATTER)
		pB->SetWindowText("Contour &view");
	else
		pB->SetWindowText("Scatter &view");
}

void CFinetuneResidualsPage::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	int pos=pScrollBar->GetScrollPos();
	BOOL UpdateView=TRUE;

	switch(nSBCode)
	{
	case SB_THUMBPOSITION:
		pScrollBar->SetScrollPos(nPos);
		break;
	case SB_THUMBTRACK:
		pScrollBar->SetScrollPos(nPos);
		UpdateView=FALSE;		// no updating while scrollbar is being moved (too slow)
		break;
	case SB_LINEUP:
		pScrollBar->SetScrollPos(max(pos-1, 10));
		break;
	case SB_LINEDOWN:
		pScrollBar->SetScrollPos(min(pos+1, 100));
		break;
	case SB_PAGELEFT:
		pScrollBar->SetScrollPos(max(pos-5, 10));
		break;
	case SB_PAGERIGHT:
		pScrollBar->SetScrollPos(min(pos+5, 100));
		break;
	}

	// update text in dialog box itself
	pos=pScrollBar->GetScrollPos();
	char buf[6];
	sprintf(buf, "%d%%", pos);
	SetDlgItemText(IDC_RESIDUAL_NORMGRAD_SCROLLBUDDY, buf);
	// and remember it within the horizon
	horizon->NormGradFactor=pos;
	if(UpdateView)
		pDoc->UpdateAllViews(NULL, DOC_CHANGE_FINETUNECHANGE);
}

void CFinetuneResidualsPage::OnFinetuneNormgrad()
{
	GrayStuff();
	horizon->NormalizeGradients=IsDlgButtonChecked(IDC_FINETUNE_NORMGRAD);
	pDoc->UpdateAllViews(NULL, DOC_CHANGE_FINETUNECHANGE);
}

BOOL CFinetuneResidualsPage::HowToHandleResiduals()
{
	BOOL answer=0;

	if(GetCheckedRadioButton(IDC_RESIDUAL_ADDRESID, IDC_RESIDUAL_MULTRESID)==IDC_RESIDUAL_MULTRESID)
		answer=WELL_MODEL_MULTIPLICATIVERESIDUALS;
	else if(GetCheckedRadioButton(IDC_RESIDUAL_ADDRESID, IDC_RESIDUAL_MULTRESID)==IDC_RESIDUAL_ADDRESID)
		answer=WELL_MODEL_ADDITIVERESIDUALS;

	if(IsDlgButtonChecked(IDC_RESIDUALS_TOTALDESELECTION))
		answer|= WELL_MODEL_NORESIDUALSIFDESELECTED;
	if(IsDlgButtonChecked(IDC_RESIDUALS_INZONLY))
		answer|= WELL_MODEL_NORESIDUALSATALL;
	return answer;
}

void CFinetuneResidualsPage::GrayStuff()
{
	if(IsDlgButtonChecked(IDC_RESIDUALS_INZONLY))
	{
		GetDlgItem(IDC_RESIDUALS_TOTALDESELECTION)->EnableWindow(FALSE);
		GetDlgItem(IDC_RESIDUAL_HYBRID)->EnableWindow(FALSE);
		GetDlgItem(IDC_RESIDUAL_ADDRESID)->EnableWindow(FALSE);
		GetDlgItem(IDC_RESIDUAL_MULTRESID)->EnableWindow(FALSE);
		SetDlgItemText(IDC_RESIDUAL_NOTIEWARN,
			"WARNING: No wells will specifically tie with the depth or interval velocity grid.");
	}
	else
	{
		GetDlgItem(IDC_RESIDUALS_TOTALDESELECTION)->EnableWindow(TRUE);
		GetDlgItem(IDC_RESIDUAL_HYBRID)->EnableWindow(TRUE);
		GetDlgItem(IDC_RESIDUAL_ADDRESID)->EnableWindow(TRUE);
		GetDlgItem(IDC_RESIDUAL_MULTRESID)->EnableWindow(TRUE);
		if(IsDlgButtonChecked(IDC_RESIDUALS_TOTALDESELECTION))
		{
			SetDlgItemText(IDC_RESIDUAL_NOTIEWARN,
				"WARNING: Those wells will not specifically tie with the depth or interval velocity grid.");
		}
		else
			SetDlgItemText(IDC_RESIDUAL_NOTIEWARN, "");
	}
}

void CFinetuneResidualsPage::OnResidualsTotaldeselection()
{
	horizon->HowToHandleWellResiduals = HowToHandleResiduals();
	GrayStuff();
	pDoc->pFTview->HaveValidContour=FALSE;
	pDoc->UpdateAllViews(NULL, DOC_CHANGE_FINETUNECHANGE);
}
