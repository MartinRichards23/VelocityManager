// FTF_P.cpp : implementation file

#include "stdafx.h"
#include "velman.h"
#include "VelmanDoc.h"
#include "finetune.h"
#include "ft_sheet.h"
#include "FTF_P.h"
#include "WarnoModDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFinetuneFitting_Page property page

IMPLEMENT_DYNCREATE(CFinetuneFitting_Page, CPropertyPage)

CFinetuneFitting_Page::CFinetuneFitting_Page(CFinetuneSheet *mysheet) : CPropertyPage(CFinetuneFitting_Page::IDD)
{
	//{{AFX_DATA_INIT(CFinetuneFitting_Page)
	//}}AFX_DATA_INIT
	sheet = mysheet;
}

CFinetuneFitting_Page::CFinetuneFitting_Page()
{
}

CFinetuneFitting_Page::~CFinetuneFitting_Page()
{
}

void CFinetuneFitting_Page::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFinetuneFitting_Page)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CFinetuneFitting_Page, CPropertyPage)
	//{{AFX_MSG_MAP(CFinetuneFitting_Page)
	ON_BN_CLICKED(IDC_FINETUNE_HIDEME, OnFinetuneHideme)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_FINETUNE_ADDBACKALL, OnFinetuneAddbackall)
	ON_BN_CLICKED(IDC_FINETUNE_SWITCHVIEWMODE, OnFinetuneSwitchviewmode)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_FINETUNE_OTHERMODEL, OnFinetuneOthermodel)
	ON_BN_CLICKED(IDC_WELLLIMITS, OnFinetuneShowallwells)
	ON_BN_CLICKED(IDC_GRIDLIMITS, OnFinetuneShowallwells)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFinetuneFitting_Page message handlers

void CFinetuneFitting_Page::OnCancel()
{
	sheet->OnCancel();
}

void CFinetuneFitting_Page::OnOK()
{
	UpdateData(TRUE);
	sheet->OnOK();
}

void CFinetuneFitting_Page::OnFinetuneHideme()
{
	sheet->Hideme();
}

void CFinetuneFitting_Page::ShowFittingParams()
{
	// Write values of fitting parameters into dialog box
	char bufa[32], bufb[32];
	int model = horizon->ModelNumber;

	if (horizon->HaveFit)
	{
		if (model == 0)
		{
			sprintf(bufa, "%-12.2f", ab2V(horizon->FitA, horizon->FitB));
			sprintf(bufb, "%-8.4f", ab2K(horizon->FitA, horizon->FitB));
		}
		else
		{
			sprintf(bufa, "%-16.12g", horizon->FitA);		// increased the precision to 12 dec.pl.
			sprintf(bufb, "%-16.12g", horizon->FitB);
		}
		SetDlgItemText(IDC_FINETUNE_PARVAL1, bufa);
		SetDlgItemText(IDC_FINETUNE_PARVAL2, bufb);
		if (horizon->UseCustomFit)
			sprintf(bufa, "[custom fit]");
		else
			sprintf(bufa, "%-5.2f%%", (horizon->FitGoodness));
		SetDlgItemText(IDC_FINETUNE_FITVAL, bufa);
		if (horizon->FitA >= 0.0)
		{
			GetDlgItem(IDC_FINETUNE_SLOPEWARN)->ShowWindow(FALSE);
			GetDlgItem(IDC_FINETUNE_SLOPEWARN2)->ShowWindow(FALSE);
		}
		else
		{
			GetDlgItem(IDC_FINETUNE_SLOPEWARN)->ShowWindow(TRUE);
			GetDlgItem(IDC_FINETUNE_SLOPEWARN2)->ShowWindow(TRUE);
		}
	}
	else
	{
		SetDlgItemText(IDC_FINETUNE_PARVAL1, "?");
		SetDlgItemText(IDC_FINETUNE_PARVAL2, "?");
		SetDlgItemText(IDC_FINETUNE_FITVAL, "?");
	}
}

