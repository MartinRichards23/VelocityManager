// FT_Sheet.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFinetuneSheet

#include "Funclib.h"	// Added by ClassView
class	CFinetuneFitting_Page;
class	CFinetuneCustomfit_Page;
class	CFinetuneWeight_Page;
class	CFinetuneMisc_Page;
class	CFinetuneQuickhelp1_Page;
class	CFinetuneQuickhelp2_Page;
class	CFinetuneResidualsPage;

class CFinetuneSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CFinetuneSheet)

	// Construction
public:
	CFinetuneSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CFinetuneSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CFinetuneFitting_Page *fitpage;
	CFinetuneCustomfit_Page *customfitpage;
	CFinetuneResidualsPage *residualpage;
	CFinetuneWeight_Page *weightpage;
	CFinetuneMisc_Page *miscpage;
	CFinetuneQuickhelp1_Page *qh1page;
	CFinetuneQuickhelp2_Page *qh2page;
	// Attributes
public:
	int layer;
	CVelmanDoc *pDoc;
	CFrameWnd *pFineTuneView;
	void Init();
	void OnOK();
	void OnCancel();
	void Hideme();
	void ShowFittingParams();
	void OtherModel();
	BOOL HaveDisplayedResidualsPage; // due circumvene a bug - may be ignored
private:
	void CleanUp();
	// Operations
public:

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFinetuneSheet)
	//}}AFX_VIRTUAL

	// Implementation
public:
	BOOL GetRebuildDepth();
	virtual ~CFinetuneSheet();

	// Generated message map functions
protected:
	//{{AFX_MSG(CFinetuneSheet)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
