// CModelChoiceDlg dialog

class CModelChoiceDlg : public CDialog
{
	// Construction
public:
	CModelChoiceDlg(CWnd* pParent = NULL);	// standard constructor
	BOOL Create();

	CVelmanDoc *pDoc;
	int layer;		// the layer for which we want to choose a model (needed because
	// we have to tell the fine tuning window which layer we want)
	BOOL ResidualTableListOpen;
private:
	void CleanUp();
	CFont smallFont;	// small Font for the list boxes
	CButton *pModeButton;	// button that switches view modes
	CNumericResiduesDlg residual_dlg;

	// Dialog Data
	//{{AFX_DATA(CModelChoiceDlg)
	enum { IDD = IDD_MODELCHOICE };
	BOOL	m_distweight4;
	BOOL	m_distweight3;
	BOOL	m_distweight2;
	BOOL	m_distweight1;
	//}}AFX_DATA

	// Implementation
	//protected:
public:		// we need to call the OnSelchange members from the outside
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	virtual void OnOK();		// we hook it with a dummy function so pressing RETURN does nothing
	// Generated message map functions
	//{{AFX_MSG(CModelChoiceDlg)
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeModelchoiceFunc1();
	afx_msg void OnSelchangeModelchoiceFunc2();
	afx_msg void OnSelchangeModelchoiceFunc3();
	afx_msg void OnSelchangeModelchoiceFunc4();
	afx_msg void OnModelchoiceResiduals();
	afx_msg void OnModelchoiceModebtn();
	afx_msg void OnModelchoiceHidedlg();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