BOOL CFinetuneFitting_Page::OnInitDialog()
{
	pView = pDoc->pFTview;
	horizon = (CHorizon *)((pDoc->horizonlist).GetAt(layer));
	model = horizon->ModelNumber;
	weight = horizon->DistWeight;
	m_ExcludePercent = horizon->ExcludePercent;

	CPropertyPage::OnInitDialog();

	// the checkboxes
	int check;
	check = AfxGetApp()->GetProfileInt("Attributes", "ShowAllWells", 1);
	CheckDlgButton(IDC_WELLLIMITS, check);

	// find out number of points. This is called "wellnum", thus number of wells, in
	// this dialog, altough it might actually be less than that, as not every well
	// need have a measurementpt in this layer.
	wellnum = pView->numpts;

	// place function number into internal variable and write it into dialog
	SetDlgItemText(IDC_FINETUNE_FUNCNAME, pDoc->well_model_lib[model]->name);
	SetDlgItemText(IDC_FINETUNE_FUNCDISTWGT, horizon->DistWeight == 0 ?
		"linear residual weighting" : "quadratic residual weighting");
	SetDlgItemText(IDC_FINETUNE_PARNAME1, pDoc->well_model_lib[model]->parnameA);
	SetDlgItemText(IDC_FINETUNE_PARNAME2, pDoc->well_model_lib[model]->parnameB);
	ShowFittingParams();	// write fitting parameters into dlg box

	// Take control of the exclusion scrollbar
	CScrollBar *pSB = (CScrollBar *)GetDlgItem(IDC_FINETUNE_EXCLSCRL);
	pSB->SetScrollRange(0, 50);
	pSB->SetScrollPos(m_ExcludePercent);
	char buf[10];
	int num = (wellnum*m_ExcludePercent) / 100;
	sprintf(buf, "%d%%=%d pt%s", m_ExcludePercent, num, num == 1 ? "" : "s");
	SetDlgItemText(IDC_FINETUNE_EXCLTEXT, buf);

	SetDlgItemText(IDC_FINETUNE_SWITCHVIEWMODE, "Contour &view");

	// if the model is "contoured intvels", we hide/disable quite a few dlg elements
	if (model == 10)
	{
		GetDlgItem(IDC_FINETUNE_FUNCDISTWGT)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_FINETUNE_PARVAL1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_FINETUNE_PARVAL2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_FINETUNE_FITVAL)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_FINETUNE_PARNAME1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_FINETUNE_PARNAME2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_FINETUNE_TEXT1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_FINETUNE_TEXT2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_FINETUNE_TEXT3)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_FINETUNE_TEXT4)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_FINETUNE_EXCLSCRL)->EnableWindow(FALSE);
		GetDlgItem(IDC_FINETUNE_ADDBACKALL)->EnableWindow(FALSE);
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CFinetuneFitting_Page::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	BOOL UpdateView = TRUE;
	// it's the scrollbar with the # of excluded points
	int pos = pScrollBar->GetScrollPos();
	int oldpos = pos;

	switch (nSBCode)
	{
	case SB_THUMBPOSITION:
		pScrollBar->SetScrollPos(nPos);
		oldpos = 100;	// for some strange reason we have to reinforce the redraw
		break;
	case SB_THUMBTRACK:
		pScrollBar->SetScrollPos(nPos);
		UpdateView = FALSE;		// no updating while scrollbar is being moved (too slow)
		break;
	case SB_LINEUP:
		pScrollBar->SetScrollPos(max(pos - 1, 0));
		break;
	case SB_LINEDOWN:
		pScrollBar->SetScrollPos(min(pos + 1, 50));
		break;
	case SB_PAGELEFT:
		pScrollBar->SetScrollPos(max(pos - 4, 0));
		break;
	case SB_PAGERIGHT:
		pScrollBar->SetScrollPos(min(pos + 4, 50));
		break;
	}

	// update text in dialog box itself
	pos = pScrollBar->GetScrollPos();
	char buf[15];
	int num = (wellnum*pos) / 100;
	sprintf(buf, "%d%%=%d pt%s", pos, num, num == 1 ? "" : "s");
	SetDlgItemText(IDC_FINETUNE_EXCLTEXT, buf);

	// inform fine tune view by writing the new value back into the horizon structure and updating the view
	m_ExcludePercent = pos;
	horizon->ExcludePercent = m_ExcludePercent;
	// invalidate the fit and inforce a redraw if the actual number of excluded points
	// has changed.
	if (UpdateView && (wellnum*pos) / 100 != (wellnum*oldpos) / 100)
	{
		pDoc->horizonlist.FitLayer(layer, pDoc);
		ShowFittingParams();
		pDoc->UpdateAllViews(NULL, DOC_CHANGE_FINETUNECHANGE);
	}
}

