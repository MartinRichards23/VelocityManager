// TieImDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTieAfterImportDlg dialog

class CTieAfterImportDlg : public CDialog
{
	// Construction
public:
	CTieAfterImportDlg(CWnd* pParent = NULL);   // standard constructor
	CVelmanDoc *pDoc;
	BOOL NormGrad;
	int NormGradFac;

	// Dialog Data
	//{{AFX_DATA(CTieAfterImportDlg)
	enum { IDD = IDD_TIMEIMPORT_WHATTOTIE };
	int		m_how;
	BOOL	m_tieGrids;
	//}}AFX_DATA

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTieAfterImportDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:
	void GrayNormalisation();

	// Generated message map functions
	//{{AFX_MSG(CTieAfterImportDlg)
	afx_msg void OnTie();
	afx_msg void OnWelltimeNormgrad();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
