// velman.h : main header file for the VELMAN application
//

#ifndef __AFXWIN_H__
#error include 'stdafx.h' before including this file for PCH
#endif
#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CVelmanApp:
// See velman.cpp for the implementation of this class
//

class CVelmanApp : public CWinApp
{
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	CVelmanApp();

	DWORD GetProfileColor(CString section, CString entry, DWORD defaultval);
	// Overrides
	virtual BOOL InitInstance();
	int ExitInstance();

	BOOL FitInXandY;  // read from INI file during InitInstance()
	BOOL HaveVelocityCube; // set in InitInstance()
	BOOL HaveRaytraceLic;   // Yes, if we have a license to enable raytracing
	// 'license' may mean different things on different platforms

	CMultiDocTemplate *m_pTemplate1;	// for coarse model view
	CMultiDocTemplate *m_pTemplate2;	// for time grid contour plots
	CMultiDocTemplate *m_pTemplate3;	// for view with table of tying deviations
	CMultiDocTemplate *m_pTemplate4;	// fine tuning a model (2nd step in model creation)
	CMultiDocTemplate *m_pTemplate5;	// for graphical value table view
	CMultiDocTemplate *m_pTemplate6;	// for view with list of models
	CMultiDocTemplate *m_pTemplate7;	// for map of RMS shotpoint locations

	CString fileExtensions;
	BOOL OpendContSettings;
	// Implementation

	//{{AFX_MSG(CVelmanApp)
	afx_msg void OnAppAbout();
	afx_msg void OnFileOpen();
	afx_msg void OnFileNew();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
