// RayTracePropsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "velman.h"
#include "raytracepropsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRayTracePropsDlg dialog

CRayTracePropsDlg::CRayTracePropsDlg(CWnd* pParent /*=NULL*/)
: CDialog(CRayTracePropsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRayTracePropsDlg)
	m_xyoffsetgrids = FALSE;
	m_depthdiffgrids = FALSE;
	m_anglegrids = FALSE;
	m_limitanglevalue = 0.0;
	m_azimuthgrids = FALSE;
	m_regridbinsize = 2;    // 150%; see InitDialogue
	//}}AFX_DATA_INIT

	cellratiosize = 7;
}

void CRayTracePropsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRayTracePropsDlg)
	DDX_Check(pDX, IDC_RAYTRACE_PROPS_XYOFFSET, m_xyoffsetgrids);
	DDX_Check(pDX, IDC_RAYTRACE_PROPS_DEPTHDIFF, m_depthdiffgrids);
	DDX_Check(pDX, IDC_RAYTRACE_PROPS_ANGLEGRIDS, m_anglegrids);
	DDX_Text(pDX, IDC_RAYTRACE_PROPS_LIMITANGLE, m_limitanglevalue);
	DDX_Check(pDX, IDC_RAYTRACE_PROPS_AZIMUTH, m_azimuthgrids);
	//}}AFX_DATA_MAP

	CSliderCtrl* rtdlg = (CSliderCtrl*) GetDlgItem(IDC_RAYTRACE_PROPS_REGRIDBINSIZE);
	if (pDX->m_bSaveAndValidate)
	{
		m_regridbinsize = rtdlg->GetPos();
	}
	else
	{
		rtdlg->SetPos(m_regridbinsize);
	}
}

BEGIN_MESSAGE_MAP(CRayTracePropsDlg, CDialog)
	//{{AFX_MSG_MAP(CRayTracePropsDlg)
	ON_WM_HSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////

static int cellratio[] = { 100, 125, 150, 175, 200, 225, 250 };
static LPCTSTR cellratiotext[] = { "100%", "125%", "150%", "175%", "200%", "225%", "250%" };

/////////////////////////////////////////////////////////////////////////////
// CRayTracePropsDlg message handlers

void CRayTracePropsDlg::OnOK()
{
	// TODO: Add extra validation here

	CDialog::OnOK();
}

BOOL CRayTracePropsDlg::OnInitDialog()
{
	CSliderCtrl* cellcap = (CSliderCtrl*) GetDlgItem(IDC_RAYTRACE_PROPS_REGRIDBINSIZE);

	//  Set the range for the cell capture ratio slider
	cellcap->SetRange(0, cellratiosize-1, TRUE);

	//  Set initial value
	GetDlgItem(IDC_RAYTRACE_PROPS_RATIOVALUE)->SetWindowText(cellratiotext[m_regridbinsize]);

	CDialog::OnInitDialog();    // calls DoDataExchange which sets the initial position of the cntrl

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CRayTracePropsDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// This gets called when the user moves the cellcaptureratio slider.
	// This handler modifies the setting value to provide feedback to the
	// user so that they know what value is set.

	int nPosition;
	int nControl = pScrollBar->GetDlgCtrlID();
	CSliderCtrl *pControl = (CSliderCtrl *) GetDlgItem(nControl);

	switch (nControl)
	{
	case IDC_RAYTRACE_PROPS_REGRIDBINSIZE:
		ASSERT(pControl != NULL );
		nPosition = pControl->GetPos();
		GetDlgItem(IDC_RAYTRACE_PROPS_RATIOVALUE)->SetWindowText(cellratiotext[nPosition]);
		break;
	default:
		CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
		break;
	}
}

int GetRTregridbinsize(int index)
{
	//  returns current binsize setting

	return( cellratio[index] );
}
