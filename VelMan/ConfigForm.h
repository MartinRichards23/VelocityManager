#if !defined(AFX_ConfigForm_H__C2809B77_AC87_4860_8637_D24A7B46C7BA__INCLUDED_)
#define AFX_ConfigForm_H__C2809B77_AC87_4860_8637_D24A7B46C7BA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ConfigForm.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// ConfigForm dialog

class ConfigForm : public CDialog
{
	// Construction
public:
	ConfigForm(CWnd* pParent = NULL);   // standard constructor
	int GetBorder(void);
	int GetSideBorder(void);
	// Dialog Data
	//{{AFX_DATA(ConfigForm)
	enum { IDD = IDD_ConfigForm_DIALOG };
	// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ConfigForm)
public:
#if _MSC_VER < 1400			// for MS Studio older than 2005
	virtual INT DoModal();
#else
	virtual INT_PTR DoModal();
#endif
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(ConfigForm)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnCancelMode();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ConfigForm_H__C2809B77_AC87_4860_8637_D24A7B46C7BA__INCLUDED_)
