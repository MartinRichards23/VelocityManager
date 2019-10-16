// CAutoCorrectOptionsDlg dialog

class CAutoCorrectOptionsDlg : public CDialog
{
	// Construction
public:
	CAutoCorrectOptionsDlg(CWnd* pParent = NULL);   // standard constructor
	BOOL shouldswap;
	int m_option;

	// Dialog Data
	//{{AFX_DATA(CAutoCorrectOptionsDlg)
	enum { IDD = IDD_HOWTO_AUTOCORR };
	//}}AFX_DATA

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAutoCorrectOptionsDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX); // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAutoCorrectOptionsDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
