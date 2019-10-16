/////////////////////////////////////////////////////////////////////////////
// CFilterManagementDlg dialog

class CFilterManagementDlg : public CDialog
{
	// Construction
public:
	CFilterManagementDlg(CWnd* pParent = NULL);	// standard constructor
	CVelmanDoc *pDoc;
private:
	CStringArray FilterTypes;

	// Dialog Data
	//{{AFX_DATA(CFilterManagementDlg)
	enum { IDD = IDD_FILTERMANAGEMENT };
	// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CFilterManagementDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnAddfilter();
	afx_msg void OnSelchangeListofinstalledfilters();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	void UpdateListbox();
};
