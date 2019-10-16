// CFFTDirectionDlg dialog

class CFFTDirectionDlg : public CDialog
{
	// Construction
public:
	CFFTDirectionDlg(CWnd* pParent = NULL);   // standard constructor

	// Dialog Data
	//{{AFX_DATA(CFFTDirectionDlg)
	enum { IDD = IDD_FFT_DIRECTION };
	int		m_direction;
	//}}AFX_DATA

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFFTDirectionDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFFTDirectionDlg)
	// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
