#include "stdafx.h"
#include "velman.h"
#include "VelmanDoc.h"
#include "WellhorzDlg.h"
#include "MapErrorDlg.h"
#include <afxcmn.h>
#include <dos.h>
#include "mainfrm.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWellhorzTimegridMapDlg dialog

CWellhorzTimegridMapDlg::CWellhorzTimegridMapDlg(CWnd* pParent /*=NULL*/)
: CDialog(CWellhorzTimegridMapDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWellhorzTimegridMapDlg)
	//}}AFX_DATA_INIT
}

void CWellhorzTimegridMapDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWellhorzTimegridMapDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CWellhorzTimegridMapDlg, CDialog)
	//{{AFX_MSG_MAP(CWellhorzTimegridMapDlg)
	ON_LBN_DBLCLK(IDC_WELLTIME_GRIDLIST, OnDblclkWelltimeGridlist)
	ON_BN_CLICKED(IDC_WELLTIME_DELETEENTRY, OnDeleteMapentry)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_WELLTIME_USEFAULTS, OnWelltimeUsefaults)
	ON_BN_CLICKED(IDC_WELLTIME_ASSIGN, OnDblclkWelltimeGridlist)
	ON_LBN_DBLCLK(IDC_WELLTIME_FAULTGRIDLIST, OnDblclkWelltimeGridlist)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWellhorzTimegridMapDlg message handlers

void CWellhorzTimegridMapDlg::OnDblclkWelltimeGridlist()
{
	// Is also called if "Assign" button is clicked, but double-clicking the
	// grid file list entry is more convenient
	// We add another entry into the "Assignments so far" listbox
	// afterwards, the next well is selected in the "wells" listbox, for ease
	// of use!

	CString entry, string;

	CListBox *pLB=(CListBox *)GetDlgItem(IDC_WELLTIME_WELLLIST);
	if(pLB->GetCurSel()!=LB_ERR)
	{
		pLB->GetText(pLB->GetCurSel(), entry);
		entry+=" -> ";
		pLB->SetCurSel(pLB->GetCurSel()+1);

		pLB=(CListBox *)GetDlgItem(IDC_WELLTIME_GRIDLIST);
		if(pLB->GetCurSel()!=LB_ERR)
		{
			pLB->GetText(pLB->GetCurSel(), string);
			entry+=string;

			if(usefaults)
			{
				pLB=(CListBox *)GetDlgItem(IDC_WELLTIME_FAULTGRIDLIST);
				// this code is for single selection listbox
				if(pLB->GetCurSel()>0)// not -1 and not 0 for "none"
				{
					pLB->GetText(pLB->GetCurSel(), string);
					entry+=" <- ";
					entry+=string;
				}
				// this code is for multiple selection listbox. For EV
				// files we originally wanted velman to use several fault files
				// per time grid. To do that, you need to make the fault file listbox
				// capable of multiple selections. In which case, comment out the code
				// above and uncomment the code below. I abandoned this approach however
				// because too much code in velman assumes that there is only ever one
				// faultgrid per timegrid. Code link the grid assign, which copies
				// properties, only copies a single fault grid. In the interests of time,
				// it was decided not to change all the code necessary to enable velman
				// to handle multiple fault files per grid. But I've left this code here
				// in case it's needed at some point in the future.
				//int faults[64], nfaults, i;
				//pLB->GetSelItems(64, faults);
				//nfaults = pLB->GetSelCount();
				//for ( i=0; i < nfaults; i++ )
				//{
				// pLB->GetText(faults[i], string);
				// entry = entry + " <- ";
				// entry = entry + string;
				//}
			}

			pLB=(CListBox *)GetDlgItem(IDC_WELLTIME_ASSIGNLIST);
			pLB->AddString(entry);
		}
	}
	// mark "none" as default for next fault grid
	if(usefaults)
		((CListBox *)GetDlgItem(IDC_WELLTIME_FAULTGRIDLIST))->SetCurSel(0);
}

void CWellhorzTimegridMapDlg::OnDeleteMapentry()
{
	// if there is an entry selected in the "Assignments so far" listbox, this
	// button removes it.

	int i = 0;

	CListBox *pLB=(CListBox *)GetDlgItem(IDC_WELLTIME_ASSIGNLIST);
	if((i=pLB->GetCurSel())!=LB_ERR)
		pLB->DeleteString(i);
}

