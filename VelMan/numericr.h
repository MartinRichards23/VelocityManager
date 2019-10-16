// numericr.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNumericResiduesDlg dialog

class CNumericResiduesDlg : public CDialog
{
	// Construction
public:
	CNumericResiduesDlg(CWnd* pParent = NULL);	// standard constructor
	BOOL Create();

	CHorizon *horizon;
	CVelmanDoc *pDoc;
	int layer;		// the layer for which we want to choose a model
private:
	CFont smallFont;

	// Dialog Data
	//{{AFX_DATA(CNumericResiduesDlg)
	enum { IDD = IDD_NUMERIC_RESIDUES };
	// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CNumericResiduesDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
