// GDbLDlg.cpp : implementation file

#include "stdafx.h"
#include "velman.h"
#include <afxcmn.h>
#include "VelmanDoc.h"
#include "GDbLDlg.h"
#include "mainfrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGridDatabaseListDlg dialog

CGridDatabaseListDlg::CGridDatabaseListDlg(CWnd* pParent /*=NULL*/)
: CDialog(CGridDatabaseListDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGridDatabaseListDlg)
	//}}AFX_DATA_INIT
	initialised=FALSE;
}

void CGridDatabaseListDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGridDatabaseListDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CGridDatabaseListDlg, CDialog)
	//{{AFX_MSG_MAP(CGridDatabaseListDlg)
	ON_BN_CLICKED(IDC_GRIDDATABASE_RADIO1, OnGriddatabaseRadio1)
	ON_BN_CLICKED(IDC_GRIDDATABASE_RADIO2, OnGriddatabaseRadio2)
	ON_WM_DESTROY()
	ON_NOTIFY(NM_RCLICK, IDC_GDB_TABLE, OnRclickGdbTable)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGridDatabaseListDlg message handlers

BOOL CGridDatabaseListDlg::OnInitDialog()
{
	smallFont.CreateFont(14, 0, 0, 0, 400, FALSE, FALSE, 0, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_SWISS, "Arial");

	CDialog::OnInitDialog();

	CListCtrl *pL=(CListCtrl *)GetDlgItem(IDC_GDB_TABLE);
	pL->SetFont(&smallFont);

	int i = 0;
	char *col_label[]={ "Name", "Type", "D", "F", "R/C", "Avg", "x", "y" };

	int col_width[]  ={    150,    60, 32,  32,    60,   60, 120, 120 };
	pL->DeleteColumn(0);
	for(i=0;i<8;i++)
		pL->InsertColumn(i, col_label[i], LVCFMT_LEFT, col_width[i], i);

	ListInternalGrids=TRUE;
	CheckRadioButton(IDC_GRIDDATABASE_RADIO1, IDC_GRIDDATABASE_RADIO2,
		IDC_GRIDDATABASE_RADIO1);
	FillList();

	SetIcon (AfxGetApp()->LoadIcon(IDR_MAINFRAME), FALSE);
	// this  is the first time that run so want to measure the distance constants
	RECT form;
	RECT component;
	this->GetWindowRect(&form);
	formWidth=form.right-form.left;
	// code to initialise variables

	//LR
	GetDlgItem (IDOK)->GetWindowRect(&component);
	border_LR_IDOK=form.right-component.left;

	//RR
	GetDlgItem (IDC_GDB_TABLE)->GetWindowRect(&component);
	border_RR_IDC_GDB_TABLE=form.right-component.right;

	//BB
	GetDlgItem (IDC_GDB_TABLE)->GetWindowRect(&component);
	border_BB_IDC_GDB_TABLE=form.bottom-component.bottom;

	initialised=TRUE;

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CGridDatabaseListDlg::FillList()
{
	CListCtrl *pL=(CListCtrl *)GetDlgItem(IDC_GDB_TABLE);
	CString s;
	int i = 0;
	char buf[256];

	pL->DeleteAllItems();
	// Just create a list of all internal grids
	if(ListInternalGrids)
	{
		CZimsGrid *grid;
		CFaultGrid *fault;
		double *xlimits, *ylimits;
		// list grids
		for(i=0;i<pDoc->zimscube.GetSize();i++)
		{
			grid=(CZimsGrid *)pDoc->zimscube.GetAt(i);
			xlimits=grid->GetXlimits();
			ylimits=grid->GetYlimits();
			pL->InsertItem(i, (const char *)(grid->GetName()));
			pL->SetItem(i, 1, LVIF_TEXT, grid->GetTypeName(), 0, 0, 0, 0);
			pL->SetItem(i, 2, LVIF_TEXT, (grid->GetID()==0 && grid->GetHorizon()>=0 &&
				grid->GetType()>=CSD_TYPE_TIME && grid->GetType()<=CSD_TYPE_DEPTH) ?
				"[P]" : "", 0, 0, 0, 0);

			pL->SetItem(i, 3, LVIF_TEXT, (grid->HasFaultGrid()) ?
				"÷" : "", 0, 0, 0, 0);
			sprintf(buf, "%d*%d", grid->GetRows(), grid->GetColumns());
			pL->SetItem(i, 4, LVIF_TEXT, buf, 0, 0, 0, 0);
			sprintf(buf, "%7.2f", grid->GetAvg());
			pL->SetItem(i, 5, LVIF_TEXT, buf, 0, 0, 0, 0);
			sprintf(buf, "%7.f..%7.f", xlimits[0], xlimits[1]);
			pL->SetItem(i, 6, LVIF_TEXT, buf, 0, 0, 0, 0);
			sprintf(buf, "%7.f..%7.f", ylimits[0], ylimits[1]);
			pL->SetItem(i, 7, LVIF_TEXT, buf, 0, 0, 0, 0);
		}
		// list faults
		for(i=0;i<pDoc->faultcube.GetSize();i++)
		{
			fault=(CFaultGrid *)pDoc->faultcube.GetAt(i);
			pL->InsertItem(i, (const char *)(fault->GetName()));
			pL->SetItem(i, 1, LVIF_TEXT, "Fault", 0, 0, 0, 0);
		}
		return;
	}

	CWaitCursor wait;
	CStringArray zimsnames;
	int index, headernum;
	struct GridFileHeaderInfo header;

	// shot line data files
	pDoc->FillListOfDOSnames();
	pDoc->seismicdata.MakeZimsNameList(zimsnames, pDoc->ListOfDOSnames, ZIMS_TYPE_LINE);
	for(i=0;i<zimsnames.GetSize();i++)
	{
		sprintf(buf, "%s", (const char *)(zimsnames.GetAt(i)));
		pL->InsertItem(i, (const char *)(zimsnames.GetAt(i)));
		pL->SetItem(i, 1, LVIF_TEXT, "Data", 0, 0, 0, 0);
	}

	// grid files
	pDoc->seismicdata.MakeZimsNameList(zimsnames, pDoc->ListOfDOSnames, ZIMS_TYPE_GRID);
	for(i=0;i<zimsnames.GetSize();i++)
	{
		s=zimsnames.GetAt(i);
		index = CP3_TYPE_GRID; //sk126 dangerous
		if((pDoc->seismicdata.MakeDOSName(s, pDoc->ListOfDOSnames, index, headernum)))
		{
			if(pDoc->seismicdata.ReadZimsFileHeader((const char *)(pDoc->ListOfDOSnames.GetAt(index)), headernum,
				0, 0, &header))
			{
				pL->InsertItem(i, (const char *)(zimsnames.GetAt(i)));
				pL->SetItem(i, 1, LVIF_TEXT, "Grid", 0, 0, 0, 0);
				sprintf(buf, "%d*%d", header.rows, header.columns);
				pL->SetItem(i, 4, LVIF_TEXT, buf, 0, 0, 0, 0);
				sprintf(buf, "%7.f..%7.f", header.xmin, header.xmax);
				pL->SetItem(i, 6, LVIF_TEXT, buf, 0, 0, 0, 0);
				sprintf(buf, "%7.f..%7.f", header.ymin, header.ymax);
				pL->SetItem(i, 7, LVIF_TEXT, buf, 0, 0, 0, 0);
			}
		}
	}

	// fault files
	pDoc->seismicdata.MakeZimsNameList(zimsnames, pDoc->ListOfDOSnames, ZIMS_TYPE_POLY);
	for(i=0;i<zimsnames.GetSize();i++)
	{
		sprintf(buf, "%s", (const char *)(zimsnames.GetAt(i)));
		pL->InsertItem(i, (const char *)(zimsnames.GetAt(i)));
		pL->SetItem(i, 1, LVIF_TEXT, "Fault", 0, 0, 0, 0);
	}
}

void CGridDatabaseListDlg::OnGriddatabaseRadio1()
{
	ListInternalGrids=TRUE;
	CheckRadioButton(IDC_GRIDDATABASE_RADIO1, IDC_GRIDDATABASE_RADIO2,
		IDC_GRIDDATABASE_RADIO1);
	FillList();
}

void CGridDatabaseListDlg::OnGriddatabaseRadio2()
{
	ListInternalGrids=FALSE;
	CheckRadioButton(IDC_GRIDDATABASE_RADIO1, IDC_GRIDDATABASE_RADIO2,
		IDC_GRIDDATABASE_RADIO2);
	FillList();
}

void CGridDatabaseListDlg::OnDestroy()
{
	CDialog::OnDestroy();
	smallFont.DeleteObject();
}

void CGridDatabaseListDlg::OnRclickGdbTable(NMHDR* pNMHDR, LRESULT* pResult)
//
//  This callback implements a popup menu on the grid directory list view.
//  We use the grid tools menu off the main velman menu and display that as
//  popup menu. However we remove the first item, Grid Directory, since the
//  menu is displayed on the window. The nice thing about using the Velman main
//  menu is that all the command callbacks have been assigned and we don't need
//  to recreate them for a separate menu. However, it would be a nice feature if
//  the popups could be initialised with whatever the user currently has selected
//  on the grid directory list. Something for the future...
//
{
	CMenu   menu;
	CPoint  pt;

	if ( ! menu.LoadMenu(IDR_VELMANTYPE) )       // get the main menu
	{
		menu.DestroyMenu();
		return;
	}
	CMenu*   pPopup = menu.GetSubMenu(6);   // 6 is the position of the grid tools menu

	// Display menu where the right mouse button went down

	if ( pPopup )
	{
		//  Get rid of the items we don't want
		//  The only one is the Grid Directory item because we're displaying on
		//  top of the Grid Directory dialogue.
		pPopup->RemoveMenu(ID_GRIDTOOLS_DIRECTORY, MF_BYCOMMAND);

		//  Put the popup menu where the cursor currently is.
		GetCursorPos(&pt);
		pPopup->TrackPopupMenu(TPM_RIGHTBUTTON, pt.x, pt.y, this);
	}

	// Tidy up
	menu.DestroyMenu();

	// should always return this
	*pResult = 0;
}

void CGridDatabaseListDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if (initialised)
	{
		RECT form;
		RECT component;

		int top, left, width, height;

		this->GetWindowRect(&form);
		// reposition/ size the components

		//LR
		GetDlgItem(IDOK)->GetWindowRect(&component);
		left=form.right - form.left- border_LR_IDOK-4;
		top=component.top-form.top-CMainFrame::GetConfigTitleBarThickness();
		GetDlgItem(IDOK)->SetWindowPos(&wndTop, left, top, 0, 0, SWP_NOSIZE);

		//RR
		GetDlgItem(IDC_GDB_TABLE)->GetWindowRect(&component);
		height=component.bottom - component.top;
		width=form.right-component.left-border_RR_IDC_GDB_TABLE;
		GetDlgItem(IDC_GDB_TABLE)->SetWindowPos(&wndTop, 0, 0, width, height, SWP_NOMOVE);

		//BB
		GetDlgItem(IDC_GDB_TABLE)->GetWindowRect(&component);
		height=form.bottom-border_BB_IDC_GDB_TABLE-component.top;
		width=component.right - component.left;
		GetDlgItem(IDC_GDB_TABLE)->SetWindowPos(&wndTop, 0, 0, width, height, SWP_NOMOVE);

		this->Invalidate();
	}
}
