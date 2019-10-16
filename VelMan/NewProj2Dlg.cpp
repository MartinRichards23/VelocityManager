#include "stdafx.h"
#include "velman.h"
#include "VelmanDoc.h"
#include "NewProj2Dlg.h"
#include "global.h"
#include <direct.h>
#include "mainfrm.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNewProjStep2 dialog

CNewProjStep2::CNewProjStep2(CWnd* pParent /*=NULL*/)
: CDialog(CNewProjStep2::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNewProjStep2)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	initialised=FALSE;
}

void CNewProjStep2::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNewProjStep2)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CNewProjStep2, CDialog)
	//{{AFX_MSG_MAP(CNewProjStep2)
	ON_CBN_SELCHANGE(IDC_COMBO_NEWPROJDRIVE2, OnSelchangeComboNewprojdrive2)
	ON_LBN_DBLCLK(IDC_LIST_NEWPROJDIR2, OnDblclkListNewprojdir2)
	ON_CBN_SELCHANGE(IDC_NEWPROJ_GRIDFILETYPE, OnSelchangeNewprojGridfiletype)
	ON_EN_KILLFOCUS(IDC_NEWPROJ2_EDITDIR, OnKillfocusNewproj2Editdir)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1, &CNewProjStep2::OnBnClickedButton1)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewProjStep2 message handlers

void CNewProjStep2::OnSelchangeComboNewprojdrive2()
{
	// User chose new drive => set new Target directory
	CString s;
	CComboBox *pCB=(CComboBox *)GetDlgItem(IDC_COMBO_NEWPROJDRIVE2);
	pCB->GetLBText(pCB->GetCurSel(), s);
#ifndef VELMAN_UNIX
	s.MakeLower();
#endif
	ZimsDir=s[2];
	ZimsDir+=":\\";

	// old code with 8.3 problem.
	//CListBox *pLB=(CListBox *)GetDlgItem(IDC_LIST_NEWPROJDIR2);
	//pLB->ResetContent();
	//pLB->Dir(0x0010 + 0x8000, ZimsDir+"*.*");
	DoFillDir();	// fills list box with dir names.

	// old code commented out when static text on proj2 changed to editable text.
	//	CWnd *pW=GetDlgItem(IDC_TARGET_PATH_TEXT2);
	//	pW->SetWindowText(ZimsDir);
	GetDlgItem(IDC_NEWPROJ2_EDITDIR)->SetWindowText(ZimsDir);
	DisplayGridNamesPreview();
}

BOOL CNewProjStep2::OnInitDialog()
{
	CDialog::OnInitDialog();
	CString entry, dir;
	char drive;

	CFont *m_Font1 = new CFont;
	m_Font1->CreatePointFont(80, "Arial Bold");

	((CStatic *)GetDlgItem(IDC_STATIC1))->SetFont(m_Font1);

#ifdef VELMAN_UNIX
	// we do not have a drive selection combobox under unix
	GetDlgItem(IDC_COMBO_NEWPROJDRIVE2)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_COMBO_NEWPROJDRIVE2_TEXT)->ShowWindow(SW_HIDE);
#endif

	// try to select meaningful entries
#ifdef VELMAN_UNIX
	drive='+'; // useless but we do not use it anyway (I hope)
	if(ZimsDir.Right(1)!='/')
		ZimsDir+='/';
#else
	if(ZimsDir[1]==':')
		drive=ZimsDir[0];
	else
	{
		drive='C';
		ZimsDir="C:"+ZimsDir;
	}
	if(ZimsDir.Right(1)!='\\')
		ZimsDir+='\\';
	ZimsDir.MakeLower();
#endif

	// Fill listboxes with all drives and all dirs
#ifdef VELMAN_UNIX
#else
	CComboBox *pCB=(CComboBox *)GetDlgItem(IDC_COMBO_NEWPROJDRIVE2);
	pCB->Dir(0x4000 + 0x8000, "*.*");

	// choose actual drive
	entry="[-";
	entry+=(char)tolower(drive);
	entry+="-]";
	pCB->SetCurSel(pCB->FindString(-1, entry));
