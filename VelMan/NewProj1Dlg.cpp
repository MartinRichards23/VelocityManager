#include "global.h"
#include "stdafx.h"
#include "velman.h"
#include "NewProj1Dlg.h"
#include "NewDirDlg.h"
#include <direct.h>
#include "mainfrm.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNewProjStep1 dialog

CNewProjStep1::CNewProjStep1(CWnd* pParent /*=NULL*/)
: CDialog(CNewProjStep1::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNewProjStep1)
	//}}AFX_DATA_INIT

	initialised=FALSE;
}

void CNewProjStep1::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNewProjStep1)
	//}}AFX_DATA_MAP
}

#define theDerivedClass CNewProjStep1 ::
BEGIN_MESSAGE_MAP(CNewProjStep1, CDialog)
	//{{AFX_MSG_MAP(CNewProjStep1)
	ON_LBN_DBLCLK(IDC_LIST_NEWPROJDIR, OnDblclkListNewprojdir)
	ON_CBN_SELCHANGE(IDC_COMBO_NEWPROJDRIVE, OnSelchangeComboNewprojdrive)
	ON_BN_CLICKED(IDC_NEWPROJ1_NEWDIR, OnNewproj1Newdir)
	ON_EN_KILLFOCUS(IDC_NEWPROJ_EDIT_DIR, OnKillfocusNewproj1EditDir)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP

	ON_BN_CLICKED(IDC_BUTTONUP, &CNewProjStep1::OnBnClickedButtonup)
END_MESSAGE_MAP()
#undef theDerivedClass

/////////////////////////////////////////////////////////////////////////////
// CNewProjStep1 message handlers

