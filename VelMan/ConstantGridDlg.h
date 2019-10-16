// CCreateConstantGridDlg dialog

class CCreateConstantGridDlg : public CDialog
{
	// Construction
public:
	CCreateConstantGridDlg(CWnd* pParent = NULL);   // standard constructor

	// Dialog Data
	//{{AFX_DATA(CCreateConstantGridDlg)
	enum { IDD = IDD_CREATE_CONSTGRID };
	double	m_value;
	int		m_listType;
	//}}AFX_DATA

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCreateConstantGridDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:
	virtual BOOL OnInitDialog();
	virtual void OnOK();

	// Generated message map functions
	//{{AFX_MSG(CCreateConstantGridDlg)
	// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
