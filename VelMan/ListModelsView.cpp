// listofmo.cpp : implementation file

#include "stdafx.h"
#include "velman.h"
#include "VelmanDoc.h"
#include "ListModelsView.h"
#include "mainfrm.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CListOfModelsView

IMPLEMENT_DYNCREATE(CListOfModelsView, CScrollView)

CListOfModelsView::CListOfModelsView()
{
}

CListOfModelsView::~CListOfModelsView()
{
}

BEGIN_MESSAGE_MAP(CListOfModelsView, CScrollView)
	//{{AFX_MSG_MAP(CListOfModelsView)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(ID_LISTOFMO_DONEBUTTON, OnClickedDoneButton)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CListOfModelsView drawing

void CListOfModelsView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	pDoc = (CVelmanDoc*)GetDocument();

	CSize sizeTotal(500, 150+180*(pDoc->horizonlist.GetSize()));
	SetScrollSizes(MM_TEXT, sizeTotal);

	CRect rect(10, 10, 80, 35);
	DoneButton.Create("Close", BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE, rect,
		this, ID_LISTOFMO_DONEBUTTON);
	GetParentFrame()->SetWindowPos(&wndTopMost, 0, 0, 0, 0,
		SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

	WINDOWPLACEMENT wp;

	GetParentFrame()->GetWindowPlacement(&wp);
	wp.showCmd=SW_SHOWMAXIMIZED;
	GetParentFrame()->SetWindowPlacement(&wp);
}

void CListOfModelsView::OnDraw(CDC* pDC)
{
	pDC->SetMapMode(MM_TEXT);

	CFont tablefont;
	tablefont.CreateFont(15, 0, 0, 0, 400, FALSE, FALSE, 0, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_SWISS, "Arial");
	CFont *pOldFont=pDC->SelectObject(&tablefont);

	// draw frame around "done" button
	CRect rect(10-5, 10-5, 80+5, 35+5);
	pDC->SelectStockObject(LTGRAY_BRUSH);
	pDC->Rectangle(rect);

	pDC->TextOut(100, 5, "Double click onto a model to fine tune it.");
	pDC->TextOut(100, 25, "Note that you have to work on each layer in sequence.");

	pDC->TextOut(10, 50, "Layer");
	pDC->TextOut(150, 50, "Model function");
	pDC->SelectStockObject(BLACK_PEN);
	pDC->MoveTo(10, 65);
	pDC->LineTo(490, 65);

	char buf[512];
	int  model = 0;
	for(int i=0;i<pDoc->horizonlist.GetSize();i++)
	{
		CHorizon *horizon=(CHorizon *) pDoc->horizonlist.GetAt(i);
		pDC->TextOut(10, 70+15*3*i, horizon->name);
		if(horizon->HaveModel)
		{
			sprintf(buf, "%s with %s residual weighting",
				(LPCTSTR) pDoc->well_model_lib[horizon->ModelNumber]->name,
				horizon->DistWeight==0 ? "linear" : "quadratic");
			pDC->TextOut(150, 70+15*3*i, buf);
			model=horizon->ModelNumber;
			if ( model == 0 )       // 6/02. Convert to Vo/K parameters for model 0.
			{
				sprintf(buf, "%s=%-12.2f",
					(LPCTSTR) pDoc->well_model_lib[horizon->ModelNumber]->parnameA,
					ab2V(horizon->FitA, horizon->FitB));
				pDC->TextOut(150, 70+15*(3*i+1), buf);
				sprintf(buf, "%s=%-8.4f",
					(LPCTSTR) pDoc->well_model_lib[horizon->ModelNumber]->parnameB,	// ditto
					ab2K(horizon->FitA, horizon->FitB));
				pDC->TextOut(150, 70+15*(3*i+2), buf);
			}
			else
			{
				sprintf(buf, "%s=%16.12g",						// 3/1/00 increased precision (ditto below for FitB)
					(LPCTSTR) pDoc->well_model_lib[horizon->ModelNumber]->parnameA,	// ditto.
					horizon->FitA);
				pDC->TextOut(150, 70+15*(3*i+1), buf);
				sprintf(buf, "%s=%16.12g",
					(LPCTSTR) pDoc->well_model_lib[horizon->ModelNumber]->parnameB,	// ditto
					horizon->FitB);
				pDC->TextOut(150, 70+15*(3*i+2), buf);
			}
		}
		else
		{
			pDC->TextOut(150, 70+15*3*i, "[ no model defined yet ]");
		}
	}
	// reset font
	pDC->SelectObject(pOldFont);
	tablefont.DeleteObject();
}