BOOL CNewProjStep1::OnInitDialog()
{
	CDialog::OnInitDialog();
	CString entry, dir;
	char drive;

	CFont *m_Font1 = new CFont;
	m_Font1->CreatePointFont(80, "Arial Bold");

	((CStatic *)GetDlgItem(IDC_STATIC))->SetFont(m_Font1);

	// try to select meaningful entries
	char current_dir[1024];

#ifdef VELMAN_UNIX
	char *defdir = getenv("VELMAN_DEFAULTDIR");
	if ( defdir )
	{
		TargetDir = defdir;
	}
	else
	{
		if(!getcwd(current_dir, 1023))
			strcpy(current_dir, "/");
		TargetDir=AfxGetApp()->GetProfileString("New Projects", "TargetDir", current_dir);
		// under Unix, we do not want a "Drive" selection box. Hide the combobox and
		// the accompaniying text
		GetDlgItem(IDC_COMBO_NEWPROJDRIVE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_COMBO_NEWPROJDRIVE_TEXT)->ShowWindow(SW_HIDE);
	}
#else
	if(!_getcwd(current_dir, 1023))
		strcpy(current_dir, "C:\\");
	TargetDir=AfxGetApp()->GetProfileString("New Projects", "TargetDir", current_dir);
#endif
	//buflen = 4096;
	//GetTempPath(buflen, lpTempPath);		// get path for temporary files

	// project units; default value from logfile
	m_Unitchoice=AfxGetApp()->GetProfileInt("New Projects", "OutputUnits", 0);

	CComboBox *pCB=(CComboBox *)GetDlgItem(IDC_NEWPROG_UNITS);
	pCB->SetCurSel(m_Unitchoice);
	// logfile detail level - default always "2"="detailed"

	((CComboBox *)GetDlgItem(IDC_NEWPROG_LOGDETAIL))->SetCurSel(2);

#ifdef VELMAN_UNIX
	drive='+';		// just a dummy variable; never really used
	if(TargetDir.Right(1)!="/")
		TargetDir+='/';
#else
	if ( TargetDir == "" )	// Added this as I got bitten by it.
	{
		drive='C';
		TargetDir="C:"+TargetDir;
	}
	else if(TargetDir.Mid(1, 1)==":")
	{
		drive=TargetDir[0];
	}
	else
	{
		drive='C';
		TargetDir="C:"+TargetDir;
	}
	if(TargetDir.Right(1)!='\\')
		TargetDir+='\\';
	TargetDir.MakeLower();
#endif
	DoFillListBox();	// puts dir and filenames in listboxes
#ifdef VELMAN_UNIX
#else
	// Win95: fill it with list of drives
	pCB=(CComboBox *)GetDlgItem(IDC_COMBO_NEWPROJDRIVE);
	pCB->Dir(0x4000 + 0x8000, "*.*");				// look for drives

	// choose actual drive
	entry="[-";
	entry+=(char)tolower(drive);
	entry+="-]";
	pCB->SetCurSel(pCB->FindString(-1, entry));
#endif

	// output target path in edit box
	GetDlgItem(IDC_NEWPROJ_EDIT_DIR)->SetWindowText(TargetDir);

	// set the focus to the first control (project dir)
	GotoDlgCtrl(GetDlgItem(IDC_LIST_NEWPROJDIR));

	// this  is the first time that run so want to measure the distance constants
	SetIcon (AfxGetApp()->LoadIcon(IDR_MAINFRAME), FALSE);

	RECT form;
	RECT component;
	this->GetWindowRect(&form);
	formWidth=form.right-form.left;

	//LR
	GetDlgItem (IDOK)->GetWindowRect(&component);
	borderLR_IDOK=form.right-component.left;

	//LR
	GetDlgItem (IDCANCEL)->GetWindowRect(&component);
	borderLR_IDCANCEL=form.right-component.left;

	//RR
	GetDlgItem (IDC_NEWPROJ_EDIT_DIR)->GetWindowRect(&component);
	borderRR_IDC_NEWPROJ_EDIT_DIR=form.right-component.right;
	////
	//RR BB
	GetDlgItem (IDC_LIST_NEWPROJDIR)->GetWindowRect(&component);
	borderBB_IDC_LIST_NEWPROJDIR=form.bottom-component.bottom;
	width_IDC_LIST_NEWPROJDIR=component.right - component.left;

	//LR BB
	GetDlgItem (IDC_LIST_NEWPROJDIRFILES)->GetWindowRect(&component);
	borderBB_IDC_LIST_NEWPROJDIRFILES=form.bottom-component.bottom;
	borderLL_IDC_LIST_NEWPROJDIRFILES=component.left - form.left;
	width_IDC_LIST_NEWPROJDIRFILES=component.right - component.left;
	//////
	//TB
	GetDlgItem (IDC_NEWPROG_LOGDETAIL)->GetWindowRect(&component);
	borderTB_IDC_NEWPROG_LOGDETAIL=form.bottom - component.top;

	//TB
	GetDlgItem (IDC_STATIC2)->GetWindowRect(&component);
	borderTB_IDC_STATIC2=form.bottom - component.top;

	//LR
	GetDlgItem (IDC_STATIC4)->GetWindowRect(&component);
	borderLR_IDC_STATIC3=form.right-component.left;

	//TB
	GetDlgItem (IDC_STATIC1)->GetWindowRect(&component);
	borderTB_IDC_STATIC1=form.bottom - component.top;

	//TB
	GetDlgItem (IDC_NEWPROG_UNITS)->GetWindowRect(&component);
	borderTB_IDC_NEWPROG_UNITS=form.bottom - component.top;

	initialised=TRUE;

	return FALSE;  // return TRUE  unless you set the focus to a control
}

void CNewProjStep1::DoFillListBox()
{
	// Simple function which takes a Target Directory
	// and fills the listboxes with directories and files.
	// Replaces the original code which used Dir() calls as
	// Dir only returns 8.3 filenames. Note we also scrap the
	// [] brackets around the dirnames.

	CListBox *pLB=(CListBox *)GetDlgItem(IDC_LIST_NEWPROJDIR);
	CListBox *pLB2=(CListBox *)GetDlgItem(IDC_LIST_NEWPROJDIRFILES);
	pLB->ResetContent();
	pLB2->ResetContent();

	CFileFind finder;
	finder.FindFile(TargetDir+ALL_FILES, 0);
	BOOL bgo = TRUE;
	while (bgo)
	{
		bgo = finder.FindNextFile();
		CString fname = finder.GetFileName();

		//... test for directory and send string to appropriate window
		if ( fname != "." )
		{
			if ( finder.IsDirectory() )
				pLB->AddString( fname );
			else
				pLB2->AddString( fname );
		}
	}
	finder.Close();
	//#endif
}

