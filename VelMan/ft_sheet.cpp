// FT_Sheet.cpp : implementation file
//

#include "stdafx.h"
#include "velman.h"
#include "VelmanDoc.h"
#include "finetune.h"
#include "mainfrm.h"
class CFinetuneSheet; // referred to in the following 6 property page header files
#include "ft_qh1.h"
#include "ft_qh2.h"
#include "ftcf_p.h"
#include "ftf_p.h"
#include "ftm_p.h"
#include "ftw_p.h"
#include "FTResPag.h"
#include "FT_Sheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFinetuneSheet

IMPLEMENT_DYNAMIC(CFinetuneSheet, CPropertySheet)

	CFinetuneSheet::CFinetuneSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

CFinetuneSheet::CFinetuneSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	fitpage=new CFinetuneFitting_Page(this);
	customfitpage=new CFinetuneCustomfit_Page(this);
	residualpage=new CFinetuneResidualsPage(this);
	weightpage=new CFinetuneWeight_Page(this);
	miscpage=new CFinetuneMisc_Page(this);
	qh1page=new CFinetuneQuickhelp1_Page(this);
	qh2page=new CFinetuneQuickhelp2_Page(this);
	AddPage(fitpage);
	AddPage(customfitpage);
	AddPage(residualpage);
	AddPage(weightpage);
	AddPage(miscpage);
	AddPage(qh1page);
	AddPage(qh2page);
	HaveDisplayedResidualsPage=FALSE;

	// no system menu
	long sty=GetWindowLong(m_hWnd, GWL_STYLE);
	sty=(sty & (~(long)(WS_SYSMENU)));
	SetWindowLong(m_hWnd, GWL_STYLE, sty);

	// a nice icon
	m_psh.dwFlags |= PSH_USEHICON;
	m_psh.hIcon=AfxGetApp()->LoadIcon(IDI_FINETUNE);
}

CFinetuneSheet::~CFinetuneSheet()
{
}

BEGIN_MESSAGE_MAP(CFinetuneSheet, CPropertySheet)
	//{{AFX_MSG_MAP(CFinetuneSheet)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFinetuneSheet message handlers

