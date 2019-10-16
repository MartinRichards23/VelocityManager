// DesGrid.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDesignateGridDlg dialog

class CDesignateGridDlg : public CDialog
{
	// Construction
public:
	CDesignateGridDlg(CWnd* pParent = NULL);   // standard constructor
	int chosengrid;
	int newhorizon;
	CVelmanDoc *pDoc;
	CWordArray entries;	// hi-byte: no in listbox, lobyte=gridno

	// Dialog Data
	//{{AFX_DATA(CDesignateGridDlg)
	enum { IDD = IDD_DESIGNATE_GRID };
	// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDesignateGridDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDesignateGridDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSelchangeDesgridList();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