#endif
	// This line commented out as Dir only returns 8.3 filenames.
	//pLB->Dir(0x0010 + 0x8000, ZimsDir+"*.*");
	DoFillDir();

	// old code commented out when static text on proj2 changed to editable text.
	// output target path in static text element
	//	CWnd *pW=GetDlgItem(IDC_TARGET_PATH_TEXT2);
	//	pW->SetWindowText(ZimsDir);
	GetDlgItem(IDC_NEWPROJ2_EDITDIR)->SetWindowText(ZimsDir);
	DisplayGridNamesPreview();

	// choose a grid filter
	((CComboBox *)GetDlgItem(IDC_NEWPROJ_GRIDFILETYPE))->SetCurSel(pDoc->GridFileType-1);

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

	//LR
	GetDlgItem (IDCANCEL)->GetWindowRect(&component);
	border_LR_IDCANCEL=form.right-component.left;

	//RR
	GetDlgItem (IDC_NEWPROJ2_EDITDIR)->GetWindowRect(&component);
	border_RR_IDC_NEWPROJ2_EDITDIR=form.right-component.right;

	//LR
	GetDlgItem (IDC_STATIC4)->GetWindowRect(&component);
	border_LR_IDC_STATIC4=form.right-component.left;

	// linked
	GetDlgItem (IDC_LIST_NEWPROJDIR2)->GetWindowRect(&component);
	borderBB_IDC_LIST_NEWPROJDIR2=form.bottom-component.bottom;
	width_IDC_LIST_NEWPROJDIR2=component.right - component.left;

	GetDlgItem (IDC_NEWPROJ_GRIDNAMESPREVIEW)->GetWindowRect(&component);
	borderBB_IDC_NEWPROJ_GRIDNAMESPREVIEW=form.bottom-component.bottom;
	borderLL_IDC_NEWPROJ_GRIDNAMESPREVIEW=component.left - form.left;
	width_IDC_NEWPROJ_GRIDNAMESPREVIEW=component.right - component.left;

	// cleanup code
	initialised=TRUE;

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CNewProjStep2::OnDblclkListNewprojdir2()
{
	// Double click in directory list changes into that directory
	CListBox *pLB=(CListBox *)GetDlgItem(IDC_LIST_NEWPROJDIR2);
	CString s;
	int i = 0;

	pLB->GetText(pLB->GetCurSel(), s);
	// no longer put brackets around dir names.
	//s=s.Mid(1, s.GetLength()-2);			// strip leading and trailing bracket
	if(s=="..")
	{
		// remove everything up to and including the last but one \ in ZimsDir
		ZimsDir=ZimsDir.Left(ZimsDir.GetLength()-1);
#ifdef VELMAN_UNIX
		i=ZimsDir.ReverseFind('/');
#else
		i=ZimsDir.ReverseFind('\\');
#endif
		if(i>0)
			ZimsDir=ZimsDir.Left(i+1);
	}
	else
	{
		ZimsDir+=s;
#ifdef VELMAN_UNIX
		ZimsDir+='/';
#else
		ZimsDir+='\\';
#endif
	}
	// old code with 8.3 filename problem
	//pLB->ResetContent();
	//pLB->Dir(0x0010 + 0x8000, ZimsDir+ALL_FILES);
	DoFillDir();

	// old code commented out when static text on proj2 changed to editable text.
	//	CWnd *pW=GetDlgItem(IDC_TARGET_PATH_TEXT2);
	//	pW->SetWindowText(ZimsDir);
	GetDlgItem(IDC_NEWPROJ2_EDITDIR)->SetWindowText(ZimsDir);
	DisplayGridNamesPreview();
}

void CNewProjStep2::OnBnClickedButton1()
{
	int i = 0;
	CString temp = ZimsDir;

	// remove everything up to and including the last but one \ in TargetDir
	temp=temp.Left(temp.GetLength()-1);
#ifdef VELMAN_UNIX
	i=temp.ReverseFind('/');
#else
	i=temp.ReverseFind('\\');
#endif
	if(i>=0)
	{
		ZimsDir = temp;

		ZimsDir=ZimsDir.Left(i+1);
		DoFillDir();
		GetDlgItem(IDC_NEWPROJ2_EDITDIR)->SetWindowText(ZimsDir);
		DisplayGridNamesPreview();
	}
}

