// TIResDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRMSTestImportResultsDlg dialog

class CRMSTestImportResultsDlg : public CDialog
{
	// Construction
public:
	CRMSTestImportResultsDlg(CWnd* pParent = NULL);   // standard constructor
	CVelmanDoc *pDoc;
private:
	CFont smallFont;
	// Dialog Data
	//{{AFX_DATA(CRMSTestImportResultsDlg)
	enum { IDD = IDD_RESULTS_RMSTESTIMPORT };
	// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// config vars
	bool initialised;
	int formWidth;
	int border_LR_IDOK;
	int border_LR_IDC_TESTIMPORT_COORDNOTE;
	int border_BB_IDC_TESTIMPORT_COORDNOTE;
	int border_RR_IDC_TESTIMPORT_LIST;
	int border_BB_IDC_TESTIMPORT_LIST;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRMSTestImportResultsDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRMSTestImportResultsDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
