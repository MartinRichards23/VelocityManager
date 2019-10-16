// rmsimpor.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRMSImportDlg dialog

class CRMSImportDlg : public CDialog
{
	// Construction
public:
	CRMSImportDlg(CWnd* pParent = NULL);	// standard constructor
	CVelmanDoc *pDoc;
	BOOL Error;			// true if no filterfile present or no filters found in there
	CString ChosenFilter;		// on return contains name of chosen filter
	CString ChosenType;			// on return contains type line of chosen filter
	CStringArray AllTheTypes;	// to make return of ChosenType easier

	// Dialog Data
	//{{AFX_DATA(CRMSImportDlg)
	enum { IDD = IDD_RMSIMPORT1 };
	int		m_Unit;
	//}}AFX_DATA
private:

	// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CRMSImportDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
