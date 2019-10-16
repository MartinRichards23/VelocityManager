// Map1View.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMapOfShotpointsView view

class CMapOfShotpointsView : public CScrollView
{
protected:
	CMapOfShotpointsView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CMapOfShotpointsView)

	CVelmanDoc *pDoc;
	// Attributes
public:
	CZimsGrid *grid;

	// internal button
private:
	CButton DoneButton, ShowZnonBox;
	CSize totalsize;
	BOOL ShowZnon;
	CFont smallfont;
	int **znon;

	// Operations
public:

	// Overrides
	afx_msg void OnClickedDoneButton();
	afx_msg void OnClickedShowZnon();
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMapOfShotpointsView)
protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnInitialUpdate();     // first time after construct
	//}}AFX_VIRTUAL

	// Implementation
protected:
	virtual ~CMapOfShotpointsView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CMapOfShotpointsView)
	afx_msg void OnDestroy();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
