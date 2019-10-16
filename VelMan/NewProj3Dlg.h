// ScopDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CProjectScopeDlg dialog

class CProjectScopeDlg : public CDialog
{
	// Construction
public:
	CProjectScopeDlg(CWnd* pParent = NULL);   // standard constructor
	int m_PrjScope;
	// Dialog Data
	//{{AFX_DATA(CProjectScopeDlg)
	enum { IDD = IDD_NEWPROJ_3 };
	//}}AFX_DATA

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProjectScopeDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CProjectScopeDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
