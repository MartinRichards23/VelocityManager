// cormodvw.cpp : implementation file

#include "stdafx.h"
#include "velman.h"
#include "VelmanDoc.h"
#include "CoarseViewDlg.h"
#include "numericr.h"
#include "ChooseModelDlg.h"
#include "global.h"
#include "mainfrm.h"
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

// handy drawing macros
#define XtoWin(x) (int)(800.0*((x)-minx)/(maxx-minx))+100+x1
#define YtoWin(y) (int)(800.0*(maxy-(y))/(maxy-miny))+100+y1

/////////////////////////////////////////////////////////////////////////////
// CCoarseModelView

IMPLEMENT_DYNCREATE(CCoarseModelView, CScrollView)

CCoarseModelView::CCoarseModelView()
{
}

CCoarseModelView::~CCoarseModelView()
{
}

BEGIN_MESSAGE_MAP(CCoarseModelView, CScrollView)
	//{{AFX_MSG_MAP(CCoarseModelView)
	ON_WM_DESTROY()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(ID_MODELCHOICE_DLGBACKBUTTON, OnClickedDlgBackButton)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCoarseModelView drawing

void CCoarseModelView::OnInitialUpdate()
{
	// This is a scale-to-fit-window, the user can alter the size and aspect ratio!
	CSize totalsize = CSize(2010, 2110);
	SetScaleToFitSize(totalsize);
	pDoc = (CVelmanDoc *)GetDocument();
	pDoc->PressCancel = TRUE;

	horizon=(CHorizon *)((pDoc->horizonlist).GetAt(pDoc->ActiveLayer));
	ViewMode=VIEWMODE_SCATTER;
	int num=pDoc->horizonlist.wellnames.GetSize();
	for(int i=0;i<4;i++)
	{
		HaveValidContour[i]=FALSE;
		cp[i]=new CContourPlot(pDoc);
		cp[i]->SetContourOption(CNTOPT_SHADELEVELS, TRUE);

		// As we use colour fill by default now, turn off labelling by default
		if ( cp[i]->GetColourShading() )
			cp[i]->SetContourOption(CNTOPT_LABELLING, 0 );
		else
			cp[i]->SetContourOption(CNTOPT_LABELLING, CNT_USERLABEL);

		cp[i]->SetContourOption(CNTOPT_LABELTYPESIZE, 33);
		cp[i]->SetContourOption(CNTOPT_UPSIDEDOWN, TRUE);
		cp[i]->SetContourOption(CNTOPT_COARSENESS, 5);

		labelx[i]=new double[num];
		labely[i]=new double[num];
		labeltext[i]=new char *[num];
	}

	pDoc->pCMview=this;

	CRect rect(0, 0, 70, 25);
	DlgBackButton.Create("Dialog!", BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE, rect,
		this, ID_MODELCHOICE_DLGBACKBUTTON);
	DlgBackButton.ShowWindow(SW_HIDE);	// show only when control dlg hidden!
}

