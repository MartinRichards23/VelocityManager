// WOModDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWarnOtherModelDlg dialog

class CWarnOtherModelDlg : public CDialog
{
	// Construction
public:
	CWarnOtherModelDlg(CWnd* pParent = NULL);   // standard constructor

	// Dialog Data
	//{{AFX_DATA(CWarnOtherModelDlg)
	enum { IDD = IDD_WARN_OTHERMODEL };
	BOOL	m_RetainWeights;
	//}}AFX_DATA

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWarnOtherModelDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CWarnOtherModelDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
