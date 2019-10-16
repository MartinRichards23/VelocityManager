// maperror.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMapErrorDlg dialog

class CMapErrorDlg : public CDialog
{
	// Construction
public:
	CMapErrorDlg(CWnd* pParent = NULL);	// standard constructor
	CStringArray *errlistptr;

	// Dialog Data
	//{{AFX_DATA(CMapErrorDlg)
	enum { IDD = IDD_MAPPING_ERROR };
	// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// config vars
	bool initialised;
	int formWidth;
	int border_LR_IDOK;
	int border_RR_IDC_MAPERR_LIST;
	int border_BB_IDC_MAPERR_LIST;

	// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CMapErrorDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
