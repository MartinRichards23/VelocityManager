// finetune.cpp : implementation file
//

#include "stdafx.h"
#include "velman.h"
#include "VelmanDoc.h"
#include "finetune.h"
#include "EditwellDlg.h"
#include "ft_sheet.h"
#include "ftrespag.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

// macros for mapping from screen/window coords to contour coords.
// remember that x increases left to right
// but y decreases as you go top to bottom on screen.
// Also need to subtract the margin from the window coords to map the window origin
// to the origin of the contour plot
#define XtoWin(x) (int)(800.0*((x)-minx)/(maxx-minx))+100
#define YtoWin(y) (int)(800.0*(maxy-(y))/(maxy-miny))+100

#define WintoX(x) ((contx2-contx1)*(x-cxplotmargin)/(winsize-2*cxplotmargin) + contx1)
#define WintoY(y) conty2 - ((conty2-conty1)*(y-cyplotmargin)/(winsize-2*cyplotmargin))

/////////////////////////////////////////////////////////////////////////////
// CFineTuneView

IMPLEMENT_DYNCREATE(CFineTuneView, CScrollView)

CFineTuneView::CFineTuneView()
{
	SuppressDisplay = FALSE;
}

CFineTuneView::~CFineTuneView()
{
}

BEGIN_MESSAGE_MAP(CFineTuneView, CScrollView)
	//{{AFX_MSG_MAP(CFineTuneView)
	ON_WM_DESTROY()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(ID_FINETUNE_DLGBACKBUTTON, OnClickedDlgBackButton)
END_MESSAGE_MAP()

void CFineTuneView::RescaleYAxis()
{
	int i = 0;
	miny = 200000.0;
	maxy = -10.0;
	double a = horizon->FitA, b = horizon->FitB; // fitting paramenters
	if (model == 0)
		SuppressDisplay = TRUE; // will be set to FALSE again if any point to be displ.
	for (i = 0; i < numpts; i++)
	{
		if (model == 0)
		{
			if (1 == 0 && !(horizon->HaveFit))
				numpts = 0; // impossible for exp. model w/o fit, sorry
			else
			{
				maxy = max(maxy, yval[i] - z1[i] * exp(xval[i] / a));
				miny = min(miny, yval[i] - z1[i] * exp(xval[i] / a));
				SuppressDisplay = FALSE;
			}
		}
		else
		{
			maxy = max(maxy, yval[i]);
			miny = min(miny, yval[i]);
		}
	}
	double factor = 100.0;
	maxy = factor * ceil(maxy / factor); // remove this to improve spread of points: +factor;
	miny = factor * floor(miny / factor); // (see also similar code in OnInitialUpdate: -factor;
	TRACE("RescaleYAxis: maxy = %f, miny = %f\n", maxy, miny);
	if (fabs(maxy - miny) < 1)
		maxy = miny + 100.0;
}

