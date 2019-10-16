// TGcontv.cpp : implementation file
//

#include "stdafx.h"
#include "velman.h"
#include "VelmanDoc.h"
#include "ChooseAGridDlg.h"
#include "ChooseManyGridsDlg.h"
#include "ContourView.h"
#include "velmanpalette.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CContourView

IMPLEMENT_DYNCREATE(CContourView, CScrollView)

CContourView::CContourView()
{
	showstuff.showwells=TRUE;
	showstuff.showfaults=FALSE;
	showstuff.showshotpoints=FALSE;
}

CContourView::~CContourView()
{
}

BEGIN_MESSAGE_MAP(CContourView, CScrollView)
	//{{AFX_MSG_MAP(CContourView)
	ON_WM_DESTROY()
	ON_WM_RBUTTONDOWN()
	ON_WM_KEYDOWN()
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(ID_ALLRESI_DONEBUTTON, OnClickedDoneButton)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CContourView drawing

void CContourView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	CRect clrect;
	GetClientRect(&clrect);
	totalsize.cx = clrect.Width()-100;
	totalsize.cy= clrect.Height()-50;

	SetScrollSizes(MM_TEXT, totalsize);
	pDoc=(CVelmanDoc *)GetDocument();

	CZimsCube *cube=&pDoc->zimscube;
	int gridno=pDoc->pTCdlg->chosengrid;
	orggrid=(CZimsGrid *)(cube->GetAt(gridno));
	grid=new CZimsGrid(pDoc);
	(*grid)=(*orggrid);
		
	/*int count = 0;
	char buffer[128];
	double maxTouchingDifference = AfxGetApp()->GetProfileInt("ArithmeticParams", "MaxTouchingDifference", 500); // max difference between touching values
	maxTouchingDifference /= 1000; // make it into milliseconds
	for(int i=0;i<grid->GetRows();i++)
	{
		for(int j=0;j<grid->GetColumns();j++)
		{
			double value = grid->GetGrid()[i][j];

			if(grid->GetZnon(value))
			{
				continue;
			}

			if(value >= -maxTouchingDifference && value <= maxTouchingDifference)
			{
				count++;
			}
		}
	}
	sprintf(buffer, "Number of 0 values: %d", count);
	AfxMessageBox(buffer);*/
	

	cp=new CContourPlot(pDoc);
	cp->InitResampledData(grid);

	cp->SetContourOption(CNTOPT_CREATEKEY, (long)(5L<<16L)+50L);

	CRect rect(10, 10, 30, 30);
	DoneButton.Create("X", BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE, rect, this, ID_ALLRESI_DONEBUTTON);
	GetParentFrame()->SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

	showstuff.showwells=TRUE;
	showstuff.showfaults=FALSE;
	showstuff.showshotpoints=FALSE;

	// offer "wells" only if there is any info on this
	// Set up all we need to show well names on contour plot
	HaveWells=(pDoc->ProjState & PRJ_HAVEWELLS) && (grid->GetHorizon()>=0);
	if(HaveWells)
	{
		int i, j;
		int numwells=pDoc->horizonlist.wellnames.GetSize();
		CHorizon *horizon=pDoc->horizonlist[grid->GetHorizon()];
		CMeasurePt *mpt;
		labeltext=new char *[numwells];
		labelx=new double[numwells];
		labely=new double[numwells];
		double *gridY;

		gridY=grid->GetYlimits();
		for(i=0, j=0;i<numwells;i++)
		{
			labeltext[i]=new char[256];
			mpt=(CMeasurePt *)horizon->GetAt(i);
			if(mpt && !(mpt->ReadStatus() & MPT_STATUS_INVALID))
			{
				sprintf(labeltext[j], "  %s", (const char *)(pDoc->horizonlist.wellnames[i]));
				labelx[j]=mpt->ReadX();
				// careful: flip north/south
				labely[j]=gridY[0]+gridY[1]-mpt->ReadY();
				j++;  // j counts actual wells; some might not exist in this layer
			}
		}
		cp->SetContourLabels(j, labelx, labely, labeltext);
		cp->SetContourOption(CNTOPT_LABELDOTSIZE, cp->dotsize);  // this turns of nice size dot.
	}
}

