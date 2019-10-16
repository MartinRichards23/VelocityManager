// OpenFileBrowser.cpp : implementation file
#include "stdafx.h"
#include "velman.h"
#include "openfilebrowser.h"
#include "mainfrm.h"

#include "global.h"
#include "NewDirDlg.h"
#include <direct.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COpenFileBrowser dialog

COpenFileBrowser::COpenFileBrowser(CWnd* pParent /*=NULL*/)
: CDialog(COpenFileBrowser::IDD, pParent)
{
	//{{AFX_DATA_INIT(COpenFileBrowser)
	m_shFileName = _T("");
	m_shFilePath = _T("");
	//}}AFX_DATA_INIT
	extension=ALL_FILES;// defaults to all files
	initialised=FALSE;
	requestedDir="";// set default to nothing

}

void COpenFileBrowser::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COpenFileBrowser)
	DDX_Text(pDX, IDC_FILENAME_EDIT, m_shFileName);
	DDX_Text(pDX, IDC_NEWPROJ_EDIT_DIR, m_shFilePath);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(COpenFileBrowser, CDialog)
	//{{AFX_MSG_MAP(COpenFileBrowser)
	ON_LBN_DBLCLK(IDC_LIST_NEWPROJDIR, OnDblclkListNewprojdir)
	ON_CBN_SELCHANGE(IDC_COMBO_NEWPROJDRIVE, OnSelchangeComboNewprojdrive)
	ON_BN_CLICKED(IDC_NEWPROJ1_NEWDIR, OnNewproj1Newdir)
	ON_LBN_SELCHANGE(IDC_LIST_NEWPROJDIRFILES, OnSelchangeListNewprojdirfiles)
	ON_EN_CHANGE(IDC_FILENAME_EDIT, OnChangeFilenameEdit)
	ON_CBN_SELCHANGE(IDC_COMBO1, OnSelchangeCombo1)
	ON_BN_CLICKED(IDC_BUTTON2, OnButton2)
	ON_WM_SIZE()
	ON_EN_CHANGE(IDC_NEWPROJ_EDIT_DIR, OnChangeNewprojEditDir)
	ON_LBN_DBLCLK(IDC_LIST_NEWPROJDIRFILES, OnDblclkListNewprojdirfiles)
	ON_WM_KEYUP()
	ON_WM_KEYDOWN()
	ON_BN_CLICKED(IDOKnew, OnOKnew)
	ON_EN_SETFOCUS(IDC_NEWPROJ_EDIT_DIR, OnSetfocusNewprojEditDir)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COpenFileBrowser message handlers