void CCoarseModelView::OnDraw(CDC* pDC)
{
	CWellModelFunction *modfunc;

	pDC->SetMapMode(MM_ANISOTROPIC);
	pDC->SetWindowExt(2010, 2110);
	// Font to do axis labels
	CFont axisfont;
	axisfont.CreateFont(55, 0, 0, 0, 400, FALSE, FALSE, 0, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_SWISS, "Arial");
	CFont *pOldFont=pDC->SelectObject(&axisfont);

	CFont msgfont;
	msgfont.CreateFont(70, 0, 0, 0, 400, FALSE, FALSE, 0, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_SWISS, "Arial");

	// red brush for "assumed vertical" points
	CBrush bluebrush(((CVelmanApp *)AfxGetApp())->GetProfileColor
		("Attributes", "FalselyVerticalPoint", RGB(0, 0, 255)));
	CPen bluepen(PS_SOLID, 1, ((CVelmanApp *)AfxGetApp())->GetProfileColor
		("Attributes", "FalselyVerticalPoint", RGB(0, 0, 255)));
	// draw thick red separating lines between the four graphics panes
	CPen seppen(PS_SOLID, 3, RGB(255, 0, 0));
	CPen *pOldPen = pDC->SelectObject(&seppen);
	pDC->MoveTo(1005, 100);
	pDC->LineTo(1005, 2100);
	pDC->MoveTo(0, 1105);
	pDC->LineTo(2000, 1105);
	pDC->SelectObject(pOldPen);

	RECT rect;
	int x1, y1;
	double *x, *y, *z1;	// will be directed towards lists stored in document
	double *xpos, *ypos;
	double minx, maxx, miny, maxy;
	int num;		// number of points to be plotted
	int i, j;

	for(int win=0;win<4;win++)
	{
		// is the current window visible anyway?
		if(win==0 || win==2) x1=0; else x1=1010;
		if(win==0 || win==1) y1=100; else y1=1110;
		rect.left=x1;
		rect.top=y1;
		rect.bottom=y1+1000;
		rect.right=x1+1000;
		if(pDC->RectVisible(&rect))
		{
			x=pDoc->xvals[win];
			y=pDoc->yvals[win];
			xpos=pDoc->xpos[win];
			ypos=pDoc->ypos[win];
			z1=pDoc->z1vals[win];
			num=pDoc->length[win];
			modfunc=pDoc->well_model_lib[pDoc->model[win]];
			if(ViewMode==VIEWMODE_SCATTER)
			{
				// make scatter plot of some dots in all four windows. We do not really know
				// what the points mean (time, depth, velocity etc.), but we do not have to.
				if(pDoc->model[win]==10)
				{
					// it's "contoured intvel only" -- very simle plot in this case, namely nothing
					pDC->SelectObject(&msgfont);
					CSize bordersize=pDC->GetTextExtent("\"Contoured interval velocities\"");
					i=bordersize.cx/2+50;
					pDC->SelectStockObject(WHITE_BRUSH);
					pDC->SelectStockObject(BLACK_PEN);
					pDC->Rectangle(x1+500-i, y1+250, x1+500+i, y1+720);
					pDC->SetTextAlign(TA_CENTER);
					pDC->TextOut(x1+500, y1+300, "\"Contoured interval velocities\"");
					pDC->TextOut(x1+500, y1+500, "Scatter view not applicable;");
					pDC->TextOut(x1+500, y1+580, "please see contour plot");
					// reset font and we're done
					pDC->SelectObject(pOldFont);
					continue;
				}
				// do not even attempt to draw the model if there are no points
				if(num<1)
				{
					pDC->SelectObject(&msgfont);
					CString modelname=pDoc->well_model_lib[pDoc->model[win]]->name;
					CSize bordersize1=pDC->GetTextExtent(modelname);
					CSize bordersize2=pDC->GetTextExtent("No well data for scatter plot");
					i=max(bordersize1.cx, bordersize2.cx)/2+50;
					pDC->SelectStockObject(WHITE_BRUSH);
					pDC->SelectStockObject(BLACK_PEN);
					pDC->Rectangle(x1+500-i, y1+250, x1+500+i, y1+620);
					pDC->SetTextAlign(TA_CENTER);
					pDC->TextOut(x1+500, y1+300, modelname);
					pDC->TextOut(x1+500, y1+500, "No well data for scatter plot");
					// reset font and we're done
					pDC->SelectObject(pOldFont);
					continue;
				}
				minx=200000.0;
				maxx=-10.0;
				miny=200000.0;
				maxy=-10.0;
				double a=pDoc->FitA[win], b=pDoc->FitB[win];		// fitting paramenters
				for(i=0;i<num;i++)
				{
					maxx=max(maxx, x[i]);
					minx=min(minx, x[i]);
					if(pDoc->model[win]==0)
					{
						if(!(pDoc->HaveFit[win]))
							num=0;	// impossible for exp. model w/o fit, sorry
						else
						{
							maxy=max(maxy, y[i]-z1[i]*exp(x[i]/a));
							miny=min(miny, y[i]-z1[i]*exp(x[i]/a));
						}
					}
					else
					{
						maxy=max(maxy, y[i]);
						miny=min(miny, y[i]);
					}
				}
				// round up/down towards next hundred
				maxx=100.0*ceil(maxx/100.0+0.01);
				minx=100.0*floor(minx/100.0-0.01);
				maxy=100.0*ceil(maxy/100.0)+100.0;
				miny=100.0*floor(miny/100.0)-100.0;
				// Draw title
				pDC->SelectObject(&axisfont);
				pDC->SetTextAlign(TA_CENTER);
				// amendment 13/10/03.
				// More precise title on Instantaneous vel/depth scatter plot.
				// Fix this by matching the model name taken from Velman.cpp.
				if ( strcmp((LPCTSTR)modfunc->name, "Instantaneous vel. ~ depth") )
					pDC->TextOut(x1+500, y1+5, modfunc->name);
				else
				{
					CSize sze = pDC->GetTextExtent(modfunc->name);
					double yoff;
					pDC->TextOut(x1+500, y1+5, "Instantaneous vel. ~ depth");
					yoff = sze.cy*1.1;
					/*pDC->TextOut(x1+500, y1+(int)yoff, "represented by, ");
					pDC->TextOut(x1+500, y1+(int)yoff*2, "Compensated depth vs. Isochronochore");*/
				}
				// Draw Coordinate system
				pDC->SelectStockObject(BLACK_PEN);
				pDC->MoveTo(x1+100, y1+100);
				pDC->LineTo(x1+100, y1+900);
				pDC->LineTo(x1+900, y1+900);

				// Draw y axis labels and ticks
				pDC->SelectObject(&axisfont);
				pDC->SetTextAlign(TA_RIGHT);
				TEXTMETRIC tm;		// needed for vertically centered output
				pDC->GetTextMetrics(&tm);

				// select axis labelling scheme
				int numlabel=3;
				double label=(maxy-miny)/(numlabel-1);
				char buf[128];
				double yval=maxy;
				int ypos;

				for(i=0;i<numlabel;i++, yval-=label)
				{
					sprintf(buf, "%4.f", yval);
					ypos=YtoWin(yval);
					pDC->MoveTo(x1+100, ypos);
					pDC->LineTo(x1+80, ypos);
					pDC->TextOut(x1+80, ypos-tm.tmHeight/2, buf, strlen(buf));
				}

				// Draw x axis labels and ticks
				pDC->SetTextAlign(TA_CENTER);
				numlabel=3;

				int xpos;
				double xval;

				xval=minx;
				label=(maxx-minx)/(numlabel-1);
				for(i=0;i<numlabel;i++, xval+=label)
				{
					sprintf(buf, "%5.f", xval);
					xpos=x1+100+(int)(800.0*(xval-minx)/(maxx-minx));
					pDC->MoveTo(xpos, y1+900);
					pDC->LineTo(xpos, y1+920);
					pDC->TextOut(xpos, y1+930, buf, 5);
				}

				// draw plot itself
				pDC->SelectStockObject(BLACK_BRUSH);
				pDC->SelectStockObject(BLACK_PEN);
				for(i=0;i<num;i++)
				{
					xpos=XtoWin(x[i]);
					if(pDoc->model[win]==0)
						ypos=YtoWin(y[i]-z1[i]*exp(x[i]/a));
					else
						ypos=YtoWin(y[i]);
					pDC->Rectangle(xpos-5, ypos-5, xpos+5, ypos+5);
				}

				if(pDoc->HaveFit[win])
				{
					// draw model line
					CPen Pen(PS_SOLID, 0, ((CVelmanApp *)AfxGetApp())->GetProfileColor
						("Attributes", "ModelChoiceModelLine", RGB(255, 0, 0)));
					CPen *pOldPen=pDC->SelectObject(&Pen);

					int numpts;
					numpts=1000;
					double dx=(maxx-minx)/numpts;
					double dy=(maxy-miny)/numpts;
					BOOL DrewSomething=FALSE;

					int toY, toX;
					double xx, yy;

					if(pDoc->model[win]!=0)
					{
						// just plot y(x) for all models except exp. one
						for(i=0, xx=minx;i<numpts;i++, xx+=dx)
						{
							xpos=XtoWin(xx);
							toX=XtoWin(xx+dx);
							ypos=YtoWin(modfunc->func(xx, a, b));
							toY=YtoWin(modfunc->func(xx+dx, a, b));
							if(toX>x1+100 && toX<x1+900 && toY>y1+100 && toY<y1+900
								&& xpos>x1+100 && xpos<x1+900 && ypos>y1+100 && ypos<y1+900)
							{
								pDC->MoveTo(xpos, ypos);
								pDC->LineTo(toX, toY);
								DrewSomething=TRUE;
							}
						}
					}
					else
					{
						// otherwise plot exp func
						for(i=0, xx=minx;i<numpts;i++, xx+=dx)
						{
							xpos=XtoWin(xx);
							toX=XtoWin(xx+dx);
							ypos=YtoWin((exp(xx/a)-1.0)/b);
							toY=YtoWin((exp((xx+dx)/a)-1.0)/b);
							if(toX>x1+100 && toX<x1+900 && toY>y1+100 && toY<y1+900
								&& xpos>x1+100 && xpos<x1+900 && ypos>y1+100 && ypos<y1+900)
							{
								pDC->MoveTo(xpos, ypos);
								pDC->LineTo(toX, toY);
								DrewSomething=TRUE;
							}
						}
					}
					// if very steep, maybe try drawing the inverse function
					if(!DrewSomething && modfunc->inversefunc)
					{
						for(i=0, yy=miny;i<numpts;i++, yy+=dy)
						{
							ypos=YtoWin(yy);
							toY=YtoWin(yy+dy);
							xpos=XtoWin(modfunc->inversefunc(yy, a, b));
							toX=XtoWin(modfunc->inversefunc(yy+dy, a, b));
							if(toX>x1+100 && toX<x1+900 && toY>y1+100 && toY<y1+900
								&& xpos>x1+100 && xpos<x1+900 && ypos>y1+100 && ypos<y1+900)
							{
								pDC->MoveTo(xpos, ypos);
								pDC->LineTo(toX, toY);
								DrewSomething=TRUE;
							}
						}
					}
					pDC->SelectObject(pOldPen);
					if(!DrewSomething)
						pDC->TextOut(x1+500, y1+75, "Warning: Function too steep to be displayed.");
				}
				// reset font
				pDC->SelectObject(pOldFont);
			}
			else // if ViewMode==residual
			{
				const char *p;
				if(pDoc->HaveFit[win])
				{
					if(!HaveValidContour[win])
					{
						// prepare list of residuals, and initialize a grid for the gridded contour data

						// find out boundaries of coordinates
						CZimsGrid *timegrid;
						if((timegrid=pDoc->zimscube.FindHorizon(pDoc->ActiveLayer, CSD_TYPE_TIME, 0)))
						{
							// have a time grid at hand ==> take its coords for contouring
							double *limits;
							limits=timegrid->GetXlimits();
							contx1=limits[0];
							contx2=limits[1];
							limits=timegrid->GetYlimits();
							conty1=limits[0];
							conty2=limits[1];
						}
						else
						{
							// else take well coords for frame for contouring
							contx1=FLT_MAX;
							conty1=FLT_MAX;
							contx2=-FLT_MAX;
							conty2=-FLT_MAX;

							for(i=0;i<num;i++)
							{
								contx2=max(contx2, xpos[i]);
								contx1=min(contx1, xpos[i]);
								conty2=max(conty2, ypos[i]);
								conty1=min(conty1, ypos[i]);
							}
							// round spatial coords by 10%
							double wid=contx2-contx1;
							double hgt=conty2-conty1;
							wid*=0.05;
							hgt*=0.05;
							contx2+=wid;
							contx1-=wid;
							conty2+=hgt;
							conty1-=hgt;
						}
						// create the data lists themselves
						contx=new double[num];
						conty=new double[num];
						residual=new double[num];
						for(i=0, j=0;i<num;i++)
						{
							if(pDoc->model[win]==0)
								residual[j]=(y[i] - z1[i]*exp(x[i]/(pDoc->FitA[win])) ) -
								(exp(x[i]/(pDoc->FitA[win]))-1.0)/(pDoc->FitB[win]);
							else
								residual[j]=y[i]-modfunc->func(x[i], pDoc->FitA[win], pDoc->FitB[win]);
							contx[j]=xpos[i];
							conty[j]=ypos[j];
							labelx[win][j]=xpos[i];
							labely[win][j]=ypos[i];
							p=(const char *) (pDoc->horizonlist.wellnames[pDoc->databaseindex[win][i]]);
							labeltext[win][j]=(char *)p;
							j++;
						}
						cp[win]->CleanUp();		// also works if there is nothing to clean up
						cp[win]->SetContourLabels(j, labelx[win], labely[win], labeltext[win]);
						if(cp[win]->InitRawData(j, contx, conty, residual))
							HaveValidContour[win]=cp[win]->ResampleData(contx1, conty1, contx2, conty2);
						delete contx;
						delete conty;
						delete residual;
					}
					if(HaveValidContour[win])
					{
						int width = 975;
						int height = 925;

						cp[win]->DrawContourPlot(pDC, x1+25, y1+75, x1+width, y1+height);
						cp[win]->DrawWellLabels(pDC, x1+25, y1+75, x1+width, y1+height);
					}
					// Draw title
					pDC->SelectObject(&axisfont);
					pDC->SetTextAlign(TA_CENTER);
					pDC->TextOut(x1+500, y1+10, modfunc->name);
				}
			}
		}
	}
	axisfont.DeleteObject();
	msgfont.DeleteObject();
}

