// 2dsupp.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRMSImportSuppfileDlg dialog

class CRMSImportSuppfileDlg : public CDialog
{
	// Construction
public:
	CRMSImportSuppfileDlg(CWnd* pParent = NULL);	// standard constructor
	CVelmanDoc *pDoc;
	CString ChosenGrid;	// contains name of chosen grid on return
	// Dialog Data
	//{{AFX_DATA(CRMSImportSuppfileDlg)
	enum { IDD = IDD_RMSIMPORT_2DSUPPFILE };
	// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA
private:

	// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CRMSImportSuppfileDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
