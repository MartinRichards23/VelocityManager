// WFCDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWellForecastDlg dialog

class CWellForecastDlg : public CDialog
{
	// Construction
public:
	CWellForecastDlg(CWnd* pParent = NULL);   // standard constructor
	CVelmanDoc *pDoc;
private:
	CWordArray entries;	// hi-byte: no in listbox, lobyte=gridno
	char FormatString[16];

	// Dialog Data
	//{{AFX_DATA(CWellForecastDlg)
	enum { IDD = IDD_WELLFORECAST };
	double	m_x;
	double	m_y;
	//}}AFX_DATA

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWellForecastDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CWellForecastDlg)
	afx_msg void OnWellfcStd();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedSave();
};
