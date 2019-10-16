// LGridDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLimitGridDlg dialog

class CLimitGridDlg : public CDialog
{
	// Construction
public:
	CLimitGridDlg(CWnd* pParent = NULL);   // standard constructor
	CVelmanDoc *pDoc;
	void UpdateOutputName();
	CWordArray entries;	// hi-byte: no in listbox, lobyte=gridno
	int grid1, grid2;	// chosen grid indeces

	// Dialog Data
	//{{AFX_DATA(CLimitGridDlg)
	enum { IDD = IDD_LIMIT_GRID };
	// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

private:

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLimitGridDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CLimitGridDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeGridlimitInput2();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