BOOL CWellhorzTimegridMapDlg::OnInitDialog()
{
	int i = 0;
	CHorizon *horizon;

	// the default font size for the "small" fonts is different under Win95 and Unix
	// on top of that you can change it via the INI file
	int smallfontsize;
#ifdef VELMAN_UNIX
#define DEFAULT_SMALLFONTSIZE 8
#else
#define DEFAULT_SMALLFONTSIZE 7
#endif
	smallfontsize=AfxGetApp()->GetProfileInt("Attributes", "GridListFontSize", DEFAULT_SMALLFONTSIZE);
	smallFont.CreatePointFont(10*smallfontsize, "Arial");
	CDialog::OnInitDialog();

	// Fill listbox with horizon names and select first entry
	CListBox *pLB=(CListBox *)GetDlgItem(IDC_WELLTIME_WELLLIST);
	int hornumber=pDoc->horizonlist.GetSize();
	numlayers=hornumber; // remember for later use (see OnOK)
	for(i=0;i<hornumber;i++)
	{
		horizon = (CHorizon *) pDoc->horizonlist.GetAt(i);
		pLB->AddString((const char *)horizon->name);
	}
	pLB->SetCurSel(0);
	pLB->SetFont(&smallFont);

	// fill other listbox with list of grid names
	pLB=(CListBox *)GetDlgItem(IDC_WELLTIME_GRIDLIST);
	pLB->SetFont(&smallFont);

	// to do this, we need a list of all the DOS files in the Z-MAP directory ending with .?H
	pDoc->FillListOfDOSnames();
	// now create a list of ZIMS grid names from that list of DOS names
	CStringArray zimsnames;
	pDoc->seismicdata.MakeZimsNameList(zimsnames, pDoc->ListOfDOSnames, ZIMS_TYPE_GRID);
	for(i=0;i<zimsnames.GetSize();i++)
		pLB->AddString((const char *)(zimsnames.GetAt(i)));

	// now create a list of fault grid names from that list of DOS names
	pLB=(CListBox *)GetDlgItem(IDC_WELLTIME_FAULTGRIDLIST);
	pLB->SetFont(&smallFont);
	zimsnames.RemoveAll();

	pDoc->seismicdata.MakeZimsNameList(zimsnames, pDoc->ListOfDOSnames, ZIMS_TYPE_POLY);
	pLB->AddString("(none)");
	for(i=0;i<zimsnames.GetSize();i++)
		pLB->AddString((const char *)(zimsnames.GetAt(i)));

	// font for assignment list
	GetDlgItem(IDC_WELLTIME_ASSIGNLIST)->SetFont(&smallFont);

	// "no fault information" as a default
	usefaults=FALSE;
	GrayUngray();

	return TRUE; // return TRUE unless you set the focus to a control
}

