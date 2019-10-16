// ContSet.h : header file

#include <afxcmn.h>
/////////////////////////////////////////////////////////////////////////////
// CContourSettingsDlg dialog

class CContourSettingsDlg : public CDialog
{
	// Construction
public:
	CContourSettingsDlg(CWnd* pParent = NULL);   // standard constructor
	int m_thicknum;		// number of thin lines between each thick line
	int m_labelsize;
	COLORREF m_textcol;
	double minval, maxval;
	COLORREF mincol, maxcol;
	int options;
	int furtheroptions;
	showspecialstuff_type *showstuff;
	int m_labelnum;
	void sQuit();
	// Dialog Data
	//{{AFX_DATA(CContourSettingsDlg)
	enum { IDD = IDD_CONTOUR_SETTINGS };
	int   m_numlabels;
	int	  m_numlines;
	BOOL  m_showwells;
	BOOL  m_showfaults;
	BOOL  m_showshotpoints;
	int		m_palette;
	BOOL	m_showlabels;
	BOOL	m_colorshading;
	//}}AFX_DATA

	void ShowLabelSize();
	void GrayItems();

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CContourSettingsDlg)
public:
	virtual void OnFinalRelease();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CContourSettingsDlg)
	afx_msg void OnContsetChangetextcol();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnPaint();
	afx_msg void OnContsetShowlabels();
	afx_msg void OnContsetLabelsmaller();
	afx_msg void OnContsetLabellarger();
	afx_msg void OnContsetNumlevels();
	afx_msg void OnContsetNumlevsLow();
	afx_msg void OnContsetNumlevsNorm();
	afx_msg void OnContsetNumlevsHigh();
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnClose();
	afx_msg void OnContsetShowfaults();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
};
