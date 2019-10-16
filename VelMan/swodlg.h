// SWODlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSmoothWndOptDlg dialog

class CSmoothWndOptDlg : public CDialog
{
	// Construction
public:
	CSmoothWndOptDlg(CWnd* pParent = NULL);   // standard constructor
	CVelmanDoc *pDoc;
	int windowsize;
	BOOL KeepHighFrequencies;
	BOOL SetAsPrimary;
private:
	int minsize, maxsize, orgminsize;

	// Dialog Data
	//{{AFX_DATA(CSmoothWndOptDlg)
	enum { IDD = IDD_SMOOTH_MOVINGWND };
	// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSmoothWndOptDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSmoothWndOptDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
