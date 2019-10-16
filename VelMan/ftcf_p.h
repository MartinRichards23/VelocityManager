// CFinetuneCustomfit_Page dialog

class CFinetuneCustomfit_Page : public CPropertyPage
{
	DECLARE_DYNCREATE(CFinetuneCustomfit_Page)

	// Construction
public:
	CFinetuneCustomfit_Page(CFinetuneSheet *mysheet);
	CFinetuneCustomfit_Page();
	~CFinetuneCustomfit_Page();
	CFinetuneSheet *sheet;
	void UpdateButtons();
	CVelmanDoc *pDoc;
	int layer;
private:
	CHorizon *horizon;
	CFineTuneView *pView;
public:
	// Dialog Data
	//{{AFX_DATA(CFinetuneCustomfit_Page)
	enum { IDD = IDD_FINETUNE_CUSTOMFIT_PAGE };
	double	m_customfita;
	double	m_customfitb;
	//}}AFX_DATA

	// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CFinetuneCustomfit_Page)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
public:
	// Generated message map functions
	//{{AFX_MSG(CFinetuneCustomfit_Page)
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnFinetuneHideme();
	virtual BOOL OnInitDialog();
	afx_msg void OnFinetuneSwitchviewmode();
	afx_msg void OnPaint();
	afx_msg void OnFinetuneDisplaycustomfit();
	afx_msg void OnKillfocusFinetuneCustomfit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