/////////////////////////////////////////////////////////////////////////////
// CCoarseModelView message handlers

void CCoarseModelView::OnDestroy()
{
	if(pDoc->PressCancel == TRUE) {
		CModelChoiceDlg *p_dlg=pDoc->pCMdlg;
		p_dlg->OnCancel();
		return;
	}

	pDoc->OpenPreviewViews&=(~1);
	CScrollView::OnDestroy();
	for(int i=0;i<4;i++) {
		delete cp[i];
		delete labelx[i];
		delete labely[i];
		delete labeltext[i];
	}
}

void CCoarseModelView::SetProjectFlags()
{
	// sets the flags in the document. Is called if any of the four "Refine this model"
	// buttons was pushed. We have SOME models in any case then, and maybe even ALL
	// models. Set the document flag accordingly.
	pDoc->ProjState|=PRJ_HAVESOMEMODELS;

	CHorizon *p_horizon;
	int hornumber=pDoc->horizonlist.GetSize();
	BOOL AllLayers=TRUE;

	for(int i=0;i<hornumber;i++)
	{
		p_horizon = (CHorizon *) pDoc->horizonlist.GetAt(i);
		if(!(p_horizon->HaveModel))
			AllLayers=FALSE;
	}
	if(AllLayers)
		pDoc->ProjState|=PRJ_HAVEMODEL;
}