void CNewProjStep1::ChangeToDir(CString s)
{
	int i = 0;

	if(s=="..")
	{
		// remove everything up to and including the last but one \ in TargetDir
		TargetDir=TargetDir.Left(TargetDir.GetLength()-1);
#ifdef VELMAN_UNIX
		i=TargetDir.ReverseFind('/');
#else
		i=TargetDir.ReverseFind('\\');
#endif
		if(i>=0)
			TargetDir=TargetDir.Left(i+1);
	}
	else
	{
		TargetDir+=s;
#ifdef VELMAN_UNIX
		TargetDir+='/';
#else
		TargetDir+='\\';
#endif
	}
	DoFillListBox();
	GetDlgItem(IDC_NEWPROJ_EDIT_DIR)->SetWindowText(TargetDir);
}

void CNewProjStep1::OnBnClickedButtonup()
{
	int i = 0;
	CString temp = TargetDir;

	// remove everything up to and including the last but one \ in TargetDir
	temp=temp.Left(temp.GetLength()-1);
#ifdef VELMAN_UNIX
	i=temp.ReverseFind('/');
#else
	i=temp.ReverseFind('\\');
#endif
	if(i>=0)
	{
		TargetDir = temp;

		TargetDir=TargetDir.Left(i+1);
		DoFillListBox();
		GetDlgItem(IDC_NEWPROJ_EDIT_DIR)->SetWindowText(TargetDir);
	}
}

void CNewProjStep1::OnDblclkListNewprojdir()
{
	// Double click in directory list changes into that directory
	CListBox *pLB=(CListBox *)GetDlgItem(IDC_LIST_NEWPROJDIR);
	CString s;
	int index=pLB->GetCurSel();
	if(index<0)
		return;
	pLB->GetText(index, s);

	// don't enclose dirs in brackets any more as Dir() is not used
	// if it is not enclosed in [..]  it is a file name, not a directory name
	//if(s[0]!='[')
	//	return;
	// strip leading and trailing bracket
	//s=s.Mid(1, s.GetLength()-2);

	ChangeToDir(s);
}

void CNewProjStep1::OnSelchangeComboNewprojdrive()
{
	// User chose new drive => set new Target directory
	CString s;
	CComboBox *pCB=(CComboBox *)GetDlgItem(IDC_COMBO_NEWPROJDRIVE);

	pCB->GetLBText(pCB->GetCurSel(), s);

#ifdef VELMAN_UNIX
	TargetDir=s;
	TargetDir+="/";
#else
	s.MakeLower();
	TargetDir=s[2];
	TargetDir+=":\\";
#endif

	DoFillListBox();	// puts dir and filenames in listbox
	GetDlgItem(IDC_NEWPROJ_EDIT_DIR)->SetWindowText(TargetDir);
}

void CNewProjStep1::OnOK()
{
	// the "TargetDir" that we save in the INI file should be the one BELOW WHICH
	// one finds the actual project directory (because this target dir will be
	// one of these "c:\data\projects\" directories)

	GetDlgItemText(IDC_NEWPROJ_EDIT_DIR, TargetDir);

	// test whether it is an OK directory, or whether you cannot change to it
	if(_chdir((const char *)TargetDir))
	{
		AfxMessageBox("The specified project directory is invalid.\n\n"
			"Please check for spelling mistakes, or use the\n"
			" \"New Directory\" button if the directory you \nplan to use does not yet exist.");
		return;
	}

	CString SaveTargetDir=TargetDir;

#ifdef VELMAN_UNIX
	int i=TargetDir.ReverseFind('/');
#else
	int i=TargetDir.ReverseFind('\\');
#endif
	if(i==0)
		m_ProjTitle=TargetDir;
	else
	{
		m_ProjTitle=TargetDir.Left(i);
#ifdef VELMAN_UNIX
		int j=m_ProjTitle.ReverseFind('/');
		m_ProjTitle=TargetDir.Mid(j+1, i-j-1);
		SaveTargetDir=TargetDir.Left(j);
#else
		int j=m_ProjTitle.ReverseFind('\\');
		if(j==0)
			m_ProjTitle=TargetDir;
		else
		{
			m_ProjTitle=TargetDir.Mid(j+1, i-j-1);
			SaveTargetDir=TargetDir.Left(j);
		}
#endif
	}
	CComboBox *pCB=(CComboBox *)GetDlgItem(IDC_NEWPROG_UNITS);
	m_Unitchoice=pCB->GetCurSel();
	pCB=(CComboBox *)GetDlgItem(IDC_NEWPROG_LOGDETAIL);
	m_LogType=pCB->GetCurSel();
	AfxGetApp()->WriteProfileInt("New Projects", "OutputUnits", m_Unitchoice);
	AfxGetApp()->WriteProfileString("New Projects", "TargetDir", SaveTargetDir);
	CDialog::OnOK();
}

