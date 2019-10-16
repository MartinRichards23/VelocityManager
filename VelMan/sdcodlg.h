// SDCODlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSecondaryDepthConvOptDlg dialog

class CSecondaryDepthConvOptDlg : public CDialog
{
	// Construction
public:
	CSecondaryDepthConvOptDlg(CWnd* pParent = NULL);   // standard constructor
	int percentage;
	int interp_method;
	BOOL may_apply_cubic;
	BOOL TimeConversion;  // to change the title
	BOOL RebuildDoesNotApply;
private:
	void ReadSettings();
public:
	// Dialog Data
	//{{AFX_DATA(CSecondaryDepthConvOptDlg)
	enum { IDD = IDD_SECONDARYDEPTHCONV_OPT };
	BOOL	m_rebuild;
	//}}AFX_DATA

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSecondaryDepthConvOptDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSecondaryDepthConvOptDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnPaint();
	afx_msg void OnChangeSettings();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
