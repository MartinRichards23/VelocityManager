// SpecADlg.h : header file
//
#include <afxcmn.h>

/////////////////////////////////////////////////////////////////////////////
// CSpecAnalysisDlg dialog

#define NUM_SLOTS 256*2

class CSpecAnalysisDlg : public CDialog
{
	// Construction
public:
	CSpecAnalysisDlg(CWnd* pParent = NULL);   // standard constructor
	CVelmanDoc *pDoc;
	CZimsGrid *grid;
	int first, last;	// first and last slot of cutting region
	double userFrom, userTo;
	double slotsize;
	double gmax, gmin;
	int DoRebuild;
	BOOL UseMinMax; // if true, removed values will be set to the min/max cutoff as appropriate, otherwise they will be nulled
	CString UnitName; // written upon exit (just for logfile entry)
	BOOL KnowUnit;
	BOOL SetAsPrimary;
private:
	int *slotfill, maxfill, secondfill;
	int totalpts;
	BOOL AreSliding;
	void PrintRange(double from, double to);
	int haveCaptured;
	CPoint MousePos;
	CRect BlockRect;

	// config vars
	bool initialised;
	int formWidth;
	int border_LR_IDOK;
	int border_LR_IDCANCEL;
	int border_RR_IDC_SPECANAL_GRAPH;
	int border_BB_IDC_SPECANAL_GRAPH;
	int border_RR_IDC_SPECANAL_INCL;
	int border_TB_IDC_SPECANAL_INCL;
	int border_RR_IDC_SPECANAL_NUMEXCL;
	int border_TB_IDC_SPECANAL_NUMEXCL;
	int border_TB_IDC_STATIC1;
	int border_TB_IDC_STATIC2;
	int border_TB_IDC_RADIO2;
	int border_TB_IDC_SPECANAL_REBUILD;

	// Dialog Data
	//{{AFX_DATA(CSpecAnalysisDlg)
	enum { IDD = IDD_SPECTR_ANALYSIS };
	// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSpecAnalysisDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSpecAnalysisDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	virtual void OnOK();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
};
