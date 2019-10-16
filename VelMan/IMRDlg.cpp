// IMRDialog.cpp : implementation file

#include "stdafx.h"
#include "velman.h"
#include "IMRDlg.h"
#include "VelmanDoc.h"
#include "mainfrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CIMRDialog dialog

CIMRDialog::CIMRDialog(CWnd* pParent /*=NULL*/)
: CDialog(CIMRDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CIMRDialog)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CIMRDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CIMRDialog)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CIMRDialog, CDialog)
	//{{AFX_MSG_MAP(CIMRDialog)
	ON_WM_HSCROLL()
	ON_LBN_DBLCLK(IDC_TIMELIST, OnDblclkTimelist)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIMRDialog messages

BOOL CIMRDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO: Add extra initialization here
	// no grid has been found as of yet
	found = FALSE;

	// fill the two listboxes
	CListBox *pLB=(CListBox *)GetDlgItem(IDC_TIMELIST);
	if(pDoc->zimscube.FillListboxWithGridnames(pLB, entries, TRUE)==0)
	{
		AfxMessageBox("There are no grids to display", MB_OK | MB_ICONINFORMATION);
		OnCancel();		// give it up if there are no grids around
	}

	pLB->SetCurSel(1);
	pLB->SetTabStops(15);

	// take control of scrollbar & display initial value next to it
	// also initialize values in the class
	CScrollBar *pSB=(CScrollBar *)GetDlgItem(IDC_SMOOTHSIZE);
	pSB->SetScrollRange(5, 125);
	pSB->SetScrollPos(10);
	SetDlgItemText(IDC_SIZETEXT, "10");
	smoothwindow=10;

	SetIcon (AfxGetApp()->LoadIcon(IDR_MAINFRAME), FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CIMRDialog::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default
	// default handler
	// CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
	// my handler

	BOOL UpdateView=TRUE;
	// it's the scrollbar with the # of excluded points
	int pos=pScrollBar->GetScrollPos();
	int oldpos=pos;
	char buf[32];

	switch(nSBCode)
	{
	case SB_THUMBPOSITION:
		pScrollBar->SetScrollPos(nPos);
		oldpos=100;	// for some strange reason we have to reinforce the redraw
		break;
	case SB_THUMBTRACK:
		pScrollBar->SetScrollPos(nPos);
		UpdateView=FALSE;		// no updating while scrollbar is being moved (too slow)
		break;
	case SB_LINEUP:
		pScrollBar->SetScrollPos(max(pos-1, 5));
		break;
	case SB_LINEDOWN:
		pScrollBar->SetScrollPos(min(pos+1, 50));
		break;
	case SB_PAGELEFT:
		pScrollBar->SetScrollPos(max(pos-4, 5));
		break;
	case SB_PAGERIGHT:
		pScrollBar->SetScrollPos(min(pos+4, 50));
		break;
	}

	// update text in dialog box itself
	pos=pScrollBar->GetScrollPos();
	sprintf(buf, "%d", pos);
	SetDlgItemText(IDC_SIZETEXT, buf);

	// initialize values by writing the new value back into the class
	smoothwindow=pos;

	// invalidate the fit and inforce a redraw if the actual number of excluded points
	// has changed.
	if(UpdateView && pos != oldpos)
		Invalidate();
}

void CIMRDialog::OnOK()
{
	// TODO: Add extra validation here
	CListBox *pLB;
	int click, i;

	// put the correct timegrid into the class structure
	pLB=(CListBox *)GetDlgItem(IDC_TIMELIST);
	click=pLB->GetCurSel();
	for(i=0;i<entries.GetSize();i++)
	{
		if((entries[i] & 0xff) == click)
		{
			timegrid = entries[i]>>8;
			found = TRUE;
			break;
		}
	}

	CDialog::OnOK();
}

void CIMRDialog::OnDblclkTimelist()
{
	// TODO: Add your control notification handler code here
	OnOK();
}