// SGO_Dlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSmoothGridOptionsDlg dialog

class CSmoothGridOptionsDlg : public CDialog
{
	// Construction
public:
	CSmoothGridOptionsDlg(CWnd* pParent = NULL);   // standard constructor
	~CSmoothGridOptionsDlg();
	double filtervalue;	// return value (pos. of scrollbar)
	CZimsGrid *GridToSmooth;
	CVelmanDoc *pDoc;
private:
	CZimsGrid *mygrid;
	CZimsGrid *dispgrid;
	CDC *m_pMemDC;
	CBitmap *m_pBitmap;
	CRect PreviewClientRect;

	// Dialog Data
	//{{AFX_DATA(CSmoothGridOptionsDlg)
	enum { IDD = IDD_SMOOTHGRID_OPTIONS };
	// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSmoothGridOptionsDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSmoothGridOptionsDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual void OnOK();
	afx_msg void OnPaint();
	virtual void OnCancel();
	afx_msg void OnSmoothoptUpdate();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton2();
	afx_msg void OnStnClickedSmoothoptYmax();
};
