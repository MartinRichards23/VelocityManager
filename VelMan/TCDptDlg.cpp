// TCDptDlg.cpp : implementation file

#include "stdafx.h"
#include "velman.h"
#include <afxcmn.h>
#include "VelmanDoc.h"
#include "TCDptDlg.h"
#include <direct.h>
#include "openfilebrowser.h"
#include "mainfrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTimeConvertDepthDlg dialog

CTimeConvertDepthDlg::CTimeConvertDepthDlg(CWnd* pParent /*=NULL*/)
: CDialog(CTimeConvertDepthDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTimeConvertDepthDlg)
	m_x = 0.0;
	m_y = 0.0;
	m_d = 0.0;
	m_name = _T("");
	//}}AFX_DATA_INIT
	EntryInDatabase=NULL;
}

void CTimeConvertDepthDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTimeConvertDepthDlg)
	DDX_Text(pDX, IDC_TCD_X, m_x);
	DDX_Text(pDX, IDC_TCD_Y, m_y);
	DDX_Text(pDX, IDC_TCD_D, m_d);
	DDX_Text(pDX, IDC_TCD_NAME, m_name);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTimeConvertDepthDlg, CDialog)
	//{{AFX_MSG_MAP(CTimeConvertDepthDlg)
	ON_BN_CLICKED(IDC_TCD_READFROMFILE, OnTcdReadfromfile)
	ON_BN_CLICKED(IDC_TCD_WRITETOFILE, OnTcdWritetofile)
	ON_BN_CLICKED(IDC_TCD_TIMECONV, OnTcdTimeconv)
	ON_BN_CLICKED(IDC_TCD_DELETE, OnTcdDelete)
	ON_BN_CLICKED(IDC_TCD_DELETEALL, OnTcdDeleteall)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTimeConvertDepthDlg message handlers

BOOL CTimeConvertDepthDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CListBox *pL=(CListBox *)GetDlgItem(IDC_TCD_LIST);
	int tabs[]  ={ 50, 100, 150 };
	pL->SetTabStops(3, tabs);

	wellpointlist=&(pDoc->wellpointlist);
	EntryInDatabase=NULL;
	FillList();

	SetIcon (AfxGetApp()->LoadIcon(IDR_MAINFRAME), FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CTimeConvertDepthDlg::OnTcdReadfromfile()
{
	CString chosenFile;

#ifndef VELMAN_UNIX
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, TEXT_ALL_FILES);
	dlg.m_ofn.lpstrTitle="Specify single well data file";
	dlg.m_ofn.Flags&=(~OFN_SHOWHELP);

	// Set initial dir for dialogue.
	dlg.m_ofn.lpstrInitialDir = (LPCTSTR)pDoc->TargetDir;
	if(dlg.DoModal()==IDCANCEL)
		return;

	chosenFile=dlg.GetPathName();
#else
	// on a unix platform
	COpenFileBrowser dlg_new;
	//	dlg_new.setFilePath(TargetDir);
	dlg_new.title="Specify single well data file";
	if (dlg_new.DoModal()==IDOK)
	{
		chosenFile=dlg_new.m_shFilePath+dlg_new.m_shFileName;
	}
	else
		return;
#endif

	CWaitCursor wait;

	if(pDoc->wellpointlist.ReadFromFile((const char *)chosenFile)>0)
		FillList();
}

void CTimeConvertDepthDlg::OnTcdWritetofile()
{
	CString filename = "Time convert output";
	filename = CMainFrame::GetNewFileName(pDoc->GetExportDir(), filename, ".txt");
	filename = pDoc->GetExportDir() + filename;

	CWaitCursor wait;

	pDoc->wellpointlist.WriteToFile((const char *)filename);

	AfxMessageBox("Saved file:\n\n" + filename, MB_OK | MB_ICONINFORMATION);
}

void CTimeConvertDepthDlg::FillList()
{
	CWellPoint *wellpt;
	int i, entry, itemno=0;
	CListBox *pL=(CListBox *)GetDlgItem(IDC_TCD_LIST);
	CString lastname="";  // we display all entries for one well below one another
	char buf[256], buf2[128];

	pL->ResetContent();
	delete EntryInDatabase;
	EntryInDatabase=new int[1+2*(wellpointlist->GetSize())];

	entry=0;
	pL->AddString("x\ty\tt [2-way msec]\td");
	EntryInDatabase[entry++]=-1;

	for(i=0;i<wellpointlist->GetSize();i++)
	{
		wellpt=(CWellPoint *)(wellpointlist->GetAt(i));
		if(wellpt->status & WLPT_STATUS_INVALID)
			continue;
		// make a special list entry with just the name? Only if the name is new
		// and there is a name at all!
		if(wellpt->name!="" && wellpt->name !=lastname)
		{
			lastname=wellpt->name;
			pL->AddString((const char *)(lastname+":"));
			EntryInDatabase[entry++]=-1;
		}
		EntryInDatabase[entry++]=i; // used for deletion: entry>i if we have  inserted
		// lines containing only well names
		sprintf(buf, "%-7.0f\t%-7.0f\t", wellpt->x, wellpt->y);
		// attempt to compute time value if necessary
		if(!(wellpt->status & WLPT_STATUS_HAVETIME))
			wellpt->TimeConvert(wellpointlist, &(pDoc->zimscube));
		if(wellpt->status & WLPT_STATUS_HAVETIME)
			sprintf(buf2, "%-7.0f", wellpt->t);
		else
			sprintf(buf2, "?");
		strcat(buf, buf2);
		sprintf(buf2, "\t%-7.0f", wellpt->d);
		strcat(buf, buf2);
		pL->AddString(buf);
		itemno++;
	}
}

void CTimeConvertDepthDlg::OnTcdTimeconv()
{
	// read out x, y, depht entries and possibly well name, add it to database
	// and time convert
	CWellPoint *wellpt;

	UpdateData();
	if(m_x!=0 && m_y!=0 && m_d!=0)
	{
		wellpt=new CWellPoint(m_x, m_y, m_d, m_name);
		wellpointlist->Add(wellpt);
		wellpointlist->SortByName();
		SetDlgItemText(IDC_TCD_D, "0");
		GotoDlgCtrl(GetDlgItem(IDC_TCD_D));
		FillList();
	}
	else
		AfxMessageBox("Illegal data entered -- no time conversion.");
}

void CTimeConvertDepthDlg::OnTcdDelete()
{
	// delete the current entry from the listbox from the database
	int index, todelete;

	CListBox *pL=(CListBox *)GetDlgItem(IDC_TCD_LIST);
	todelete=pL->GetCurSel();
	if(todelete>=0 && (index=EntryInDatabase[todelete])>=0)
	{
		wellpointlist->RemoveAt(index);
		FillList();
	}
}

void CTimeConvertDepthDlg::OnTcdDeleteall()
{
	wellpointlist->RemoveAll();
	FillList();
}
