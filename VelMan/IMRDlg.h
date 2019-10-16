#if !defined(AFX_IMRDIALOG_H__824BCA40_280C_11D3_8854_0080C7C10C6E__INCLUDED_)
#define AFX_IMRDIALOG_H__824BCA40_280C_11D3_8854_0080C7C10C6E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// IMRDialog.h : header file
//
class CVelmanDoc;

////////////////////////////////////////////////////////////////////////////
// CIMRDialog dialog

class CIMRDialog : public CDialog
{
	// Construction
public:
	CIMRDialog(CWnd* pParent = NULL);   // standard constructor
	CVelmanDoc *pDoc;
	CWordArray entries;
	int timegrid;
	int smoothwindow;
	BOOL found;

	// Dialog Data
	//{{AFX_DATA(CIMRDialog)
	enum { IDD = IDD_IMRDIALOGUE };
	// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIMRDialog)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CIMRDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual void OnOK();
	afx_msg void OnDblclkTimelist();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IMRDIALOG_H__824BCA40_280C_11D3_8854_0080C7C10C6E__INCLUDED_)