void CFinetuneFitting_Page::OnFinetuneAddbackall()
{
	// add back all points to the regression calculation
	int s;
	for (int i = 0; i < pView->numpts; i++)
	{
		s = pView->datapoint[i]->ReadStatus();
		pView->datapoint[i]->WriteStatus(s & (~MPT_STATUS_USER_EXCLUDE));
	}

	// set slider bar back to zero and enforce redraw
	// (thumbposition always yields redraw)
	OnHScroll(SB_THUMBPOSITION, 0, (CScrollBar*)GetDlgItem(IDC_FINETUNE_EXCLSCRL));
}

void CFinetuneFitting_Page::OnFinetuneSwitchviewmode()
{
	// flip viewmode
	(pDoc->pFTview)->ViewMode = 3 - (pDoc->pFTview)->ViewMode;
	Invalidate();
	(pDoc->pFTview)->ZoomDepth = 0;
	(pDoc->pFTview)->Invalidate();	// enforce redraw
	GrayStuff();
}

void CFinetuneFitting_Page::GrayStuff()
{
	if (model == 10)
		GetDlgItem(IDC_FINETUNE_EXCLSCRL)->EnableWindow(FALSE);
}

void CFinetuneFitting_Page::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CButton *pB = (CButton *)GetDlgItem(IDC_FINETUNE_SWITCHVIEWMODE);
	if ((pDoc->pFTview)->ViewMode == VIEWMODE_SCATTER)
		pB->SetWindowText("Contour &view");		// switch into scatter view
	else
		pB->SetWindowText("Scatter &view");		// switch into contour view
}

void CFinetuneFitting_Page::OnFinetuneOthermodel()
{
	// Clicked "Choose other model", which means: Do the same as if Cancel had
	// been pressed, but then go back to the coarse model choice but pretending
	// the user had chosen "Model/Create" from the menu!
	int i, s, reply;
	CWarnOtherModelDlg	WarnOtherModelDialog;

	// send fine tune dialog to back before opening question dialog
	sheet->SetWindowPos(&wndNoTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);

	reply = WarnOtherModelDialog.DoModal();
	// dialog back to foreground
	sheet->SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);

	if (reply == IDCANCEL)
		return;

	// remove all "excluded" flags; and potentially reset all weighting information
	for (i = 0; i < pView->numpts; i++)
	{
		s = pView->datapoint[i]->ReadStatus();
		pView->datapoint[i]->WriteStatus
		(s & (~(MPT_STATUS_USER_EXCLUDE | MPT_STATUS_SCRL_EXCLUDE)));
		if (!WarnOtherModelDialog.m_RetainWeights)
			pView->datapoint[i]->WriteWeight(1.0);
	}
	// reset gradient normalisation
	horizon->NormalizeGradients = TRUE;
	horizon->NormGradFactor = 50.0;
	horizon->UseCustomFit = FALSE;

	pDoc->Log("Choice of different model for layer '" + horizon->name + "'.", LOG_BRIEF);
	sheet->OtherModel();
}

void CFinetuneFitting_Page::OnFinetuneShowallwells()
{
	pDoc->pFTview->ShowAllWells = (BOOL)(IsDlgButtonChecked(IDC_WELLLIMITS));

	pDoc->UpdateAllViews(NULL, DOC_CHANGE_FINETUNECHANGE);

	pDoc->pFTview->SetContLimits();
}