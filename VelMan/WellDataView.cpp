// TT_View.cpp : implementation file
//

#include "stdafx.h"
#include "velman.h"
#include "VelmanDoc.h"
#include "WellDataView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWellDataView

IMPLEMENT_DYNCREATE(CWellDataView, CScrollView)

CWellDataView::CWellDataView()
{
	spacing = 60;			// spacing between lines
	WellDist = 50;			// distance between wells
	lineHtMult = -25;		// multiplies line height
	SeperationDist = 190;	// distance between well titles and first line
}

CWellDataView::~CWellDataView()
{
	// this view is closed now, so update value to show this
	pDoc->TyingTableOpen=FALSE;
}

BEGIN_MESSAGE_MAP(CWellDataView, CScrollView)
	//{{AFX_MSG_MAP(CWellDataView)
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(ID_LISTOFMO_DONEBUTTON, OnClickedDoneButton)
	ON_BN_CLICKED(ID_LISTOFTIE_DUMPBUTTON, OnClickedDumpButton)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWellDataView drawing

void CWellDataView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	CSize sizeTotal;
	pDoc = (CVelmanDoc *)GetDocument();
	sizeTotal.cy = SeperationDist + spacing *pDoc->horizonlist.GetSize();
	sizeTotal.cx = 100+ WellDist *pDoc->horizonlist.wellnames.GetSize();
	SetScrollSizes(MM_TEXT, sizeTotal);

	CRect rect(10, 10, 80, 35);
	DoneButton.Create("Close", BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE, rect,
		this, ID_LISTOFMO_DONEBUTTON);

	rect=CRect(100, 10, 240, 35);
	DumpButton.Create("Save to spreadsheet ", BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE, rect,
		this, ID_LISTOFTIE_DUMPBUTTON);

	GetParentFrame()->SetWindowPos(&wndTopMost, 0, 0, 0, 0,
		SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

	WINDOWPLACEMENT wp;

	GetParentFrame()->GetWindowPlacement(&wp);
	//wp.showCmd=SW_SHOWMAXIMIZED;
	GetParentFrame()->SetWindowPlacement(&wp);

	chosentype=pDoc->TieTableChosenType;
}

