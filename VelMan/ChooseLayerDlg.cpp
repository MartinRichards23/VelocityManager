// modelcho.cpp : implementation file

#include "stdafx.h"
#include "velman.h"
#include "VelmanDoc.h"
#include "ChooseLayerDlg.h"
#include "mainfrm.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CModelChooselayer dialog

CModelChooselayer::CModelChooselayer(CWnd* pParent /*=NULL*/)
	: CDialog(CModelChooselayer::IDD, pParent)
{
	//{{AFX_DATA_INIT(CModelChooselayer)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CModelChooselayer::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CModelChooselayer)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CModelChooselayer, CDialog)
	//{{AFX_MSG_MAP(CModelChooselayer)
	ON_LBN_DBLCLK(IDC_CHOOSE_MODELLIST, OnOK)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CModelChooselayer message handlers

BOOL CModelChooselayer::OnInitDialog()
{
	CListBox *pLB = (CListBox *)GetDlgItem(IDC_CHOOSE_MODELLIST);
	CHorizon *horizon;
	int hornumber = pDoc->horizonlist.GetSize();
	CString line;

	CDialog::OnInitDialog();

	LastModel = -1;
	for (int i = 0; i < hornumber; i++)
	{
		horizon = (CHorizon *)pDoc->horizonlist.GetAt(i);
		line = horizon->name;
		if (horizon->HaveModel)
		{
			line += " [model exists]";
			LastModel = i;
		}
		pLB->AddString((const char *)line);
	}
	pLB->SetCurSel(LastModel == hornumber - 1 ? 0 : LastModel + 1);
	ChosenLayer = 0;

	SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME), FALSE);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CModelChooselayer::OnOK()
{
	CListBox *pLB = (CListBox *)GetDlgItem(IDC_CHOOSE_MODELLIST);
	CHorizon *horizon;

	ChosenLayer = pLB->GetCurSel();	 // !!!!note that the list box must not internally
	// change the order of entries by sorting them!!!!

	CString s = "";
	int i = 0;

	if (ChosenLayer > LastModel + 1)
	{
		// reaching too far
		horizon = (CHorizon *)pDoc->horizonlist.GetAt(LastModel + 1);

		s = "ERROR:\nYou need to work on each layer in sequence. The next layer you can choose"
			" is '" + horizon->name + "'.";
		AfxMessageBox((const char *)s, MB_OK);
		return;
	}

	if (ChosenLayer < LastModel)
	{
		// not reaching far enough: warn that work will be destroyed
		if (ChosenLayer < LastModel - 1)
		{
			// invalidating more than one layer
			s = "WARNING:\nYou have already worked out models for layers under this one."
				" Revising this model"
				" means invalidating all depth conversion performed for layers '";
			horizon = (CHorizon *)pDoc->horizonlist.GetAt(ChosenLayer + 1);
			s += horizon->name;
			s += "' through '";
			horizon = (CHorizon *)pDoc->horizonlist.GetAt(LastModel);
			s += horizon->name;
			s += "'.\nAre you sure you want to continue?";
		}
		else
		{
			// invalidating one layer
			s = "WARNING:\nYou have already worked out a model for the layer '";
			horizon = (CHorizon *)pDoc->horizonlist.GetAt(LastModel);
			s += horizon->name;
			s += "' which lies under '";
			horizon = (CHorizon *)pDoc->horizonlist.GetAt(ChosenLayer);
			s += horizon->name;
			s += "', and you will lose the depth conversion that might already have been performed for it.\nAre you sure you want to continue?";
		}

		if (AfxMessageBox((const char *)s, MB_YESNO) != IDYES)
			return;

		// user wants to continue; remove "have model" flags from all invalidated horizons
		for (i = ChosenLayer + 1; i <= LastModel; i++)
		{
			horizon = (CHorizon *)pDoc->horizonlist.GetAt(i);
			horizon->HaveModel = FALSE;
			horizon->HaveFit = FALSE;
			horizon->UseCustomFit = FALSE;
		}
	}
	pDoc->UpdateAllViews(NULL);
	CDialog::OnOK();
}