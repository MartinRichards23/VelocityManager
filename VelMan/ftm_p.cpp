// FTM_P.cpp : implementation file
//

#include "stdafx.h"
#include "velman.h"
#include "VelmanDoc.h"
#include "finetune.h"
#include "mainfrm.h"
#include "ft_sheet.h"
#include "ftm_p.h"
#include "EditwellDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFinetuneMisc_Page property page

IMPLEMENT_DYNCREATE(CFinetuneMisc_Page, CPropertyPage)

CFinetuneMisc_Page::CFinetuneMisc_Page(CFinetuneSheet *mysheet) : CPropertyPage(CFinetuneMisc_Page::IDD)
{
	//{{AFX_DATA_INIT(CFinetuneMisc_Page)
	m_RebuildDepth = FALSE;
	//}}AFX_DATA_INIT
	sheet=mysheet;
}

CFinetuneMisc_Page::CFinetuneMisc_Page()
{
}

CFinetuneMisc_Page::~CFinetuneMisc_Page()
{
}

void CFinetuneMisc_Page::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFinetuneMisc_Page)
	DDX_Check(pDX, IDC_REBUILD_DEPTH, m_RebuildDepth);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CFinetuneMisc_Page, CPropertyPage)
	//{{AFX_MSG_MAP(CFinetuneMisc_Page)
	ON_BN_CLICKED(IDC_FINETUNE_HIDEME, OnFinetuneHideme)
	ON_BN_CLICKED(IDC_FINETUNE_SHOWGRID, OnFinetuneShowgrid)
	ON_BN_CLICKED(IDC_FINETUNE_SHOWWELLLABELS, OnFinetuneShowwelllabels)
	ON_BN_CLICKED(IDC_FINETUNE_DOEDIT, OnFinetuneDoedit)
	ON_BN_CLICKED(IDC_FINETUNE_SWITCHVIEWMODE, OnFinetuneSwitchviewmode)
	ON_WM_PAINT()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_FINETUNE_KEEPRESIDUALS, OnFinetuneKeepresiduals)
	ON_BN_CLICKED(IDC_WELLLIMITS, OnFinetuneShowallwells)
	ON_BN_CLICKED(IDC_GRIDLIMITS, OnFinetuneShowallwells)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFinetuneMisc_Page message handlers

void CFinetuneMisc_Page::OnOK()
{
	UpdateData(TRUE);
	sheet->OnOK();
}

void CFinetuneMisc_Page::OnCancel()
{
	sheet->OnCancel();
}

void CFinetuneMisc_Page::OnFinetuneHideme()
{
	sheet->Hideme();
}

void CFinetuneMisc_Page::GrayStuff()
{
	BOOL enable=((pDoc->pFTview)->ViewMode==VIEWMODE_SCATTER);
	GetDlgItem(IDC_FINETUNE_SHOWGRID)->EnableWindow(enable);
}

void CFinetuneMisc_Page::OnFinetuneShowgrid()
{
	(pDoc->pFTview)->ShowGridLines=(BOOL)(IsDlgButtonChecked(IDC_FINETUNE_SHOWGRID));
	pDoc->UpdateAllViews(NULL, DOC_CHANGE_FINETUNECHANGE);
}

void CFinetuneMisc_Page::OnFinetuneShowwelllabels()
{
	(pDoc->pFTview)->ShowWellLabels=(BOOL)(IsDlgButtonChecked(IDC_FINETUNE_SHOWWELLLABELS));
	pDoc->UpdateAllViews(NULL, DOC_CHANGE_FINETUNECHANGE);
}

void CFinetuneMisc_Page::OnFinetuneShowallwells()
{
	(pDoc->pFTview)->ShowAllWells=(BOOL)(IsDlgButtonChecked(IDC_WELLLIMITS));
	pDoc->UpdateAllViews(NULL, DOC_CHANGE_FINETUNECHANGE);
	(pDoc->pFTview)->SetContLimits();
}