void CNewProjStep2::DoFillDir()
{
	// Simple function which takes a Target Directory
	// and fills the listbox with directories.
	// Replaces the original code which used Dir() calls as
	// Dir only returns 8.3 filenames. Note we also scrap the
	// [] brackets around the dirnames.

	CListBox *pLB=(CListBox *)GetDlgItem(IDC_LIST_NEWPROJDIR2);
	pLB->ResetContent();
	// don't need this ifdef code any more. Just use same code for both
	// unix and pc cases and use the ALL_FILES macro. Can't use Dir
	// for just Unix as Dir seems to give directories surrounded by [].

	//#ifdef VELMAN_UNIX
	// Strictly Dir is the wrong way to do this but MainWin seems to
	// cope and returns long filenames.
	//	pLB->Dir(0x0010 + 0x8000, ZimsDir+"*");
	//#else
	CFileFind finder;
	finder.FindFile(ZimsDir+ALL_FILES, 0);
	BOOL bgo = TRUE;
	while (bgo)
	{
		bgo = finder.FindNextFile();
		CString fname = finder.GetFileName();
		//... test for directory and send string to appropriate window
		if ( finder.IsDirectory() && fname != "." )
			pLB->AddString( fname );
	}
	finder.Close();
	//#endif
}

void CNewProjStep2::DisplayGridNamesPreview()
{
	CListBox *pLB=(CListBox *)GetDlgItem(IDC_NEWPROJ_GRIDNAMESPREVIEW);
	CStringArray zimsnames;
	
	pLB->ResetContent();
	pDoc->ZimsDir=ZimsDir;
	pDoc->FillListOfDOSnames();

	pDoc->seismicdata.MakeZimsNameList(zimsnames, pDoc->ListOfDOSnames, ZIMS_TYPE_GRID);
	for(int i=0;i<zimsnames.GetSize();i++)
	{
		pLB->AddString((const char *)(zimsnames.GetAt(i)));
	}
}

void CNewProjStep2::OnSelchangeNewprojGridfiletype()
{
	pDoc->GridFileType=((CComboBox *)GetDlgItem(IDC_NEWPROJ_GRIDFILETYPE))->GetCurSel()+1;
	DisplayGridNamesPreview();
}

void CNewProjStep2::OnOK()
{
	// we want to preserve the setting and offer it as a default next time
	AfxGetApp()->WriteProfileInt("Grid Import", "GridFileType", pDoc->GridFileType);
	CDialog::OnOK();
}

void CNewProjStep2::OnKillfocusNewproj2Editdir()
{
	// callback to update dir and file listboxes when edit dir loses focus.
	// Very similar to OnKillfocusNewproj1Editdir()
	// 14/11/99

	CString	NewDir, entry;
	static	CString	OldDir = "";
	char	drive;

	GetDlgItemText(IDC_NEWPROJ2_EDITDIR, NewDir);

	//  check to see if edit path box has changed value. If not then exit
	if ( NewDir == OldDir )
		return;
	OldDir = NewDir;

	//  make sure new dir specified is valid, if not exit.
	if ( _chdir((const char *)NewDir) )
	{
		AfxMessageBox("The directory you have specified is invalid"
			" and cannot be accessed.\nCheck the spelling, permissions"
			" or create a new directory." );
		return;
	}

	//  update the list boxes
	ZimsDir = NewDir;

	// the following code is copied and marginally different from InitDialog
#ifdef VELMAN_UNIX
	if(ZimsDir.Right(1)!='/')
		ZimsDir+='/';
#else
	if(ZimsDir[1]==':')
		drive=ZimsDir[0];
	else
	{
		drive='C';
		ZimsDir="C:"+ZimsDir;
	}
	if(ZimsDir.Right(1)!='\\')
		ZimsDir+='\\';
	ZimsDir.MakeLower();
#endif

	// Fill listboxes with all drives and all dirs
#ifdef VELMAN_UNIX
#else
	CComboBox *pCB=(CComboBox *)GetDlgItem(IDC_COMBO_NEWPROJDRIVE2);
	pCB->Dir(0x4000 + 0x8000, "*.*");

	// choose actual drive
	entry="[-";
	entry+=(char)tolower(drive);
	entry+="-]";
	pCB->SetCurSel(pCB->FindString(-1, entry));
#endif
	DoFillDir();

	GetDlgItem(IDC_NEWPROJ2_EDITDIR)->SetWindowText(ZimsDir);
	DisplayGridNamesPreview();

	// choose a grid filter
	((CComboBox *)GetDlgItem(IDC_NEWPROJ_GRIDFILETYPE))->SetCurSel(pDoc->GridFileType-1);

}

