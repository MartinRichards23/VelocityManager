// FTM_P.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFinetuneMisc_Page dialog

#include "Funclib.h"	// Added by ClassView
class CFinetuneMisc_Page : public CPropertyPage
{
	DECLARE_DYNCREATE(CFinetuneMisc_Page)

	// Construction
public:
	CFinetuneMisc_Page(CFinetuneSheet *mysheet);
	CFinetuneMisc_Page();
	~CFinetuneMisc_Page();
	CFinetuneSheet *sheet;
	void UpdateButtons();
	void GrayStuff();
	CVelmanDoc *pDoc;
	int layer, model;
private:
	CFineTuneView *pView;
	CHorizon *horizon;

	// Dialog Data
	//{{AFX_DATA(CFinetuneMisc_Page)
	enum { IDD = IDD_FINETUNE_MISC_PAGE };
	BOOL	m_RebuildDepth;
	//}}AFX_DATA
private:

	// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CFinetuneMisc_Page)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
public:
	BOOL GetRebuildDepth();
	// Generated message map functions
	//{{AFX_MSG(CFinetuneMisc_Page)
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnFinetuneHideme();
	afx_msg void OnFinetuneShowgrid();
	afx_msg void OnFinetuneShowwelllabels();
	virtual BOOL OnInitDialog();
	afx_msg void OnFinetuneDoedit();
	afx_msg void OnFinetuneSwitchviewmode();
	afx_msg void OnPaint();
	afx_msg void OnClose();
	afx_msg void OnFinetuneKeepresiduals();
	afx_msg void OnFinetuneShowallwells();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