BOOL CFinetuneMisc_Page::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	pView=pDoc->pFTview;
	horizon=(CHorizon *)((pDoc->horizonlist).GetAt(layer));
	model=horizon->ModelNumber;

	// the checkboxes
	int check;
	check=AfxGetApp()->GetProfileInt("Attributes", "ShowWellLabels", 1);
	CheckDlgButton(IDC_FINETUNE_SHOWWELLLABELS, check);
	check=AfxGetApp()->GetProfileInt("Attributes", "ShowCoordGridLines", 1);
	CheckDlgButton(IDC_FINETUNE_SHOWGRID, check);
	check=AfxGetApp()->GetProfileInt("Attributes", "ShowAllWells", 1);
	CheckDlgButton(IDC_WELLLIMITS, check);
	CheckDlgButton(IDC_FINETUNE_KEEPRESIDUALS, 1);

	SetDlgItemText(IDC_FINETUNE_SWITCHVIEWMODE, "Contour &view");

	// list of well names
	CListBox *pLB=(CListBox *)GetDlgItem(IDC_CHOOSEEDIT_LIST);
	int i, num;

	num=pDoc->horizonlist.wellnames.GetSize();
	for(i=0;i<num;i++)
		pLB->AddString(pDoc->horizonlist.wellnames[i]);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CFinetuneMisc_Page::OnFinetuneDoedit()
{
	CEditWellPointDlg	dlg2;
	CListBox *pLB=(CListBox *)GetDlgItem(IDC_CHOOSEEDIT_LIST);
	int selection=pLB->GetCurSel();

	// send fine tune dialog to back
	sheet->SetWindowPos(&wndNoTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);

	if(selection==LB_ERR)
		AfxMessageBox("Please select the well to be edited first.");
	else
	{
		// evoke edit dialog
		dlg2.datapoint=(CMeasurePt *)horizon->GetAt(selection);
		dlg2.pDoc=pDoc;
		dlg2.layer=layer;
		dlg2.DidChangePoint=FALSE;
		if(dlg2.datapoint!=NULL)
			dlg2.DoModal();
		else
			AfxMessageBox("There is no data for this point.");	// thus we cannot add
		// new points, just edit the existing ones! A certain shortcoming.

		if(dlg2.DidChangeAnything)
		{
			// search index of the edited point in internal list in view
			int iopt=-1;
			for(int i=0;i<pView->numpts;i++)
			{
				if(pView->datapoint[i]==dlg2.datapoint)
				{
					iopt=i;
					break;
				}
			}

			// this code has just been copied from fintune.cpp, see OnLButtonDown
			if(iopt>=0)
			{
				// reprocess the layer
				pDoc->horizonlist.ProcessHorizon(layer);
				pDoc->horizonlist.FitLayer(layer, pDoc);
				// The dialog has already written any changes into the
				// measurept of the point itself. copy the change into the local copy of the
				// point list.
				double x, y;
				pView->datapoint[iopt]->ReadValuePair(model, &x, &y);
				pView->xval[iopt]=x;
				pView->yval[iopt]=y;
				// force update view (will also compute new model in the course of OnDraw() )
				pView->Invalidate();
			}
			pDoc->UpdateAllViews(NULL, DOC_CHANGE_FINETUNECHANGE);
		}
	}
	// dialog back to foreground
	sheet->SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
}

void CFinetuneMisc_Page::OnFinetuneSwitchviewmode()
{
	// flip viewmode
	(pDoc->pFTview)->ViewMode=3-(pDoc->pFTview)->ViewMode;
	Invalidate();
	(pDoc->pFTview)->ZoomDepth=0;
	(pDoc->pFTview)->Invalidate();	// enforce redraw
}

void CFinetuneMisc_Page::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	int mode=(pDoc->pFTview)->ViewMode;
	CButton *pB=(CButton *)GetDlgItem(IDC_FINETUNE_SWITCHVIEWMODE);
	if(mode==VIEWMODE_SCATTER)
		pB->SetWindowText("Contour &view");
	else
		pB->SetWindowText("Scatter &view");
}

void CFinetuneMisc_Page::OnClose()
{
	// remember grid line setting etc.
	AfxGetApp()->WriteProfileInt("Attributes", "ShowCoordGridLines",
		IsDlgButtonChecked(IDC_FINETUNE_SHOWGRID));
	AfxGetApp()->WriteProfileInt("Attributes", "ShowWellLabels",
		IsDlgButtonChecked(IDC_FINETUNE_SHOWWELLLABELS));

	CPropertyPage::OnClose();
}

void CFinetuneMisc_Page::OnFinetuneKeepresiduals()
{
	horizon->KeepResidualGridAfterDC=IsDlgButtonChecked(IDC_FINETUNE_KEEPRESIDUALS);
}

BOOL CFinetuneMisc_Page::GetRebuildDepth()
{	// access function for RebuildDepth switch
	return m_RebuildDepth;
}
