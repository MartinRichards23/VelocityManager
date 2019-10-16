// TT_View.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWellDataView view

class CWellDataView : public CScrollView
{
protected:
	CWellDataView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CWellDataView)

	// Attributes
public:
	int chosentype;
	CVelmanDoc *pDoc;
	// Operations
public:
	// internal button
private:
	CButton DoneButton, DumpButton;
	int spacing;
	int WellDist;
	int lineHtMult;
	int SeperationDist;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWellDataView)
protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnInitialUpdate();     // first time after construct
	//}}AFX_VIRTUAL

	// Implementation
protected:
	virtual ~CWellDataView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CWellDataView)
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	afx_msg void OnClickedDoneButton();
	afx_msg void OnClickedDumpButton();
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
