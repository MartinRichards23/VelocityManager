// CDepthConvertTimesliceDlg dialog

class CDepthConvertTimesliceDlg : public CDialog
{
	// Construction
public:
	CDepthConvertTimesliceDlg(CWnd* pParent = NULL);   // standard constructor

	// Dialog Data
	//{{AFX_DATA(CDepthConvertTimesliceDlg)
	enum { IDD = IDD_DEPTHCONV_TIMESLICE };
	double	value;
	//}}AFX_DATA

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDepthConvertTimesliceDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDepthConvertTimesliceDlg)
	// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
