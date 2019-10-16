#include "stdafx.h"
#include "velman.h"
#include "VelmanDoc.h"
#include "FiltManDlg.h"
#include "global.h"
#ifdef VELMAN_UNIX
#include <fstream.h>
#else
#include <fstream>
#endif
#include "openfilebrowser.h"
#include "mainfrm.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFilterManagementDlg dialog

CFilterManagementDlg::CFilterManagementDlg(CWnd* pParent /*=NULL*/)
: CDialog(CFilterManagementDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFilterManagementDlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CFilterManagementDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFilterManagementDlg)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

#define theDerivedClass CFilterManagementDlg ::
BEGIN_MESSAGE_MAP(CFilterManagementDlg, CDialog)
	//{{AFX_MSG_MAP(CFilterManagementDlg)
	ON_BN_CLICKED(IDC_ADDFILTER, OnAddfilter)
	ON_LBN_SELCHANGE(IDC_LISTOFINSTALLEDFILTERS, OnSelchangeListofinstalledfilters)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#undef theDerivedClass

/////////////////////////////////////////////////////////////////////////////
// CFilterManagementDlg message handlers

BOOL CFilterManagementDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	UpdateListbox();
	return TRUE;  // return TRUE  unless you set the focus to a control

	SetIcon (AfxGetApp()->LoadIcon(IDR_MAINFRAME), FALSE);
}

void CFilterManagementDlg::UpdateListbox()
{
	// fills the listbox with the names of the installed filters

	int err;
	CString name;
	CString type;
	ifstream filterfile;

	CListBox *pLB=(CListBox *)GetDlgItem(IDC_FILTERNAMELIST);
	pLB->ResetContent();
	FilterTypes.RemoveAll();

#ifdef VELMAN_UNIX
	filterfile.open(pDoc->GetFilterFileDir()+"/velman.flt");
#else
	filterfile.open(pDoc->GetFilterFileDir()+"\\VELMAN.FLT");
#endif

	do
	{
		name=pDoc->GetNextFiltername(err, &filterfile, &type);
		if(!err)
		{
			pLB->AddString(name);
			FilterTypes.Add(type);
		}
	} while(err==0);

	filterfile.close();
	pLB->SetCurSel(0);	// select first item; list box is just for display anyway
	OnSelchangeListofinstalledfilters();		// and explain what 1st filter does
}

