// RebSDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRebuildSettingsDlg dialog

class CRebuildSettingsDlg : public CDialog
{
	// Construction
public:
	CRebuildSettingsDlg(CWnd* pParent = NULL);   // standard constructor

	// Dialog Data
	//{{AFX_DATA(CRebuildSettingsDlg)
	enum { IDD = IDD_REBUILD_SETTINGS };
	BOOL	m_doall;
	BOOL	SetAsPrimary;
	int		m_number;
	//}}AFX_DATA

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRebuildSettingsDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRebuildSettingsDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnRebuildsettingsDoall();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