void CFinetuneSheet::OnOK()
{
	// add a logfile entry
	CString s;
	char buf[1024];
	CHorizon *horizon=(CHorizon *)pDoc->horizonlist.GetAt(layer);
	CFineTuneView *pView=pDoc->pFTview;

	s="Created a velocity model for layer "+horizon->GetName();
	pDoc->Log(s, LOG_BRIEF);

	if (horizon->UseCustomFit)
	{
		pDoc->Log("The 'Custom fit' feature was used.", LOG_BRIEF);
		pDoc->horizonlist.FitLayer(layer, pDoc);
	}

	// add a list of excluded wells to the logfile (if there are any)
	BOOL AnyExcluded=FALSE;
	int i, index;

	for (i=0; i<pView->numpts; i++)
	{
		if ((pView->datapoint[i])->ReadStatus() & (MPT_STATUS_USER_EXCLUDE | MPT_STATUS_SCRL_EXCLUDE))
		{
			if (!AnyExcluded)
			{
				pDoc->Log("The following wells were excluded from the optimisation:", LOG_STD);
				AnyExcluded=TRUE;
			}
			index=pView->wellindex[i];
			pDoc->Log("  "+pDoc->horizonlist.wellnames[index], LOG_STD);
		}
	}
	if (AnyExcluded)
		pDoc->Log("", LOG_STD);

	pDoc->Log("The formula for depth conversion is:", LOG_BRIEF);
	switch (horizon->ModelNumber)
	{
	case 0:
		// exp. fitting
		sprintf(buf,
			"z2 = Vo(exp(k(t2-t1))-1)/k + z1exp(k(t2-t1))"
			"                                            "
			"                                            "
			"\nwith Vo=%-12.2f, K=%-8.4f", ab2V(horizon->FitA, horizon->FitB),
			ab2K(horizon->FitA, horizon->FitB)); // 3/1/00 increased precision for each case
		break;
	case 1:
		// dInt vs tInt
		sprintf(buf,
			"z2 = z1 + %16.12g*(t2-t1) + %16.12g + R", horizon->FitA, horizon->FitB);
		break;
	case 2:
		// vInt ~ mpdepth
		sprintf(buf,
			"     z1*(1 + %16.12g*dT/2) + (%16.12g+R)*dT   \n"
			"z2 = --------------------------------------   \n"
			"             1 + %16.12g*dT/2\n"
			"(note: dT=(t2-t1)\n", horizon->FitA, horizon->FitB, horizon->FitA);
		break;
	case 3:
		// vInt ~ mptime
		sprintf(buf,
			"z2 = z1 + (dV+R) dT\n"
			"with dT=(t2-t1), dV=%16.12g*(t2+t1)/2 + %16.12g", horizon->FitA, horizon->FitB);
		break;
	case 4:
		// vInt ~ tTop
		sprintf(buf,
			"z2 = z1 + (dV+R) dT\n"
			"with dT=(t2-t1), dV=%16.12g*t1 + %16.12g", horizon->FitA, horizon->FitB);
		break;
	case 5:
		// vInt ~ tBottom
		sprintf(buf,
			"z2 = z1 + (dV+R) dT\n"
			"with dT=(t2-t1), dV=%16.12g*t2 + %16.12g", horizon->FitA, horizon->FitB);
		break;
	case 6:
		// vInt ~ dTop
		break;
	case 7:
		// vInt ~ dBottom
		sprintf(buf,
			"     z1 + (%16.12g+R)*dT   \n"
			"z2 = --------------------   \n"
			"       1 - %16.12g*dT\n"
			"note: dT=(t2-t1)\n", horizon->FitB, horizon->FitA);
		break;
	case 8:
		// vInt ~ dInt
		sprintf(buf,
			"     z1*(1 - %16.12g*dT) + (%16.12g+R)*dT   \n"
			"z2 = ------------------------------------   \n"
			"           1 - %16.12g*dT\n"
			"(note: dT=(t2-t1)\n", horizon->FitA, horizon->FitB, horizon->FitA);
		break;
	case 9:
		// vInt ~ tInt
		sprintf(buf,
			"z2 = z1 + (dV+R) dT\n"
			"with dT=(t2-t1), dV=%16.12g*(t2-t1) + %16.12g", horizon->FitA, horizon->FitB);
		break;
	case 10:
		// contoured vInt
		sprintf(buf,
			"z2 = z1 + dV dT\n"
			"with dT=(t2-t1), dV=contoured interval velocity");
		break;

	}
	pDoc->Log(CString(buf), LOG_BRIEF);
	sprintf(buf, "(%s distance weighting)", horizon->DistWeight==0 ? "linear" : "quadratic");
	pDoc->Log(CString(buf), LOG_BRIEF);

	// 30/12/99. decided to never rebuilddepths. So, the option is made
	// invisible on the dialogue and the RebuildDepth() call commented out. The
	// rest of the code is left in place in case it's ever needed again.
	//if ( miscpage->GetRebuildDepth() )
	//	pDoc->Log("Rebuilding missing bottom depth values was done.", LOG_DETAIL );
	//else
	//	pDoc->Log("Rebuilding missing bottom depth values was not done.", LOG_DETAIL );
	pDoc->bRebuildDepth = miscpage->GetRebuildDepth();

	// remember the model choice in the document
	if (HaveDisplayedResidualsPage)
		horizon->HowToHandleWellResiduals=residualpage->HowToHandleResiduals();

	// perform depth conversion for this horizon, delete "Have Model" flag if it
	// returns a nonzero value.
	CleanUp();
	DestroyWindow();

	CString stats = "";
	char distunitname[8]; // ft or m
	double intmin=0, intmax=0;
	double min=0, max=0;

	// do depth conversion
	if(pDoc->DepthConvertModelLayer(layer, min, max)==0)
	{
		horizon->HaveModel = TRUE;

		// calc the interval velocity now
		pDoc->ComputeIntervalvelocity(layer, intmin, intmax);

		if(pDoc->Units & UNIT_OUTPUTINFEET)
			strcpy(distunitname, "feet");
		else
			strcpy(distunitname, "m");

		stats="Successfully depth-converted layer " + horizon->GetName() +
			"\n\nInterval depth range:\n";

		if(min!=1.0E30)
		{
			sprintf(buf, "%1.0f%s..%1.0f%s", min, distunitname, max, distunitname);
			stats+=buf;
		}
		else
			stats+="(invalid; only NULL values computed)";

		stats+="\n\nInterval velocity range:\n";
		if(intmin!=1.0E30)
		{
			sprintf(buf, "%1.0f%s/sec..%1.0f%s/sec", intmin, distunitname, intmax, distunitname);
			stats+=buf;
		}
		else
			stats+="(invalid; only NULL values computed)";

		stats+= "\nDepth conversion of layer is successfully completed\n\n";

		AfxMessageBox(stats, MB_ICONINFORMATION | MB_OK);
		pDoc->Log(stats, LOG_BRIEF);
	}

	// the dialog object itself was created dynamically, this is the last function that
	// is called after the windows has been removed. We can now destroy the object
	// itself
	delete this;
}

