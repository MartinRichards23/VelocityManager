// FTW_P.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFinetuneWeight_Page dialog

class CFinetuneWeight_Page : public CPropertyPage
{
	DECLARE_DYNCREATE(CFinetuneWeight_Page)

	// Construction
public:
	CFinetuneWeight_Page(CFinetuneSheet *mysheet);
	~CFinetuneWeight_Page();
	CFinetuneWeight_Page();
	CFinetuneSheet *sheet;
	void UpdateButtons();
	CVelmanDoc *pDoc;
	int layer, model;
	int m_method;	// value of combobox after OK clicked
	double m_downto;	// weight down to which it should be reduced (0 up to 0.99)
private:
	int downto_display;  // like downto, but integer 0..99 (for display purposes)
	double radius;
	CFineTuneView *pView;
	CHorizon *horizon;	// so we can read the current weighting model from horizon

	// Dialog Data
	//{{AFX_DATA(CFinetuneWeight_Page)
	enum { IDD = IDD_FINETUNE_WEIGHTS_PAGE };
	double	m_coi_rad;
	double	m_coi_x;
	double	m_coi_y;
	static double sx;	// static variables to remember choice for next grid
	static double sy;
	static double sr;
	//}}AFX_DATA

	// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CFinetuneWeight_Page)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
public:
	// Generated message map functions
	//{{AFX_MSG(CFinetuneWeight_Page)
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnFinetuneHideme();
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnPaint();
	afx_msg void OnChangeChgweightRad();
	afx_msg void OnChgweightDefaults();
	afx_msg void OnSelchangeChgweightMethod2();
	afx_msg void OnFinetuneSwitchviewmode();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