void CContourView::OnDraw(CDC* pDC)
{
	// multi form behaviour removed
	//cp->ForceContSetToClose();// if the contour window is open then shut it!
	int i, j;
	double cx, cy;
	int height, width;
	double xpos, ypos, x1pos, y1pos;
	int x, y;
	int xoffset = 10;
	int yoffset = 120;

	// find out client rectangle size and from that contour plot size
	CRect clrect;
	GetClientRect(&clrect);

	totalsize.cx = clrect.Width()-100;
	totalsize.cy= clrect.Height()-50;

	// find out whether it's the x or the y dimension of the grid that
	// determines the contour plot size
	double *xlimits, *ylimits;
	xlimits=grid->GetXlimits();
	cx=xlimits[1]-xlimits[0];
	ylimits=grid->GetYlimits();
	cy=ylimits[1]-ylimits[0];
	if(cx<=0 || cy<=0)
	{
		// grid size indetermined
		height=1000;
		width=1000;
	}
	else
	{
		if(cx>cy)
		{
			// width determines plot size
			width=totalsize.cx-20;
			height=(int)((cy/cx)*width)-100;
		}
		else
		{
			height=totalsize.cy-100;
			width=(int)((cx/cy)*height)-20;
		}
	}

	//  Draw the contour plot
	cp->DrawContourPlot(pDC, xoffset, yoffset, xoffset+width, yoffset+height);
	
	// Draw the axis labels
	cp->DrawAxisLabels(pDC, xoffset, yoffset, xoffset+width, yoffset+height);

	// draw well labels
	cp->DrawWellLabels(pDC, xoffset, yoffset, xoffset+width, yoffset+height);
	
	// draw shotpoints if requested
	if(showstuff.showshotpoints)
	{
		CSeismicData *seisdata=&(pDoc->seismicdata);
		CShotLine *sline;
		CShotPt *spt;
		// 15/10/00 Showpoint plotting does not select pen colour.
		// this appears to be causing crashes on the sun but not on the pc.
		CPen thickpen(PS_SOLID, 1, RGB(50, 50, 50));
		CPen *pOldPen=pDC->SelectObject(&thickpen);
		CBrush solidbrush(RGB(50, 50, 50));
		CBrush *pOldBrush=pDC->SelectObject(&solidbrush);

		for(i=0;i<seisdata->GetSize();i++)
		{
			sline = (CShotLine *) seisdata->GetAt(i);
			for(j=0;j<sline->GetSize();j++)
			{
				spt = (CShotPt *) sline->GetAt(j);
				xpos=spt->GetX();
				ypos=spt->GetY();
				if(xpos<xlimits[0] || xpos>xlimits[1] || ypos<ylimits[0] || ypos>ylimits[1])
					continue;
				x=xoffset+(int)(width/cx*(xpos-xlimits[0]));
				y=yoffset+(int)(height/cy*(-ypos+ylimits[1]));
				pDC->Ellipse(x-2, y-2, x+2, y+2);
			}
		}
		pDC->SelectObject(pOldPen);
		pDC->SelectObject(pOldBrush);
	}

	// draw faultlines if requested
	if(showstuff.showfaults)
	{
		// so we don't have the hassle with "how many points BETWEEN" etc later
		width--;
		height--;
		CFaultGrid *fgrid=grid->GetFaultGrid();
		CPen thickpen(PS_SOLID, 1, RGB(0, 0, 0));
		CPen *pOldPen=pDC->SelectObject(&thickpen);
		CFaultLine *fline;
		for(i=0;i<fgrid->GetSize();i++)
		{
			fline = (CFaultLine *) fgrid->GetAt(i);
			xpos=fline->GetX0();
			ypos=fline->GetY0();
			x1pos=fline->GetX1();
			y1pos=fline->GetY1();
			if(xpos<xlimits[0] || xpos>xlimits[1] || ypos<ylimits[0] || ypos>ylimits[1]
			|| x1pos<xlimits[0] || x1pos>xlimits[1] || y1pos<ylimits[0] || y1pos>ylimits[1])
				continue;
			x=xoffset+(int)(width/cx*(xpos-xlimits[0]));
			y=yoffset+(int)(height/cy*(-ypos+ylimits[1]));
			pDC->MoveTo(x, y);
			x=xoffset+(int)(width/cx*(x1pos-xlimits[0]));
			y=yoffset+(int)(height/cy*(-y1pos+ylimits[1]));
			pDC->LineTo(x, y);
		}
		pDC->SelectObject(pOldPen);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CContourView diagnostics

#ifdef _DEBUG
void CContourView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CContourView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CContourView message handlers

void CContourView::OnDestroy()
{
	delete grid;
	delete cp;
	// clean up well label name database
	if(HaveWells)
	{
		int numwells=pDoc->horizonlist.wellnames.GetSize();
		delete labelx;
		delete labely;
		for(int i=0;i<numwells;i++)
			delete labeltext[i];
		delete labeltext;
	}
	CScrollView::OnDestroy();

}

void CContourView::OnClickedDoneButton()
{
	// clicked on the "Done" button
	pDoc->TimegridContourOpen=FALSE;
	GetParentFrame()->DestroyWindow();	// close parent window=the frame window around this view
}

void CContourView::OnRButtonDown(UINT nFlags, CPoint point)
{
	// right mouse button evokes "Contour settings" dialog
	int options=CNTDLG_OPT_NOZOOM;
	if(grid->HasFaultGrid())
		options|=CNTDLG_SHOWFAULTS;
	if(pDoc->ProjState & PRJ_HAVERMSVEL)
		options|=CNTDLG_SHOWSHOTPOINTS;
	if( (pDoc->ProjState & PRJ_HAVEWELLS) && grid->GetHorizon()>=0)
		options|=CNTDLG_SHOWWELLS;

	cp->ContourOptionsDialog(options, &showstuff);

	cp->SetContourOption(CNTOPT_LABELLING, ((cp->GetLabelOptions()) & ~CNT_USERLABEL) |
		(showstuff.showwells ? CNT_USERLABEL : 0) );
	Invalidate();
}

void CContourView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if(nChar==VK_ESCAPE)
		OnClickedDoneButton();
	else
		CScrollView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CContourView::OnSetFocus(CWnd* pOldWnd)
{
	CScrollView::OnSetFocus(pOldWnd);

	/* multi form behaviour removed
	//code to force re-draw as soon as the window gets
	// focus
	Invalidate();
	*/
}