void CFinetuneSheet::OnCancel()
{
	//clicked cancel, so set havemodel to false
	CHorizon *horizon=(CHorizon *)pDoc->horizonlist.GetAt(layer);
	horizon->HaveModel=FALSE;

	CleanUp();
	DestroyWindow();
	// the dialog object itself was created dynamically, this is the last function that
	// is called after the windows has been removed. We can now destroy the object
	// itself
	delete this;
}

void CFinetuneSheet::CleanUp()
{
	// close the "fine tune" frame window if it is still open
	if (pDoc->OpenPreviewViews & 8)
		pFineTuneView->DestroyWindow();
	pDoc->OpenPreviewViews=0;		// should not really be necessary
	// re-enable the "Model/Create model" menu item
	pDoc->ModelViewOpen=FALSE;

	fitpage->DestroyWindow();
	delete fitpage;
	customfitpage->DestroyWindow();
	delete customfitpage;
	residualpage->DestroyWindow();
	delete residualpage;
	weightpage->DestroyWindow();
	delete weightpage;
	miscpage->DestroyWindow();
	delete miscpage;
	qh1page->DestroyWindow();
	delete qh1page;
	qh2page->DestroyWindow();
	delete qh2page;
}

void CFinetuneSheet::Hideme()
{
	// Hide the dialog and unhide the "show dialog" button in the fine tune window

	ShowWindow(SW_HIDE);
	(pDoc->pFTview->DlgBackButton).ShowWindow(SW_RESTORE);
}

void CFinetuneSheet::ShowFittingParams()
{
	fitpage->ShowFittingParams();
}

void CFinetuneSheet::Init()
{
	fitpage->layer=layer;
	fitpage->pDoc=pDoc;
	miscpage->layer=layer;
	miscpage->pDoc=pDoc;
	weightpage->pDoc=pDoc;
	weightpage->layer=layer;
	customfitpage->layer=layer;
	customfitpage->pDoc=pDoc;
	residualpage->layer=layer;
	residualpage->pDoc=pDoc;
	qh1page->pDoc=pDoc;
	qh2page->pDoc=pDoc;
}

void CFinetuneSheet::OtherModel()
{
	CleanUp();
	DestroyWindow();
	((CMainFrame *)(AfxGetApp()->m_pMainWnd))->OpenModelChoiceViews(layer);
	// the dialog object itself was created dynamically, this is the last function that
	// is called after the windows has been removed. We can now destroy the object
	// itself
	delete this;
}

int CFinetuneSheet::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CPropertySheet::OnCreate(lpCreateStruct) == -1)
		return -1;

	long sty=GetWindowLong(m_hWnd, GWL_STYLE);
	sty &= (~((long)WS_SYSMENU));
	SetWindowLong(m_hWnd, GWL_STYLE, sty);
	return 0;
}

BOOL CFinetuneSheet::GetRebuildDepth()
{	// access function for rebuilddepth switch
	return miscpage->GetRebuildDepth();
}
