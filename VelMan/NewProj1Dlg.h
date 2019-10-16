// newproj1.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNewProjStep1 dialog

class CNewProjStep1 : public CDialog
{
	// Construction
public:
	CNewProjStep1(CWnd* pParent = NULL);	// standard constructor

	// Dialog Data
	//{{AFX_DATA(CNewProjStep1)
	enum { IDD = IDD_NEWPROJ_1 };
	//}}AFX_DATA
	int m_LogType;
	int m_Unitchoice;
	CString m_ProjTitle;
	CString TargetDir;
private:
	void ChangeToDir(CString dir);
	void DoFillListBox();

	// config vars
	bool initialised;
	int formWidth;

	int borderLR_IDOK;

	int borderLR_IDCANCEL;

	int borderRR_IDC_NEWPROJ_EDIT_DIR;

	int width_IDC_LIST_NEWPROJDIR;
	int borderBB_IDC_LIST_NEWPROJDIR;
	int width_IDC_LIST_NEWPROJDIRFILES;
	int borderBB_IDC_LIST_NEWPROJDIRFILES;
	int borderLL_IDC_LIST_NEWPROJDIRFILES;

	int borderTB_IDC_NEWPROG_LOGDETAIL;

	int borderTB_IDC_STATIC2;

	int borderLR_IDC_STATIC3;

	int borderTB_IDC_STATIC1;

	int borderTB_IDC_NEWPROG_UNITS;

	// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CNewProjStep1)
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkListNewprojdir();
	afx_msg void OnSelchangeComboNewprojdrive();
	virtual void OnOK();
	afx_msg void OnNewproj1Newdir();
	afx_msg void OnKillfocusNewproj1EditDir();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonup();
};