void CWellhorzTimegridMapDlg::OnOK()
{
	// Now we have to check the assignments for possible errors:
	// -have all wells been assigned?
	// -has no well been assigned twice?
	// -has every well gotten a different time grid?

	int i, j;
	CStringArray assignleft, assignright, assignfault;
	CStringArray ErrList;
	char text[255];
	CString string;
	CMapErrorDlg errdlg;
	BOOL noerror=TRUE;
	char *p, *p2;

	// go through "Assignments" listbox and retrieve all entries, separating them
	// into left and right part
	CListBox *pLB=(CListBox *)GetDlgItem(IDC_WELLTIME_ASSIGNLIST);
	for(i=0;i<pLB->GetCount();i++)
	{
		pLB->GetText(i, text);
		p=strstr(text, " -> ");
		if(p!=NULL)
		{
			*p=0;
			assignleft.Add(text);
			// assignright only contains grid name, not the fault name bit
			p2=strstr(p+1, " <- ");
			if(p2!=NULL)
			{
				*p2=0; // mark a premature end of the string
				assignfault.Add(p2+strlen(" <- "));
			}
			else
				assignfault.Add("");
			assignright.Add(p+strlen(" -> "));
		}
	}

	// first check: Has no well been assigned twice? If that was the case, there
	// would be at least one element in the assignleft array that appears twice,
	// so go trough that array, and for every element check the entire list of
	// following elements for a double occurence.
	int numassigns=assignleft.GetSize();
	BOOL twice;

	for(i=0;i<numassigns-1;i++)
	{
		twice=FALSE;
		string=assignleft.GetAt(i);
		for(j=i+1;j<numassigns;j++)
		{
			if(assignleft.GetAt(j)==string)
			{
				twice=TRUE;
				break;
			}
		}
		if(twice)
		{
			ErrList.Add("· Well horizon \""+string+"\" has been assigned more than one grid.");
			noerror=FALSE;
		}
	}
	if(!noerror)
	{
		errdlg.errlistptr=&ErrList;
		errdlg.DoModal();
		return;
	}

	// second check:
	//ask to remove horizons that aren't assigned

	bool b= false;
	pLB=(CListBox *)GetDlgItem(IDC_WELLTIME_WELLLIST);
	for(i=pLB->GetCount()-1;i>=0;i--)
	{
		b = false;

		pLB->GetText(i, text);
		CString s = text;

		for(j=0;j<numassigns;j++)
		{
			CString r = assignleft.GetAt(j);
			if(r==s)
			{
				b = true;
				break;
			}
		}

		if(!b)
		{
			if(IDYES== AfxMessageBox("The following horizon was not assigned, would you like to continue without it? \n\n" + s, MB_YESNO | MB_ICONINFORMATION))
			{
				//remove from horizon list and the list box
				pDoc->horizonlist.RemoveAt(i);
				pLB->DeleteString(i);
			}
			else
			{
				return;
			}
		}
	}

	/*pLB=(CListBox*)GetDlgItem(IDC_WELLTIME_WELLLIST);
	if(numassigns<pLB->GetCount())
	{
		ErrList.Add("·Not all well horizons have been assigned time data grids.");
		errdlg.errlistptr=&ErrList;
		errdlg.DoModal();
		return;
	}*/

	//CHorizon *horizon = (CHorizon *) pDoc->horizonlist.GetAt(1);
	//pDoc->horizonlist.RemoveAt(3);

	// third check:
	// are there any double occurences in the right hand side of the assignments?
	for(i=0;i<numassigns-1;i++)
	{
		twice=FALSE;
		string=assignright.GetAt(i);
		for(j=i+1;j<numassigns;j++)
		{
			if(assignright.GetAt(j)==string)
			{
				twice=TRUE;
				break;
			}
		}
		if(twice)
		{
			ErrList.Add("· Grid \""+string+"\" has been assigned to more than one well horizon.");
			noerror=FALSE;
		}
	}
	if(!noerror)
	{
		errdlg.errlistptr=&ErrList;
		errdlg.DoModal();
		return;
	}

	// now that everything seems to have worked, store the grid file names going with each layer
	// in order of the horizons.
	ZimsGridNameList.SetSize(numassigns);
	FaultGridNameList.SetSize(numassigns);
	pLB=(CListBox *)GetDlgItem(IDC_WELLTIME_WELLLIST);
	for(i=0;i<numassigns;i++)
	{
		// go through list of wellnames. If we find the left hand side of the current assignment in the well
		// list, then the index of that entry is the horizonnumber, and we set the corresponding entry in the
		// ZimsGridNameList to the right hand side of the current assignment.
		j=pLB->FindString(-1, assignleft.GetAt(i));
		ZimsGridNameList.SetAt(j, assignright.GetAt(i));
		FaultGridNameList.SetAt(j, assignfault.GetAt(i));
	}

	for(i=0;i<numassigns;i++)
		TRACE("\n%d/%s/%s/", i, (const char *)ZimsGridNameList[i], (const char *)FaultGridNameList[i]);
	if(noerror)
		CDialog::OnOK();
}

void CWellhorzTimegridMapDlg::OnDestroy()
{
	CDialog::OnDestroy();

	smallFont.DeleteObject();
}

void CWellhorzTimegridMapDlg::GrayUngray()
{
	if(usefaults)
	{
		SetDlgItemText(IDC_WELLTIME_HELPTEXT, "To assign a time grid to a well:"
			"\n1) click the well name"
			"\n2) click the corresponding fault file name"
			"\n3) double-click the grid file name\n"
			"\nNote: New assignments appear in the list box.");
		GetDlgItem(IDC_WELLTIME_FAULTGRIDLIST)->EnableWindow(TRUE);
	}
	else
	{
		SetDlgItemText(IDC_WELLTIME_HELPTEXT, "To assign a time grid to a well:\n"
			"\n1) click the well name"
			"\n2) double-click the corresponding grid file name\n"
			"\nNote: New assignments appear in the list box.");
		GetDlgItem(IDC_WELLTIME_FAULTGRIDLIST)->EnableWindow(FALSE);
	}
}

void CWellhorzTimegridMapDlg::OnWelltimeUsefaults()
{
	usefaults=IsDlgButtonChecked(IDC_WELLTIME_USEFAULTS);
	GrayUngray();
}
