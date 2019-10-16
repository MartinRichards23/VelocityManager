/////////////////////////////////////////////////////////////////////////////
// CProgressMonit dialog

class CProgressMonit : public CDialog
{
	// Construction
public:
	CProgressMonit(CWnd* pParent = NULL);	// standard constructor
	CProgressMonit(CDocument *pDocument);
	void ChangeGeneralDescription(const char *newtext);
	void ChangeText(const char *newtext);
	void ChangeText2(const char *newtext);
	void InitializePercentage(BOOL enable=TRUE);	// initialize percentage display in line 2 of text
	void ChangePercentage(int value);	// set percent display to a certain value
	BOOL Create();
	CDocument *m_pDocument;
private:
	int currentpercentage;
	// Dialog Data
	//{{AFX_DATA(CProgressMonit)
	enum { IDD = IDD_PROGRESS };
	//}}AFX_DATA

	// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CProgressMonit)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
