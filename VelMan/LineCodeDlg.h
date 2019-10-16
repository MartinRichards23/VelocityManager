// linecode.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLinecode2Coord dialog

class CLinecode2Coord : public CDialog
{
	// Construction
public:
	CLinecode2Coord(CWnd* pParent = NULL);	// standard constructor

	// Dialog Data
	//{{AFX_DATA(CLinecode2Coord)
	enum { IDD = IDD_RMS_COORDCONV2 };
	int		m_shot1;
	int		m_shot2;
	int		m_shot3;
	double	m_glbx1;
	double	m_glbx2;
	double	m_glbx3;
	double	m_glby1;
	double	m_glby2;
	double	m_glby3;
	int		m_lin1;
	int		m_lin2;
	int		m_lin3;
	//}}AFX_DATA

	CVelmanDoc *pDoc;

	// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CLinecode2Coord)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