void CNewProjStep1::OnNewproj1Newdir()
{
	CCreateNewDirDlg dlg;
	CString completepath;

	dlg.BasePath=TargetDir;
	if(dlg.DoModal()==IDOK)
	{
		completepath=TargetDir;
#ifdef VELMAN_UNIX
		if(completepath.Right(1)!="/")
			completepath+='/';
#else
		if(completepath.Right(1)!="\\" && completepath.Right(1)!=":")
			completepath+='\\';
#endif
		completepath+=dlg.NewDir;
		if(_mkdir((const char *)completepath))
			AfxMessageBox("Error creating directory.");
		else
			ChangeToDir(dlg.NewDir);
	}
}

void CNewProjStep1::OnKillfocusNewproj1EditDir()
{	// To handle updating the file list box when path changes.
	// This callback is called whenever the focus moves out of the box. This
	// can happen by a mouse move or keyboard press. A TAB will work fine
	// however a RETURN will cause this to be called but also OnOK which will
	// exit the dialogue. This is normal windows behaviour. I could set a flag
	// to indicate that this fn was called but it would go against consistent
	// windows behaviour and I have seen the OnOK fn called before this one.
	// For now, live with the notion that a TAB or mouse click to another item
	// must be used.

	//AfxMessageBox( "Inside OnKIllfocusNewProj1EditDir\n\n", MB_OK );

	CString	NewDir, entry;
	static	CString	OldDir = "";
	char	drive;

	GetDlgItemText(IDC_NEWPROJ_EDIT_DIR, NewDir);

	// If the directory currently in the edit box hasn't changed we leave.
	if ( NewDir == OldDir )
		return;
	OldDir = NewDir;

	// make sure the new directory is there and we can get into it
	if (_chdir((const char *)NewDir) )
	{
		AfxMessageBox("The directory you have specified is invalid"
			" and cannot be accessed.\nCheck the spelling, permissions"
			" or create a new directory." );
		return;
	}

	//  update the list boxes
	TargetDir = NewDir;
	// next lines are copied from above. So much for code reuse! 8-)
	// check TargetDir has something in it, if not set default, watch for blank
	// TargetDir and change drive if the user's new path has changed the drive.
#ifdef VELMAN_UNIX
	if(TargetDir.Right(1)!="/")
		TargetDir+='/';
#else
	if(TargetDir.Mid(1, 1)==":")
		drive=TargetDir[0];
	else
	{
		drive='C';
		TargetDir="C:"+TargetDir;
	}
	if(TargetDir.Right(1)!='\\')
		TargetDir+='\\';
	TargetDir.MakeLower();
#endif

	DoFillListBox();

	//  reset edit box in case TargetDir has changed
	GetDlgItem(IDC_NEWPROJ_EDIT_DIR)->SetWindowText(TargetDir);

#ifdef VELMAN_UNIX
#else
	// Win95: fill it with list of drives
	CComboBox *pCB=(CComboBox *)GetDlgItem(IDC_COMBO_NEWPROJDRIVE);
	pCB->Dir(0x4000 + 0x8000, "*.*");				// look for drives

	// choose actual drive
	entry="[-";
	entry+=(char)tolower(drive);
	entry+="-]";
	pCB->SetCurSel(pCB->FindString(-1, entry));
#endif
}

