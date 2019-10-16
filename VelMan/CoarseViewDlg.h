// cormodvw.h : header file

/////////////////////////////////////////////////////////////////////////////
// CCoarseModelView view

class CCoarseModelView : public CScrollView
{
	DECLARE_DYNCREATE(CCoarseModelView)
protected:
	CCoarseModelView();			// protected constructor used by dynamic creation
	CHorizon *horizon;	// set during OnInitialUpdate, using information from document temporary variable
	// ActiveLayer that contains the number of the layer currently worked on
	CFrameWnd *pCoarseModelFrame;	// ptr to the views frame window; set by mainframe
	// just after opening the view
public:
	int ViewMode;	// 1=model functions in scatter diag, 2=residual contour plot
	// set and read by CModelChoiceDlg, depending on "Mode switch" button
	// initialized in both OnInitialUpdate and OnInitDialog, both set it to 1
	CButton DlgBackButton;  // button to bring back hidden control dlg

private:
	void SetProjectFlags();	// sets flags in document (see function itself)
	BOOL HaveValidContour[4];	// TRUE if contour in a window has been resampled
	CVelmanDoc *pDoc;		// set during OnInitialUpdate
	CContourPlot *cp[4];
	double *contx, *conty;	// (x, y) positions of the points in the contour plot
	double *residual;	// list of residual values
	double contx1, contx2, conty1, conty2; // frame for contour plot (from time grid if available)
	char **labeltext[4];
	double *labelx[4], *labely[4];
	// Attributes
public:

	// Operations
public:

	// Implementation
protected:
	virtual ~CCoarseModelView();
	virtual	void OnDraw(CDC* pDC);		// overridden to draw this view
	virtual void OnInitialUpdate();
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	afx_msg void OnClickedDlgBackButton();

	// Generated message map functions
protected:
	//{{AFX_MSG(CCoarseModelView)
	afx_msg void OnDestroy();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
