// wellhorz.h : header file

/////////////////////////////////////////////////////////////////////////////
// CWellhorzTimegridMapDlg dialog

class CWellhorzTimegridMapDlg : public CDialog
{
	// Construction
public:
	CWellhorzTimegridMapDlg(CWnd* pParent = NULL);	// standard constructor
	CVelmanDoc *pDoc;
	CStringArray ZimsGridNameList, FaultGridNameList;
	BOOL usefaults;
private:
	int numlayers;          // set in OnInitDialog()
	CFont smallFont;
	void GrayUngray();

public:
	// Dialog Data
	//{{AFX_DATA(CWellhorzTimegridMapDlg)
	enum { IDD = IDD_WELLHORIZ_TIMEGRID_MAP };
	//}}AFX_DATA
	// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	void GrayNormalisation();
	// Generated message map functions
	//{{AFX_MSG(CWellhorzTimegridMapDlg)
	afx_msg void OnDblclkWelltimeGridlist();
	afx_msg void OnDeleteMapentry();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnDestroy();
	afx_msg void OnWelltimeUsefaults();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
