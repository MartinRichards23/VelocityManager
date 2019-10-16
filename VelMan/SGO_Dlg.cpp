// SGO_Dlg.cpp : implementation file

// this dialog has two different user interfaces: The user has to define a
// filter threshold value in the range 0..95%. This can either be done using
// a scrollbar (VISUAL_INPUT), or a text input field.

#ifndef VELMAN_UNIX
#define VISUAL_INPUT
#endif

#include <math.h>
#include <stdlib.h>
#include "stdafx.h"
#include "velman.h"
#include "VelmanDoc.h"
#include "SGO_Dlg.h"
#include "mainfrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSmoothGridOptionsDlg dialog

CSmoothGridOptionsDlg::CSmoothGridOptionsDlg(CWnd* pParent /*=NULL*/)
: CDialog(CSmoothGridOptionsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSmoothGridOptionsDlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pMemDC=new CDC;
	m_pBitmap=new CBitmap;
}

CSmoothGridOptionsDlg::~CSmoothGridOptionsDlg()
{
	delete m_pBitmap;
	delete m_pMemDC;
}

void CSmoothGridOptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSmoothGridOptionsDlg)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSmoothGridOptionsDlg, CDialog)
	//{{AFX_MSG_MAP(CSmoothGridOptionsDlg)
	ON_WM_HSCROLL()
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_SMOOTHOPT_UPDATE, OnSmoothoptUpdate)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON2, &CSmoothGridOptionsDlg::OnBnClickedButton2)
	ON_STN_CLICKED(IDC_SMOOTHOPT_YMAX, &CSmoothGridOptionsDlg::OnStnClickedSmoothoptYmax)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSmoothGridOptionsDlg message handlers

BOOL CSmoothGridOptionsDlg::OnInitDialog()
{
	if(m_pMemDC->GetSafeHdc()==NULL)
	{
		CClientDC dc(this);
		CWnd *pWnd=GetDlgItem(IDC_SMOOTHOPT_PREVIEW);
		pWnd->GetClientRect(PreviewClientRect);
		dc.LPtoDP(PreviewClientRect);
		m_pMemDC->CreateCompatibleDC(&dc);
		m_pBitmap->CreateCompatibleBitmap(&dc, PreviewClientRect.right, PreviewClientRect.bottom);
		m_pMemDC->SetMapMode(MM_ANISOTROPIC);
	}

	CDialog::OnInitDialog();

	// make a copy of the grid and FFT it
	mygrid = new CZimsGrid(pDoc);
	(*mygrid) = (*GridToSmooth);
	mygrid->FFT2D(1);
	mygrid->FFT2Amp();		// compute wave amplitudes

	double **data=mygrid->GetGrid();
	data[0][0]=0.0;						// overwrite overly large value at 0 (=sum of all points)

	dispgrid = new CZimsGrid(pDoc);	// copy to use for display purposes only so we don't need to repeat the fft
	(*dispgrid) = (*mygrid);		// dispgrid is now a copy of the wave amplitudes of the fft'd grid

	filtervalue=50;

	SetDlgItemText(IDC_SMOOTHOPT_XMAX, "y");	// orig code had these wrong way round
	SetDlgItemText(IDC_SMOOTHOPT_YMAX, "x");

	// Take control of the scrollbar
	CScrollBar *pSB=(CScrollBar *)GetDlgItem(IDC_SMOOTHOPT_SCRL);
	pSB->SetScrollRange(0, 100);
	pSB->SetScrollPos(50);

#ifdef VISUAL_INPUT
	// scrollbar, text box is disabled
	GetDlgItem(IDC_SMOOTHOPT_UPDATE)->ShowWindow(FALSE);
	GetDlgItem(IDC_SMOOTHOPT_FILTVAL)->EnableWindow(FALSE);
	SetDlgItemText(IDC_SMOOTHOPT_FILTVAL, "85%");
#else
	// No scrollbar, but an active text box to input a number
	GetDlgItem(IDC_SMOOTHOPT_0)->ShowWindow(FALSE);
	GetDlgItem(IDC_SMOOTHOPT_95)->ShowWindow(FALSE);
	GetDlgItem(IDC_SMOOTHOPT_SCRL)->ShowWindow(FALSE);
	SetDlgItemText(IDC_SMOOTHOPT_FILTVAL, "50");
#endif
	Invalidate();

	return TRUE;
}

void CSmoothGridOptionsDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	int pos=pScrollBar->GetScrollPos();
	BOOL UpdateView=TRUE;

	switch(nSBCode)
	{
	case SB_THUMBPOSITION:
		pScrollBar->SetScrollPos(nPos);
		break;
	case SB_THUMBTRACK:
		pScrollBar->SetScrollPos(nPos);
		UpdateView=FALSE;		// no updating while scrollbar is being moved (too slow)
		break;
	/*case SB_LINEUP:
		pScrollBar->SetScrollPos(max(pos-1, 0));
		break;
	case SB_LINEDOWN:
		pScrollBar->SetScrollPos(min(pos+1, 100));
		break;
	case SB_PAGELEFT:
		pScrollBar->SetScrollPos(max(pos-10, 0));
		break;
	case SB_PAGERIGHT:
		pScrollBar->SetScrollPos(min(pos+10, 100));
		break;*/
	}

	// update text in dialog box itself
	pos=pScrollBar->GetScrollPos();
	filtervalue= 75 + (double)pos/5;
	char buf[16];
	sprintf(buf, "%2.1f %", filtervalue);
	SetDlgItemText(IDC_SMOOTHOPT_FILTVAL, buf);

	if(UpdateView)
	{
		Invalidate();
	}
}

void CSmoothGridOptionsDlg::OnOK()
{
#ifdef VISUAL_INPUT
	CDialog::OnOK();
	delete mygrid;
	delete dispgrid;
#else
	BOOL translateOk;
	int pos;

	pos=GetDlgItemInt(IDC_SMOOTHOPT_FILTVAL, &translateOk);
	if(pos>=0 && pos<=95 && translateOk)
	{
		filtervalue=pos;
		CDialog::OnOK();
		delete mygrid;
		delete dispgrid;
	}
	else
	{
		AfxMessageBox("Please enter a number in the range 0..95");
		GotoDlgCtrl(GetDlgItem(IDC_SMOOTHOPT_FILTVAL));
		return;
	}
#endif
}

void CSmoothGridOptionsDlg::OnPaint()
{
	CPaintDC(this);
	CWnd *pWnd=GetDlgItem(IDC_SMOOTHOPT_PREVIEW);
	CDC *pDC=pWnd->GetDC();

	CBitmap *pOldBitmap=(CBitmap *)(m_pMemDC->SelectObject(m_pBitmap));

	UpdateWindow();
	CRect clientRect;
	pWnd->GetClientRect(clientRect);

	int cols=dispgrid->GetColumns()/2;		// using wave amplitudes so only half of each dimension needed.
	int rows=dispgrid->GetRows()/2;
	int rstep, cstep;
	rstep=rows/PreviewClientRect.Height()+1;
	cstep=cols/PreviewClientRect.Width()+1;

	double cx=cstep*(PreviewClientRect.Width()*1.0)/cols;	// # pixels per gridpoint horizontally
	double cy=rstep*(PreviewClientRect.Height()*1.0)/rows;
	int i, j, ii, jj, col;

	(*dispgrid) = (*mygrid);		// if paint is slow, then inline lowpassfile to work on row/2 & cols/2 instead of full grid
	dispgrid->LowPassFilter(filtervalue);

	// use log10 to compress data range so it fits reasonably into a 1->255 set of values
	// and gives a better visual impression.
	double zmax = log10(dispgrid->CalcMaxVal()+1);

	double **data=dispgrid->GetGrid();
	for(i=0, ii=0;i<rows;ii++, i+=rstep)
	{
		for(j=0, jj=0;j<cols;jj++, j+=cstep)
		{
			col=(int)( 255.0*log10(data[i][j]+1)/zmax );	// n.b. minimum is assured to be zero
			m_pMemDC->FillSolidRect((int)(jj*cx), (int)(ii*cy), (int)cx+1, (int)cy+1, RGB(col, col, col));
		}
	}

	pDC->BitBlt(0, 0, PreviewClientRect.Width(), PreviewClientRect.Height(), m_pMemDC, 0, 0, SRCCOPY);
	m_pMemDC->SelectObject(pOldBitmap);
	pWnd->ReleaseDC(pDC);
}

