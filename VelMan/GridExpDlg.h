#if !defined(AFX_CHOOSEEXPORT_H__2216E121_31CE_4EB8_B311_0CCD0134D25B__INCLUDED_)
#define AFX_CHOOSEEXPORT_H__2216E121_31CE_4EB8_B311_0CCD0134D25B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ChooseExport.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CChooseExport dialog

class CGridExportDlg : public CDialog
{
	// Construction
public:
	CGridExportDlg(CWnd* pParent = NULL);   // standard constructor
	CVelmanDoc *pDoc;
	int *chosengrids;
	int numgrids;
	int OutputFileType;
	CWordArray entries;	// hi-byte: no in listbox, lobyte=gridno

	// Dialog Data
	//{{AFX_DATA(CChooseExport)
	enum { IDD = IDD_GRIDEXPORT };
	// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChooseExport)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CChooseExport)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHOOSEEXPORT_H__2216E121_31CE_4EB8_B311_0CCD0134D25B__INCLUDED_)
