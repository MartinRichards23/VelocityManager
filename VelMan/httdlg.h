// HTTDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CHowToTieDlg dialog

class CHowToTieDlg : public CDialog
{
	// Construction
public:
	CHowToTieDlg(CWnd* pParent = NULL);   // standard constructor
	BOOL NormGrad;
	int NormGradFac;

	// Dialog Data
	//{{AFX_DATA(CHowToTieDlg)
	enum { IDD = IDD_HOWTOTIE };
	// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHowToTieDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:
	void GrayNormalisation();

	// Generated message map functions
	//{{AFX_MSG(CHowToTieDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHowtotieNormgrad();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