void CNewProjStep1::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if (initialised)
	{
		RECT form;
		RECT component;

		int top, left, width, height, deltaWidth;

		this->GetWindowRect(&form);

		//LR
		GetDlgItem(IDCANCEL)->GetWindowRect(&component);
		left=form.right - form.left- borderLR_IDCANCEL-4;
		top=component.top-form.top-CMainFrame::GetConfigTitleBarThickness();
		GetDlgItem(IDCANCEL)->SetWindowPos(&wndTop, left, top, 0, 0, SWP_NOSIZE);

		//LR
		GetDlgItem(IDOK)->GetWindowRect(&component);
		left=form.right - form.left- borderLR_IDOK-4;
		top=component.top-form.top-CMainFrame::GetConfigTitleBarThickness();
		GetDlgItem(IDOK)->SetWindowPos(&wndTop, left, top, 0, 0, SWP_NOSIZE);

		//R
		GetDlgItem(IDC_NEWPROJ_EDIT_DIR)->GetWindowRect(&component);
		height=component.bottom - component.top;
		width=form.right-component.left-borderRR_IDC_NEWPROJ_EDIT_DIR;
		GetDlgItem(IDC_NEWPROJ_EDIT_DIR)->SetWindowPos(&wndTop, 0, 0, width, height, SWP_NOMOVE);

		//Linked boxes
		GetDlgItem (IDC_LIST_NEWPROJDIR)->GetWindowRect(&component);
		height=form.bottom-borderBB_IDC_LIST_NEWPROJDIR-component.top;
		deltaWidth=(int)(((form.right-form.left)-formWidth)/2.0);
		width=width_IDC_LIST_NEWPROJDIR+deltaWidth;
		GetDlgItem(IDC_LIST_NEWPROJDIR)->SetWindowPos(&wndTop, 0, 0, width, height, SWP_NOMOVE);

		GetDlgItem (IDC_LIST_NEWPROJDIRFILES)->GetWindowRect(&component);
		left=borderLL_IDC_LIST_NEWPROJDIRFILES+deltaWidth-4;
		top=component.top-form.top-CMainFrame::GetConfigTitleBarThickness();
		GetDlgItem(IDC_LIST_NEWPROJDIRFILES)->SetWindowPos(&wndTop, left, top, 0, 0, SWP_NOSIZE);

		height=form.bottom-borderBB_IDC_LIST_NEWPROJDIRFILES-component.top;
		width=width_IDC_LIST_NEWPROJDIRFILES+deltaWidth;
		GetDlgItem(IDC_LIST_NEWPROJDIRFILES)->SetWindowPos(&wndTop, 0, 0, width, height, SWP_NOMOVE);

		//L R
		GetDlgItem(IDC_STATIC4)->GetWindowRect(&component);
		//left=form.right - form.left- borderLR_IDC_STATIC3;
		top=component.top-form.top-CMainFrame::GetConfigTitleBarThickness();
		GetDlgItem(IDC_STATIC4)->SetWindowPos(&wndTop, left, top, 0, 0, SWP_NOSIZE);

		//T B
		GetDlgItem (IDC_NEWPROG_LOGDETAIL)->GetWindowRect(&component);
		left=component.left - form.left-4;
		top=form.bottom-borderTB_IDC_NEWPROG_LOGDETAIL-form.top - CMainFrame::GetConfigTitleBarThickness();
		GetDlgItem(IDC_NEWPROG_LOGDETAIL)->SetWindowPos(&wndTop, left, top, 0, 0, SWP_NOSIZE|SWP_NOZORDER);

		//T B
		GetDlgItem (IDC_STATIC2)->GetWindowRect(&component);
		left=component.left - form.left-4;
		top=form.bottom-borderTB_IDC_STATIC2-form.top - CMainFrame::GetConfigTitleBarThickness();
		GetDlgItem(IDC_STATIC2)->SetWindowPos(&wndTop, left, top, 0, 0, SWP_NOSIZE|SWP_NOZORDER);

		//T B
		GetDlgItem (IDC_STATIC1)->GetWindowRect(&component);
		left=component.left - form.left-4;
		top=form.bottom-borderTB_IDC_STATIC1-form.top - CMainFrame::GetConfigTitleBarThickness();
		GetDlgItem(IDC_STATIC1)->SetWindowPos(&wndTop, left, top, 0, 0, SWP_NOSIZE|SWP_NOZORDER);

		//T B
		GetDlgItem (IDC_NEWPROG_UNITS)->GetWindowRect(&component);
		left=component.left - form.left-4;
		top=form.bottom-borderTB_IDC_NEWPROG_UNITS-form.top - CMainFrame::GetConfigTitleBarThickness();
		GetDlgItem(IDC_NEWPROG_UNITS)->SetWindowPos(&wndTop, left, top, 0, 0, SWP_NOSIZE|SWP_NOZORDER);

		this->Invalidate();

	}
}