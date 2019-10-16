// NDirDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCreateNewDirDlg dialog

class CCreateNewDirDlg : public CDialog
{
	// Construction
public:
	CCreateNewDirDlg(CWnd* pParent = NULL);   // standard constructor
	CString BasePath, NewDir;

	// Dialog Data
	//{{AFX_DATA(CCreateNewDirDlg)
	enum { IDD = IDD_CREATEPROJDIR };
	// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCreateNewDirDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCreateNewDirDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