void CFineTuneView::OnInitialUpdate()
{
	int i = 0;

	CScrollView::OnInitialUpdate();

	// Get default values for panel controls
	ShowGridLines = (BOOL)AfxGetApp()->GetProfileInt("Attributes", "ShowCoordGridLines", 1);
	ShowWellLabels = (BOOL)AfxGetApp()->GetProfileInt("Attributes", "ShowWellLabels", 1);
	ShowAllWells = (BOOL)AfxGetApp()->GetProfileInt("Attributes", "ShowAllWells", 1);

	// This is a scale-to-fit-window, the user can alter the size and aspect ratio!
	cxplotmargin = 60;
	cyplotmargin = 120;
	winsize = 1000;
	CSize totalsize = CSize(winsize, winsize);
	SetScaleToFitSize(totalsize);
	//SetScrollSizes(MM_TEXT, totalsize);

	pDoc = (CVelmanDoc *)GetDocument();
	pDoc->pFTview = this;
	cp = new CContourPlot(pDoc);
	HaveValidContour = FALSE;
	ViewMode = VIEWMODE_SCATTER;
	ZoomDepth = 0;

	// find out what layer we work with (was set in MainFrame::OpenFineTune)
	layer = pDoc->ActiveLayer;
	horizon = (CHorizon *)((pDoc->horizonlist).GetAt(layer));
	horizon->HaveFit = FALSE; // we need a completely new fit for starters!
	model = horizon->ModelNumber;
	// allocate memory for a copy of the list of points contained in this view.
	// those lists are filled by CopyLayer(), called by OnDraw() whenever a new
	// fit has to be calculated
	// deallocation takes place in the OnDestroy() handler.
	numpts = pDoc->horizonlist.wellnames.GetSize();
	numallocpts = numpts;
	xval = new double[numpts];
	yval = new double[numpts];
	z1 = new double[numpts];
	contx = new double[numpts];
	conty = new double[numpts];
	labeltext = new char *[numpts];
	for (i = 0; i < numpts; i++)
		labeltext[i] = new char[256];
	labelx = new double[numpts];
	labely = new double[numpts];

	residual = new double[numpts];
	wellindex = new int[numpts];
	ptx = new int[numpts]; // position of point in window coordinates (set during OnDraw)
	pty = new int[numpts];
	datapoint = new CMeasurePt *[numpts]; // pointer to actual measurept object for each point (for editing)

	CopyLayer(); // fills contents of arrays
	minx = 200000.0;
	maxx = -10.0;
	miny = 200000.0;
	maxy = -10.0;

	pDoc->horizonlist.FitLayer(layer, pDoc);
	HaveValidContour = FALSE;
	double a = horizon->FitA, b = horizon->FitB; // fitting paramenters
	for (i = 0; i < numpts; i++)
	{
		maxx = max(maxx, xval[i]);
		minx = min(minx, xval[i]);
		if (model == 0)
		{
			if (!(horizon->HaveFit))
				numpts = 0; // impossible for exp. model w/o fit, sorry
			else
			{
				maxy = max(maxy, yval[i] - z1[i] * exp(xval[i] / a));
				miny = min(miny, yval[i] - z1[i] * exp(xval[i] / a));
			}
		}
		else
		{
			maxy = max(maxy, yval[i]);
			miny = min(miny, yval[i]);
		}
	}

	// Set axis limits by rounding to nearest 100 or 10 depending
	// on range of the data.
	double factor;
	factor = 100.0;
	if (fabs(maxx - minx) < 100.0)
		factor = 10.0;
	maxx = factor * ceil(maxx / factor); // remove this to improve spread of points +factor;
	minx = factor * floor(minx / factor); // see similar code in RescaleYAxis -factor;
	factor = 100.0;
	if (fabs(maxy - miny) < 100.0)
		factor = 10.0;
	maxy = factor * ceil(maxy / factor); // as above +factor;
	miny = factor * floor(miny / factor); // -factor;

	// set limits for contour plotting
	SetContLimits();

	// create "Bring Dialog back" button
	CRect rect(0, 0, 70, 25);
	DlgBackButton.Create("Dialog!", BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE, rect,
		this, ID_FINETUNE_DLGBACKBUTTON);
	DlgBackButton.ShowWindow(SW_HIDE); // show only when control dlg hidden!

	cp->SetContourOption(CNTOPT_LABELTYPESIZE, 13);
	cp->SetContourOption(CNTOPT_SHADELEVELS, TRUE);

	// As we use colour fill by default now, turn off labelling by default
	if (cp->GetColourShading())
		cp->SetContourOption(CNTOPT_LABELLING, 0);
	else
		cp->SetContourOption(CNTOPT_LABELLING, CNT_USERLABEL | CNT_AUTOLABEL);

	cp->SetContourOption(CNTOPT_UPSIDEDOWN, TRUE);

	WellUnderCursor = -1; // see OnMouseMove
	WellNameFont.CreateFont(30, 0, 0, 0, 400, FALSE, FALSE, 0, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_SWISS, "Arial");
}

void CFineTuneView::SetContLimits()
{
	// sets limits for contour view
	int i = 0;

	TotalDim.MinX = FLT_MAX;
	TotalDim.MinY = FLT_MAX;
	TotalDim.MaxX = -FLT_MAX;
	TotalDim.MaxY = -FLT_MAX;

	for (i = 0; i < numpts; i++)
	{
		TotalDim.MaxX = max(TotalDim.MaxX, datapoint[i]->ReadX());
		TotalDim.MinX = min(TotalDim.MinX, datapoint[i]->ReadX());
		TotalDim.MaxY = max(TotalDim.MaxY, datapoint[i]->ReadY());
		TotalDim.MinY = min(TotalDim.MinY, datapoint[i]->ReadY());
	}

	// round spatial coords by 10%
	double wid = TotalDim.MaxX - TotalDim.MinX;
	double hgt = TotalDim.MaxY - TotalDim.MinY;
	double *xlimits, *ylimits;
	wid *= 0.05;
	hgt *= 0.05;

	TotalDim.MaxX += wid;
	TotalDim.MinX -= wid;
	TotalDim.MaxY += hgt;
	TotalDim.MinY -= hgt;

	CurrentDim.MaxX = TotalDim.MaxX;
	CurrentDim.MinX = TotalDim.MinX;
	CurrentDim.MaxY = TotalDim.MaxY;
	CurrentDim.MinY = TotalDim.MinY;

	CZimsGrid *timegrid;
	timegrid = pDoc->zimscube.FindHorizon(layer, CSD_TYPE_TIME, 0);

	if (timegrid)
	{
		if (ShowAllWells == 0)
		{
			// have a time grid at hand ==> take its coords for contouring
			xlimits = timegrid->GetXlimits();
			ylimits = timegrid->GetYlimits();
			contx1 = xlimits[0];
			contx2 = xlimits[1];
			conty1 = ylimits[0];
			conty2 = ylimits[1];
		}
		else
		{
			// else take min of grid and well coords for frame for contouring
			xlimits = timegrid->GetXlimits();
			ylimits = timegrid->GetYlimits();
			contx1 = min(CurrentDim.MinX, xlimits[0]);
			contx2 = max(CurrentDim.MaxX, xlimits[1]);
			conty1 = min(CurrentDim.MinY, ylimits[0]);
			conty2 = max(CurrentDim.MaxY, ylimits[1]);
		}
	}
	else
	{
		// else take just well coords for frame for contouring
		contx1 = CurrentDim.MinX;
		contx2 = CurrentDim.MaxX;
		conty1 = CurrentDim.MinY;
		conty2 = CurrentDim.MaxY;
	}
}

