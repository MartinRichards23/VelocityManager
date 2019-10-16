// CFinetuneQuickhelp2_Page dialog

class CFinetuneQuickhelp2_Page : public CPropertyPage
{
	DECLARE_DYNCREATE(CFinetuneQuickhelp2_Page)

	// Construction
public:
	CFinetuneQuickhelp2_Page(CFinetuneSheet *mysheet);
	CFinetuneQuickhelp2_Page();
	~CFinetuneQuickhelp2_Page();
	CFinetuneSheet *sheet;
	void UpdateButtons();
	CVelmanDoc *pDoc;

	// Dialog Data
	//{{AFX_DATA(CFinetuneQuickhelp2_Page)
	enum { IDD = IDD_FINETUNE_QUICKHELP2_PAGE };
	// NOTE - ClassWizard will add data members here.
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA

	// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CFinetuneQuickhelp2_Page)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
public:
	// Generated message map functions
	//{{AFX_MSG(CFinetuneQuickhelp2_Page)
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnFinetuneHideme();
	virtual BOOL OnInitDialog();
	afx_msg void OnFinetuneSwitchviewmode();
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
