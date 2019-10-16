// CEditWellPointDlg dialog

class CEditWellPointDlg : public CDialog
{
	// Construction
public:
	CEditWellPointDlg(CWnd* pParent = NULL);	// standard constructor
	CVelmanDoc *pDoc;
	CMeasurePt *datapoint;		// data point to be edited
	BOOL DidChangePoint, DidChangeAnything;
	int layer;
	BOOL MayEdit; // can be FALSE, so that only display possible
	// Dialog Data
	//{{AFX_DATA(CEditWellPointDlg)
	enum { IDD = IDD_EDITWELL };
	BOOL	m_exclude;
	double	m_depth;
	double	m_time;
	double	m_weight;
	double	m_x;
	double	m_y;
	//}}AFX_DATA

	// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CEditWellPointDlg)
	afx_msg void OnChangePointData();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnEditwellExcl();
	afx_msg void OnEditwellLabelsmaller();
	afx_msg void OnEditwellLabellarger();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

