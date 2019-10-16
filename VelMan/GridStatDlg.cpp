// GStatDlg.cpp : implementation file
//

#include "stdafx.h"
#include "velman.h"
#include "VelmanDoc.h"
#include "GridStatDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGridStatisticsDlg dialog

CGridStatisticsDlg::CGridStatisticsDlg(CWnd* pParent /*=NULL*/)
: CDialog(CGridStatisticsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGridStatisticsDlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CGridStatisticsDlg::CGridStatisticsDlg(CVelmanDoc *arg) : CDialog()
{
	pDoc=arg;
}

void CGridStatisticsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGridStatisticsDlg)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BOOL CGridStatisticsDlg::Create()
{
	return CDialog::Create(CGridStatisticsDlg::IDD);
}

BEGIN_MESSAGE_MAP(CGridStatisticsDlg, CDialog)
	//{{AFX_MSG_MAP(CGridStatisticsDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGridStatisticsDlg message handlers

BOOL CGridStatisticsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	char buf[256];
	char *unit=grid->GetUnitName();

	double *ext;
	ext=grid->GetXlimits();
	sprintf(buf, "x: %7.0f..%7.0f", ext[0], ext[1]);
	SetDlgItemText(IDC_GRIDSTAT_XEXT, buf);
	sprintf(buf, "grid node spacing=%6.1f", (ext[1]-ext[0])/(grid->GetColumns()-1));
	SetDlgItemText(IDC_GRIDSTAT_DELTAX, buf);

	ext=grid->GetYlimits();
	sprintf(buf, "y: %7.0f..%7.0f", ext[0], ext[1]);
	SetDlgItemText(IDC_GRIDSTAT_YEXT, buf);
	sprintf(buf, "grid node spacing=%6.1f", (ext[1]-ext[0])/(grid->GetRows()-1));
	SetDlgItemText(IDC_GRIDSTAT_DELTAY, buf);

	sprintf(buf, "Statistics for '%s'", (const char *)grid->GetName());
	SetWindowText(buf);

	if(grid->GetZnon(grid->GetAvg()))
	{
		SetDlgItemText(IDC_GRIDSTAT_MINMAX, "");
		sprintf(buf, "All grid nodes contain the value NULL");
	}
	else
	{
		sprintf(buf, "Z min: %5.3f%s  Z max: %5.3f%s", grid->CalcMinVal(), unit,
			grid->CalcMaxVal(), unit);
		SetDlgItemText(IDC_GRIDSTAT_MINMAX, buf);
		sprintf(buf, "Z avg: %5.3f%s  Number of NULL values: %d", grid->GetAvg(), unit,
			grid->GetZnonVals());
	}
	SetDlgItemText(IDC_GRIDSTAT_AVG, buf);

	sprintf(buf, "No. rows: %d No. columns: %d", grid->GetRows(), grid->GetColumns());
	SetDlgItemText(IDC_GRIDSTAT_GRIDSIZE, buf);

	if(grid->GetFaultGrid())
		sprintf(buf, "Fault lines from '%s'", (const char *)((grid->GetFaultGrid())->GetName()));
	else
		buf[0]=0;
	SetDlgItemText(IDC_GRIDSTAT_FAULTS, buf);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
