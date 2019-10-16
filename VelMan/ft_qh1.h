// CFinetuneQuickhelp1_Page dialog

class CFinetuneQuickhelp1_Page : public CPropertyPage
{
	DECLARE_DYNCREATE(CFinetuneQuickhelp1_Page)

	// Construction
public:
	CFinetuneQuickhelp1_Page(CFinetuneSheet *mysheet);
	CFinetuneQuickhelp1_Page();
	~CFinetuneQuickhelp1_Page();
	CFinetuneSheet *sheet;
	void UpdateButtons();
	CVelmanDoc *pDoc;

	// Dialog Data
	//{{AFX_DATA(CFinetuneQuickhelp1_Page)
	enum { IDD = IDD_FINETUNE_QUICKHELP1_PAGE };
	// NOTE - ClassWizard will add data members here.
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA

	// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CFinetuneQuickhelp1_Page)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
public:
	// Generated message map functions
	//{{AFX_MSG(CFinetuneQuickhelp1_Page)
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnFinetuneHideme();
	virtual BOOL OnInitDialog();
	afx_msg void OnFinetuneSwitchviewmode();
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