BOOL COpenFileBrowser::OnInitDialog()
{
	CDialog::OnInitDialog();
	SetIcon (AfxGetApp()->LoadIcon(IDR_MAINFRAME), FALSE);
	CString entry, dir;

	SetWindowText(title);
	char drive;

	// try to select meaningful entries
	char current_dir[1024];

	SetExtensions();

#ifdef VELMAN_UNIX
	char *defdir = getenv("VELMAN_DEFAULTDIR");
	if ( defdir )
	{
		TargetDir = defdir;
	}
	if (requestedDir!="")// see if user has selected a starting dir
	{
		TargetDir=requestedDir;
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
	if (requestedDir!="")
	{
		TargetDir=requestedDir;
	}
#endif

#ifdef VELMAN_UNIX
	drive='+';		// just a dummy variable; never really used
	if(TargetDir.Right(1)!="/")
		TargetDir+='/';
#else
	if ( TargetDir == "" )	// Added this as I got bitten by it.
	{
		AfxMessageBox( "Velman Internal Error\nGetProfileString failed in"
			"NewProjStep1::OnInitDialog\nThis should not have happened.\n"
			"It may indicate a memory problem. Try restarting velman or your computer.\n"
			"Please contact CPS and report the problem stating the "
			"version number and circumstances.\n\n", MB_OK );
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
	CComboBox *pCB=(CComboBox *)GetDlgItem(IDC_COMBO_NEWPROJDRIVE);
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

	//stretch form code
	// this  is the first time that run so want to measure the distance constants
	RECT form;
	RECT component;
	this->GetWindowRect(&form);
	formWidth=form.right-form.left;
	// code to initialise variables

	//RR
	GetDlgItem (IDC_COMBO_NEWPROJDRIVE)->GetWindowRect(&component);
	border_RR_IDC_COMBO_NEWPROJDRIVE=form.right-component.right;

	//LR
	GetDlgItem (IDC_NEWPROJ1_NEWDIR)->GetWindowRect(&component);
	border_LR_IDC_NEWPROJ1_NEWDIR=form.right-component.left;

	//RR
	GetDlgItem (IDC_NEWPROJ_EDIT_DIR)->GetWindowRect(&component);
	border_RR_IDC_NEWPROJ_EDIT_DIR=form.right-component.right;

	//LR
	GetDlgItem (IDOKnew)->GetWindowRect(&component);
	border_LR_IDOK=form.right-component.left;

	//TB
	GetDlgItem (IDOKnew)->GetWindowRect(&component);
	border_TB_IDOK=form.bottom - component.top;

	//LR
	GetDlgItem (IDCANCEL)->GetWindowRect(&component);
	border_LR_IDCANCEL=form.right-component.left;

	//TB
	GetDlgItem (IDCANCEL)->GetWindowRect(&component);
	border_TB_IDCANCEL=form.bottom - component.top;

	//RR
	GetDlgItem (IDC_FILENAME_EDIT)->GetWindowRect(&component);
	border_RR_IDC_FILENAME_EDIT=form.right-component.right;

	//TB
	GetDlgItem (IDC_FILENAME_EDIT)->GetWindowRect(&component);
	border_TB_IDC_FILENAME_EDIT=form.bottom - component.top;

	//RR
	GetDlgItem (IDC_COMBO1)->GetWindowRect(&component);
	border_RR_IDC_COMBO1=form.right-component.right;

	//TB
	GetDlgItem (IDC_COMBO1)->GetWindowRect(&component);
	border_TB_IDC_COMBO1=form.bottom - component.top;

	//TB
	GetDlgItem (IDC_STATIC1)->GetWindowRect(&component);
	border_TB_IDC_STATIC1=form.bottom - component.top;

	//TB
	GetDlgItem (IDC_STATIC2)->GetWindowRect(&component);
	border_TB_IDC_STATIC2=form.bottom - component.top;

	// linked Horz
	GetDlgItem (IDC_LIST_NEWPROJDIR)->GetWindowRect(&component);
	width_IDC_LIST_NEWPROJDIR=component.right - component.left;

	GetDlgItem (IDC_LIST_NEWPROJDIRFILES)->GetWindowRect(&component);
	border_BB_IDC_LIST_NEWPROJDIRFILES=form.bottom-component.bottom;
	border_LL_IDC_LIST_NEWPROJDIRFILES=component.left - form.left;
	width_IDC_LIST_NEWPROJDIRFILES=component.right - component.left;

	initialised=TRUE;

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void COpenFileBrowser::DoFillListBox()
{
	// Simple function which takes a Target Directory
	// and fills the listboxes with directories and files.
	// Replaces the original code which used Dir() calls as
	// Dir only returns 8.3 filenames. Note we also scrap the
	// [] brackets around the dirnames.
	int locOfPeriod=0;

	CListBox *pLB=(CListBox *)GetDlgItem(IDC_LIST_NEWPROJDIR);
	CListBox *pLB2=(CListBox *)GetDlgItem(IDC_LIST_NEWPROJDIRFILES);
	pLB->ResetContent();
	pLB2->ResetContent();

	CFileFind finder;
	BOOL bgo;
	//	bgo=finder.FindFile(TargetDir+ALL_FILES, 0);// mod to original code
	CString fileExtToUse;
	CComboBox *extBox;
	extBox=(CComboBox *)GetDlgItem(IDC_COMBO1);

	if ((extBox->GetCurSel()==0)&&(extBox->GetCount()>1))// the following works on basis that only going to have
		// specified one type of file extension to use if not using
		// this then using *.* or unix equiv
		fileExtToUse=extension.Mid(2, extension.GetLength()-2);
	else
		fileExtToUse=ALL_FILES;

	bgo=finder.FindFile(TargetDir+ALL_FILES, 0);// mod to original code
	// if dir didnt have files in then would
	// try and do a find next and crash becuase
	// was setting bgo to FALSE to start with

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
			{
				// need to see if the file is of the correct extension
				if (fileExtToUse==ALL_FILES)
				{// if *.* then add the file automatically
					pLB2->AddString(fname);
				}
				else
				{
					locOfPeriod=stringFind(fname,'.', 0);
					if (locOfPeriod!=-1)// just a precaution
					{
						CString temp=fname.Right(fname.GetLength()-locOfPeriod-1);
						temp.MakeLower();
						fileExtToUse.MakeLower();
						if (temp==fileExtToUse)// only if the extension is the desired
							// one the will it be added to users list
							pLB2->AddString(fname);
					}
				}
			}
		}
	}
	finder.Close();
	//#endif
}

void COpenFileBrowser::ChangeToDir(CString s)
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

void COpenFileBrowser::OnDblclkListNewprojdir()
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

void COpenFileBrowser::OnSelchangeComboNewprojdrive()
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

void COpenFileBrowser::OnNewproj1Newdir()
{
	CCreateNewDirDlg dlg;
	CString completepath;

	dlg.BasePath=TargetDir;
	if(dlg.DoModal()==IDOKnew)
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

void COpenFileBrowser::OnSelchangeListNewprojdirfiles()
{
	CString FileName;
	CListBox *fileBox=(CListBox *)GetDlgItem(IDC_LIST_NEWPROJDIRFILES);
	int pos=fileBox->GetCurSel();

	fileBox->GetText(pos, FileName);
	GetDlgItem(IDC_FILENAME_EDIT)->SetWindowText(FileName);

}

void COpenFileBrowser::OnChangeFilenameEdit()
{
	CString text;
	CListBox *fileBox=(CListBox *)GetDlgItem(IDC_LIST_NEWPROJDIRFILES);

	GetDlgItem(IDC_FILENAME_EDIT)->GetWindowText(text);
	int pos=fileBox->SelectString(text.GetLength(), text);
	if (pos==-1)
	{
		fileBox->SetCurSel(-1);// deslect anything in box
	}
	GetDlgItem(IDOKnew)->EnableWindow(text!="");// disable ok button if no file name
}

void COpenFileBrowser::SetExtensions()
{// have used this funciton to fill in the file extensions
	// that the dialog box will allow
	// text is of format description, extension; description, extension etc...
	CComboBox *extBox;
	extBox=(CComboBox *)GetDlgItem(IDC_COMBO1);
	extBox->Clear();

	int startPos=0, endPos=0;
	do
	{// the following function will allow multiple extensions to be
		// sent however because current useage is only ever one extension
		// and "all files" this functionality is not taken any further
		// would need an array or vector to hold the actual extensions
		// that correspond to the different phrases describing them
		endPos=stringFind(extensionPhrase,',', startPos);
		if (endPos!=-1)
		{
			// have located a list separator

			extBox->AddString(extensionPhrase.Mid(startPos, endPos-startPos));
			startPos=endPos+1;

			endPos=stringFind(extensionPhrase,';', startPos);// this is needed if were to do multiple entrie
			// then would separate with colon
			if (endPos!=-1)
			{
				// have located a list separator
				extension=(extensionPhrase.Mid(startPos, endPos-startPos));
				startPos=endPos+1;
			}
			else // last entry so no colon
				extension=(extensionPhrase.Mid(startPos, extensionPhrase.GetLength()-startPos));
		}

	}while (endPos!=-1);
	// now add the "all files" section as this is present in all load dialogs
	extBox->AddString("All Files ("+(CString)ALL_FILES+")");
	extBox->SetCurSel(0);
}

void COpenFileBrowser::OnSelchangeCombo1()
{// user has changed the file extension
	DoFillListBox();// update the files shown
}

void COpenFileBrowser::OnButton2()
{
	ChangeToDir("..");
}

void COpenFileBrowser::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if (initialised)
	{
		RECT form;
		RECT component;

		int top, left, width, height, deltaWidth;

		this->GetWindowRect(&form);
		// reposition/ size the components

		//RR
		GetDlgItem(IDC_COMBO_NEWPROJDRIVE)->GetWindowRect(&component);
		height=component.bottom - component.top;
		width=form.right-component.left-border_RR_IDC_COMBO_NEWPROJDRIVE;
		GetDlgItem(IDC_COMBO_NEWPROJDRIVE)->SetWindowPos(&wndTop, 0, 0, width, height, SWP_NOMOVE);

		//LR
		GetDlgItem(IDC_NEWPROJ1_NEWDIR)->GetWindowRect(&component);
		left=form.right - form.left- border_LR_IDC_NEWPROJ1_NEWDIR-4;
		top=component.top-form.top-CMainFrame::GetConfigTitleBarThickness();
		GetDlgItem(IDC_NEWPROJ1_NEWDIR)->SetWindowPos(&wndTop, left, top, 0, 0, SWP_NOSIZE);

		//RR
		GetDlgItem(IDC_NEWPROJ_EDIT_DIR)->GetWindowRect(&component);
		height=component.bottom - component.top;
		width=form.right-component.left-border_RR_IDC_NEWPROJ_EDIT_DIR;
		GetDlgItem(IDC_NEWPROJ_EDIT_DIR)->SetWindowPos(&wndTop, 0, 0, width, height, SWP_NOMOVE);

		//LR
		GetDlgItem(IDOKnew)->GetWindowRect(&component);
		left=form.right - form.left- border_LR_IDOK-4;
		top=component.top-form.top-CMainFrame::GetConfigTitleBarThickness();
		GetDlgItem(IDOKnew)->SetWindowPos(&wndTop, left, top, 0, 0, SWP_NOSIZE);

		//TB
		GetDlgItem (IDOKnew)->GetWindowRect(&component);
		left=component.left - form.left-4;
		top=form.bottom-border_TB_IDOK-form.top - CMainFrame::GetConfigTitleBarThickness();
		GetDlgItem(IDOKnew)->SetWindowPos(&wndTop, left, top, 0, 0, SWP_NOSIZE);

		//LR
		GetDlgItem(IDCANCEL)->GetWindowRect(&component);
		left=form.right - form.left- border_LR_IDCANCEL-4;
		top=component.top-form.top-CMainFrame::GetConfigTitleBarThickness();
		GetDlgItem(IDCANCEL)->SetWindowPos(&wndTop, left, top, 0, 0, SWP_NOSIZE);

		//TB
		GetDlgItem (IDCANCEL)->GetWindowRect(&component);
		left=component.left - form.left-4;
		top=form.bottom-border_TB_IDCANCEL-form.top - CMainFrame::GetConfigTitleBarThickness();
		GetDlgItem(IDCANCEL)->SetWindowPos(&wndTop, left, top, 0, 0, SWP_NOSIZE);

		//RR
		GetDlgItem(IDC_FILENAME_EDIT)->GetWindowRect(&component);
		height=component.bottom - component.top;
		width=form.right-component.left-border_RR_IDC_FILENAME_EDIT;
		GetDlgItem(IDC_FILENAME_EDIT)->SetWindowPos(&wndTop, 0, 0, width, height, SWP_NOMOVE);

		//TB
		GetDlgItem (IDC_FILENAME_EDIT)->GetWindowRect(&component);
		left=component.left - form.left-4;
		top=form.bottom-border_TB_IDC_FILENAME_EDIT-form.top - CMainFrame::GetConfigTitleBarThickness();
		GetDlgItem(IDC_FILENAME_EDIT)->SetWindowPos(&wndTop, left, top, 0, 0, SWP_NOSIZE);

		//RR
		GetDlgItem(IDC_COMBO1)->GetWindowRect(&component);
		height=component.bottom - component.top;
		width=form.right-component.left-border_RR_IDC_COMBO1;
		GetDlgItem(IDC_COMBO1)->SetWindowPos(&wndTop, 0, 0, width, height, SWP_NOMOVE);

		//TB
		GetDlgItem (IDC_COMBO1)->GetWindowRect(&component);
		left=component.left - form.left-4;
		top=form.bottom-border_TB_IDC_COMBO1-form.top - CMainFrame::GetConfigTitleBarThickness();
		GetDlgItem(IDC_COMBO1)->SetWindowPos(&wndTop, left, top, 0, 0, SWP_NOSIZE);

		//TB
		GetDlgItem (IDC_STATIC1)->GetWindowRect(&component);
		left=component.left - form.left-4;
		top=form.bottom-border_TB_IDC_STATIC1-form.top - CMainFrame::GetConfigTitleBarThickness();
		GetDlgItem(IDC_STATIC1)->SetWindowPos(&wndTop, left, top, 0, 0, SWP_NOSIZE);

		//TB
		GetDlgItem (IDC_STATIC2)->GetWindowRect(&component);
		left=component.left - form.left-4;
		top=form.bottom-border_TB_IDC_STATIC2-form.top - CMainFrame::GetConfigTitleBarThickness();
		GetDlgItem(IDC_STATIC2)->SetWindowPos(&wndTop, left, top, 0, 0, SWP_NOSIZE);

		//Linked Horz
		GetDlgItem (IDC_LIST_NEWPROJDIR)->GetWindowRect(&component);
		height=form.bottom-border_BB_IDC_LIST_NEWPROJDIRFILES-component.top;
		deltaWidth=(int)(((form.right-form.left)-formWidth)/2.0);
		width=width_IDC_LIST_NEWPROJDIR+deltaWidth;
		GetDlgItem(IDC_LIST_NEWPROJDIR)->SetWindowPos(&wndTop, 0, 0, width, height, SWP_NOMOVE);

		GetDlgItem (IDC_LIST_NEWPROJDIRFILES)->GetWindowRect(&component);
		left=border_LL_IDC_LIST_NEWPROJDIRFILES+deltaWidth-4;
		top=component.top-form.top-CMainFrame::GetConfigTitleBarThickness();
		GetDlgItem(IDC_LIST_NEWPROJDIRFILES)->SetWindowPos(&wndTop, left, top, 0, 0, SWP_NOSIZE);

		height=form.bottom-border_BB_IDC_LIST_NEWPROJDIRFILES-component.top;
		width=width_IDC_LIST_NEWPROJDIRFILES+deltaWidth;
		GetDlgItem(IDC_LIST_NEWPROJDIRFILES)->SetWindowPos(&wndTop, 0, 0, width, height, SWP_NOMOVE);

		this->Invalidate();
	}

}

void COpenFileBrowser::OnChangeNewprojEditDir()
{
	CString temp;
	GetDlgItem(IDC_NEWPROJ_EDIT_DIR)->GetWindowText(temp);

	if ((temp.Right(1)=='/')||(temp.Right(1)=='\\'))
	{// user has terminated the line somehow
		// so process the line of text as a filepath

		/* lines no longer necessary because not auto appending \
		auto appending slash means that as type name in, will get the
		dir list boxes clering out untill get a path that works
		this looks bad!
		#ifdef VELMAN_UNIX
		if ((TargetDir.Right(1))!="/")
		{
		TargetDir+='/';
		}
		#else
		if ((TargetDir.Right(1))!="\\")
		{
		TargetDir+='\\';
		}
		#endif
		*/
		TargetDir=temp;
		DoFillListBox();
	}
}

void COpenFileBrowser::OnDblclkListNewprojdirfiles()
{
	// user has double clicked so want to choose the file and
	// close the dialog
	CString FileName;
	CListBox *fileBox=(CListBox *)GetDlgItem(IDC_LIST_NEWPROJDIRFILES);
	int pos=fileBox->GetCurSel();
	fileBox->GetText(pos, FileName);
	GetDlgItem(IDC_FILENAME_EDIT)->SetWindowText(FileName);
	CDialog::OnOK();
}

void COpenFileBrowser::OnOK()
{
	// do nothing !

}

void COpenFileBrowser::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CDialog::OnKeyUp(nChar, nRepCnt, nFlags);
}

void COpenFileBrowser::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CDialog::OnKeyDown(nChar, nRepCnt, nFlags);
}

void COpenFileBrowser::OnOKnew()
{
	// used this as intermediate to stop automatic seelction of the ok
	//button with the return key
	CDialog::OnOK();
}

void COpenFileBrowser::setFilePath(CString path)
{
	requestedDir=path;
}

void COpenFileBrowser::setTitle(CString title)
{
	SetWindowText(title);
}

int COpenFileBrowser::stringFind(CString haystack, char needle, int startPos)
{
	// function to replicate the behaviour of find(char, int)
	// looks through string for a specific character given
	// a starting pos and returns the posittion it finds it at
	// if not found then returns -1

	for (int i=startPos; i<haystack.GetLength();++i)
	{
		if (haystack[i]==needle)
			return i;
	}
	return -1;// no match so return -1
}

void COpenFileBrowser::OnSetfocusNewprojEditDir()
{
	GetDlgItem(IDC_SLASH_TEXT)->ShowWindow(1);
}

void COpenFileBrowser::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	// TODO: Add your message handler code here

	// Do not call CDialog::OnPaint() for painting messages
}
