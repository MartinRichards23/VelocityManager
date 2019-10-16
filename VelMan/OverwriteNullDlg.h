#if !defined(AFX_OVERWRITENULL_H__3249C9BD_7253_4009_85A3_6E8D5353BDCC__INCLUDED_)
#define AFX_OVERWRITENULL_H__3249C9BD_7253_4009_85A3_6E8D5353BDCC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OverwriteNULL.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COverwriteNULL dialog

class COverwriteNULL : public CDialog
{
	// Construction
public:
	COverwriteNULL(CWnd* pParent = NULL);   // standard constructor
	CVelmanDoc *pDoc;
	int chosengrid;
	CWordArray entries;	// hi-byte: no in listbox, lobyte=gridno

	// Dialog Data
	//{{AFX_DATA(COverwriteNULL)
	enum { IDD = IDD_GRID_OVERWRITENULL };
	double	m_overwrite_value;
	//}}AFX_DATA

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COverwriteNULL)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COverwriteNULL)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OVERWRITENULL_H__3249C9BD_7253_4009_85A3_6E8D5353BDCC__INCLUDED_)
