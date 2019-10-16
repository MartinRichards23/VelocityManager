// mainfrm.cpp : implementation of the CMainFrame class

#include "stdafx.h"
#include "velman.h"
#include "mainfrm.h"
#include "VelmanDoc.h"
#include "numericr.h"
#include "ChooseAGridDlg.h"
#include "ChooseLayerDlg.h"
#include "ChooseManyGridsDlg.h"
#include "ChooseModelDlg.h"
#include "ConfigForm.h"
#include "WellTablesDlg.h"
#include "ft_sheet.h"
#include "Shlobj.h"
#include "stdio.h"
#include "string.h"

#ifdef VELMAN_UNIX
#include <iostream.h>
#else
#include <iostream>
#endif

#include <shlwapi.h>
#include <io.h>

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_COMMAND(ID_MODEL_CREATEMODEL, OnModelCreatemodel)
	ON_UPDATE_COMMAND_UI(ID_MODEL_CREATEMODEL, OnUpdateModelCreatemodel)
	ON_COMMAND(ID_MODEL_SHOWRESIDUALTABLE, OnModelShowresidualtable)
	ON_UPDATE_COMMAND_UI(ID_MODEL_SHOWRESIDUALTABLE, OnUpdateModelShowresidualtable)
	ON_COMMAND(ID_MODEL_SHOWLISTOFMODELS, OnModelShowlistofmodels)
	ON_UPDATE_COMMAND_UI(ID_MODEL_SHOWLISTOFMODELS, OnUpdateModelShowlistofmodels)	
	ON_COMMAND(ID_GRIDS_CONTOURPLOT, OnGridsContourplot)
	ON_COMMAND(ID_SHOWWELLDATATABLE, OnShowWellDataTables)	
	ON_UPDATE_COMMAND_UI(ID_SHOWWELLDATATABLE, OnUpdateShowWellDataTables)
	ON_COMMAND(ID_SHOWWELLDATATABLEBUTTON, OnShowWellDataTables)
	ON_UPDATE_COMMAND_UI(ID_SHOWWELLDATATABLEBUTTON, OnUpdateShowWellDataTables)

	ON_WM_CLOSE()
	ON_COMMAND(ID_SEISMICDATA_MAPOFSHOTPOINTS, OnSeismicdataMapofshotpoints)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
	// Global help commands
	ON_COMMAND(ID_HELP_INDEX, OnHelpClicked)
	ON_COMMAND(ID_HELP, CMDIFrameWnd::OnHelpIndex)
	ON_COMMAND(ID_CONTEXT_HELP, CMDIFrameWnd::OnContextHelp)
	ON_COMMAND(ID_DEFAULT_HELP, CMDIFrameWnd::OnHelpIndex)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// arrays of IDs used to initialize control bars

// toolbar buttons - IDs are command buttons
static UINT BASED_CODE buttons[] =
{
	// same order as in the bitmap 'toolbar.bmp'
	ID_FILE_NEW,
	ID_FILE_OPEN,
	ID_FILE_SAVE,
	ID_SEPARATOR,
	ID_MODEL_CREATEMODEL,
	ID_DEPTHCONVERSION_CREATEDEPTHGRIDS,
	ID_SEPARATOR,
	ID_GRIDS_CONTOURPLOT,
	ID_PROJECT_VIEWLOGFILE,
	ID_GRIDTOOLS_DIRECTORY,
	ID_GRIDTOOLS_SMOOTH,
	ID_GRIDS_SPECTRUMANALYSIS,
	ID_GRIDTOOLS_STATISTICS,
	ID_GRIDS_GRIDARITHMETIC,
	ID_DEPTHCONVERSION_CONVERTSECONDARYTIMEGRID,
	ID_SEPARATOR,
	ID_PROJECT_3DVIS,
	ID_SHOWWELLDATATABLEBUTTON,
	ID_MODEL_SHOWRESIDUALTABLE,
	ID_SEPARATOR,
	ID_SEPARATOR,
	ID_HELP_WHATNOW,
	ID_HELP_INDEX,
};

