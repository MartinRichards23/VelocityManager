// WFCDlg.cpp : implementation file
//

#include "stdafx.h"
#include "velman.h"
#include "VelmanDoc.h"
#include "WellForecastDlg.h"
#include "mainfrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWellForecastDlg dialog

CWellForecastDlg::CWellForecastDlg(CWnd* pParent /*=NULL*/)
: CDialog(CWellForecastDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWellForecastDlg)
	m_x = 0.0;
	m_y = 0.0;
	//}}AFX_DATA_INIT
}

void CWellForecastDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWellForecastDlg)
	DDX_Text(pDX, IDC_WELLFC_X, m_x);
	DDX_Text(pDX, IDC_WELLFC_Y, m_y);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CWellForecastDlg, CDialog)
	//{{AFX_MSG_MAP(CWellForecastDlg)
	ON_BN_CLICKED(IDC_WELLFC_STD, OnWellfcStd)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_SAVE, &CWellForecastDlg::OnBnClickedSave)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWellForecastDlg message handlers

void CWellForecastDlg::OnWellfcStd()
{
	// unmark all entries, then select all primary time and depth grids
	int i, j;
	CListBox *pLB=(CListBox *)GetDlgItem(IDC_WELLFC_LIST);
	CZimsGrid *grid;

	pLB->SetSel(-1, FALSE);  // unselect all

	// now mark some
	for(i=0;i<pDoc->zimscube.GetSize();i++)
	{
		grid=(CZimsGrid *)(pDoc->zimscube.GetAt(i));
		// must be time or depth, and must be primary
		if(grid->GetID()==0 &&
			(grid->GetType()==CSD_TYPE_TIME || grid->GetType()==CSD_TYPE_DEPTH))
		{
			// find listbox index of grid with index i
			for(j=0;j<entries.GetSize();j++)
			{
				if(entries[j]>>8==i)
				{
					pLB->SetSel(entries[j] & 0xFF, TRUE);
					break;
				}
			}
		}
	}
}

BOOL CWellForecastDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CListBox *pLB=(CListBox *)GetDlgItem(IDC_WELLFC_LIST);

	// decide whether decimals or not
	if(AfxGetApp()->GetProfileInt("Debug", "WellForecastWithDecimals", 0)==1)
		strcpy(FormatString, "%s\t%-10.3f");
	else
		strcpy(FormatString, "%s\t%-10.0f");

	if(pDoc->zimscube.FillListboxWithGridnames(pLB, entries)==0)
	{
		AfxMessageBox("There are no grids to display", MB_OK | MB_ICONINFORMATION);
		OnCancel();		// give it up if there are no grids around
	}
	pLB->SetCurSel(1);
	pLB->SetTabStops(15);

	CEdit *pE=(CEdit *)GetDlgItem(IDC_WELLFC_RESULT);
	pE->SetTabStops(80);

	SetIcon (AfxGetApp()->LoadIcon(IDR_MAINFRAME), FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CWellForecastDlg::OnOK()
{
	// this does not close the dialog, but rather starts the calculation
	// and fills the "results" listbox

	CListBox *pLB=(CListBox *)GetDlgItem(IDC_WELLFC_LIST);
	CEdit *pE=(CEdit *)GetDlgItem(IDC_WELLFC_RESULT);
	CZimsGrid *grid;
	char buf[256];
	double value;
	int click[128], i, j;
	CString outbuf="";

	if(pLB->GetSelCount()==0)
	{
		pE->SetWindowText("(no grids selected)");
		return;
	}

	UpdateData();
	pLB->GetSelItems(128, click);

	sprintf(buf, "Well forecast for location (%7.0f,%7.0f):", m_x, m_y);
	pDoc->Log(buf, LOG_BRIEF);

	for(j=0;j<pLB->GetSelCount();j++)
	{
		for(i=0;i<entries.GetSize();i++)
		{
			if( ((entries[i])&0xFF) == click[j])
			{
				grid=(CZimsGrid *)(pDoc->zimscube.GetAt(entries[i]>>8));
				value=grid->Evaluate(m_x, m_y);
				if(grid->GetZnon(value))
					sprintf(buf, "%s\t?", (const char *)(grid->GetName()));
				else
					sprintf(buf, FormatString, (const char *)(grid->GetName()), value);
				pDoc->Log(buf, LOG_BRIEF);
				outbuf+=buf;
				outbuf+="\r\n";
				break;
			}
		}
	}
	pE->SetWindowText(outbuf);
}

void CWellForecastDlg::OnBnClickedSave()
{
	CListBox *pLB=(CListBox *)GetDlgItem(IDC_WELLFC_LIST);
	CEdit *pE=(CEdit *)GetDlgItem(IDC_WELLFC_RESULT);
	CZimsGrid *grid;
	char buf[256];
	double value;
	int click[128], i, j;
	CString filename="";
	CStdioFile f;

	if(pLB->GetSelCount()==0)
	{
		pE->SetWindowText("(no grids selected)");
		return;
	}

	UpdateData();

	//make filename
	sprintf(buf, "Well_forecast_(%7.0f,%7.0f)", m_x, m_y);
	filename = buf;
	//ensure doesn't exist already
	filename = CMainFrame::GetNewFileName(pDoc->GetExportDir(), filename, ".txt");
	//make full path
	filename = pDoc->GetExportDir() + filename;

	//open file
	if(!f.Open( (LPCTSTR)filename, CFile::modeCreate | CFile::modeReadWrite | CFile::typeText ))
	{
		pDoc->Error("Could not write file: " + filename, MSG_ERR);
		return;
	}

	pLB->GetSelItems(128, click);

	sprintf(buf, "Well forecast for location (%7.0f,%7.0f)", m_x, m_y);
	f.WriteString(buf);
	f.WriteString("\n");

	for(j=0;j<pLB->GetSelCount();j++)
	{
		for(i=0;i<entries.GetSize();i++)
		{
			if( ((entries[i])&0xFF) == click[j])
			{
				grid=(CZimsGrid *)(pDoc->zimscube.GetAt(entries[i]>>8));
				value=grid->Evaluate(m_x, m_y);
				if(grid->GetZnon(value))
					sprintf(buf, "%s\t?", (const char *)(grid->GetName()));
				else
					sprintf(buf, FormatString, (const char *)(grid->GetName()), value);

				f.WriteString(buf);
				f.WriteString("\n");
				break;
			}
		}
	}

	f.Close();
	AfxMessageBox("Saved file:\n\n" + filename, MB_OK | MB_ICONINFORMATION);
}

