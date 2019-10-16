/////////////////////////////////////////////////////////////////////////////
// CModelChooselayer dialog

class CModelChooselayer : public CDialog
{
	// Construction
public:
	CModelChooselayer(CWnd* pParent = NULL);	// standard constructor
	CVelmanDoc *pDoc;
	int ChosenLayer;
	// Dialog Data
	//{{AFX_DATA(CModelChooselayer)
	enum { IDD = IDD_MODEL_CHOOSELAYER };
	// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA
private:
	int LastModel;	// index of last horizon that already has a model (or -1 if none)
	// set in OnInit, read in OnOK

	// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CModelChooselayer)
	virtual BOOL OnInitDialog();
	afx_msg void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
