// editwell.cpp : implementation file
//

#include "stdafx.h"
#include "velman.h"
#include "VelmanDoc.h"
#include "EditwellDlg.h"
#include "mainfrm.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditWellPointDlg dialog

CEditWellPointDlg::CEditWellPointDlg(CWnd* pParent /*=NULL*/)
: CDialog(CEditWellPointDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEditWellPointDlg)
	m_exclude = FALSE;
	m_depth = 0.0;
	m_time = 0.0;
	m_weight = 0.0;
	m_x = 0.0;
	m_y = 0.0;
	//}}AFX_DATA_INIT
	MayEdit=TRUE;
}

void CEditWellPointDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditWellPointDlg)
	DDX_Check(pDX, IDC_EDITWELL_EXCL, m_exclude);
	DDX_Text(pDX, IDC_EDITWELL_DEP, m_depth);
	DDX_Text(pDX, IDC_EDITWELL_TIME, m_time);
	DDX_Text(pDX, IDC_EDITWELL_WEIGHT, m_weight);
	DDX_Text(pDX, IDC_EDITWELL_X, m_x);
	DDX_Text(pDX, IDC_EDITWELL_Y, m_y);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CEditWellPointDlg, CDialog)
	//{{AFX_MSG_MAP(CEditWellPointDlg)
	ON_EN_CHANGE(IDC_EDITWELL_DEP, OnChangePointData)
	ON_BN_CLICKED(IDC_EDITWELL_EXCL, OnEditwellExcl)
	ON_EN_CHANGE(IDC_EDITWELL_TIME, OnChangePointData)
	ON_EN_CHANGE(IDC_EDITWELL_X, OnChangePointData)
	ON_EN_CHANGE(IDC_EDITWELL_Y, OnChangePointData)
	ON_EN_CHANGE(IDC_EDITWELL_WEIGHT, OnChangePointData)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditWellPointDlg message handlers
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CEditWellChooseDlg message handlers

void CEditWellPointDlg::OnChangePointData()
{
	DidChangePoint=TRUE;	// called if x, y, time or depth change
	DidChangeAnything=TRUE;
}

BOOL CEditWellPointDlg::OnInitDialog()
{
	m_depth=ceil(100.0*datapoint->ReadDepth())/100.0;	// want only 2 dec.pl. displayed
	m_time=ceil(100.0*datapoint->ReadTime())/100.0;
	m_x=datapoint->ReadX();
	m_y=datapoint->ReadY();
	m_weight=datapoint->ReadWeight();
	m_exclude=((datapoint->ReadStatus() & MPT_STATUS_USER_EXCLUDE)==MPT_STATUS_USER_EXCLUDE);

	CDialog::OnInitDialog();
	char buf[255];

	sprintf(buf, "Well '%s'", (const char *) (pDoc->horizonlist.wellnames[datapoint->wellindex]));
	SetDlgItemText(IDC_EDITWELL_WELLANDLAYER, buf);
	sprintf(buf, "Edit well data in layer '%s'",	(const char *)(((CHorizon *)(pDoc->horizonlist.GetAt(layer)))->name));

	SetWindowText(buf);

	DidChangePoint=FALSE;	// set to TRUE if x, y, time or depth change
	DidChangeAnything=FALSE;	// set to TRUE if x, y, time, depth or exclude bit change
	// change enable status of "weight" input field according to checkbox setting
	CEdit *pE=(CEdit *)GetDlgItem(IDC_EDITWELL_WEIGHT);
	pE->EnableWindow(!m_exclude);

	// disable all text fields if "MayEdit" not set
	if(!MayEdit)
	{
		GetDlgItem(IDC_EDITWELL_WEIGHT)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDITWELL_X)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDITWELL_Y)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDITWELL_DEP)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDITWELL_TIME)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDITWELL_EXCL)->EnableWindow(FALSE);
		SetDlgItemText(IDC_EDITWELL_NOTE, "You cannot edit this data.");
	}
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CEditWellPointDlg::OnOK()
{
	CDialog::OnOK();

	UBYTE status=(datapoint->ReadStatus() & (~MPT_STATUS_USER_EXCLUDE));
	if(m_exclude)
		status |= MPT_STATUS_USER_EXCLUDE;
	datapoint->WriteStatus(status);

	if(DidChangePoint && MayEdit)
	{
		datapoint->WriteX(m_x);
		datapoint->WriteY(m_y);

		if(datapoint->ReadTime() != m_time)
		{
			datapoint->WriteTime(m_time);
			datapoint->WriteStatus(datapoint->ReadStatus()&~MPT_STATUS_CALC_TIME);
		}

		if(datapoint->ReadDepth() != m_depth) 
		{
			datapoint->WriteDepth(m_depth);
			datapoint->WriteStatus(datapoint->ReadStatus()&~MPT_STATUS_CALC_DEPTH);
		}
		datapoint->WriteWeight(m_weight);
	}
}

void CEditWellPointDlg::OnEditwellExcl()
{
	// change enable status of "weight" input field according to checkbox setting
	CEdit *pE=(CEdit *)GetDlgItem(IDC_EDITWELL_WEIGHT);
	CButton *pB=(CButton *)GetDlgItem(IDC_EDITWELL_EXCL);

	pE->EnableWindow(!((pB->GetState()) & 1));
	DidChangeAnything=TRUE;
}
