// TCDptDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTimeConvertDepthDlg dialog

class CTimeConvertDepthDlg : public CDialog
{
	// Construction
public:
	CTimeConvertDepthDlg(CWnd* pParent = NULL);   // standard constructor
	~CTimeConvertDepthDlg() { delete EntryInDatabase; }
	CVelmanDoc *pDoc;
private:
	void FillList();
	CWellPointList *wellpointlist;
	int *EntryInDatabase; // see FillList()

	// Dialog Data
	//{{AFX_DATA(CTimeConvertDepthDlg)
	enum { IDD = IDD_TIMECONVERT };
	double	m_x;
	double	m_y;
	double	m_d;
	CString	m_name;
	//}}AFX_DATA

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTimeConvertDepthDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTimeConvertDepthDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnTcdReadfromfile();
	afx_msg void OnTcdWritetofile();
	afx_msg void OnTcdTimeconv();
	afx_msg void OnTcdDelete();
	afx_msg void OnTcdDeleteall();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
