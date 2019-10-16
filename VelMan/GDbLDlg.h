// GDbLDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGridDatabaseListDlg dialog

class CGridDatabaseListDlg : public CDialog
{
	// Construction
public:
	CGridDatabaseListDlg(CWnd* pParent = NULL);   // standard constructor
	CVelmanDoc *pDoc;
private:
	void FillList();
	BOOL ListInternalGrids;
	CFont smallFont;

	// Dialog Data
	//{{AFX_DATA(CGridDatabaseListDlg)
	enum { IDD = IDD_GRIDDATABASE_LIST };
	//}}AFX_DATA

	// config vars
	bool initialised;
	int formWidth;
	int border_LR_IDOK;
	int border_RR_IDC_GDB_TABLE;
	int border_BB_IDC_GDB_TABLE;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGridDatabaseListDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGridDatabaseListDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnGriddatabaseRadio1();
	afx_msg void OnGriddatabaseRadio2();
	afx_msg void OnDestroy();
	afx_msg void OnRclickGdbTable(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