// copies data from well data base into internal lists. WARNING: Assumes that
// these internal lists have already been allocated! (and are large enough...)
void CFineTuneView::CopyLayer()
{
	double x = 0, y = 0;
	int l = 0;

	for (int i = 0; i < pDoc->horizonlist.wellnames.GetSize(); i++)
	{
		// get measurement point
		CMeasurePt *measurept = (CMeasurePt *)horizon->GetAt(i);
		if (measurept && !((measurept->ReadStatus()) & MPT_STATUS_INVALID))
		{
			measurept->ReadValuePair(model, &x, &y);
			if (x != MPT_ILLEGAL && y != MPT_ILLEGAL && measurept->ReadZ1() != MPT_ILLEGAL
				&& !(layer == 0 && x == 0 && (model == 4 || model == 6)))
			{
				xval[l] = x;
				yval[l] = y;
				z1[l] = measurept->ReadZ1();
				datapoint[l] = measurept;
				wellindex[l] = i;
				l++;
			}
		}
	}
	numpts = l; // now we know the number of valid points (i.e. with measurept != NULL)
}

/////////////////////////////////////////////////////////////////////////////
// CFineTuneView drawing
// Draws the Scatter graph or alternatively the contour plot
void CFineTuneView::OnDraw(CDC* pDC)
{
	char buf[256];
	TEXTMETRIC tm; // needed for vertically centered output
	int hgt = 0;
	CFont *pOldFont;

	pDC->SetMapMode(MM_ANISOTROPIC);
	pDC->SetWindowExt(winsize, winsize);

	CopyLayer(); // fill internal lists with most recent values
	int i = 0, j = 0, k = 0;
	CWellModelFunction *modfunc = pDoc->well_model_lib[model];

	if (!(horizon->HaveFit))// try once to compute a new fit if necessary
	{
		pDoc->horizonlist.FitLayer(layer, pDoc);
		HaveValidContour = FALSE;
	}

	if (model == 0 && ViewMode == VIEWMODE_SCATTER)
		RescaleYAxis();

	if (pDoc->pFTsheet != NULL)
		pDoc->pFTsheet->ShowFittingParams();

	// Font to do axis labels
	CFont axisfont;
	axisfont.CreatePointFont(120, "Arial");

	if (ViewMode == VIEWMODE_SCATTER)
	{
		// Draw title
		pOldFont = pDC->SelectObject(&axisfont);
		pDC->SetTextAlign(TA_CENTER);
		CSize titlesize = pDC->GetTextExtent(modfunc->name);
		if (strcmp((LPCTSTR)modfunc->name, "Instantaneous vel. ~ depth"))
		{
			pDC->TextOut(500, 5, modfunc->name);
			pDC->MoveTo(500 - titlesize.cx / 2, 5 + titlesize.cy); // draw line under
			pDC->LineTo(500 + titlesize.cx / 2, 5 + titlesize.cy);
		}
		else
		{
			double yoff;
			pDC->TextOut(500, 5, "Instantaneous vel. ~ depth, ");
			yoff = titlesize.cy*1.1;
			pDC->TextOut(500, 5 + (int)yoff, "represented by, ");
			pDC->TextOut(500, 5 + (int)yoff * 2, "Compensated depth .vs. Isochronochore");
		}

		if (model == 0 && SuppressDisplay)
			return;
		if (model == 10)
		{
			// it's "contoured intvel only" -- very simple plot in this case, namely nothing
			CFont msgfont;
			msgfont.CreateFont(70, 0, 0, 0, 400, FALSE, FALSE, 0, DEFAULT_CHARSET,
				OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
				DEFAULT_PITCH | FF_SWISS, "Arial");
			CFont *pOldFont = pDC->SelectObject(&msgfont);
			CSize bordersize = pDC->GetTextExtent("\"Contoured interval velocities\"");
			i = bordersize.cx / 2 + 50;
			pDC->SelectStockObject(WHITE_BRUSH);
			pDC->SelectStockObject(BLACK_PEN);
			pDC->Rectangle(500 - i, 250, 500 + i, 720);
			pDC->SetTextAlign(TA_CENTER);
			pDC->TextOut(500, 300, "\"Contoured interval velocities\"");
			pDC->TextOut(500, 500, "Scatter view not applicable;");
			pDC->TextOut(500, 580, "please see contour plot");
			pDC->SelectObject(pOldFont);
			axisfont.DeleteObject();
			msgfont.DeleteObject();
			return;
		}
		// smallish font for well names
		CFont SmallWellnameFont;
		SmallWellnameFont.CreateFont(16, 0, 0, 0, 400, FALSE, FALSE, 0, DEFAULT_CHARSET,
			OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
			DEFAULT_PITCH | FF_SWISS, "Arial");

		CDWordArray *horstatus = &(pDoc->horizonlist.wellstatus);

		// Draw Coordinate system
		pDC->SelectStockObject(BLACK_PEN);
		pDC->MoveTo(100, 100);
		pDC->LineTo(100, 900);
		pDC->LineTo(900, 900);

		// Draw y axis labels and ticks
		pDC->SelectObject(&axisfont);
		pDC->SetTextAlign(TA_RIGHT);
		pDC->GetTextMetrics(&tm);

		// select axis labelling scheme
		int numlabel = 11; // changed from 5
		double label = (maxy - miny) / (numlabel - 1);
		double y = maxy;
		int yposition = 0;

		for (i = 0; i < numlabel; i++, y -= label)
		{
			sprintf(buf, "%5.0f", y);
			yposition = YtoWin(y);
			pDC->MoveTo(100, yposition);
			pDC->LineTo(80, yposition);
			pDC->TextOut(80, yposition - tm.tmHeight / 2, buf, strlen(buf));
		}

		// draw dotted lines
		CPen dotpen(PS_DOT, 1, RGB(0, 0, 0));
		CPen *pOldpen = pDC->SelectObject(&dotpen);
		if (ShowGridLines)
		{
			for (i = 0, y = maxy; i < numlabel - 1; i++, y -= label)
			{
				yposition = YtoWin(y);
				pDC->MoveTo(100, yposition);
				pDC->LineTo(900, yposition);
			}
		}

		// Draw x axis labels and ticks
		pDC->SetTextAlign(TA_CENTER);
		pDC->SelectStockObject(BLACK_PEN);
		numlabel = 11;

		int xposition = 0;
		double x = 0;

		x = minx;
		label = (maxx - minx) / (numlabel - 1);
		for (i = 0; i < numlabel; i++, x += label)
		{
			sprintf(buf, "%5.0f", x);
			xposition = 100 + (int)(800.0*(x - minx) / (maxx - minx));
			pDC->MoveTo(xposition, 900);
			pDC->LineTo(xposition, 920);
			pDC->TextOut(xposition, 930, buf, 5);
		}

		if (ShowGridLines)
		{
			// draw dotted lines
			pDC->SelectObject(&dotpen);
			for (i = 1, x = minx + label; i < numlabel; i++, x += label)
			{
				xposition = XtoWin(x);
				pDC->MoveTo(xposition, 900);
				pDC->LineTo(xposition, 100);
			}
		}

		// draw plot of scattered dots and of fitting function itself
		pDC->SelectStockObject(NULL_BRUSH);
		CPen graypen(PS_SOLID, 1, ((CVelmanApp *)AfxGetApp())->GetProfileColor
		("Attributes", "DeselectedPoint", RGB(192, 192, 192)));
		CPen bluepen(PS_SOLID, 1, ((CVelmanApp *)AfxGetApp())->GetProfileColor
		("Attributes", "FalselyVerticalPoint", RGB(0, 0, 255)));

		pDC->SelectObject(&graypen);
		pDC->SelectObject(&SmallWellnameFont);
		pDC->GetTextMetrics(&tm);
		hgt = tm.tmHeight / 2; // half the height of a textline (used for centered output)
		pDC->SetTextAlign(TA_LEFT | TA_TOP);

		double a = horizon->FitA, b = horizon->FitB; // fitting paramenters
		CBrush redbrush(RGB(200, 0, 0));
		CPen redpen(PS_SOLID, 1, RGB(200, 0, 0));
		int is = 0;

		for (i = 0; i < numpts; i++)
		{
			xposition = XtoWin(xval[i]);
			if (model == 0)
				yposition = YtoWin(yval[i] - z1[i] * exp(xval[i] / a));
			else
				yposition = YtoWin(yval[i]);
			ptx[i] = xposition;
			pty[i] = yposition;
			strcpy(labeltext[i], pDoc->horizonlist.wellnames[wellindex[i]]);
			for (is = 0; *(labeltext[i] + is) == ' '; is++)
				;
			if (ShowWellLabels)
			{
				if (!_strnicmp(labeltext[i] + is, "XX", 2))
				{
					pDC->SelectObject(&redbrush);
					pDC->SetTextColor(RGB(200, 0, 0));
				}
				else
				{
					pDC->SelectStockObject(BLACK_BRUSH);
					pDC->SetTextColor(RGB(0, 0, 0));
				}
				pDC->TextOut(ptx[i] + 15, pty[i] - hgt, (const char *)
					(pDoc->horizonlist.wellnames[wellindex[i]]));
			}
			if ((datapoint[i]->ReadStatus()) & MPT_STATUS_USER_EXCLUDE)
			{
				// user-excluded point: just draw a light gray circle
				pDC->SelectObject(&graypen);
				pDC->Ellipse(xposition - 5, yposition - 5, xposition + 5, yposition + 5);
			}
			else
			{
				if ((datapoint[i]->ReadStatus()) & MPT_STATUS_SCRL_EXCLUDE)
					// pt excluded due to scrollbar setting: draw light gray cross
					pDC->SelectObject(&graypen);
				else
					// otherwise a black cross (normal) or blue (dev.)
					// red overrides black and red wells will never be deviated (blue).
				{
					if ((horstatus->GetAt(wellindex[i])) & WELL_STATUS_DEVIATED)
						pDC->SelectObject(&bluepen);
					else
					{
						if (!_strnicmp(labeltext[i] + is, "XX", 2))
						{
							pDC->SelectObject(&redpen);
						}
						else
						{
							pDC->SelectStockObject(BLACK_PEN);
							pDC->SetTextColor(RGB(0, 0, 0));
						}
					}
				}
				pDC->MoveTo(xposition, yposition - 10);
				pDC->LineTo(xposition, yposition + 10);
				pDC->MoveTo(xposition - 10, yposition);
				pDC->LineTo(xposition + 10, yposition);
			}
		}

		pDC->SelectObject(pOldpen);

		// if we have one, draw the model line (cannot do a complete line for exp. fitting)
		if (horizon->HaveFit && modfunc->func)
		{
			// draw model line (800 dots)
			CPen Pen(PS_SOLID, 0, ((CVelmanApp *)AfxGetApp())->GetProfileColor
			("Attributes", "FineTuneModelLine", RGB(139, 33, 121)));
			CPen *pOldPen = pDC->SelectObject(&Pen);

			int numdots = 0;
			numdots = 1000;
			double dx = (maxx - minx) / numdots;
			double dy = (maxy - miny) / numdots;
			BOOL DrewSomething = FALSE;
			int toY = 0, toX = 0;

			for (i = 0, x = minx; i < numdots; i++, x += dx)
			{
				xposition = XtoWin(x);
				toX = XtoWin(x + dx);
				if (model == 0)
				{
					yposition = YtoWin((exp(x / a) - 1) / b);
					toY = YtoWin((exp((x + dx) / a) - 1) / b);
				}
				else
				{
					yposition = YtoWin(modfunc->func(x, a, b));
					toY = YtoWin(modfunc->func(x + dx, a, b));
				}
				if (toX > 100 && toX < 900 && toY>100 && toY < 900
					&& xposition>100 && xposition < 900 && yposition>100 && yposition < 900)
				{
					pDC->MoveTo(xposition, yposition);
					pDC->LineTo(toX, toY);
					DrewSomething = TRUE;
				}
			}
			// if very steep, maybe try drawing the inverse function
			if (!DrewSomething && modfunc->inversefunc)
			{
				for (i = 0, y = miny; i < numdots; i++, y += dy)
				{
					yposition = YtoWin(y);
					toY = YtoWin(y + dy);
					if (model == 0)
					{
						break; //!! cannot do this...
					}
					else
					{
						xposition = XtoWin(modfunc->inversefunc(y, a, b));
						toX = XtoWin(modfunc->inversefunc(y + dy, a, b));
					}
					if (toX > 100 && toX < 900 && toY>100 && toY < 900
						&& xposition>100 && xposition < 900 && yposition>100 && yposition < 900)
					{
						pDC->MoveTo(xposition, yposition);
						pDC->LineTo(toX, toY);
						DrewSomething = TRUE;
					}
				}
			}
			pDC->SelectObject(pOldPen);
			if (!DrewSomething)
				pDC->TextOut(500, 75, "Warning: Function too steep to be displayed.");
		}

		// draw warning if negative slope
		if (model > 0 && a < 0)
		{
			pDC->SelectObject(&axisfont);
			pDC->SetTextColor(RGB(255, 0, 0));
			pDC->SetTextAlign(TA_RIGHT | TA_BOTTOM);
			char *warning = "Negative Slope!";
			CSize msgsize = pDC->GetTextExtent(warning);
			CPen warnpen(PS_SOLID, 1, RGB(255, 0, 0));
			pDC->SelectObject(&warnpen);
			pDC->TextOut(900, 890, warning);
			pDC->MoveTo(900, 890);
			pDC->LineTo(900 - msgsize.cx, 890);
			pDC->SetTextColor(RGB(0, 0, 0));
			pDC->SelectStockObject(BLACK_PEN);
			warnpen.DeleteObject();
		}
		// reset font
		pDC->SelectObject(pOldFont);
		graypen.DeleteObject();
		dotpen.DeleteObject();
		SmallWellnameFont.DeleteObject();
	}
	else // viewmode "contour"
	{
		// Draw title (underlined) and remember its width so the key goes to right of it
		pOldFont = pDC->SelectObject(&axisfont);
		pDC->SetTextAlign(TA_LEFT);
		pDC->TextOut(100, 5, modfunc->name);
		CSize titlesize = pDC->GetTextExtent(modfunc->name);
		pDC->MoveTo(100, 5 + titlesize.cy);
		pDC->LineTo(100 + titlesize.cx, 5 + titlesize.cy);

		// prepare list of residuals, and initialize a grid for the gridded contour data
		// i is counting the number of wells
		// k is counting the entries into labelx/labely and labeltext
		// j is indexing contx/conty and residual
		for (i = 0, j = 0, k = 0; i < numpts; i++)
		{
			labelx[k] = datapoint[i]->ReadX();
			labely[k] = datapoint[i]->ReadY();

			// In this first block, if wells are deselected AND the user has set
			// 'Exclude all deselected wells' on the finetune dialogue then do not
			// pass the data for contouring, just create a label for the well.
			// MPT_STATUS_SCRL_EXCLUDE bit is set by the user moving the scroll bar to
			// exclude points.
			// MPT_STATUS_USER_EXCLUDE bit is set when the user clicks on a well in scatter view
			// or when user deselects the well in 'Edit well'.
			if (datapoint[i]->ReadStatus() & (MPT_STATUS_SCRL_EXCLUDE | MPT_STATUS_USER_EXCLUDE) &&
				(horizon->HowToHandleWellResiduals & WELL_MODEL_NORESIDUALSIFDESELECTED))
			{
				if (ShowWellLabels)
				{
					sprintf(labeltext[k], "%s (ex)",
						(const char *)(pDoc->horizonlist.wellnames[wellindex[i]]));
				}
				else
					labeltext[k][0] = 0; // no text displayed, but we still want the blob
				k++;
				continue;
			}

			if (model == 0)
				residual[j] = (yval[i] - z1[i] * exp(xval[i] / (horizon->FitA))) -
				(exp(xval[i] / (horizon->FitA)) - 1.0) / (horizon->FitB);
			else
				residual[j] = yval[i] - modfunc->func(xval[i], horizon->FitA, horizon->FitB);
			contx[j] = datapoint[i]->ReadX();
			conty[j] = datapoint[i]->ReadY();
			if (ShowWellLabels)
			{
				if (datapoint[i]->ReadStatus() & (MPT_STATUS_SCRL_EXCLUDE | MPT_STATUS_USER_EXCLUDE))
					sprintf(labeltext[k], "%s: %.1f (ds)", (const char *)(pDoc->horizonlist.wellnames[wellindex[i]]), residual[j]);
				else
					sprintf(labeltext[k], "%s: %.1f", (const char *)(pDoc->horizonlist.wellnames[wellindex[i]]), residual[j]);
			}
			else
				labeltext[k][0] = 0; // no text displayed, but we still want the blob
			j++;
			k++;
		}
		cp->CleanUp(); // also works if there is nothing to clean up
		cp->SetContourLabels(k, labelx, labely, labeltext);

		cp->SetContourOption(CNTOPT_NORMALIZEGRADIENTS, horizon->NormalizeGradients, horizon->NormGradFactor);

		// draw contour key next to title y=5 x=right of title
		cp->SetContourOption(CNTOPT_CREATEKEY, (long)((5L << 16) | (100 + titlesize.cx + 50)));
		cp->SetContourOption(CNTOPT_KEYTYPESIZE, 100);

		if (cp->InitRawData(j, contx, conty, residual))
		{
			HaveValidContour = cp->ResampleData(contx1, conty1, contx2, conty2);
			//HaveValidContour=cp->ResampleData(minpx, minpy, maxpx, maxpy);
		}

		if (HaveValidContour)
		{
			cp->DrawContourPlot(pDC, cxplotmargin, cyplotmargin, winsize - cxplotmargin, winsize - cyplotmargin);
			cp->DrawAxisLabels(pDC, cxplotmargin, cyplotmargin, winsize - cxplotmargin, winsize - cyplotmargin);
			cp->DrawWellLabels(pDC, cxplotmargin, cyplotmargin, winsize - cxplotmargin, winsize - cyplotmargin);

			CZimsGrid *boxgrid = (CZimsGrid *)(pDoc->zimscube.GetAt(0));
			cp->DrawBox(pDC, cxplotmargin, cyplotmargin, winsize - cxplotmargin, winsize - cyplotmargin, boxgrid);
		}
	}
	axisfont.DeleteObject();
}

