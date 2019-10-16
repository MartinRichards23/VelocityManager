// GAritDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGridArithmeticDlg dialog

class CGridArithmeticDlg : public CDialog
{
	// Construction
public:
	CGridArithmeticDlg(CWnd* pParent = NULL);   // standard constructor
	CVelmanDoc *pDoc;
	CWordArray entries;	// hi-byte: no in listbox, lobyte=gridno
	int operation;		// selected grid operation
	double val1, val2;	// constant values
	int grid1, grid2;	// chosen grid indeces
	CString resultname;

	// Dialog Data
	//{{AFX_DATA(CGridArithmeticDlg)
	enum { IDD = IDD_GRID_ARITHMETIC };
	// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

private:
	BOOL AutoResultName, InternalNameChange;
	void MakeUpName();
	CFont smallFont;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGridArithmeticDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGridArithmeticDlg)
	afx_msg void OnChangeArithEdit2();
	afx_msg void OnSelchangeArithInput1();
	afx_msg void OnSelchangeArithInput2();
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeArithEdit1();
	virtual void OnOK();
	afx_msg void OnSelchangeArithOperator();
	afx_msg void OnChangeArithResultname();
	afx_msg void OnArithRadio();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
