#if !defined(AFX_OPENFILEBROWSER_H__1A6F50C9_6F83_4F53_8E57_6DD58370087A__INCLUDED_)
#define AFX_OPENFILEBROWSER_H__1A6F50C9_6F83_4F53_8E57_6DD58370087A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OpenFileBrowser.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COpenFileBrowser dialog

class COpenFileBrowser : public CDialog
{
	// Construction
public:
	COpenFileBrowser(CWnd* pParent = NULL);   // standard constructor

	// Dialog Data
	//{{AFX_DATA(COpenFileBrowser)
	enum { IDD = IDD_OPEN_FILE_BRWS };
	CString	m_shFileName;
	CString	m_shFilePath;
	//}}AFX_DATA
	void SetExtensions();
	void setFilePath(CString path);
	void setTitle(CString title);
	CString extensionPhrase, extension, title;
	int stringFind(CString haystack, char needle, int startPos);
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COpenFileBrowser)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	CString m_ProjTitle;
	CString TargetDir;

	// Implementation
private:
	void ChangeToDir(CString dir);
	void DoFillListBox();

	bool initialised;
	int formWidth;
	int border_RR_IDC_COMBO_NEWPROJDRIVE;
	int border_LR_IDC_NEWPROJ1_NEWDIR;
	int border_RR_IDC_NEWPROJ_EDIT_DIR;
	int border_LR_IDOK;
	int border_TB_IDOK;
	int border_LR_IDCANCEL;
	int border_TB_IDCANCEL;
	int border_RR_IDC_FILENAME_EDIT;
	int border_TB_IDC_FILENAME_EDIT;
	int border_RR_IDC_COMBO1;
	int border_TB_IDC_COMBO1;
	int border_TB_IDC_STATIC1;
	int border_TB_IDC_STATIC2;

	int width_IDC_LIST_NEWPROJDIR;
	int border_BB_IDC_LIST_NEWPROJDIR;
	int width_IDC_LIST_NEWPROJDIRFILES;
	int border_BB_IDC_LIST_NEWPROJDIRFILES;
	int border_LL_IDC_LIST_NEWPROJDIRFILES;

	CString requestedDir;
protected:

	// Generated message map functions
	//{{AFX_MSG(COpenFileBrowser)
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkListNewprojdir();
	afx_msg void OnSelchangeComboNewprojdrive();
	afx_msg void OnNewproj1Newdir();
	afx_msg void OnSelchangeListNewprojdirfiles();
	afx_msg void OnChangeFilenameEdit();
	afx_msg void OnSelchangeCombo1();
	afx_msg void OnButton2();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnChangeNewprojEditDir();
	afx_msg void OnDblclkListNewprojdirfiles();
	virtual void OnOK();
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnOKnew();
	afx_msg void OnSetfocusNewprojEditDir();
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPENFILEBROWSER_H__1A6F50C9_6F83_4F53_8E57_6DD58370087A__INCLUDED_)