void CFilterManagementDlg::OnAddfilter()
{
	// Add a new filter: Bring up a dialog box to pick a file out of which new
	// filters can come to replace old ones or add new ones to VELMAN.FLT

	CFileDialog addfiltdlg(TRUE, NULL, NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, TEXT_ALL_FILES);
	CString chosenFile;
	CString filterdir;
	ifstream filterfile, newfile;
	ofstream scratchpad;
	CString addedmsg, oneline, newname;
	CString oldline1, oldline2, currentname;
	char szBuf[256];
	BOOL doCopy, IsUpdate;
	int i, num, i1, i2;

#ifndef VELMAN_UNIX
	addfiltdlg.m_ofn.lpstrTitle="Specify the file from which to read new filters";
	addfiltdlg.m_ofn.Flags&=(~OFN_SHOWHELP);
	if(addfiltdlg.DoModal()==IDCANCEL)
		return;
	//	newfile.open(addfiltdlg.GetPathName());
	chosenFile=addfiltdlg.GetPathName();
#else
	COpenFileBrowser addfiltdlg_new;
	//	addfiltdlg_new.setFilePath(TargetDir);
	addfiltdlg_new.title="Specify the file from which to read new filters";
	if (addfiltdlg_new.DoModal()==IDOK)
	{
		chosenFile=addfiltdlg_new.m_shFilePath+addfiltdlg_new.m_shFileName;
	}
	else
		return;
#endif
	// open source file
	newfile.open(chosenFile);

	// open standard filter file
	filterdir=pDoc->GetFilterFileDir();

	// do a loop over all filters contained in the new file, creating a new internal
	// filter file every time
	while(!(newfile.fail() || newfile.eof()))
	{
#ifdef VELMAN_UNIX
		filterfile.open(filterdir+"/velman.flt");
		scratchpad.open(filterdir+"/SCRATCH.PAD");
#else
		filterfile.open(filterdir+"\\VELMAN.FLT");
		scratchpad.open(filterdir+"\\SCRATCH.PAD");
#endif
		newfile.getline(szBuf, 254);
		oneline=szBuf;		// oneline contains comment line of the new filter
		newfile.getline(szBuf, 254);	// now read filter description line
		newname=szBuf;
		if(newfile.fail() || newfile.eof())
			break; // maybe there was a blank line at the end of newfile, but now
		// we have surely reached the end of file

		IsUpdate=FALSE; // we assume it is a new filter, not a replacement

		// now copy entire old filterfile into the scratchpad. If we pass a filter with
		// the same description as the one that has to be added, we skip it and rather
		// add the new one at the end. Then we also know it's a replacement, not a new filter
		do
		{
			filterfile.getline(szBuf, 254);		// read comment line of next filter
			oldline1=szBuf;
			filterfile.getline(szBuf, 254);		// read description line of next filter
			oldline2=szBuf;
			currentname=oldline2;
			if(currentname==newname)	// if this is the filter to be added, ignore old version
			{
				doCopy=FALSE;
				IsUpdate=TRUE; // aha, we are replacing, not adding this filter
			}
			else
				doCopy=TRUE;

			if(filterfile.fail() || filterfile.eof())
				break; // done

			if(doCopy)
			{
				scratchpad.write((const char *)oldline1, oldline1.GetLength());
				scratchpad.write("\n", 1);
				scratchpad.write((const char *)oldline2, oldline2.GetLength());
				scratchpad.write("\n", 1);
			}
			filterfile.getline(szBuf, 254);	// type
			if(doCopy)
			{
				scratchpad.write(szBuf, strlen(szBuf));
				scratchpad.write("\n", 1);
			}
			filterfile.getline(szBuf, 254);	// number of following lines
			if(doCopy)
			{
				scratchpad.write(szBuf, strlen(szBuf));
				scratchpad.write("\n", 1);
			}
			num=atoi(szBuf);
			if(num<1)
			{
				pDoc->Error("The present filter file contains the invalid filter '"+currentname+
					"'.\n\nThis function is aborted now. You should re-install the original "
					"filter file, then attempt adding new filter(s) again.", MSG_ERR);
				return;
			}

			for(i=0;i<num;i++)   // copy all following lines
			{
				filterfile.getline(szBuf, 254);
				if(doCopy)
				{
					scratchpad.write(szBuf, strlen(szBuf));
					scratchpad.write("\n", 1);
				}
			}
		}	while(TRUE);

		// and now add the new filter description
		// the first two lines have already been read
		scratchpad.write((const char *)oneline, oneline.GetLength());
		scratchpad.write("\n", 1);
		scratchpad.write((const char *)newname, newname.GetLength());
		scratchpad.write("\n", 1);
		newfile.getline(szBuf, 254);	// type
		scratchpad.write(szBuf, strlen(szBuf));
		scratchpad.write("\n", 1);
		newfile.getline(szBuf, 254);	// number of following lines
		scratchpad.write(szBuf, strlen(szBuf));
		scratchpad.write("\n", 1);
		num=atoi(szBuf);
		if(num<1)
		{
			pDoc->Error("The file '"+chosenFile+
				"' contains the invalid filter '"+newname+
				"'.\n\nInstallation of filters aborted.", MSG_ERR);
			return;
		}
		for(i=0;i<num;i++)   // copy all following lines
		{
			newfile.getline(szBuf, 254);
			scratchpad.write(szBuf, strlen(szBuf));
			scratchpad.write("\n", 1);
		}
		scratchpad.close();
		filterfile.close();
		// now delete old filter file and rename scratchpad
#ifdef VELMAN_UNIX
		i1=unlink((const char *)(filterdir+"/velman.flt"));
		i2=rename((const char *)(filterdir+"/SCRATCH.PAD"),
			(const char *)(filterdir+"/velman.flt"));
#else
		i1=_unlink((const char *)(filterdir+"\\VELMAN.FLT"));
		i2=rename((const char *)(filterdir+"\\SCRATCH.PAD"),
			(const char *)(filterdir+"\\VELMAN.FLT"));
#endif
		if(i1 | i2)
		{
			if(IsUpdate)
				addedmsg="Problems updating filter '"+newname+"':\n";
			else
				addedmsg="Problems adding filter '"+newname+"':\n";
			if(i1)
				addedmsg+="· Could not delete old filter file\n";
			if(i2)
				addedmsg+="· Could not rename scratch pad file to replace old filter file\n";
			pDoc->Error(addedmsg, LOG_BRIEF);
		}
		else
		{
			if(IsUpdate)
				addedmsg="Updated filter '"+newname+"'.";
			else
				addedmsg="Added new filter '"+newname+"'.";
			AfxMessageBox(addedmsg);
			pDoc->Log(addedmsg, LOG_BRIEF);
		}
	} // end while() --  read next filter from newfile stream

	newfile.close();
	OnOK(); // we can close the complete dialog here
	return;
}

// If the user clicks on any filter in the list, it gets explained in the
// static text field below the listbox. Note that the order of entries in the
// listbox must be the same as the FilterTypes array, they are both
// created in UpdateListbox(), see above.

void CFilterManagementDlg::OnSelchangeListofinstalledfilters()
{
	CListBox *pLB=(CListBox *)GetDlgItem(IDC_FILTERNAMELIST);
	CString type;
	CString typestring;
	BOOL KnowAbout;
	int current=pLB->GetCurSel();
	if(current>=0 && current <= FilterTypes.GetUpperBound())
		type=FilterTypes.GetAt(current);
	else
		type="?";

	if(type=="")
		type="?";

	switch(type[0])
	{
	case 'R':
		typestring="row-orientated file; ";
		KnowAbout=TRUE;
		break;
	case 'C':
		typestring="column-orientated file; ";
		KnowAbout=TRUE;
		break;
	default:
		typestring="unknown";
		KnowAbout=FALSE;
	}
	if(KnowAbout)
	{
		switch(type[1])
		{
		case '1':
			typestring+="shotpoint coordinates present in file";
			break;
		case '2':
			typestring+="3d-data without coordinate information";
			break;
		case '3':
			typestring+="shotpoint coordinates are read from another file";
			break;
		}
	}
	SetDlgItemText(IDC_INFO_SELECTED_FILTER, typestring);
}