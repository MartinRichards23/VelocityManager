// GImpDlg.cpp : implementation file
//

#include "stdafx.h"
#include "velman.h"
#include "velmadoc.h"
#include "imp.h"

/////////////////////////////////////////////////////////////////////////////
// CGridImportDlg dialog


CGridImportDlg::CGridImportDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGridImportDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGridImportDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	initialised = FALSE;
}


void CGridImportDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGridImportDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGridImportDlg, CDialog)
	//{{AFX_MSG_MAP(CGridImportDlg)
	ON_LBN_DBLCLK(IDC_GRIDIMP_GRIDLIST, OnOK)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGridImportDlg message handlers

BOOL CGridImportDlg::OnInitDialog() 
{
	int i;

	CDialog::OnInitDialog();
	
	CListBox *pLB=(CListBox *)GetDlgItem(IDC_GRIDIMP_GRIDLIST);
	// now create a list of ZIMS grid names from that list of DOS names
	CStringArray zimsnames;
	pDoc->seismicdata.MakeZimsNameList(zimsnames,pDoc->ListOfDOSnames,ZIMS_TYPE_GRID);
	
	for(i=0;i<zimsnames.GetSize();i++)
		pLB->AddString((const char *)(zimsnames.GetAt(i)));
  
    pLB->SetCurSel(0);

	// list of fault grids
	pLB=(CListBox *)GetDlgItem(IDC_GRIDIMP_FAULTGRIDLIST);
	zimsnames.RemoveAll();

	pDoc->seismicdata.MakeZimsNameList(zimsnames,pDoc->ListOfDOSnames,ZIMS_TYPE_POLY);
	pLB->AddString("(none)");
	for(i=0;i<zimsnames.GetSize();i++)
		pLB->AddString((const char *)(zimsnames.GetAt(i)));

	// "Time grid" as default
	((CComboBox *)GetDlgItem(IDC_GRIDIMP_GRIDTYPE))->SetCurSel(0);

	//shauns code	
	SetIcon (AfxGetApp()->LoadIcon(IDR_MAINFRAME),FALSE);
	// this  is the first time that run so want to measure the distance constants	
	RECT *form = new RECT();	
	RECT *component = new RECT();	
	this->GetWindowRect(form);	
	formWidth=form->right-form->left;
	formHeight=form->bottom-form->top;
	// code to initialise shauns variables	
	
	//set text showing current zims dir
	CString s = "Directory: " + pDoc->ZimsDir;
	GetDlgItem(IDC_STATICDIR)->SetWindowTextA((LPCSTR)s);

	//LR
	GetDlgItem (IDOK)->GetWindowRect(component);
	border_LR_IDOK=form->right-component->left;

	//LR
	GetDlgItem (IDCANCEL)->GetWindowRect(component);
	border_LR_IDCANCEL=form->right-component->left;

	//TB
	GetDlgItem (IDC_GRIDIMP_GRIDTYPE)->GetWindowRect(component);
	border_TB_IDC_GRIDIMP_GRIDTYPE=form->bottom-component->top;

	//TB
	GetDlgItem (IDC_STATIC1)->GetWindowRect(component);
	border_TB_IDC_STATIC1=form->bottom-component->top;

	// vertically linked
	GetDlgItem (IDC_GRIDIMP_GRIDLIST)->GetWindowRect(component);
	border_RR_IDC_GRIDIMP_GRIDLIST=form->right-component->right;
	height_IDC_GRIDIMP_GRIDLIST=component->bottom-component->top;

	GetDlgItem (IDC_GRIDIMP_FAULTGRIDLIST)->GetWindowRect(component);
	border_TT_IDC_GRIDIMP_FAULTGRIDLIST=component->top-form->top;
	height_IDC_GRIDIMP_FAULTGRIDLIST=component->bottom-component->top;
	
	////TB
	int temp=component->top;
	GetDlgItem (IDC_STATIC2)->GetWindowRect(component);
	border_YSpacer_IDC_STATIC2=temp-component->top;


	// shauns cleanup code	
	delete form;	
	delete component;	
	initialised=TRUE;	



	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CGridImportDlg::OnOK() 
{
	// find out chosen grid
	CListBox *pLB=(CListBox *)GetDlgItem(IDC_GRIDIMP_GRIDLIST);
	if(pLB->GetCurSel()<0)
		return;
	pLB->GetText(pLB->GetCurSel(),ChosenGrid);

	pLB=(CListBox *)GetDlgItem(IDC_GRIDIMP_FAULTGRIDLIST);	
	if(pLB->GetCurSel()>0)
	{
		pLB->GetText(pLB->GetCurSel(),FaultGrid);
		UseFaults=TRUE;
	}
	else
		UseFaults=FALSE;

	// find out chosen type
	int entry=((CComboBox *)GetDlgItem(IDC_GRIDIMP_GRIDTYPE))->GetCurSel();
	switch(entry)
	{
		case 0:gridtype=CSD_TYPE_TIME;break;
		case 1:gridtype=CSD_TYPE_DEPTH;break;
		case 2:gridtype=CSD_TYPE_INTVEL;break;
		default:gridtype=CSD_TYPE_OTHER;break;
	}

	CDialog::OnOK();
}

void CGridImportDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	//shaun code	
	if (initialised)	
		{
		RECT *form = new RECT();
		RECT *component = new RECT();
		
		int top,left,width,height,deltaHeight;
		int LeftDistance = 10; //distance to align left aligned items

		this->GetWindowRect(form);
		// reposition/ size the components

		//LR
		GetDlgItem(IDOK)->GetWindowRect(component);
		left=form->right -form->left- border_LR_IDOK-4;
		top=component->top-form->top-CMainFrame::GetShaunTitleBarThickness();
		GetDlgItem(IDOK)->SetWindowPos(&wndTop,left,top,0,0,SWP_NOSIZE);

		//LR
		GetDlgItem(IDCANCEL)->GetWindowRect(component);
		left=form->right -form->left- border_LR_IDCANCEL-4;
		top=component->top-form->top-CMainFrame::GetShaunTitleBarThickness();
		GetDlgItem(IDCANCEL)->SetWindowPos(&wndTop,left,top,0,0,SWP_NOSIZE);

		//TB
		GetDlgItem (IDC_GRIDIMP_GRIDTYPE)->GetWindowRect(component);
		left=LeftDistance;
		top=form->bottom-border_TB_IDC_GRIDIMP_GRIDTYPE-form->top-CMainFrame::GetShaunTitleBarThickness();
		GetDlgItem(IDC_GRIDIMP_GRIDTYPE)->SetWindowPos(&wndTop,left,top,0,0,SWP_NOSIZE);

		//TB
		GetDlgItem (IDC_STATIC1)->GetWindowRect(component);
		left=LeftDistance;
		top=form->bottom-border_TB_IDC_STATIC1-form->top-CMainFrame::GetShaunTitleBarThickness();
		GetDlgItem(IDC_STATIC1)->SetWindowPos(&wndTop,left,top,0,0,SWP_NOSIZE);

		//Linked boxes	
		GetDlgItem (IDC_GRIDIMP_GRIDLIST)->GetWindowRect(component);	
		width=form->right-component->left-border_RR_IDC_GRIDIMP_GRIDLIST;
		deltaHeight=(int)(((form->bottom-form->top)-formHeight)/2.0);	
		height=height_IDC_GRIDIMP_GRIDLIST+deltaHeight;	
		GetDlgItem(IDC_GRIDIMP_GRIDLIST)->SetWindowPos(&wndTop,0,0,width,height,SWP_NOMOVE);	
			
		GetDlgItem (IDC_GRIDIMP_FAULTGRIDLIST)->GetWindowRect(component);	
		top=border_TT_IDC_GRIDIMP_FAULTGRIDLIST+deltaHeight-23;
		GetDlgItem(IDC_GRIDIMP_FAULTGRIDLIST)->SetWindowPos(&wndTop,left,top,0,0,SWP_NOSIZE);	
			
		//width=form->bottom-border_BB_YYYY-component->top;	
		height=height_IDC_GRIDIMP_FAULTGRIDLIST+deltaHeight;	
		GetDlgItem(IDC_GRIDIMP_FAULTGRIDLIST)->SetWindowPos(&wndTop,0,0,width,height,SWP_NOMOVE);	

		//TB
		top=top-border_YSpacer_IDC_STATIC2;
		GetDlgItem(IDC_STATIC2)->SetWindowPos(&wndTop,left,top,0,0,SWP_NOSIZE);

		// cleanup shauns variables and repaint form
		delete form;
		delete component;
		this->Invalidate();
		}
	
	


	
}
// imp.cpp : implementation file
//

#include "stdafx.h"
#include "Velman.h"
#include "imp.h"


// imp

IMPLEMENT_DYNAMIC(imp, CWnd)

imp::imp()
{

}

imp::~imp()
{
}


BEGIN_MESSAGE_MAP(imp, CWnd)
END_MESSAGE_MAP()



// imp message handlers


