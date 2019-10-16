// projguid.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CProjGuideDlg dialog

class CProjGuideDlg : public CDialog
{
	// Construction
public:
	CProjGuideDlg(CWnd* pParent = NULL);	// standard constructor
	CString HelpText;
	// Dialog Data
	//{{AFX_DATA(CProjGuideDlg)
	enum { IDD = IDD_PROJ_GUIDE_DLG };
	// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CProjGuideDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
