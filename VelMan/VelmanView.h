// velmavw.h : interface of the CVelmanView class
//
/////////////////////////////////////////////////////////////////////////////

class CVelmanView : public CView
{
protected: // create from serialization only
	CVelmanView();
	DECLARE_DYNCREATE(CVelmanView)

private:
public:
	CVelmanDoc* GetDocument();

	// Operations
public:

	// Implementation
public:
	virtual ~CVelmanView();
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual void OnUpdate(CView *pSender, LPARAM lHint, CObject *pHint);
	virtual	void OnInitialUpdate();		// first time after construct
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

	// Printing support
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

	// Generated message map functions
protected:
	//{{AFX_MSG(CVelmanView)
	// NOTE - the ClassWizard will add and remove member functions here.
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in velmavw.cpp
inline CVelmanDoc* CVelmanView::GetDocument()
{ return (CVelmanDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////