void CWellDataView::OnDraw(CDC* pDC)
{
	// the 2 fonts used: horizontal and vertical system font
	CFont tablefont;
	tablefont.CreateFont(12, 0, 0, 0, 400, FALSE, FALSE, 0, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_SWISS, "Arial");
	CFont verticalfont;
	verticalfont.CreateFont(12, 0, 900, 0, 400, FALSE, FALSE, 0, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_SWISS, "Arial");

	CPen redpen(PS_SOLID, 5, RGB(255, 0, 0));			// thick red pen for negative lines
	CPen bluepen(PS_SOLID, 5, RGB(0, 0, 255));         // thick blue pen for positive lines

	CHorizon *horizon;
	TEXTMETRIC tm;		// needed for vertically centered output
	pDC->GetTextMetrics(&tm);
	int hgt=tm.tmHeight/2;		// half the height of a textline (used for centered output)
	int layernum=pDoc->horizonlist.GetSize();
	int wellnum=pDoc->horizonlist.wellnames.GetSize();
	int i, j;
	double v;
	double *val=new double[wellnum];
	double minvalue, maxvalue;
	double x;
	int xPos, yPos;
	char buf[128];

	// draw frame around "done" button
	CRect rect(10-5, 10-5, 240+5, 35+5);
	pDC->SelectStockObject(LTGRAY_BRUSH);
	pDC->Rectangle(rect);
	pDC->SelectStockObject(BLACK_PEN);
	pDC->MoveTo(90, 10-5);
	pDC->LineTo(90, 35+5);

	// draw note on measurement units
	CFont *pOldFont=pDC->SelectObject(&tablefont);
	char buf2[128];
	sprintf(buf2, (pDoc->Units & UNIT_OUTPUTINFEET) ? "feet" : "metres");

	char buf3[128];
	switch(chosentype)
	{
		//grid to well ties
	case 0: sprintf(buf3, "milliseconds"); break;
	case 1: sprintf(buf3, buf2); break;
	case 2: sprintf(buf3, strcat(buf2, "/second")); break;

		//interval tops
	case 3: sprintf(buf3, "milliseconds"); break;
	case 4: sprintf(buf3, buf2); break;

		//interval bases
	case 5: sprintf(buf3, "milliseconds"); break;
	case 6: sprintf(buf3, buf2); break;

		//interval midpoint values
	case 7: sprintf(buf3, "milliseconds"); break;
	case 8: sprintf(buf3, buf2); break;

		//interval values
	case 9: sprintf(buf3, "milliseconds"); break;
	case 10: sprintf(buf3, buf2); break;
	case 11: sprintf(buf3, strcat(buf2, "/second")); break;

		//weighting and residuals
	case 12: sprintf(buf3, "coefficient"); break;
	case 13: sprintf(buf3, "as Y axis of function"); break;
	}

	sprintf(buf, "Units: %s", buf3);
	pDC->TextOut(260, 20, buf);

	// display all well names vertically
	pDC->SelectObject(&verticalfont);
	pDC->SetTextAlign(TA_LEFT);
	CSize textsize;

	for(i=0;i<wellnum;i++)
	{
		strcpy(buf, (const char *)(pDoc->horizonlist.wellnames[i]));
		textsize=pDC->GetTextExtent(buf, strlen(buf));
		// truncate well name until it is short enough
		while(textsize.cx>150-3*hgt-40 && strlen(buf)>0)
		{
			buf[strlen(buf)-1]=0;
			textsize=pDC->GetTextExtent(buf, strlen(buf));
		};
		pDC->TextOut(115+WellDist*i, 180-3*hgt, buf);
	}

	// now go through all layers. Draw layer name
	// if they have a grid assigned to it, display tying deviation
	// If there is no grid, just display the layer name in gray
	pDC->SelectObject(&tablefont);
	pDC->SetTextAlign(TA_RIGHT);
	for(i=0;i<layernum;i++)
	{
		// layer name
		horizon=(CHorizon *)pDoc->horizonlist.GetAt(i);
		pDC->TextOut(100, SeperationDist+spacing*i-hgt, horizon->name);
	}
	// output deviation table itself
	pDC->SetTextAlign(TA_LEFT);
	char FormatString[16];
	if(AfxGetApp()->GetProfileInt("Debug", "TieDiffTableWithDecimals", 0)==1)
		strcpy(FormatString, "%6.2f");
	else
		strcpy(FormatString, "%4.1f");
	for(i=0;i<layernum;i++)
	{
		horizon=(CHorizon *)pDoc->horizonlist.GetAt(i);
		maxvalue=-1000000;	// to find maximum value for this layer
		minvalue=1000000;	// to find minimum value for this layer

		for(j=0;j<wellnum;j++)
		{
			CMeasurePt *measurept = (CMeasurePt *) horizon->GetAt(j);
			v=MPT_INVALID;
			if(measurept!=NULL && !(measurept->ReadStatus() & MPT_STATUS_INVALID))
			{
				switch(chosentype)
				{
					//grid well ties
				case 0:v=measurept->ReadTieTdiff();break;
				case 1:v=measurept->ReadTieDdiff();break;
				case 2:v=measurept->ReadTieVdiff();break;

					//interval tops
				case 3:v=measurept->ReadtTop();break;
				case 4:v=measurept->ReadZ1();break;

					//interval bases
				case 5:v=measurept->ReadTime();break;
				case 6:v=measurept->ReadDepth();break;

					//interval midpoint values
				case 7:v=measurept->Readmptime();break;
				case 8:v=measurept->Readmpdepth();break;

					//interval values
				case 9:v=measurept->ReadtInt();break;
				case 10:v=measurept->ReaddInt();break;
				case 11:v=measurept->ReadvInt();break;

					//weighting and residuals
				case 12:v=measurept->ReadWeight();break;

				case 13:
					if(!horizon->HaveFit || horizon->ModelNumber==10)
					{
						v=MPT_INVALID;
						break;
					}

					double x, y, z1;
					CWellModelFunction *modfunc;
					modfunc=pDoc->well_model_lib[horizon->ModelNumber];

					measurept->ReadValuePair(horizon->ModelNumber, &x, &y);
					z1=measurept->ReadZ1();
					if(x != MPT_ILLEGAL && y != MPT_ILLEGAL && z1 != MPT_ILLEGAL)
					{
						if(horizon->ModelNumber==0)
						{
							v = y+( -z1-1.0/(horizon->FitB) )*exp(x/(horizon->FitA))+1.0/(horizon->FitB);
						}
						else
							v = y-modfunc->func(x, horizon->FitA, horizon->FitB);
					}
					else
						v=MPT_INVALID;

					break;
				}
			}

			val[j] = v;

			//is it the max value?
			if(val[j]!=MPT_INVALID)
			{
				if(fabs(val[j])>maxvalue)
					maxvalue=fabs(val[j]);
				if(fabs(val[j])<minvalue)
					minvalue=fabs(val[j]);
			}
		}

		for(j=0;j<wellnum;j++)
		{
			if(val[j]!=MPT_INVALID)
			{
				//make string
				sprintf(buf, FormatString, val[j]);  // FormatString set above

				// make rescaled value for bar graph
				if(val[j] > 0)
				{//positive
					x = val[j] - minvalue;
					x= x/(maxvalue - minvalue);
					pDC->SelectObject(&redpen);
				}
				else
				{//negative
					x = val[j] + minvalue;
					x= x/(maxvalue - minvalue);
					pDC->SelectObject(&bluepen);
				}

				// draw graph
				xPos = 100+WellDist*j+hgt - 2; // the - 2 is to offset from the text
				yPos = SeperationDist+spacing*i;
				pDC->MoveTo(xPos, yPos);

				yPos += (signed int)(lineHtMult*x);
				pDC->LineTo(xPos, yPos);
			}
			else
			{
				sprintf(buf, "?");
			}

			// write value
			xPos = 100+WellDist*j+hgt + 2;
			yPos = SeperationDist-10+spacing*i;
			pDC->TextOut(xPos, yPos, buf, strlen(buf));
		}

	}	// end of loop over all layers
	pDC->SelectObject(pOldFont);
	tablefont.DeleteObject();
	verticalfont.DeleteObject();
}

