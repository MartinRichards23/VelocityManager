// mainfrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

class CMainFrame : public CMDIFrameWnd
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

	// Attributes
public:
	// Operations
public:
	void OpenFineTuneView(int);
	void OpenModelChoiceViews(int);
	static int CMainFrame::GetConfigTitleBarThickness(void);
	static int CMainFrame::GetConfigSideBarThickness(void);
	static void CMainFrame::ShellExecuteAndWait(LPCSTR, LPCSTR, BOOL);
	static bool CMainFrame::FileExists(CString); // Does file exist?
	static CString CMainFrame::GetMyDocsFolder(); // gets the mydocuments location, creates if doesn't exist.
	static CString CMainFrame::GetNewFileName(CString dir, CString filename, CString extension); // sees if file exists, returns modified filename + externsion
	static CString CMainFrame::GetExeFolder(); // gets folder exe was launched from
	// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;

	// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnModelCreatemodel();
	afx_msg void OnUpdateModelCreatemodel(CCmdUI* pCmdUI);
	afx_msg void OnModelShowresidualtable();
	afx_msg void OnModelShowlistofmodels();
	afx_msg void OnUpdateModelShowlistofmodels(CCmdUI* pCmdUI);
	afx_msg void OnUpdateModelShowresidualtable(CCmdUI* pCmdUI);
	afx_msg void OnGridsContourplot();
	afx_msg void OnShowWellDataTables();
	afx_msg void OnUpdateShowWellDataTables(CCmdUI* pCmdUI);
	afx_msg void OnClose();
	afx_msg void OnSeismicdataMapofshotpoints();
	afx_msg void OnHelpClicked();
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	BOOL FineTuneDialogOnTop;
	static int shBdr;  // added on 17/10/06
	static int shBdrSide;  // added on 17/10/06
};

/////////////////////////////////////////////////////////////////////////////
