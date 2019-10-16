// linecode.cpp : implementation file
//

#include "stdafx.h"
#include "velman.h"
#include "VelmanDoc.h"
#include "LineCodeDlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLinecode2Coord dialog

CLinecode2Coord::CLinecode2Coord(CWnd* pParent /*=NULL*/)
: CDialog(CLinecode2Coord::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLinecode2Coord)
	m_shot1 = 0;
	m_shot2 = 0;
	m_shot3 = 0;
	m_glbx2 = 0.0;
	m_glbx3 = 0.0;
	m_glby1 = 0.0;
	m_glby2 = 0.0;
	m_glby3 = 0.0;
	m_lin1 = 0;
	m_lin2 = 0;
	m_lin3 = 0;
	m_glbx1 = 0.0;
	//}}AFX_DATA_INIT
}

void CLinecode2Coord::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLinecode2Coord)
	DDX_Text(pDX, IDC_COORD2_SP1, m_shot1);
	DDX_Text(pDX, IDC_COORD2_SP2, m_shot2);
	DDX_Text(pDX, IDC_COORD2_SP3, m_shot3);
	DDX_Text(pDX, IDC_COORD2_GLBX1, m_glbx1);
	DDV_MinMaxDouble(pDX, m_glbx1, 0., 10000000000000.);
	DDX_Text(pDX, IDC_COORD2_GLBX2, m_glbx2);
	DDV_MinMaxDouble(pDX, m_glbx2, 0., 10000000000000.);
	DDX_Text(pDX, IDC_COORD2_GLBX3, m_glbx3);
	DDV_MinMaxDouble(pDX, m_glbx3, 0., 10000000000000.);
	DDX_Text(pDX, IDC_COORD2_GLBY1, m_glby1);
	DDV_MinMaxDouble(pDX, m_glby1, 0., 10000000000000.);
	DDX_Text(pDX, IDC_COORD2_GLBY2, m_glby2);
	DDV_MinMaxDouble(pDX, m_glby2, 0., 10000000000000.);
	DDX_Text(pDX, IDC_COORD2_GLBY3, m_glby3);
	DDV_MinMaxDouble(pDX, m_glby3, 0., 10000000000000.);
	DDX_Text(pDX, IDC_COORD2_LIN1, m_lin1);
	DDX_Text(pDX, IDC_COORD2_LIN2, m_lin2);
	DDX_Text(pDX, IDC_COORD2_LIN3, m_lin3);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CLinecode2Coord, CDialog)
	//{{AFX_MSG_MAP(CLinecode2Coord)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLinecode2Coord message handlers

BOOL CLinecode2Coord::OnInitDialog()
{
	int		oldp;		// See OnOK()
	int		newp = 14;

	CDialog::OnInitDialog();

	// if there is a file with coord information in there (i.e., if the coord
	// conversion parameters have already been entered once for this project)
	// read them from that file)
	///////////////////////////////
	// the following lines are here to find out why the unix version doesn't
	// pick up the filename??
	//AfxMessageBox( "TargetDir = " + pDoc->TargetDir + "\n BaseName = " + pDoc->BaseName + "\n", MB_OK );

	CString filename=pDoc->TargetDir+pDoc->BaseName+".crd";
#ifdef VELMAN_UNIX
	ifstream coordfile(filename);
#else
	std::ifstream coordfile(filename);
#endif

	if(coordfile.good())
	{
		//AfxMessageBox( "linecode: Inside if block ok! Found .crd file\n", MB_OK );
		coordfile >> m_shot1;
		coordfile >> m_shot2;
		coordfile >> m_shot3;
		oldp = coordfile.precision( newp );
		coordfile >> m_glbx1;
		coordfile >> m_glbx2;
		coordfile >> m_glbx3;
		coordfile >> m_glby1;
		coordfile >> m_glby2;
		coordfile >> m_glby3;
		oldp = coordfile.precision( oldp );
		coordfile >> m_lin1;
		coordfile >> m_lin2;
		coordfile >> m_lin3;
		UpdateData(FALSE);
		// test, write the data back out again to new file and see what's in it
		//  trying to find out problem with .crd file on unix
		/*	CString filename=pDoc->TargetDir+pDoc->BaseName+".crd2";
		ofstream coordfile2(filename);
		if(coordfile2.good())
		{
		coordfile2 << m_shot1 << endl;
		coordfile2 << m_shot2 << endl;
		coordfile2 << m_shot3 << endl;
		oldp = coordfile2.precision( newp );
		coordfile2 << m_glbx1 << endl;
		coordfile2 << m_glbx2 << endl;
		coordfile2 << m_glbx3 << endl;
		coordfile2 << m_glby1 << endl;
		coordfile2 << m_glby2 << endl;
		coordfile2 << m_glby3 << endl;
		oldp = coordfile2.precision( oldp );
		coordfile2 << m_lin1 << endl;
		coordfile2 << m_lin2 << endl;
		coordfile2 << m_lin3 << endl;
		} */
		// end of test code
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CLinecode2Coord::OnOK()
{
	// Have to explicitly
	// set the precision required and reinstate it after the change.

	int		oldp;		// old precision setting for output stream
	int		newp = 14;	// no. of sig. figs. (see ios::precision)

	CDialog::OnOK();

	// try to remember the data entered, so we can use clever default values next time
	CString filename=pDoc->TargetDir+pDoc->BaseName+".crd";
	ofstream coordfile(filename);
	if(coordfile.good())
	{
		coordfile << m_shot1 << endl;
		coordfile << m_shot2 << endl;
		coordfile << m_shot3 << endl;
		oldp = coordfile.precision( newp );
		coordfile << m_glbx1 << endl;
		coordfile << m_glbx2 << endl;
		coordfile << m_glbx3 << endl;
		coordfile << m_glby1 << endl;
		coordfile << m_glby2 << endl;
		coordfile << m_glby3 << endl;
		oldp = coordfile.precision( oldp );
		coordfile << m_lin1 << endl;
		coordfile << m_lin2 << endl;
		coordfile << m_lin3 << endl;
	}
}