/////////////////////////////////////////////////////////////////////////////
// CFineTuneView message handlers

void CFineTuneView::OnDestroy()
{
	delete xval;
	delete yval;
	delete z1;
	delete ptx;
	delete pty;
	delete datapoint;
	delete wellindex;
	delete residual;
	delete cp;
	delete contx;
	delete conty;
	delete labelx;
	delete labely;
	for (int i = 0; i < numallocpts; i++)
		delete labeltext[i];
	delete labeltext;

	WellNameFont.DeleteObject();

	CScrollView::OnDestroy();

	((CVelmanDoc *)GetDocument())->OpenPreviewViews &= (~8);
}

void CFineTuneView::OnUpdate(CView *pSender, LPARAM lHint, CObject *pHint)
{
	if (lHint == 0 || lHint == DOC_CHANGE_FINETUNECHANGE)
	{
		CScrollView::OnUpdate(pSender, lHint, pHint);
		HaveValidContour = FALSE;
	}
}

void CFineTuneView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// Left button click on point toggles "User exclude" flag or zooms in

	int i = 0;
	int iopt = -1; // will contain index of optimal element

	// but if pressed together with Ctrl, it works like the right mouse button
	if (nFlags & MK_CONTROL)
	{
		OnRButtonDown(nFlags, point);
		return;
	}

	CClientDC dc(this);
	OnPrepareDC(&dc);
	dc.DPtoLP(&point); // this converts the device coords of point into window coords
	if (ViewMode == VIEWMODE_SCATTER)
	{

		// now find point in list (ptx, pty) which is closest to the mouse position and not too far away from it
		int d, dopt = 1000;
		for (i = 0; i < numpts; i++)
		{
			d = abs(ptx[i] - point.x) + abs(pty[i] - point.y);
			if (d < dopt)
			{
				iopt = i;
				dopt = d;
			}
		}

		// user should not be farther off than 20 pts in each direction
		if (dopt > 20)
			return; // did not hit any point

		// toggle status bit
		int status = datapoint[iopt]->ReadStatus();
		status ^= MPT_STATUS_USER_EXCLUDE;
		datapoint[iopt]->WriteStatus(status);

		// we need a new fit now that this point must be included/excluded
		horizon->HaveFit = FALSE;
		pDoc->UpdateAllViews(NULL);
		Invalidate();
	}
	else// contour view
	{// same mapping code as in OnMouseMove
		// left mouse button in contour view means deselect well as for scatter view.
		double x1, y1; // point in contour coords
		double d, dopt = contx2 - contx1; // large initial value will be changed below

		// map window coords to contour coords
		x1 = WintoX(point.x);
		y1 = WintoY(point.y);

		// see if we are near a well
		for (int i = 0; i < numpts; i++)
		{
			d = fabs(x1 - datapoint[i]->ReadX()) + fabs(y1 - datapoint[i]->ReadY());
			if (d < dopt)
			{
				iopt = i;
				dopt = d;
			}
		}

		// check if mouse is close to a well point
		if (dopt > 0.02*(contx2 - contx1)) // within 2% of the x range
			return;

		// toggle status bit
		int status = datapoint[iopt]->ReadStatus();
		status ^= MPT_STATUS_USER_EXCLUDE;
		datapoint[iopt]->WriteStatus(status);

		// we need a new fit now that this point must be included/excluded
		horizon->HaveFit = FALSE;
		pDoc->UpdateAllViews(NULL);
		Invalidate();
	}
}

