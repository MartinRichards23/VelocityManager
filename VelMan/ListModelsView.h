// listofmo.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CListOfModelsView view

class CListOfModelsView : public CScrollView
{
	DECLARE_DYNCREATE(CListOfModelsView)
protected:
	CListOfModelsView();			// protected constructor used by dynamic creation
	CVelmanDoc* pDoc;

	// Attributes
public:

	// internal button
private:
	CButton DoneButton;

	// Operations
public:

	// Implementation
protected:
	virtual ~CListOfModelsView();
	virtual	void OnDraw(CDC* pDC);		// overridden to draw this view
	virtual	void OnInitialUpdate();		// first time after construct
	virtual void OnUpdate(CView *pSender, LPARAM lHint, CObject *pHint);

	// Generated message map functions
	//{{AFX_MSG(CListOfModelsView)
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	afx_msg void OnClickedDoneButton();
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