/////////////////////////////////////////////////////////////////////////////
// CListOfModelsView message handlers

void CListOfModelsView::OnUpdate(CView *pSender, LPARAM lHint, CObject *pHint)
{
	if(lHint==0 || lHint==DOC_CHANGE_FINETUNECHANGE || lHint==DOC_CHANGE_MODELCHOICE)
		CScrollView::OnUpdate(pSender, lHint, pHint);
}

void CListOfModelsView::OnClickedDoneButton()
{
	// clicked on the "Done" button
	pDoc->ListOfModelsOpen=FALSE;
	GetParentFrame()->DestroyWindow();	// close parent window=the frame window around this view
}

void CListOfModelsView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// get mouse pointer position into p
	CPoint p=GetDeviceScrollPosition( );
	p.x=p.x+point.x;
	p.y=p.y+point.y;

	int ChosenLayer=(p.y-70)/(3*15);
	int i = 0;
	CMainFrame *mf;
	CHorizon *horizon;
	CString s;
	int hornumber=pDoc->horizonlist.GetSize();

	if(ChosenLayer<0 || ChosenLayer>=hornumber)
		return;

	if(pDoc->ModelViewOpen)
	{
		AfxMessageBox("You already have a modelling window open.\n"
			"Close this window first, then you can double-click on model in this list"
			" to open the corresponding fine tuning window.");
		return;
	}

	// you have to model all layers in sequence. This here is just a copy of the same
	// check as we perform it in modelcho.cpp after OK was clicked, including some
	// code from that dialog's INIT routine (to find LastModel)

	int LastModel=-1;
	for(i=0;i<hornumber;i++)
	{
		horizon = (CHorizon *) pDoc->horizonlist.GetAt(i);
		if(horizon->HaveModel)
			LastModel=i;
	}

	if(ChosenLayer>LastModel+1)
	{
		// reaching too far
		horizon = (CHorizon *) pDoc->horizonlist.GetAt(LastModel+1);

		s="ERROR:\nYou need to work on each layer in sequence. The next layer you can choose"
			" is '"+horizon->name+"'.";
		AfxMessageBox((const char *)s, MB_OK);
		return;
	}

	if(ChosenLayer<LastModel)
	{
		// not reaching far enough: warn that work will be destroyed
		if(ChosenLayer<LastModel-1)
		{
			// invalidating more than one layer
			s="WARNING:\nYou have already worked out models for layers under this one."
				" Revising this model"
				" means invalidating all depth conversion performed for layers '";
			horizon = (CHorizon *) pDoc->horizonlist.GetAt(ChosenLayer+1);
			s+=horizon->name;
			s+="' through '";
			horizon = (CHorizon *) pDoc->horizonlist.GetAt(LastModel);
			s+=horizon->name;
			s+="'.\nAre you sure you want to continue?";
		}
		else
		{
			// invalidating one layer
			s="ERROR:\nYou have already worked out a model for the layer '";
			horizon = (CHorizon *) pDoc->horizonlist.GetAt(LastModel);
			s+=horizon->name;
			s+="' which lies under '";
			horizon = (CHorizon *) pDoc->horizonlist.GetAt(ChosenLayer);
			s+=horizon->name;
			s+="', and you will lose the depth conversion that might already have been performed for"
				" it.\nAre you sure you want to continue?";
		}
		if(AfxMessageBox((const char *)s, MB_YESNO)!=IDYES)
			return;
		// user wants to continue; remove "have model" flags from all invalidated horizons
		for(i=ChosenLayer+1;i<=LastModel;i++)
		{
			horizon = (CHorizon *) pDoc->horizonlist.GetAt(i);
			horizon->HaveModel=FALSE;
			horizon->HaveFit=FALSE;
		}
	}

	horizon=(CHorizon *)pDoc->horizonlist.GetAt(ChosenLayer);

	mf=(CMainFrame *)GetParentFrame()->GetParentFrame();	// find out main frame window
	if(!(horizon->HaveModel))
	{
		mf->OpenModelChoiceViews(ChosenLayer);
	}
	else
	{
		mf->OpenFineTuneView(ChosenLayer);
	}
}

void CListOfModelsView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if(nChar==VK_ESCAPE)
		OnClickedDoneButton();
	else
		CScrollView::OnKeyDown(nChar, nRepCnt, nFlags);
}