void CFineTuneView::OnRButtonDown(UINT nFlags, CPoint point)
{
	// Right mouse button with CNTRL brings up Contour options dialogue
	// otherwise brings up 'edit data point' dialogue.

	bool HitAPoint = true;
	int iopt = -1; // will contain index of optimal element;

	//pressing control means never hit a point
	if (nFlags & MK_CONTROL)
	{
		HitAPoint = false;
	}
	else
	{
		// Right button click on point evokes "Edit data point" dialog

		CClientDC dc(this);
		OnPrepareDC(&dc);
		dc.DPtoLP(&point); // this converts the device coords of point into window coords

		// now find point in list (ptx, pty) which is closest to the mouse position and not too far away from it
		if (ViewMode == VIEWMODE_SCATTER)
		{
			int d, dopt = 1000;
			for (int i = 0; i < numpts; i++)
			{
				d = abs(ptx[i] - point.x) + abs(pty[i] - point.y);
				if (d < dopt)
				{
					iopt = i;
					dopt = d;
				}
			}

			// user should not be farther off than 20 pts in each direction
			if (dopt > 20)
				HitAPoint = false;
		}
		else// contour view
		{
			double x1, y1;
			double d, dopt = contx2 - contx1; // large initial value, will be changed later

			// map window coords to contour coords
			x1 = WintoX(point.x);
			y1 = WintoY(point.y);

			// see if we are near to well
			for (int i = 0; i < numpts; i++)
			{
				d = fabs(x1 - datapoint[i]->ReadX()) + fabs(y1 - datapoint[i]->ReadY());
				if (d < dopt)
				{
					iopt = i;
					dopt = d;
				}
			}
			// check if mouse is close to a well point
			if (dopt > 0.02*(contx2 - contx1)) // within 2% of the x range
				HitAPoint = false;
		}
	}

	if (HitAPoint)
	{
		// Now display the edit well data dialogue
		CEditWellPointDlg dlg;

		dlg.datapoint = datapoint[iopt];
		dlg.layer = layer;
		dlg.pDoc = pDoc;

		// send fine tune dialog to back before opening question dialog
		pDoc->pFTsheet->SetWindowPos(&wndNoTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);

		int result = dlg.DoModal();

		// dialog back to foreground
		pDoc->pFTsheet->SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);

		if (result != IDOK)
			return;

		if (dlg.DidChangeAnything)
		{
			// we have to compute a new fit if the user has changed anything
			horizon->HaveFit = FALSE;
			// The dialog has already written any changes into the
			// measurept of the point itself. copy the change into the local copy of the
			// point list.
			double x, y;
			datapoint[iopt]->ReadValuePair(model, &x, &y);
			xval[iopt] = x;
			yval[iopt] = y;
			// reprocess the layer
			pDoc->horizonlist.ProcessHorizon(layer);
			// force update view (will also compute new model in the course of OnDraw() )
			pDoc->UpdateAllViews(NULL);
			Invalidate();
		}
	}
	else if (ViewMode != VIEWMODE_SCATTER)
	{
		// send fine tune dialog to back before opening question dialog
		pDoc->pFTsheet->SetWindowPos(&wndNoTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
		int result = cp->ContourOptionsDialog(0);
		// dialog back to foreground
		pDoc->pFTsheet->SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
		if (result & CNTDLG_NEEDRESAMPLE)
		{
			// resample and unzoom
			CurrentDim.MinX = contx1;
			CurrentDim.MinY = conty1;
			CurrentDim.MaxX = contx2;
			CurrentDim.MaxY = conty2;
			ZoomDepth = 0;
			cp->ResampleData(contx1, conty1, contx2, conty2);
		}
		if (result & CNTDLG_NEEDREDRAW)
		{
			HaveValidContour = FALSE;
			ZoomDepth = 0;
			Invalidate();
		}

		return;
	}
}

