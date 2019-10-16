// CChooseManyGridsDlg dialog

class CChooseManyGridsDlg : public CDialog
{
	// Construction
public:
	CChooseManyGridsDlg(CWnd* pParent = NULL);   // standard constructor
	CVelmanDoc *pDoc;
	int *chosengrids;
	int numgrids;
	CWordArray entries;	// hi-byte: no in listbox, lobyte=gridno
	void SetDlgTexts(const char *title, const char *request);
	~CChooseManyGridsDlg() { delete chosengrids; }
	BOOL OnlyTimeGrids;
private:
	CString dlg_title, dlg_request;

	// Dialog Data
	//{{AFX_DATA(CChooseManyGridsDlg)
	enum { IDD = IDD_CHOOSE_ANY_GRID_MULT };
	// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	int border_LR_IDOK;
	int border_LR_IDCANCEL;

	int border_RR_IDC_CHOOSEGRID_LIST;
	int border_BB_IDC_CHOOSEGRID_LIST;

	int border_TB_IDC_STATIC1;
	int border_LR_IDC_STATIC1;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChooseManyGridsDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX); // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CChooseManyGridsDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