/////////////////////////////////////////////////////////////////////////////
// CWellDataView diagnostics

#ifdef _DEBUG
void CWellDataView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CWellDataView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}
#endif

/////////////////////////////////////////////////////////////////////////////
// CWellDataView message handlers

void CWellDataView::OnClickedDoneButton()
{
	// clicked on the "Done" button
	
	GetParentFrame()->DestroyWindow();	// close parent window=the frame window around this view	
}

void CWellDataView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if(nChar==VK_ESCAPE)
		OnClickedDoneButton();
	else
		CScrollView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CWellDataView::OnClickedDumpButton()
{
	// clicked on the "Dump to spreadsheet" button
	// writes to external file as well as recording in logfile
	char buf2[128];
	char buf[2048];
	char s_buf[2048];
	char buf3[128];
	int i, j;
	double val;
	const char *s;
	int layernum=pDoc->horizonlist.GetSize();
	int wellnum=pDoc->horizonlist.wellnames.GetSize();
	CMeasurePt *measurept;
	CHorizon *horizon;
	CStdioFile spread;
	CString file;
	CString Title;

	// title
	sprintf(buf2, (pDoc->Units & UNIT_OUTPUTINFEET) ? "feet" : "metres");

	switch(chosentype)
	{
		//grid well ties
	case 0: sprintf(buf3, "milliseconds"); Title="Table of time corrections"; break;
	case 1: sprintf(buf3, buf2);           Title="Table of depth corrections"; break;
	case 2: sprintf(buf3, strcat(buf2, "/second")); Title="Table of interval velocity corrections"; break;

		//interval tops
	case 3: sprintf(buf3, "milliseconds"); Title="Table of interval tops times"; break;
	case 4: sprintf(buf3, buf2);		Title="Table of interval tops depths"; break;

		//interval bases
	case 5: sprintf(buf3, "milliseconds"); Title="Table of interval bases times"; break;
	case 6: sprintf(buf3, buf2);           Title="Table of interval bases depths"; break;

		//interval midpoint values
	case 7: sprintf(buf3, "milliseconds"); Title="Table of interval mid point times"; break;
	case 8: sprintf(buf3, buf2);           Title="Table of interval mid point depths'"; break;

		//interval values
	case 9: sprintf(buf3, "milliseconds"); Title="Table of interval times"; break;
	case 10: sprintf(buf3, buf2);           Title="Table of interval depths"; break;
	case 11: sprintf(buf3, strcat(buf2, "/second")); Title="Table of interval velocities"; break;

		//weighting and residuals
	case 12: sprintf(buf3, "coefficient");           Title="Table of well weighting coefficients"; break;
	case 13: sprintf(buf3, "as Y axis of function"); Title="Table of residual values"; break;
	}

	file = pDoc->GetExportDir() + Title + ".csv";

	if(!spread.Open( (LPCTSTR)file, CFile::modeCreate | CFile::modeReadWrite | CFile::typeText ))
	{
		AfxMessageBox("Could not open spreadsheet file.\n\nProject creation aborted.",
			MB_OK | MB_ICONEXCLAMATION);
		return;
	}

	sprintf(buf, "\nUnits: %s", buf3);

	pDoc->Log(buf, LOG_BRIEF);
	spread.WriteString( Title + "\n" );
	spread.WriteString( (CString) buf+"\n" );

	// top table line with horizon names/numbers
	sprintf(buf, "Well name           ");
	sprintf(s_buf, "Well name, ");
	for(i=0;i<layernum;i++)
	{
		horizon=(CHorizon *)pDoc->horizonlist.GetAt(i);

		sprintf(buf2, "|Hor %-2d", i+1);
		strcat(buf, buf2);

		sprintf( buf2, "%s, ", horizon->name);
		strcat( s_buf, buf2 );
	}
	pDoc->Log(buf, LOG_BRIEF);
	spread.WriteString( (CString)s_buf+"\n");

	// dividing line
	sprintf(buf, "--------------------+");
	for(i=0;i<layernum;i++)
	{
		strcat(buf, "------");
		if(i<layernum-1)
			strcat(buf, "+");
	}
	pDoc->Log(buf, LOG_BRIEF);

	// all wells one by one
	for(i=0;i<wellnum;i++)
	{
		CString a = "Well: " + (pDoc->horizonlist.wellnames[i]);
		// well name. <=25 chars: full name  else first 22 chars, then "..."
		s=(const char *) a;
		if(strlen(s)<=25)
		{
			sprintf(buf, "%-20s|", s);
			sprintf(s_buf, "%-20s, ", s);
		}
		else
		{
			strcpy(buf2, s);
			buf2[22]='\0';  // fake end of string
			sprintf(buf, "%22s...|", buf2);
			sprintf(s_buf, "%22s..., ", buf2);
		}
		for(j=0;j<layernum;j++)
		{
			horizon=(CHorizon *)pDoc->horizonlist.GetAt(j);
			measurept = (CMeasurePt *) horizon->GetAt(i);
			val=MPT_INVALID;
			if(measurept!=NULL && !(measurept->ReadStatus() & MPT_STATUS_INVALID))
			{
				switch(chosentype)
				{
					//grid well ties
				case 0:val=measurept->ReadTieTdiff();break;
				case 1:val=measurept->ReadTieDdiff();break;
				case 2:val=measurept->ReadTieVdiff();break;

					//interval tops
				case 3:val=measurept->ReadtTop();break;
				case 4:val=measurept->ReadZ1();break;

					//interval bases
				case 5:val=measurept->ReadTime();break;
				case 6:val=measurept->ReadDepth();break;

					//interval midpoint values
				case 7:val=measurept->Readmptime();break;
				case 8:val=measurept->Readmpdepth();break;

					//interval values
				case 9:val=measurept->ReadtInt();break;
				case 10:val=measurept->ReaddInt();break;
				case 11:val=measurept->ReadvInt();break;

					//weighting and residuals
				case 12:val=measurept->ReadWeight();break;
				case 13:
					if(horizon->ModelNumber==10)
						break;
					if(horizon->HaveFit)
					{
						double x, y, z1;
						CWellModelFunction *modfunc;
						modfunc=pDoc->well_model_lib[horizon->ModelNumber];
						measurept->ReadValuePair(horizon->ModelNumber, &x, &y);
						if(x!=MPT_ILLEGAL && y!=MPT_ILLEGAL)
						{
							if(horizon->ModelNumber==0)
							{
								z1=measurept->ReadZ1();
								val=y+( -z1-1.0/(horizon->FitB) )*exp(x/(horizon->FitA))+1.0/(horizon->FitB);
							}
							else
								val=y-modfunc->func(x, horizon->FitA, horizon->FitB);
						}
						else
							val=MPT_INVALID;

						break;
					}
				}

			}
			if(val==MPT_INVALID)
				sprintf(buf2, "  ?   ");
			else
				sprintf(buf2, "%6.0f", val);
			strcat(buf, buf2);
			strcat(s_buf, buf2);
			if(j<layernum-1)
			{
				strcat(buf, "|");
				strcat(s_buf, ", ");
			}
		}
		pDoc->Log(buf, LOG_BRIEF);
		spread.WriteString( (CString) s_buf+"\n" );
	}
	pDoc->Log("\n", LOG_BRIEF);

	spread.Close();
	AfxMessageBox("Spreadsheet file written to project export sub directory.\n\n" + file, MB_OK );
}
