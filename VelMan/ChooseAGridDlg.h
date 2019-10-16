// ChooseAnyGrid.h : header file

/////////////////////////////////////////////////////////////////////////////
// CChooseAnyGrid dialog

class CChooseAnyGrid : public CDialog
{
	// Construction
public:
	CChooseAnyGrid(CWnd* pParent = NULL);   // standard constructor
	CVelmanDoc *pDoc;
	int chosengrid;
	CWordArray entries;	// hi-byte: no in listbox, lobyte=gridno
	void SetDlgTexts(const char *title, const char *request);

private:
	CString dlg_title, dlg_request;
	// Dialog Data
	//{{AFX_DATA(CChooseAnyGrid)
	enum { IDD = IDD_CHOOSE_ANY_GRID };
	//}}AFX_DATA

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChooseAnyGrid)
protected:
	virtual void DoDataExchange(CDataExchange* pDX); // DDX/DDV support
	//}}AFX_VIRTUAL

	int border_LR_IDOK;
	int border_LR_IDCANCEL;

	int border_RR_IDC_CHOOSEGRID_LIST;
	int border_BB_IDC_CHOOSEGRID_LIST;

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CChooseAnyGrid)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
