// CFinetuneResidualsPage dialog

class CFinetuneResidualsPage : public CPropertyPage
{
	// Construction
public:
	CFinetuneResidualsPage(CFinetuneSheet *mysheet);
	CFinetuneResidualsPage();   // standard constructor
	CFinetuneSheet *sheet;
	void UpdateButtons();
	CVelmanDoc *pDoc;
	int layer;
	BOOL ResidualHandling;	// how to set the various checkboxes/radio buttons
	BOOL HowToHandleResiduals();
private:
	CHorizon *horizon;
	CFineTuneView *pView;
	// Dialog Data
	//{{AFX_DATA(CFinetuneResidualsPage)
	enum { IDD = IDD_FINETUNE_RESIDUALS_PAGE };
	// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFinetuneResidualsPage)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFinetuneResidualsPage)
	virtual void OnCancel();
	afx_msg void OnFinetuneHideme();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnFinetuneSwitchviewmode();
	afx_msg void OnPaint();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnFinetuneNormgrad();
	afx_msg void GrayStuff();
	afx_msg void OnResidualsTotaldeselection();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
