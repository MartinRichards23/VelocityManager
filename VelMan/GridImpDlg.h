// GImpDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGridImportDlg dialog

class CGridImportDlg : public CDialog
{
	// Construction
public:
	CGridImportDlg(CWnd* pParent = NULL);   // standard constructor
	CVelmanDoc *pDoc;
	CString ChosenGrid;
	CString FaultGrid;
	BOOL UseFaults;
	UBYTE gridtype;
	// Dialog Data
	//{{AFX_DATA(CGridImportDlg)
	enum { IDD = IDD_GRIDIMPORT };
	// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA
private:
	// config vars
	bool initialised;
	int formWidth;
	int formHeight;

	int border_LR_IDOK;
	int border_LR_IDCANCEL;
	int border_TB_IDC_GRIDIMP_GRIDTYPE;
	int border_TB_IDC_STATIC1;
	int border_YSpacer_IDC_STATIC2;

	int border_RR_IDC_GRIDIMP_GRIDLIST;
	int height_IDC_GRIDIMP_GRIDLIST;
	int border_TT_IDC_GRIDIMP_FAULTGRIDLIST;
	int height_IDC_GRIDIMP_FAULTGRIDLIST;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGridImportDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGridImportDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
