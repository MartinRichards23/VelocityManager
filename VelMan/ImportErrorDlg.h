// TImpErr.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTimeImportErrorDlg dialog

class CTimeImportErrorDlg : public CDialog
{
	// Construction
public:
	CTimeImportErrorDlg(CWnd* pParent = NULL);   // standard constructor
	void SetTexts(CStringArray &msg, BOOL swap);
	int answer;
	BOOL shouldswap;
	CStringArray *msgs;

	// Dialog Data
	//{{AFX_DATA(CTimeImportErrorDlg)
	enum { IDD = IDD_TIMEGRIDIMP_ERROR };
	// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

private:
	CFont smallFont;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTimeImportErrorDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTimeImportErrorDlg)
	afx_msg void OnAutocorrectBtn();
	afx_msg void OnRetrybtn();
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