static UINT BASED_CODE indicators[] =
{
	ID_SEPARATOR, // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

//int CMainFrame::shBrdr=NULL;
int CMainFrame::GetConfigTitleBarThickness(void)
{
	return shBdr;
}

int CMainFrame::GetConfigSideBarThickness(void)
{
	return shBdrSide;
}

int CMainFrame::shBdr;
int CMainFrame::shBdrSide;

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// initialization code here
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	shBdr=24; // added 17/10/06  just as a default - value shouldnt get used

	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndToolBar.Create(this) ||
		!m_wndToolBar.LoadBitmap(IDR_MAINFRAME) ||
		!m_wndToolBar.SetButtons(buttons,
		sizeof(buttons)/sizeof(UINT)))
	{
		TRACE("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	//enable tootipes on toolbar
	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle()|CBRS_TOOLTIPS);

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		sizeof(indicators)/sizeof(UINT)))
	{
		TRACE("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// some Velman debug stuff
	FineTuneDialogOnTop=AfxGetApp()->GetProfileInt("Debug", "FineTuneDialogOnTop", 1);

#ifndef VELMAN_UNIX
	// working on a windows build
	// dynamically work out the height of the title border.
	ConfigForm* sf = new ConfigForm();
	sf->Create(IDD_ConfigForm_DIALOG, this);

	sf->ShowWindow(SW_SHOW);
	shBdr = sf->GetBorder();
	shBdrSide = sf->GetSideBorder();
	sf->CloseWindow();
	this->ActivateFrame(1);
	delete sf;
#endif
	// if the above didnt run then the shbdr will be at the default of 24

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

void CMainFrame::OnModelCreatemodel()
{
	// present a dialog and let the user choose a layer. Then close the dialog,
	// and proceed: If the well has been worked on before, go to the "fine tuning"
	// dialog directly. Otherwise go to the "Choose Model" mode, opening two
	// views as explained in OpenModelChoiceViews().

	CModelChooselayer dlg;

	CDocument* pDocument;
	CVelmanDoc *pDoc;
	CHorizon *horizon;
	CMDIChildWnd* pActiveChild = MDIGetActive();

	if (pActiveChild == NULL ||
		(pDocument = pActiveChild->GetActiveDocument()) == NULL)
	{
		TRACE0("Warning: No active document for CreateModel command\n");
		AfxMessageBox(AFX_IDP_COMMAND_FAILURE);
		return;     // command failed
	}

	pDoc=(CVelmanDoc *)pDocument;

	if(pDoc->ModelViewOpen)
		return;	// cannot do this while model is already being worked on

	dlg.pDoc=pDoc;		// so that dialog can get hold of list of layer names
	if(dlg.DoModal()==IDOK)
	{
		// disable the "Create model" menu item, it will be reinstated when the
		// user has chosen a model to work with. But we end the menu handler function
		// after opening the model preview windows, and have to avoid that the same
		// command is chosen again before they are closed.
		horizon=(CHorizon *)((pDoc->horizonlist).GetAt(dlg.ChosenLayer));
		// can we depth convert the layer? ignore the possibility here that we might not
		// have a model for the layer yet, but check all other error possibilities
		int warn=pDoc->zimscube.CanDepthConvertLayer(dlg.ChosenLayer) & ~4;
		CString s;
		if(warn)
		{
			s="Will not be able to depth convert layer '"
				+((CHorizon *)pDoc->horizonlist.GetAt(dlg.ChosenLayer))->GetName()+"':";
			if(warn & 1)
				s+="\n� No time grid found.";
			if(warn & 32)
				s+="\n� No interval velocity grid found.";
			if(warn & 2)
				s+="\n� No horizon data found.";
			if(warn & 4)
				s+="\n� No velocity model for this layer found.";
			if(warn & 8)
				s+="\n� No time grid for layer above found (you might have to designate one).";
			if(warn & 16)
				s+="\n� No depth grid for layer above found (you might have deleted it).";

			s+="\n\nRefer to the \"What next?\" dialog on how to remedy the situation.\n\n"
				"Do you want to continue?";
			if(AfxMessageBox(s, MB_YESNO | MB_DEFBUTTON2)!=IDYES)
				return;
		}

		pDoc->ActiveLayer=dlg.ChosenLayer;
		pDoc->horizonlist.ProcessHorizon(dlg.ChosenLayer);
		if(horizon->HaveModel)		// we did choose a model before!
			OpenFineTuneView(dlg.ChosenLayer);
		else		// else let the user choose a model first, the go to fine tuning
			OpenModelChoiceViews(dlg.ChosenLayer);
	}
}

void CMainFrame::OpenModelChoiceViews(int layer)
{
	char buf[255];
	CHorizon *horizon;

	CDocument* pDocument;
	CVelmanDoc *pDoc;

	CMDIChildWnd* pActiveChild = MDIGetActive();

	if (pActiveChild == NULL ||
		(pDocument = pActiveChild->GetActiveDocument()) == NULL)
	{
		TRACE0("Warning: No active document for CreateModel command\n");
		AfxMessageBox(AFX_IDP_COMMAND_FAILURE);
		return;     // command failed
	}

	pDoc=(CVelmanDoc *)pDocument;
	pDoc->ModelViewOpen=TRUE;
	pDoc->ActiveLayer=layer;
	horizon=(CHorizon *)((pDoc->horizonlist).GetAt(layer));

	// Preparation: create control dialog
	// its constructor gets information about the two frames created shortly, so that
	// the dialog can close them once it terminates.
	CModelChoiceDlg *p_dlg;
	if((p_dlg=new CModelChoiceDlg)==NULL)
	{
		AfxMessageBox(AFX_IDP_COMMAND_FAILURE);
		return;     // command failed
	}
	pDoc->pCMdlg=p_dlg;	// the view that goes with this dialog needs the ptr to close
	// the dlg!
	if(p_dlg->GetSafeHwnd()==0)
	{
		p_dlg->layer=layer;
		p_dlg->pDoc=(CVelmanDoc *)pDocument;
		p_dlg->Create();
	}

	// Step 0: Allocate memory for dynamic lists in document that simplify access
	// to well data from within the view. Deallocation takes place in CModelChoiceDlg,
	// because this dialog closes the preview windows again.
	int wellnumber=pDoc->horizonlist.wellnames.GetSize();
	for (int i=0;i<4;i++)
	{
		pDoc->xvals[i]=new double[wellnumber];
		pDoc->yvals[i]=new double[wellnumber];
		pDoc->z1vals[i]=new double[wellnumber];
		pDoc->xpos[i]=new double[wellnumber];
		pDoc->ypos[i]=new double[wellnumber];
		pDoc->residuals[i]=new double[wellnumber];
		pDoc->databaseindex[i]=new int[wellnumber];
	}
	// now fill the lists. This is also done by a member function of CModelChoiceDlg,
	// because the dialog has to be able to do that sort of thing anyway if the user
	// changes the model choice.
	// we also compute the fitting parameters in that button handler!
	p_dlg->OnSelchangeModelchoiceFunc1();
	p_dlg->OnSelchangeModelchoiceFunc2();
	p_dlg->OnSelchangeModelchoiceFunc3();
	p_dlg->OnSelchangeModelchoiceFunc4();

	// create coarse model preview window
	CDocTemplate* pTemplate = ((CVelmanApp*) AfxGetApp())->m_pTemplate1;
	ASSERT_VALID(pTemplate);

	CFrameWnd* pFrame = pTemplate->CreateNewFrame(pDocument, pActiveChild);
	pDoc->pCoarseModelFrame=pFrame;	// the view needs that to close itself!
	// and the dialog needs it as well, to close the view due to CANCEL button

	if (pFrame == NULL)
	{
		TRACE0("Warning: failed to create new frame\n");
		AfxMessageBox(AFX_IDP_COMMAND_FAILURE);
		return;     // command failed
	}
	pFrame->ShowWindow(SW_SHOWNORMAL);
	//pFrame->SetWindowPos(NULL, 0, 0, 100, 100, SWP_NOMOVE);
	pTemplate->InitialUpdateFrame(pFrame, pDocument);

	// change window title
	sprintf(buf, "Function browser for layer %s", (LPCTSTR) horizon->name);
	pFrame->SetWindowText(buf);

	// Now disable automatic window title changes and system menu
	long sty=GetWindowLong(pFrame->m_hWnd, GWL_STYLE);
	sty=(sty & (~(long)(FWS_ADDTOTITLE | WS_SYSMENU)));

	SetWindowLong(pFrame->m_hWnd, GWL_STYLE, sty);

	// remember that is a view open
	((CVelmanDoc *)pDocument)->OpenPreviewViews=1;
}

void CMainFrame::OnUpdateModelCreatemodel(CCmdUI* pCmdUI)
{
	CDocument* pDocument;

	CMDIChildWnd* pActiveChild = MDIGetActive();

	if (pActiveChild != NULL &&
		(pDocument = pActiveChild->GetActiveDocument()) != NULL)
	{
		pCmdUI->Enable(!(((CVelmanDoc *)pDocument)->ModelViewOpen) &&
			((CVelmanDoc *)pDocument)->ProjectScope==SCOPE_TIMEWELL &&
			(((CVelmanDoc *)pDocument)->ProjState & PRJ_HAVEWELLS));
	}
	else
		pCmdUI->Enable(FALSE);
}

void CMainFrame::OpenFineTuneView(int layer)
{
	char buf[255];
	CHorizon *horizon;
	CDocument* pDocument;
	CVelmanDoc *pDoc;

	CMDIChildWnd* pActiveChild = MDIGetActive();

	if (pActiveChild == NULL ||
		(pDocument = pActiveChild->GetActiveDocument()) == NULL)
	{
		TRACE0("Warning: No active document for CreateModel command\n");
		AfxMessageBox(AFX_IDP_COMMAND_FAILURE);
		return;     // command failed
	}

	pDoc=(CVelmanDoc *)pDocument;
	pDoc->ModelViewOpen=TRUE;
	pDoc->ActiveLayer=layer;
	pDoc->pFTdlg=NULL;	// will be set further down. The FineTuneView reads this to
	// access the dialog box function ShowFittingParams if necessary
	horizon=(CHorizon *)((pDoc->horizonlist).GetAt(layer));

	CDocTemplate* pTemplate = ((CVelmanApp*) AfxGetApp())->m_pTemplate4;
	ASSERT_VALID(pTemplate);
	CFrameWnd* pFrame = pTemplate->CreateNewFrame(pDocument, pActiveChild);
	if (pFrame == NULL)
	{
		TRACE0("Warning: failed to create new frame\n");
		AfxMessageBox(AFX_IDP_COMMAND_FAILURE);
		return;     // command failed
	}
	pFrame->ShowWindow(SW_SHOWNORMAL);

	pTemplate->InitialUpdateFrame(pFrame, pDocument);
	// change window title
	sprintf(buf, "Fine tuning of model function for layer %s", (LPCTSTR) horizon->name);
	pFrame->SetWindowText(buf);

	// Now disable automatic window title changes
	long sty=GetWindowLong(pFrame->m_hWnd, GWL_STYLE);
	sty=(sty & (~(long)(FWS_ADDTOTITLE)));
	sty=(sty & (~(long)(WS_SYSMENU))); // hide minimise/maximse/close buttons

	SetWindowLong(pFrame->m_hWnd, GWL_STYLE, sty);

	// remember that there is a view open
	((CVelmanDoc *)pDocument)->OpenPreviewViews=8;

	// create dialog to control fine tuning of model (modeless dialog that destroys
	// itself once OK or CANCEL has been pressed)
	CFinetuneSheet *p_dlg;

	if((p_dlg=new CFinetuneSheet("Fine tune model"))==NULL)
	{
		AfxMessageBox(AFX_IDP_COMMAND_FAILURE);
		return;     // command failed
	}
	if(p_dlg->GetSafeHwnd()==0)
	{
		pDoc->pFTsheet=p_dlg;

		p_dlg->layer=layer;
		p_dlg->pDoc=(CVelmanDoc *)pDocument;
		p_dlg->pFineTuneView=pFrame;
		p_dlg->Init();
		p_dlg->Create(pFrame, WS_VISIBLE);

		if(FineTuneDialogOnTop)
			p_dlg->SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
	}
}

void CMainFrame::OnModelShowlistofmodels()
{
	CVelmanDoc *pDoc;
	CMDIChildWnd* pActiveChild = MDIGetActive();

	if (pActiveChild == NULL ||
		(pDoc = (CVelmanDoc *)pActiveChild->GetActiveDocument()) == NULL)
	{
		TRACE0("Warning: No active document for CreateModel command\n");
		AfxMessageBox(AFX_IDP_COMMAND_FAILURE);
		return;     // command failed
	}

	if(pDoc->ListOfModelsOpen)	// do nothing if this view is already open
		return;

	CDocTemplate* pTemplate = ((CVelmanApp*) AfxGetApp())->m_pTemplate6;
	ASSERT_VALID(pTemplate);
	CFrameWnd* pFrame = pTemplate->CreateNewFrame(pDoc, pActiveChild);
	if (pFrame == NULL)
	{
		TRACE0("Warning: failed to create new frame\n");
		AfxMessageBox(AFX_IDP_COMMAND_FAILURE);
		return;     // command failed
	}
	pFrame->ShowWindow(SW_SHOWMAXIMIZED);
	pDoc->ListOfModelsOpen=TRUE;
	pTemplate->InitialUpdateFrame(pFrame, pDoc);

	// change window title
	pFrame->SetWindowText("Table of models");

	// Now disable automatic window title changes
	long sty=GetWindowLong(pFrame->m_hWnd, GWL_STYLE);
	sty=(sty & (~(long)(FWS_ADDTOTITLE)));
	SetWindowLong(pFrame->m_hWnd, GWL_STYLE, sty);
}

void CMainFrame::OnUpdateModelShowlistofmodels(CCmdUI* pCmdUI)
{
	CMDIChildWnd* pActiveChild = MDIGetActive();
	CVelmanDoc *pDoc;

	if (pActiveChild != NULL &&
		(pDoc = (CVelmanDoc *)pActiveChild->GetActiveDocument()) != NULL)
	{
		pCmdUI->Enable(  ((pDoc->ProjState) & PRJ_HAVESOMEMODELS)
			&& !(pDoc->ListOfModelsOpen) );
	}
}

void CMainFrame::OnGridsContourplot()
{
	CChooseAnyGrid dlg;
	char buf[255];

	CDocument* pDocument;
	CVelmanDoc *pDoc;
	CMDIChildWnd* pActiveChild = MDIGetActive();

	if (pActiveChild == NULL ||
		(pDocument = pActiveChild->GetActiveDocument()) == NULL)
	{
		TRACE0("Warning: No active document for CreateModel command\n");
		AfxMessageBox(AFX_IDP_COMMAND_FAILURE);
		return;     // command failed
	}

	pDoc=(CVelmanDoc *)pDocument;
	/*Multi form behaviour removed
	#ifdef VELMAN_UNIX// only want one window if using unix
	if(pDoc->TimegridContourOpen)
	return;	// cannot do this while view is still open//shuan
	#endif
	*/
	//	if(pDoc->TimegridContourOpen)
	//return;	// cannot do this while view is still open

	dlg.pDoc=pDoc;		// so that dialog can get hold of list of grid names
	dlg.SetDlgTexts("Contour plot of any grid", "Choose a grid to be contoured:");
	if(dlg.DoModal()!=IDOK)
		return;

	// section that creates & opens the contour window
	CZimsGrid *grid=(CZimsGrid *)(pDoc->zimscube.GetAt(dlg.chosengrid));
	if(grid->IsAllNull())
	{
		AfxMessageBox("Cannot contour a grid that only consists of NULL values.\n\nYou should delete this grid.");
		return;
	}
	CDocTemplate* pTemplate = ((CVelmanApp*) AfxGetApp())->m_pTemplate2;
	ASSERT_VALID(pTemplate);
	/* MultiForm Behaviour Removed
	CFrameWnd* pFrame = pTemplate->CreateNewFrame(pDoc, NULL);//was pActiveChild);
	if (pFrame == NULL)
	{
	TRACE0("Warning: failed to create new frame\n");
	AfxMessageBox(AFX_IDP_COMMAND_FAILURE);
	return;     // command failed
	}
	pFrame->ShowWindow(SW_SHOWMINIMIZED);// used to be this SW_SHOWMAXIMIZED);
	// above line is needed to make it not display full screen shouldnt need to be minimize but
	// seems to  be only thing that works - bit of a bodge thought!
	*/
	CFrameWnd* pFrame = pTemplate->CreateNewFrame(pDoc, pActiveChild);
	if (pFrame == NULL)
	{
		TRACE0("Warning: failed to create new frame\n");
		AfxMessageBox(AFX_IDP_COMMAND_FAILURE);
		return;     // command failed
	}
	pFrame->ShowWindow(SW_SHOWNORMAL);// was SW_SHOWMAXIMIZED

	pDoc->TimegridContourOpen=TRUE;
	pDoc->pTCdlg=&dlg;		// so the frame can retrieve chosen grid number in InitialUpdate()

	pTemplate->InitialUpdateFrame(pFrame, pDoc);
	// change window title
	sprintf(buf, "Contour plot of %s", (const char *)grid->GetName());
	pFrame->SetWindowText(buf);
	// Now disable automatic window title changes
	long sty=GetWindowLong(pFrame->m_hWnd, GWL_STYLE);
	sty=(sty & (~(long)(FWS_ADDTOTITLE)));
	SetWindowLong(pFrame->m_hWnd, GWL_STYLE, sty);
	pFrame->SetMenu(NULL);// hides the menu bar
	Invalidate();
	UpdateWindow();
}

void CMainFrame::OnModelShowresidualtable()
{
	CVelmanDoc *pDoc;
	CMDIChildWnd* pActiveChild = MDIGetActive();

	if (pActiveChild == NULL ||
		(pDoc = (CVelmanDoc *)pActiveChild->GetActiveDocument()) == NULL)
	{
		TRACE0("Warning: No active document for CreateModel command\n");
		AfxMessageBox(AFX_IDP_COMMAND_FAILURE);
		return;     // command failed
	}

	if(pDoc->TyingTableOpen)	// do nothing if this view is already open
		return;

	//set value to 13 so it shows residual values
	pDoc->TieTableChosenType=13;

	CDocTemplate* pTemplate = ((CVelmanApp*) AfxGetApp())->m_pTemplate3;

	ASSERT_VALID(pTemplate);
	CFrameWnd* pFrame = pTemplate->CreateNewFrame(pDoc, pActiveChild);
	if (pFrame == NULL)
	{
		TRACE0("Warning: failed to create new frame\n");
		AfxMessageBox(AFX_IDP_COMMAND_FAILURE);
		return;     // command failed
	}
	pFrame->ShowWindow(SW_SHOWMAXIMIZED);
	pDoc->TyingTableOpen=TRUE;

	pTemplate->InitialUpdateFrame(pFrame, pDoc);
	// change window title
	pFrame->SetWindowText("Table of residuals for all wells");
	// Now disable automatic window title changes
	long sty=GetWindowLong(pFrame->m_hWnd, GWL_STYLE);
	sty=(sty & (~(long)(FWS_ADDTOTITLE)));
	SetWindowLong(pFrame->m_hWnd, GWL_STYLE, sty);

}

void CMainFrame::OnShowWellDataTables()
{
	CVelmanDoc *pDoc;
	CMDIChildWnd* pActiveChild = MDIGetActive();

	if (pActiveChild == NULL ||
		(pDoc = (CVelmanDoc *)pActiveChild->GetActiveDocument()) == NULL)
	{
		TRACE0("Warning: No active document for CreateModel command\n");
		AfxMessageBox(AFX_IDP_COMMAND_FAILURE);
		return;     // command failed
	}

	//if(pDoc->TyingTableOpen)	// do nothing if this view is already open
	//	return;

	CWellTablesDlg dlg;
	dlg.pDoc = pDoc;

	if(dlg.DoModal()!=IDOK)
		return;

	//get title for window
	char buf[128];
	switch(pDoc->TieTableChosenType)
	{
		//grid well ties
	case 0: sprintf(buf, "Time corrections"); break;
	case 1: sprintf(buf, "Depth corrections"); break;
	case 2: sprintf(buf, "Interval velocity corrections"); break;

		//interval tops
	case 3: sprintf(buf, "Interval tops times"); break;
	case 4: sprintf(buf, "Interval tops depths"); break;

		//interval bases
	case 5: sprintf(buf, "Interval bases times"); break;
	case 6: sprintf(buf, "Interval bases depths"); break;

		//interval midpoint values
	case 7: sprintf(buf, "Interval mid point times"); break;
	case 8: sprintf(buf, "Interval mid point depths'"); break;

		//interval values
	case 9: sprintf(buf, "Interval times"); break;
	case 10: sprintf(buf, "Interval depths"); break;
	case 11: sprintf(buf, "Interval velocities"); break;

		//weighting and residuals
	case 12: sprintf(buf, "Well weighting coefficients"); break;
	case 13: sprintf(buf, "Residual values"); break;
	}

	CDocTemplate* pTemplate = ((CVelmanApp*) AfxGetApp())->m_pTemplate3;
	ASSERT_VALID(pTemplate);
	CFrameWnd* pFrame = pTemplate->CreateNewFrame(pDoc, pActiveChild);
	if (pFrame == NULL)
	{
		TRACE0("Warning: failed to create new frame\n");
		AfxMessageBox(AFX_IDP_COMMAND_FAILURE);
		return;     // command failed
	}
	pFrame->ShowWindow(SW_SHOWNORMAL);
	pDoc->TyingTableOpen=TRUE;

	pTemplate->InitialUpdateFrame(pFrame, pDoc);

	pFrame->SetWindowText(buf);

	// Now disable automatic window title changes
	long sty=GetWindowLong(pFrame->m_hWnd, GWL_STYLE);
	sty=(sty & (~(long)(FWS_ADDTOTITLE)));
	SetWindowLong(pFrame->m_hWnd, GWL_STYLE, sty);
}

void CMainFrame::OnUpdateShowWellDataTables(CCmdUI* pCmdUI)
{
	CMDIChildWnd* pActiveChild = MDIGetActive();
	CVelmanDoc *pDoc;

	if (pActiveChild != NULL &&
		(pDoc = (CVelmanDoc *)pActiveChild->GetActiveDocument()) != NULL)
	{
		pCmdUI->Enable((pDoc->ProjState) & PRJ_HAVEWELLS);
	}
	else
	{
		pCmdUI->Enable(0);
	}
}

void CMainFrame::OnUpdateModelShowresidualtable(CCmdUI* pCmdUI)
{
	CMDIChildWnd* pActiveChild = MDIGetActive();
	CVelmanDoc *pDoc;

	if (pActiveChild != NULL &&
		(pDoc = (CVelmanDoc *)pActiveChild->GetActiveDocument()) != NULL)
	{
		pCmdUI->Enable( ((pDoc->ProjState) & PRJ_HAVESOMEMODELS)
			&& !(pDoc->TyingTableOpen));
	}
	else
	{
		pCmdUI->Enable(0);
	}
}

// write window size to INI file

void CMainFrame::OnClose()
{
	// This does NOT call ExitInstance(), but rather just exit() right here in the
	// main window closing routine. Not nice, but it prevents difficulties on those
	// systems where you have to define a window position by clicking the mouse:
	// ExitInstance() seems to open two invisible windows, forcing the user to click
	// twice without anything happening.

	// in any case we remember the current window size for the next time
	WINDOWPLACEMENT wplace;

	GetWindowPlacement(&wplace);
	AfxGetApp()->WriteProfileInt("Window Placement", "Width",
		wplace.rcNormalPosition.right-wplace.rcNormalPosition.left);
	AfxGetApp()->WriteProfileInt("Window Placement", "Height",
		wplace.rcNormalPosition.bottom-wplace.rcNormalPosition.top);

	// now check whether we need a quick exit
	char *env=getenv("VELMAN_QUICKEXIT");
	if(!env || _stricmp(env, "TRUE")!=0)
		CMDIFrameWnd::OnClose(); // no quick exit required, exit cleanly
	else // uh-oh
	{
		// close all open documents first (usually this is done automatically later)
		CVelmanDoc *pDoc;
		CMDIChildWnd* pActiveChild = MDIGetActive();

		// walk along all active child windows, find their documents and close them.
		// that automatically then activates another child, until there are not children
		// left, by which time we can exit
		while(pActiveChild)
		{
			pDoc = (CVelmanDoc *)pActiveChild->GetActiveDocument();
			if(pDoc)
				pDoc->OnCloseDocument();
			pActiveChild = MDIGetActive();
		};

		// now clean up all the memory we can...
		// (the rest is essentially copied from CVelmanApp::ExitInstance() )
		CVelmanApp *app=(CVelmanApp *)AfxGetApp();
		delete app->m_pTemplate1;
		delete app->m_pTemplate2;
		delete app->m_pTemplate3;
		delete app->m_pTemplate4;
		delete app->m_pTemplate5;
		delete app->m_pTemplate6;
		delete app->m_pTemplate7;

		// write end time for licensing purposes
#ifdef VELMAN_UNIX
		//forget_caleb			end_program();
#endif
		exit(0);
	}
}

void CMainFrame::OnSeismicdataMapofshotpoints()
{
	CVelmanDoc *pDoc;
	CMDIChildWnd* pActiveChild = MDIGetActive();

	if (pActiveChild == NULL ||
		(pDoc = (CVelmanDoc *)pActiveChild->GetActiveDocument()) == NULL)
	{
		TRACE0("Warning: No active document for seismic map command\n");
		AfxMessageBox(AFX_IDP_COMMAND_FAILURE);
		return;     // command failed
	}

	if(pDoc->SeisdataMapOpen)	// do nothing if this view is already open
		return;

	// check whether we have a primary time grid to use as a template
	CZimsGrid *grid=pDoc->zimscube.FindHorizon(0, CSD_TYPE_TIME, 0);
	if(grid==NULL)
	{
		AfxMessageBox("You need to import and designate time grids first", MB_OK);
		return;
	}

	CDocTemplate* pTemplate = ((CVelmanApp*) AfxGetApp())->m_pTemplate7;
	ASSERT_VALID(pTemplate);
	CFrameWnd* pFrame = pTemplate->CreateNewFrame(pDoc, pActiveChild);
	if (pFrame == NULL)
	{
		TRACE0("Warning: failed to create new frame\n");
		AfxMessageBox(AFX_IDP_COMMAND_FAILURE);
		return;     // command failed
	}
	pFrame->ShowWindow(SW_SHOWMAXIMIZED);
	pDoc->SeisdataMapOpen=TRUE;

	pTemplate->InitialUpdateFrame(pFrame, pDoc);
	// change window title
	char buf[256];
	sprintf(buf, "Map of imported RMS data");
	pFrame->SetWindowText(buf);

	// Now disable automatic window title changes
	long sty=GetWindowLong(pFrame->m_hWnd, GWL_STYLE);
	sty=(sty & (~(long)(FWS_ADDTOTITLE)));
	SetWindowLong(pFrame->m_hWnd, GWL_STYLE, sty);
}

void CMainFrame::OnHelpClicked()
{
	CString Path = CMainFrame::GetExeFolder() + "\\Velman.chm";

	if(CMainFrame::FileExists(Path))
	{
		//::HtmlHelp(this->m_hWnd, Path, NULL, NULL);
		ShellExecute(NULL, "open", Path, NULL, NULL, SW_SHOW);
	}
	else
	{
		AfxMessageBox("Help file not installed", MB_OK);
	}

	//CMDIFrameWnd::OnHelpIndex();   // launches old style hlp help file
	//CMainFrame::ShellExecuteAndWait("Velman.chm", "", false);
}

void CMainFrame::OnPaint()
{
	// we do not want to paint the window, we just want to open a startup dialog
	// and a splash screen the very first time round

	CPaintDC(this);

	static BOOL StartupFinished=FALSE;

	if(StartupFinished==FALSE)
	{
		// the following commands rely on the fact that this routine here is only
		// ever called once, at the very beginning of execution of VelMan

		StartupFinished=TRUE; // make sure the above if() never triggers again
	}
}

// Determines if files exists
bool CMainFrame::FileExists(CString path)
{
	bool exists = false;
	std::fstream fin;
	fin.open(path, std::ios::in);
	if( fin.is_open() )
	{
		fin.close();
		exists = true;
	}
	fin.close();

	return exists;
}

// Gets location of MyDocuments folder
CString CMainFrame::GetMyDocsFolder()
{
	char szBuf[2048];
	CString dir;
	SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, 0, szBuf);
	sprintf( szBuf, "%s\\VelMan", szBuf );
	dir=szBuf;

	CreateDirectory(dir, NULL);

	return dir;
}

// Makes a new file name, e.g. if "readme.txt" exists, it will return "readme_1.txt"
CString CMainFrame::GetNewFileName(CString dir, CString filename, CString extension)
{
	CString fullpath = "";
	CString fname = "";
	char buf[256];

	int version = 0;
	do
	{
		if(version == 0)
		{
			fullpath = dir + filename + extension;
			fname = filename + extension;
		}
		else
		{
			sprintf(buf, "%s_(%d)", (const char *)filename, version);
			fname = (CString)buf + extension;

			fullpath = dir + fname;
		}
		version++;

	} while(CMainFrame::FileExists(fullpath));

	return fname;
}

//gets folder exe was launched from
CString CMainFrame::GetExeFolder()
{
	TCHAR acAppPath[MAX_PATH];
	GetModuleFileName(NULL, acAppPath, MAX_PATH);
	LPSTR exefolder = (LPSTR)acAppPath;
	PathRemoveFileSpec(exefolder);

	CString OriginalFltPath = exefolder;

	return OriginalFltPath;
}

// Launches file, with parameters, then optionally waits for file to close
void CMainFrame::ShellExecuteAndWait(LPCSTR fileName, LPCSTR params, BOOL Wait)
{
	TCHAR acAppPath[MAX_PATH];
	GetModuleFileName(NULL, acAppPath, MAX_PATH);
	LPSTR folder = (LPSTR)acAppPath;
	PathRemoveFileSpec(folder);

	SHELLEXECUTEINFO ShExecInfo = {0};
	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	ShExecInfo.hwnd = NULL;
	ShExecInfo.lpVerb = NULL;
	ShExecInfo.lpFile = fileName;
	ShExecInfo.lpParameters = params;
	ShExecInfo.lpDirectory = folder;
	ShExecInfo.nShow = SW_SHOW;
	ShExecInfo.hInstApp = NULL;

	//http://msdn.microsoft.com/en-us/library/bb762154(VS.85).aspx
	ShellExecuteEx(&ShExecInfo);

	if(Wait)
		WaitForSingleObject(ShExecInfo.hProcess, INFINITE);
}