void CCoarseModelView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// left mouse button click in model choice window
	CClientDC dc(this);
	OnPrepareDC(&dc);	// inform device context about size of this view, so it can
	// perform coordinate transformations
	dc.DPtoLP(&point);	// convert from device to logical coordinates
	int x=point.x;
	int y=point.y;

	// Previously there were four buttons in the "ChooseModelDlg" dialog, to chose
	// one of the four models. Each of them had a handler going with it, and what we
	// do here is calling these handlers directly. In MainFrm.CPP, the function that
	// handles the "CreateModel" menu command has entered the address of the dialog
	// into the document. We retrieve it from there, call the button handler (which
	// also causes the dialog to destroy itself), then destroy our own frame window
	// and call another MainFrame function to open the fine tune view.
	// we left the original code in the dialog box handler and just commented it out.

	// we need these variables to read out the dialog settings
	CModelChoiceDlg *p_dlg=pDoc->pCMdlg;
	CComboBox *pCB;
	CButton *pB;

	// to start with, we cannot do it if the "Numerical residues" table dialog is
	// still open
	if(p_dlg->ResidualTableListOpen)
	{
		Beep(500, 500);
		return;
	}

	// find out in which of the four quadrants the click took place
	int quad = 0;
	if(x<1005 && y<1005)
	{
		// model 1
		pCB=(CComboBox *)(p_dlg->GetDlgItem(IDC_MODELCHOICE_FUNC1));
		pB=(CButton *)(p_dlg->GetDlgItem(IDC_MODELCHOICE_LIN1));
		quad=0;
	}
	if(x>=1005 && y<1005)
	{
		// model 2
		pCB=(CComboBox *)(p_dlg->GetDlgItem(IDC_MODELCHOICE_FUNC2));
		pB=(CButton *)(p_dlg->GetDlgItem(IDC_MODELCHOICE_LIN2));
		quad=1;
	}
	if(x<1005 && y>=1005)
	{
		// model 3
		pCB=(CComboBox *)(p_dlg->GetDlgItem(IDC_MODELCHOICE_FUNC3));
		pB=(CButton *)(p_dlg->GetDlgItem(IDC_MODELCHOICE_LIN3));
		quad=2;
	}
	if(x>=1005 && y>=1005)
	{
		// model 4
		pCB=(CComboBox *)(p_dlg->GetDlgItem(IDC_MODELCHOICE_FUNC4));
		pB=(CButton *)(p_dlg->GetDlgItem(IDC_MODELCHOICE_LIN4));
		quad=3;
	}

	// warn user if there is no real model in that window
	int model=pCB->GetCurSel();
	if(pDoc->length[quad]<2)
	{
		// less than 2 wells => likely no fit in this window
		if(model<10)		// model which requires a fit line
		{
			if(pDoc->length[quad]<2)	// we need at least two points to perform a fit
			{
				if(AfxMessageBox("This horizon does not contain enough well data to perform "
					"a function fit for the model you selected. In order to use the model "
					"anyway, you will have to"
					" work with the \"Custom Fit\" feature, entering the model parameters"
					" yourself. Do you want to proceed?", MB_YESNO | MB_DEFBUTTON2)==IDNO)
					return;
				// switch on custom fit
				horizon->UseCustomFit=TRUE;
				horizon->FitA=0;
				horizon->FitB=0;
				horizon->CustomFitA=0;
				horizon->CustomFitB=0;
			}
		}
		else					// model=10 means contoured interval velocities
		{
			if(pDoc->length[quad]<1)	// we need at least one interval velocity to be contoured
			{
				AfxMessageBox("This horizon contains no wells from which to calculate"
					" and then to contour interval velocities; you must select one of the"
					" other models."
					"\n\nIn order to use a constant interval velocity \"Vint\" for this layer, "
					"follow these steps:"
					"\n\t1) select model \"Interval velocity ~ top time\""
					"\n\t2) choose \"additive\" under \"Gridding options\""
					"\n\t3) select the \"Custom Fit\" tab"
					"\n\t3) use 0 as the slope, and Vint as the intercept"
					"\n\t4) push OK", MB_OK | MB_ICONINFORMATION);
				return;
			}
		}
	}

	// store data about model choice in horizon.
	horizon->HaveModel=TRUE;
	horizon->ExcludePercent=0;
	horizon->ModelNumber=model;
	horizon->DistWeight=1-(pB->GetCheck());
	SetProjectFlags();

	// log the activity
	CString line;
	line="Fine tuning model '"+pDoc->well_model_lib[model]->name+"' for horizon "+horizon->name;
	pDoc->Log(line, LOG_STD);
	// write the dialog's settings into the INI file
	pB=(CButton *)(p_dlg->GetDlgItem(IDC_MODELCHOICE_LIN1));
	AfxGetApp()->WriteProfileInt("Well models", "WeightDefault1", pB->GetCheck());
	pB=(CButton *)(p_dlg->GetDlgItem(IDC_MODELCHOICE_LIN2));
	AfxGetApp()->WriteProfileInt("Well models", "WeightDefault2", pB->GetCheck());
	pB=(CButton *)(p_dlg->GetDlgItem(IDC_MODELCHOICE_LIN3));
	AfxGetApp()->WriteProfileInt("Well models", "WeightDefault3", pB->GetCheck());
	pB=(CButton *)(p_dlg->GetDlgItem(IDC_MODELCHOICE_LIN4));
	AfxGetApp()->WriteProfileInt("Well models", "WeightDefault4", pB->GetCheck());

	pCB=(CComboBox *)(p_dlg->GetDlgItem(IDC_MODELCHOICE_FUNC1));
	AfxGetApp()->WriteProfileInt("Well models", "FuncDefault1", pCB->GetCurSel());
	pCB=(CComboBox *)(p_dlg->GetDlgItem(IDC_MODELCHOICE_FUNC2));
	AfxGetApp()->WriteProfileInt("Well models", "FuncDefault2", pCB->GetCurSel());
	pCB=(CComboBox *)(p_dlg->GetDlgItem(IDC_MODELCHOICE_FUNC3));
	AfxGetApp()->WriteProfileInt("Well models", "FuncDefault3", pCB->GetCurSel());
	pCB=(CComboBox *)(p_dlg->GetDlgItem(IDC_MODELCHOICE_FUNC4));
	AfxGetApp()->WriteProfileInt("Well models", "FuncDefault4", pCB->GetCurSel());
	// kill the dialog and free its memory
	pDoc->PressCancel=FALSE;
	p_dlg->DestroyWindow();
	delete p_dlg;
	// delete lists which were allocated within the document
	// ALSO NEEDS TO BE DONE AS A RESPONSE FOR THE CANCEL BUTTON!!!!
	for(int i=0;i<4;i++)
	{
		delete pDoc->xvals[i];
		delete pDoc->yvals[i];
		delete pDoc->z1vals[i];
		delete pDoc->xpos[i];
		delete pDoc->ypos[i];
		delete pDoc->residuals[i];
		delete pDoc->databaseindex[i];
	}
	// We want to do now is open a view and controlling
	// dialog box for the "fine tuning" process. It is easiest to do this from within
	// the main frame window (which usually handles "Window/New" commands as well)
	// That's why we get its address here and call its member function FineTune.
	((CMainFrame *)(AfxGetApp()->m_pMainWnd))->OpenFineTuneView(pDoc->ActiveLayer);
	// now kill the view itself, by killing its frame window
	(pDoc->pCoarseModelFrame)->DestroyWindow();
	//CRect sheetRect;
	//(pDoc->pCoarseModelFrame)->CalcWindowRect(
}

void CCoarseModelView::OnClickedDlgBackButton()
{
	// the controlling dialog was hidden and this button brings it back to life,
	// then hides itself. Works exactly opposite from the "Hide me" button in the
	// dialog itself.

	CModelChoiceDlg *p_dlg=pDoc->pCMdlg;
	p_dlg->ShowWindow(SW_RESTORE);

	DlgBackButton.ShowWindow(SW_HIDE);
}

void CCoarseModelView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	CView::OnUpdate(pSender, lHint, pHint);
	// if any model choice changes, make all four contours invalid
	for(int i=0;i<4;i++)
		HaveValidContour[i]=FALSE;
}
