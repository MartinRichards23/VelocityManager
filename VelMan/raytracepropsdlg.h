#if !defined(AFX_RAYTRACEPROPSDLG_H__3EB0E6A0_4B08_11D5_A4D4_002018723DE7__INCLUDED_)
#define AFX_RAYTRACEPROPSDLG_H__3EB0E6A0_4B08_11D5_A4D4_002018723DE7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RayTracePropsDlg.h : header file
//

int GetRTregridbinsize(int index);

/////////////////////////////////////////////////////////////////////////////
// CRayTracePropsDlg dialog

class CRayTracePropsDlg : public CDialog
{
	// Construction

private:
	int cellratiosize;

public:
	CRayTracePropsDlg(CWnd* pParent = NULL);   // standard constructor

	// Dialog Data
	//{{AFX_DATA(CRayTracePropsDlg)
	enum { IDD = IDD_RAYTRACE_PROPS };
	BOOL	m_xyoffsetgrids;
	BOOL	m_depthdiffgrids;
	BOOL	m_anglegrids;
	double	m_limitanglevalue;
	BOOL	m_azimuthgrids;
	int		m_regridbinsize;
	//}}AFX_DATA

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRayTracePropsDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRayTracePropsDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RAYTRACEPROPSDLG_H__3EB0E6A0_4B08_11D5_A4D4_002018723DE7__INCLUDED_)