void CSmoothGridOptionsDlg::OnBnClickedButton2()
{
	CPaintDC(this);
	CWnd *pWnd=GetDlgItem(IDC_SMOOTHOPT_PREVIEW);
	CDC *pDC=pWnd->GetDC();

	CBitmap *pOldBitmap=(CBitmap *)(m_pMemDC->SelectObject(m_pBitmap));

	UpdateWindow();
	CRect clientRect;
	pWnd->GetClientRect(clientRect);

	int cols=dispgrid->GetColumns()/2;		// using wave amplitudes so only half of each dimension needed.
	int rows=dispgrid->GetRows()/2;
	int rstep, cstep;
	rstep=rows/PreviewClientRect.Height()+1;
	cstep=cols/PreviewClientRect.Width()+1;

	double cx=cstep*(PreviewClientRect.Width()*1.0)/cols;	// # pixels per gridpoint horizontally
	double cy=rstep*(PreviewClientRect.Height()*1.0)/rows;
	int i, j, ii, jj, col;

	(*dispgrid) = (*mygrid);		// if paint is slow, then inline lowpassfile to work on row/2 & cols/2 instead of full grid
	dispgrid->LowPassFilter(filtervalue);

	// use log10 to compress data range so it fits reasonably into a 1->255 set of values
	// and gives a better visual impression.
	double zmax = log10(dispgrid->CalcMaxVal()+1);

	CStdioFile spread;
	CString val;
	CString file = pDoc->GetExportDir() + "fft.txt";
	if(!spread.Open( (LPCTSTR)file, CFile::modeCreate | CFile::modeReadWrite | CFile::typeText ))
	{
		AfxMessageBox("Could not open file.\n\nProject creation aborted.",
			MB_OK | MB_ICONEXCLAMATION);
		return;
	}

	double **data=dispgrid->GetGrid();
	for(i=0, ii=0;i<rows;ii++, i+=rstep)
	{
		for(j=0, jj=0;j<cols;jj++, j+=cstep)
		{
			val.Format("%6.0f", data[i][j]);
			spread.WriteString(val + ", ");

			col=(int)( 255.0*log10(data[i][j]+1)/zmax );	// n.b. minimum is assured to be zero
			m_pMemDC->FillSolidRect((int)(jj*cx), (int)(ii*cy), (int)cx+1, (int)cy+1, RGB(col, col, col));
		}

		spread.WriteString("\n");
	}

	spread.Close();

	pDC->BitBlt(0, 0, PreviewClientRect.Width(), PreviewClientRect.Height(), m_pMemDC, 0, 0, SRCCOPY);
	m_pMemDC->SelectObject(pOldBitmap);
	pWnd->ReleaseDC(pDC);
}

void CSmoothGridOptionsDlg::OnCancel()
{
	delete mygrid;
	delete dispgrid;
	CDialog::OnCancel();
}

void CSmoothGridOptionsDlg::OnSmoothoptUpdate()
{
	BOOL translateOk;
	int pos;

	pos=GetDlgItemInt(IDC_SMOOTHOPT_FILTVAL, &translateOk);
	if(pos>=0 && pos<=95 && translateOk)
	{
		filtervalue=pos;
		Invalidate();
	}
	else
	{
		AfxMessageBox("Please enter a number in the range 0..95");
		GotoDlgCtrl(GetDlgItem(IDC_SMOOTHOPT_FILTVAL));
	}
}

#undef VISUAL_INPUT

void CSmoothGridOptionsDlg::OnStnClickedSmoothoptYmax()
{
	// TODO: Add your control notification handler code here
}
