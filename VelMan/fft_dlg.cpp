// FFT_Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "velman.h"
#include "FFT_Dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFFTDirectionDlg dialog

CFFTDirectionDlg::CFFTDirectionDlg(CWnd* pParent /*=NULL*/)
: CDialog(CFFTDirectionDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFFTDirectionDlg)
	m_direction = -1;
	//}}AFX_DATA_INIT
}

void CFFTDirectionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFFTDirectionDlg)
	DDX_Radio(pDX, IDC_FFTDIR_BUTTON1, m_direction);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CFFTDirectionDlg, CDialog)
	//{{AFX_MSG_MAP(CFFTDirectionDlg)
	// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFFTDirectionDlg message handlers
