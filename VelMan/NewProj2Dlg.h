// newproj2.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNewProjStep2 dialog

class CNewProjStep2 : public CDialog
{
	// Construction
public:
	CNewProjStep2(CWnd* pParent = NULL);	// standard constructor
	CString ZimsDir;	// Directory that contains ZIMS database files
	CVelmanDoc *pDoc;

	// Dialog Data
	//{{AFX_DATA(CNewProjStep2)
	enum { IDD = IDD_NEWPROJ_2 };
	// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

private:
	void DoFillDir();
	// config vars
	bool initialised;
	int formWidth;
	int border_LR_IDOK;

	int border_LR_IDCANCEL;

	int border_RR_IDC_NEWPROJ2_EDITDIR;

	int border_LR_IDC_STATIC4;

	int width_IDC_LIST_NEWPROJDIR2;
	int borderBB_IDC_LIST_NEWPROJDIR2;
	int width_IDC_NEWPROJ_GRIDNAMESPREVIEW;
	int borderBB_IDC_NEWPROJ_GRIDNAMESPREVIEW;
	int borderLL_IDC_NEWPROJ_GRIDNAMESPREVIEW;

	// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	void DisplayGridNamesPreview();
	// Generated message map functions
	//{{AFX_MSG(CNewProjStep2)
	afx_msg void OnSelchangeComboNewprojdrive2();
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkListNewprojdir2();
	afx_msg void OnSelchangeNewprojGridfiletype();
	virtual void OnOK();
	afx_msg void OnKillfocusNewproj2Editdir();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
};
