// PLogDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CProjectLogDlg dialog

class CProjectLogDlg : public CDialog
{
	// Construction
public:
	CProjectLogDlg(CWnd* pParent = NULL);   // standard constructor
	CVelmanDoc *pDoc;
	CStdioFile *logfile;
	BOOL SkipFirstLine;
private:
	CFont listfont;
	// Dialog Data
	//{{AFX_DATA(CProjectLogDlg)
	enum { IDD = IDD_LOGFILEVIEW };
	// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA
	// config vars
	bool initialised;
	int formWidth;
	int border_RR_IDC_PROJECTLOG_TEXT;
	int border_BB_IDC_PROJECTLOG_TEXT;
	int border_LR_IDOK;
	int border_TB_IDOK;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProjectLogDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CProjectLogDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