void CFineTuneView::OnClickedDlgBackButton()
{
	// the controlling dialog was hidden and this button brings it back to life,
	// then hides itself. Works exactly opposite from the "Hide me" button in the
	// dialog itself.

	pDoc->pFTsheet->ShowWindow(SW_RESTORE);
	DlgBackButton.ShowWindow(SW_HIDE);
}

void CFineTuneView::OnMouseMove(UINT nFlags, CPoint point)
{
	int iopt = -1; // will contain index of optimal element

	// if mouse is moved above a well, display its name in a screen corner
	// but only update this display if it is not already there

	CClientDC dc(this);
	OnPrepareDC(&dc);
	dc.DPtoLP(&point); // this converts the device coords of point into window coords

	if (ViewMode == VIEWMODE_SCATTER)
	{
		// now find point in list (ptx, pty) which is closest to the mouse position and not too far away from it
		int d, dopt = 1000;
		for (int i = 0; i < numpts; i++)
		{
			d = abs(ptx[i] - point.x) + abs(pty[i] - point.y);
			if (d < dopt)
			{
				iopt = i;
				dopt = d;
			}
		}

		// user should not be farther off than 20 pts in each direction
		// (stronger test than usual)
		if (dopt > 20)
		{
			// did not hit any point. Do we have to delete old text?
			if (WellUnderCursor != -1)
			{
				dc.FillSolidRect(750, 0, 150, 50, RGB(255, 255, 255));
				WellUnderCursor = -1;
			}

			return;
		}
		// are we on the same well as last time we checked, is the name thus already
		// displayed?
		if (iopt == WellUnderCursor)
			return;
		// else display well name
		WellUnderCursor = iopt;
		dc.Rectangle(750, 0, 900, 48);
		CFont *pOldFont = dc.SelectObject(&WellNameFont);
		dc.TextOut(760, 5, (const char *)(pDoc->horizonlist.wellnames[wellindex[iopt]]));
		dc.SelectObject(pOldFont);
	}
	else// contour view
	{
		double x1, y1;
		double d, dopt = contx2 - contx1; // large initial value, will be changed below.

		// map window coords to contour coords.
		x1 = WintoX(point.x);
		y1 = WintoY(point.y);

		// see if we are near to a well
		for (int i = 0; i < numpts; i++)
		{
			d = fabs(x1 - datapoint[i]->ReadX()) + fabs(y1 - datapoint[i]->ReadY());
			if (d < dopt)
			{
				iopt = i;
				dopt = d;
			}
		}
		// check if mouse is close to a well point
		if (dopt > 0.02*(contx2 - contx1))// within 2% of the x range
		{
			// did not hit any point. Do we have to delete old text?
			if (WellUnderCursor != -1)
			{
				dc.FillSolidRect(50, 50, 150, 50, RGB(255, 255, 255));
				WellUnderCursor = -1;
			}
			return;
		}
		// are we on the same well as last time we checked, is the name thus already
		// displayed?
		if (iopt == WellUnderCursor)
			return;

		// otherwise display the well name
		WellUnderCursor = iopt;
		dc.Rectangle(50, 50, 200, 98);
		CFont *pOldFont = dc.SelectObject(&WellNameFont);
		dc.TextOut(60, 55, (const char*)(pDoc->horizonlist.wellnames[wellindex[iopt]])); // was 760, 5
		dc.SelectObject(pOldFont);
	}
}

