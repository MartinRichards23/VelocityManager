// GRenDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRenameGridDlg dialog

class CRenameGridDlg : public CDialog
{
	// Construction
public:
	CRenameGridDlg(CWnd* pParent = NULL);   // standard constructor
	char *oldname;		// used for window title
	int gridtype;
	// Dialog Data
	//{{AFX_DATA(CRenameGridDlg)
	enum { IDD = IDD_RENAME_GRID };
	CString	m_gridname;
	int		m_listType;
	//}}AFX_DATA
private:

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRenameGridDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRenameGridDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