void CNewProjStep2::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if (initialised)
	{
		RECT form;
		RECT component;

		int top, left, width, height, deltaWidth;

		this->GetWindowRect(&form);
		// reposition/ size the components

		//LR
		GetDlgItem(IDOK)->GetWindowRect(&component);
		left=form.right - form.left- border_LR_IDOK-4;
		top=component.top-form.top-CMainFrame::GetConfigTitleBarThickness();
		GetDlgItem(IDOK)->SetWindowPos(&wndTop, left, top, 0, 0, SWP_NOSIZE);

		//LR
		GetDlgItem(IDCANCEL)->GetWindowRect(&component);
		left=form.right - form.left- border_LR_IDCANCEL-4;
		top=component.top-form.top-CMainFrame::GetConfigTitleBarThickness();
		GetDlgItem(IDCANCEL)->SetWindowPos(&wndTop, left, top, 0, 0, SWP_NOSIZE);

		//RR
		GetDlgItem(IDC_NEWPROJ2_EDITDIR)->GetWindowRect(&component);
		height=component.bottom - component.top;
		width=form.right-component.left-border_RR_IDC_NEWPROJ2_EDITDIR;
		GetDlgItem(IDC_NEWPROJ2_EDITDIR)->SetWindowPos(&wndTop, 0, 0, width, height, SWP_NOMOVE);

		//Linked boxes
		GetDlgItem (IDC_LIST_NEWPROJDIR2)->GetWindowRect(&component);
		height=form.bottom-borderBB_IDC_LIST_NEWPROJDIR2-component.top;
		deltaWidth=(int)(((form.right-form.left)-formWidth)/2.0);
		width=width_IDC_LIST_NEWPROJDIR2+deltaWidth;
		GetDlgItem(IDC_LIST_NEWPROJDIR2)->SetWindowPos(&wndTop, 0, 0, width, height, SWP_NOMOVE);

		GetDlgItem (IDC_NEWPROJ_GRIDNAMESPREVIEW)->GetWindowRect(&component);
		left=borderLL_IDC_NEWPROJ_GRIDNAMESPREVIEW+deltaWidth-4;
		top=component.top-form.top-CMainFrame::GetConfigTitleBarThickness();
		GetDlgItem(IDC_NEWPROJ_GRIDNAMESPREVIEW)->SetWindowPos(&wndTop, left, top, 0, 0, SWP_NOSIZE);

		height=form.bottom-borderBB_IDC_NEWPROJ_GRIDNAMESPREVIEW-component.top;
		width=width_IDC_NEWPROJ_GRIDNAMESPREVIEW+deltaWidth;
		GetDlgItem(IDC_NEWPROJ_GRIDNAMESPREVIEW)->SetWindowPos(&wndTop, 0, 0, width, height, SWP_NOMOVE);

		//LR
		GetDlgItem(IDC_STATIC4)->GetWindowRect(&component);
		//left=form.right - form.left- border_LR_IDC_STATIC4;
		top=component.top-form.top-CMainFrame::GetConfigTitleBarThickness();
		GetDlgItem(IDC_STATIC4)->SetWindowPos(&wndTop, left, top, 0, 0, SWP_NOSIZE);

		// cleanup variables and repaint form
		this->Invalidate();
	}

}