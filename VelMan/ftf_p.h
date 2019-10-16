// CFinetuneFitting_Page dialog

class CFinetuneFitting_Page : public CPropertyPage
{
	DECLARE_DYNCREATE(CFinetuneFitting_Page)

	// Construction
public:
	CFinetuneFitting_Page(CFinetuneSheet *mysheet);
	CFinetuneFitting_Page();
	~CFinetuneFitting_Page();
	CFinetuneSheet *sheet;
	void UpdateButtons();
	void ShowFittingParams();

	CVelmanDoc *pDoc;
	int layer;		// the layer for which we want to tune the model
	CHorizon *horizon;	// initialised in OnInitDlg()
	int wellnum;	// number of wells in the current regression (might be smaller than doc.wellnames.getsize() )
	BOOL NormGrad;
	int NormGradFac;

	CFrameWnd *pFineTuneView;
	CFineTuneView *pView;
	int model;		// the model function chosen before
	int weight;		// the weighting style chosen
	int m_ExcludePercent;
	void GrayStuff();
private:

	// Implementation
public:

	// Dialog Data
	//{{AFX_DATA(CFinetuneFitting_Page)
	enum { IDD = IDD_FINETUNE_FIT_PAGE };
	//}}AFX_DATA

	// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CFinetuneFitting_Page)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
public:
	// Generated message map functions
	//{{AFX_MSG(CFinetuneFitting_Page)
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnFinetuneHideme();
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnFinetuneAddbackall();
	afx_msg void OnFinetuneSwitchviewmode();
	afx_msg void OnPaint();
	afx_msg void OnFinetuneOthermodel();
	afx_msg void OnFinetuneShowallwells();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
