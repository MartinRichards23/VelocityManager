// CDefineHorizonsDlg dialog

class CDefineHorizonsDlg : public CDialog
{
	// Construction
public:
	CDefineHorizonsDlg(CWnd* pParent = NULL);   // standard constructor
	CVelmanDoc *pDoc;

	// Dialog Data
	//{{AFX_DATA(CDefineHorizonsDlg)
	enum { IDD = IDD_DEFINE_HORIZONS };
	// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDefineHorizonsDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDefineHorizonsDlg)
	virtual void OnOK();
	afx_msg void OnDefhorRemove();
	afx_msg void OnDone();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
