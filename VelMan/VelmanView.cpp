#include "stdafx.h"
#include "velman.h"

#include "VelmanDoc.h"
#include "VelmanView.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVelmanView

IMPLEMENT_DYNCREATE(CVelmanView, CView)

BEGIN_MESSAGE_MAP(CVelmanView, CView)
	//{{AFX_MSG_MAP(CVelmanView)
	// NOTE - the ClassWizard will add and remove mapping macros here.
	//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVelmanView construction/destruction

CVelmanView::CVelmanView()
{
	// TODO: add construction code here
}

CVelmanView::~CVelmanView()
{
}

void CVelmanView::OnInitialUpdate()
{
	CRect rect(10, 50, 180, 70);
}

/////////////////////////////////////////////////////////////////////////////
// CVelmanView drawing

void CVelmanView::OnDraw(CDC* pDC)
{
	CVelmanDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	CFont font, bigfont;
	char buf[256];
	int i = 0;
	CString s;

	font.CreateFont(-12, 0, 0, 0, 400, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Arial");
	bigfont.CreateFont(-20, 0, 0, 0, 400, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Arial");

	CFont *pOldFont=(CFont *)pDC->SelectObject(&bigfont);
	pDC->TextOut(10, 10, pDoc->Title);

	CSize size=pDC->GetTextExtent((const char *)pDoc->Title,
		strlen((const char *)pDoc->Title));
	pDC->MoveTo(10, 10+size.cy);
	pDC->LineTo(10+size.cx, 10+size.cy);

	pDC->SelectObject(&font);
	int state=pDoc->ProjState;

	if(state & PRJ_HAVEHORIZONS)
	{
		sprintf(buf, "%d layers have been defined.",
			pDoc->horizonlist.GetSize());
		pDC->TextOut(10, 80, buf);
	}

	if(state & PRJ_HAVETIME)
		pDC->TextOut(10, 100, "Time grids have been assigned to these layers.");

	if(state & PRJ_HAVEWELLS)
	{
		sprintf(buf, "%d wells with %d layers have been imported.",
			pDoc->horizonlist.wellnames.GetSize(), pDoc->horizonlist.GetSize());
		pDC->TextOut(10, 40, buf);
		sprintf(buf, "Well survey data was read from %s.", (const char *)pDoc->WellSurv);
		pDC->TextOut(10, 60, buf);
		sprintf(buf, "Well time/depth data was read from %s.", (const char *)pDoc->WellTiDp);
		pDC->TextOut(10, 80, buf);

		if(state & PRJ_HAVESOMEMODELS)
		{
			if(state & PRJ_HAVEMODEL)
			{
				pDC->TextOut(10, 120, "All layers have been assigned model functions.");
				pDC->TextOut(10, 140, "Depth conversion complete.");
				if ( state & PRJ_RAYTRACED )
					pDC->TextOut(10, 160, "Ray tracing complete.");
			}
			else
			{
				int num=0;
				for(int i=0;i<pDoc->horizonlist.GetSize();i++)
				{
					if( ((CHorizon *)pDoc->horizonlist.GetAt(i))->HaveModel)
						num++;
				}
				if(num==1)
					sprintf(buf, "1 of the layers has been assigned a model function and"
					" depth converted.");
				else
					sprintf(buf, "%d of the layers have been assigned model functions and"
					" depth converted.", num);
				pDC->TextOut(10, 120, buf);
			}
		}
	}

	if(state & (PRJ_HAVERMSVEL | PRJ_PROCESSEDVEL))
	{
		if(pDoc->RMSsource.GetSize()>0)
		{
			pDC->SetTextAlign(TA_UPDATECP);
			pDC->MoveTo(10, 120);
			pDC->TextOut(0, 0, "RMS velocity data was read from ");
			for(i=0;i<pDoc->RMSsource.GetSize();i++)
			{
				// separate entries by commas, last one by "and"
				if(i>0)
				{
					if(i==pDoc->RMSsource.GetUpperBound())
						pDC->TextOut(0, 0, " and ");
					else
						pDC->TextOut(0, 0, ", ");
				}
				s=pDoc->RMSsource[i];
#ifdef VELMAN_UNIX
				if(s.Find('/')>-1)
					s=s.Mid(s.ReverseFind('/')+1);
#else
				if(s.Find('\\')>-1)
					s=s.Mid(s.ReverseFind('\\')+1);
#endif
				pDC->TextOut(0, 0, (const char *)s);
			}
			pDC->SetTextAlign(TA_NOUPDATECP);
		}
		if(state & PRJ_PROCESSEDVEL)
			pDC->TextOut(10, 140, "The velocity data was processed.");
		if(state & PRJ_HAVEDEPTHCONV)
			pDC->TextOut(10, 160, "Depth conversion has been performed.");
		if ( state & PRJ_RAYTRACED )
			pDC->TextOut(10, 180, "Ray tracing complete.");
	}

	pDC->SelectObject(pOldFont);
	font.DeleteObject();
	bigfont.DeleteObject();
}

/////////////////////////////////////////////////////////////////////////////
// CVelmanView printing

BOOL CVelmanView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CVelmanView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CVelmanView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CVelmanView diagnostics

#ifdef _DEBUG
void CVelmanView::AssertValid() const
{
	CView::AssertValid();
}

void CVelmanView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CVelmanDoc* CVelmanView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CVelmanDoc)));
	return (CVelmanDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CVelmanView message handlers

void CVelmanView::OnUpdate(CView *pSender, LPARAM lHint, CObject *pHint)
{
	if(lHint==0 || lHint==DOC_CHANGE_STATECHANGE)
	{
		CView::OnUpdate(pSender, lHint, pHint);
	}
}
