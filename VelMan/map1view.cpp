// Map1View.cpp : implementation file
//

#include "stdafx.h"
#include "velman.h"
#include "VelmanDoc.h"
#include "Map1View.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMapOfShotpointsView

IMPLEMENT_DYNCREATE(CMapOfShotpointsView, CScrollView)

CMapOfShotpointsView::CMapOfShotpointsView()
{
}

CMapOfShotpointsView::~CMapOfShotpointsView()
{
}

BEGIN_MESSAGE_MAP(CMapOfShotpointsView, CScrollView)
	//{{AFX_MSG_MAP(CMapOfShotpointsView)
	ON_WM_DESTROY()
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(ID_SEISMICMAP_DONEBUTTON, OnClickedDoneButton)
	ON_BN_CLICKED(ID_SEISMICMAP_SHOWZNON, OnClickedShowZnon)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMapOfShotpointsView drawing

void CMapOfShotpointsView::OnInitialUpdate()
{
	int i, j, k, rows, cols;
	CZimsGrid *timegrid;

	CScrollView::OnInitialUpdate();

	CRect clrect;
	GetClientRect(&clrect);
	totalsize.cx = clrect.Width()+20;
	totalsize.cy= clrect.Height()+140;

	SetScrollSizes(MM_TEXT, totalsize);
	pDoc=(CVelmanDoc *)GetDocument();

	CRect rect(10, 10, 30, 30);
	DoneButton.Create("X", BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE, rect,
		this, ID_SEISMICMAP_DONEBUTTON);

	rect=CRect(40, 10, 140, 30);
	smallfont.CreatePointFont(70, "Arial");
	ShowZnonBox.Create("Show NULL",
		(BS_AUTOCHECKBOX | BS_PUSHLIKE | WS_CHILD | WS_VISIBLE) & (~WS_EX_CLIENTEDGE), rect, this, ID_SEISMICMAP_SHOWZNON);
	GetDlgItem(ID_SEISMICMAP_SHOWZNON)->SetFont(&smallfont);

	GetParentFrame()->SetWindowPos(&wndTopMost, 0, 0, 0, 0,
		SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

	grid=pDoc->zimscube.FindHorizon(0, CSD_TYPE_TIME, 0);
	// never NULL, that was checked in MainFrm before we open this view!

	// znon will be TRUE at any grid position where any of the time grids contains
	// a znon value
	rows=grid->GetRows();
	cols=grid->GetColumns();
	znon=imatrix(0, rows-1, 0, cols-1);
	// it's faster if we do the following in two steps of two loops each
	for(i=0;i<rows;i++)
		for(j=0;j<cols;j++)
			znon[i][j]=0;
	for(k=0;k<pDoc->horizonlist.GetSize();k++)
	{
		timegrid=pDoc->zimscube.FindHorizon(k, CSD_TYPE_TIME, 0);
		if(timegrid)
		{
			timegrid->Stamp();
			for(i=0;i<rows;i++)
				for(j=0;j<cols;j++)
				{
					if(timegrid->GetZnon(((double **)(*timegrid))[i][j]))
						znon[i][j]=1;
				}
		}
	}

	ShowZnon=0;
}

void CMapOfShotpointsView::OnDraw(CDC* pDC)
{
	int i, j, rows, cols;
	double cx, cy;
	double *xlimits, *ylimits;
	double xpos, ypos;
	int height, width;
	CSeismicData *seisdata=&(pDoc->seismicdata);
	CShotLine *sline;
	CShotPt *spt;

	int hgt, x, y, numlabel, numpts, numexcl;
	char buf[256];

	// find out client rectangle size and from that map size
	CRect clrect;
	GetClientRect(&clrect);
	totalsize.cx = clrect.Width()-100;
	totalsize.cy= clrect.Height()-100;

	// find out whether it's the x or the y dimension of the grid that
	// determines the map size
	xlimits=grid->GetXlimits();
	cx=xlimits[1]-xlimits[0];
	ylimits=grid->GetYlimits();
	cy=ylimits[1]-ylimits[0];
	if(cx<=0 || cy<=0)
	{
		// grid size somewhat indetermined...
		height=1000;
		width=1000;
	}
	else
	{
		if(cx>cy)
		{
			// width determines plot size
			width=totalsize.cx;
			height=(int)((cy/cx)*width);
		}
		else
		{
			height=totalsize.cy;
			width=(int)((cx/cy)*height);
		}
	}

	pDC->Rectangle(10, 90, 10+width, 90+height);
	// so we don't have the hassle with "how many points BETWEEN" etc later
	width--;
	height--;

	numlabel=width > height ? 5 : 3;
	TEXTMETRIC tm;
	pDC->GetTextMetrics(&tm);
	hgt=tm.tmHeight/2;
	CFont *pOldFont=pDC->SelectObject(&smallfont);

	pDC->SetTextAlign(TA_LEFT | TA_BOTTOM);

	for(i=0;i<numlabel;i++)
	{
		x=10+(i*width)/(numlabel-1);
		pDC->MoveTo(x, 90);
		pDC->LineTo(x, 80);
		sprintf(buf, "%7.f", xlimits[0]+i*cx/(numlabel-1));
		pDC->TextOut(x, 80, buf);
		y=90+(i*height)/(numlabel-1);
		pDC->MoveTo(10+width, y);
		pDC->LineTo(20+width, y);
		sprintf(buf, "%7.f", ylimits[1]-i*cy/(numlabel-1));
		pDC->TextOut(25+width, y+hgt, buf);
	}

	numexcl=0;		// count number of points not in the map
	numpts=0;			// total number of points

	for(i=0;i<seisdata->GetSize();i++)
	{
		sline = (CShotLine *) seisdata->GetAt(i);
		for(j=0;j<sline->GetSize();j++)
		{
			spt = (CShotPt *) sline->GetAt(j);
			numpts++;
			xpos=spt->GetX();
			ypos=spt->GetY();
			if(xpos<xlimits[0] || xpos>xlimits[1] || ypos<ylimits[0] || ypos>ylimits[1])
			{
				numexcl++;
				continue;
			}
			x=10+(int)(width/cx*(xpos-xlimits[0]));
			y=90+(int)(height/cy*(-ypos+ylimits[1]));
			pDC->Ellipse(x-2, y-2, x+2, y+2);
		}
	}

	if(numexcl>0)
	{
		sprintf(buf, "%d%% of the shotpoints (%d in total) are not shown on the map",
			(numexcl*100)/numpts, numexcl);
		pDC->SetTextAlign(TA_LEFT | TA_TOP);
		pDC->TextOut(10, 40, buf);
	}

	// draw ZNON areas in a shade
	if(ShowZnon)
	{
		CPen graypen(PS_SOLID, 1, RGB(22, 175, 233));
		rows=grid->GetRows();
		cols=grid->GetColumns();
		pDC->SelectObject(&graypen);
		cx=(double)width/cols;	// # pixels per gridpoint horizontally
		cy=(double)height/rows;
		for(i=0;i<rows;i++)
			for(j=0;j<cols;j++)
			{
				if(znon[i][j])
					pDC->Rectangle(10+(int)(j*cx), 90+(int)(i*cy),
					9+(int)((j+1)*cx), 89+(int)((i+1)*cy));
			}
			pDC->SelectStockObject(BLACK_PEN);
			graypen.DeleteObject();
	}
	pDC->SelectObject(pOldFont);
}

/////////////////////////////////////////////////////////////////////////////
// CMapOfShotpointsView diagnostics

#ifdef _DEBUG
void CMapOfShotpointsView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CMapOfShotpointsView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMapOfShotpointsView message handlers

void CMapOfShotpointsView::OnDestroy()
{
	smallfont.DeleteObject();
	free_imatrix(znon, 0, grid->GetRows()-1, 0, grid->GetColumns()-1);
	CScrollView::OnDestroy();
}

void CMapOfShotpointsView::OnClickedDoneButton()
{
	// clicked on the "Done" button
	pDoc->SeisdataMapOpen=FALSE;
	GetParentFrame()->DestroyWindow();	// close parent window=the frame window around this view
}

void CMapOfShotpointsView::OnClickedShowZnon()
{
	// clicked on the "Show Znons" checkbox
	ShowZnon=IsDlgButtonChecked(ID_SEISMICMAP_SHOWZNON);
	Invalidate();
}

void CMapOfShotpointsView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if(nChar==VK_ESCAPE)
		OnClickedDoneButton();
	else
		CScrollView::OnKeyDown(nChar, nRepCnt, nFlags);
}
