// finetune.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFineTuneView view

class CFineTuneView : public CScrollView
{
	DECLARE_DYNCREATE(CFineTuneView)
protected:
	CFineTuneView();			// protected constructor used by dynamic creation
	CContourPlot *cp;

	// Attributes
public:
	int layer;			// set by controlling dialog
	CHorizon *horizon;	// set in OnInitialUpdate
	int model;					// ditto
	int numpts;		// number of valid points in this layer (might be < wellnumber)
	int numallocpts;	// numpts that we started with (so we know how much to deallocate)
	double *xval, *yval, *z1;
	int *wellindex;
	int *ptx, *pty;
	CMeasurePt **datapoint;
	CButton DlgBackButton;  // button to bring back hidden control dlg
	int ViewMode;
	BOOL ShowGridLines, ShowWellLabels, ShowAllWells; // controlled from the dialog
	int WellUnderCursor;
	int cxplotmargin, cyplotmargin;		// margin to leave around the contour plot (in device window coordinates)
	int winsize;			// size of window to use for scatter/contour plots. e.g. contour plot will therefore
	// occupy : cplotsize - 2*cplotmargin

	BOOL HaveValidContour;	// TRUE if contour plot only needs resampling+display, but no init
	double *residual;	// list of residual values

private:
	double minx, maxx, miny, maxy;	// scale of the graph (set in OnInitialUpdate)

	Dimensions TotalDim; // dimension in space of plot, set in OnIntialUpdate. min/max of grid AND wells
	//double minpx, minpy, maxpx, maxpy;
			
	Dimensions CurrentDim; // current dimesions of plot (might be smaller than minpx.. if zoomed)
	//double mincx, mincy, maxcx, maxcy;

	//Dimensions FrameDim; // frame for contour plot (from time grid if available)
	double contx1, contx2, conty1, conty2; 
	
	double *contx, *conty;	// (x, y) positions of the points in the contour plot
	char **labeltext;
	double *labelx, *labely;
	CVelmanDoc *pDoc;	// set during OnInitialUpdate()
	// Operations
	void RescaleYAxis();
	BOOL SuppressDisplay;	// TRUE if NO points selected at all in exp. fitting
	// graph is suppressed then
	CFont WellNameFont;
public:
	int ZoomDepth;  // must not zoom more than 5 times
	void SetContLimits();

	// Implementation
protected:
	virtual ~CFineTuneView();
	virtual	void OnDraw(CDC* pDC);		// overridden to draw this view
	virtual	void OnInitialUpdate();		// first time after construct
	virtual void OnUpdate(CView *pSender, LPARAM lHint, CObject *pHint);
	void CopyLayer();	// copies data from CHorizon object into internal list
	afx_msg void OnClickedDlgBackButton();

	// Generated message map functions
	//{{AFX_MSG(CFineTuneView)
	afx_msg void OnDestroy();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
