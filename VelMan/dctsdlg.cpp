// DCTSDlg.cpp : implementation file
//

#include "stdafx.h"
#include "velman.h"
#include "DCTSDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDepthConvertTimesliceDlg dialog

CDepthConvertTimesliceDlg::CDepthConvertTimesliceDlg(CWnd* pParent /*=NULL*/)
: CDialog(CDepthConvertTimesliceDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDepthConvertTimesliceDlg)
	value = 0.0;
	//}}AFX_DATA_INIT
}

void CDepthConvertTimesliceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDepthConvertTimesliceDlg)
	DDX_Text(pDX, IDC_DCTIMESLICE_VALUE, value);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDepthConvertTimesliceDlg, CDialog)
	//{{AFX_MSG_MAP(CDepthConvertTimesliceDlg)
	// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDepthConvertTimesliceDlg message handlers
