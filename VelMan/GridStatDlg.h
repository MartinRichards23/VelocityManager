// GStatDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGridStatisticsDlg dialog

class CGridStatisticsDlg : public CDialog
{
	// Construction
public:
	CGridStatisticsDlg(CWnd* pParent = NULL);   // standard constructor
	CZimsGrid *grid;
	BOOL Create();
	CVelmanDoc *pDoc;
	CGridStatisticsDlg(CVelmanDoc *);
	// Dialog Data
	//{{AFX_DATA(CGridStatisticsDlg)
	enum { IDD = IDD_GRID_STATISTICS };
	// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGridStatisticsDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGridStatisticsDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
