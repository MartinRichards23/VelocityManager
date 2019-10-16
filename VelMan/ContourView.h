// TGcontv.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CContourView view

class CContourView : public CScrollView
{
protected:
	CContourView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CContourView)

	CContourPlot *cp;
	CVelmanDoc *pDoc;
	// Attributes
public:
	CZimsGrid *grid, *orggrid;

	// internal button
private:
	CButton DoneButton;
	CSize totalsize;
	BOOL HaveWells;
	showspecialstuff_type showstuff;
	char **labeltext;
	double *labelx, *labely;
	// Operations
public:

	// Overrides
	afx_msg void OnClickedDoneButton();
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CContourView)
protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnInitialUpdate();     // first time after construct
	//}}AFX_VIRTUAL
	afx_msg void OnClickedShowFaults();
	afx_msg void OnClickedShowShotpoints();
	void OnClose();
	// Implementation
protected:
	virtual ~CContourView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CContourView)
	afx_msg void OnDestroy();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
