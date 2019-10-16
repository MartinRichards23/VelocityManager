// TDT_Dlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWellTablesDlg dialog

class CWellTablesDlg : public CDialog
{
	// Construction
public:
	CWellTablesDlg(CWnd* pParent = NULL);   // standard constructor

	// Dialog Data
	//{{AFX_DATA(CWellTablesDlg)
	enum { IDD = IDD_WELLTABLES };
	CVelmanDoc *pDoc;
	//}}AFX_DATA

private:
	int m_chosentype;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWellTablesDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CWellTablesDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
