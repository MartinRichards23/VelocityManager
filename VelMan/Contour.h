// options that can be set using SetContourOption

#define CNTOPT_STD_THICKNESS 0		// set thickness of most lines
#define CNTOPT_NTH_THICKNESS 1		// set thickness of every n-th line
#define CNTOPT_EVERY_NTH_THICK 2	// set how many lines to draw thick
#define CNTOPT_NUMLEVELS 3				// set number of levels
#define CNTOPT_LABELTYPESIZE 4		// font size of labels
#define CNTOPT_SHADELEVELS 5			// shade levels
#define CNTOPT_TEXTCOL 6					// set text color (use SetContOptLong!)
#define CNTOPT_LINECOL 7					// set line color (use SetContOptLong!)
#define CNTOPT_COARSENESS 8				// set coarseness of gridding
#define CNTOPT_LABELLING 9				// set standard/user labels
#define CNTOPT_LABELDOTSIZE 10		// set size of dots next to labels
#define CNTOPT_CREATEKEY 11				// enable key + set its coords
#define CNTOPT_UPSIDEDOWN 12			// enable/disable upside down plotting (std: no swap,
// but is necessary for coarse/fine contour previwe
// note: only the labels are upside down, it's some
// weird coordinate problem
#define CNTOPT_NORMALIZEGRADIENTS 13 // flatten the tps plane towards the edges
#define CNTOPT_EVERY_NTH_LABEL 14 // label every n-th contour line
#define CNTOPT_KEYTYPESIZE 100	    // size of contour key (default=-1=automatic)

// options for "contour settings" dialog
// if set, shows the appropriate checkbox
#define CNTDLG_SHOWWELLS	4
#define CNTDLG_SHOWFAULTS 8
#define CNTDLG_SHOWSHOTPOINTS	16
// return values of settings dialog
#define CNTDLG_NEEDREDRAW 1
#define CNTDLG_NEEDRESAMPLE 2
// the CNTDLG_SHOWxx values can also be return values!

// labelling types
#define CNT_AUTOLABEL	1					// label next to most lines
#define CNT_USERLABEL 2						// user-defined labels

// parts of "con.h"
#define DOWN	'd'		/* approximate direction of gradient */
#define LEFT	'l'
#define UP	'u'
#define RIGHT	'r'

typedef struct data             /* data description */
{
	short nx;                     /* dataset size in X direction */
	short ny;                     /* dataset size in Y direction */
	double **cells;		/* array of cells */
}
Data;

typedef struct terrain		/* shape of data */
{
	BOOL **hori;			/* array: contour passes below? */
	BOOL **vert;			/* array: contour passes to the left? */
	double **hy;			/* array: offset of contour below */
	double **vx;			/* array: offset of contour to the left */
}
Terrain;

typedef struct showspecialstuff_type
{
	BOOL showwells;
	BOOL showfaults;
	BOOL showshotpoints;
} showspecialstuff;

typedef struct level		/* contour level description */
{
	double height;			/* height of contour level */
	double shade;			/* 0 <= shade <= 1 */
	short linewidth;		/* width (in pts) of contour */
	BOOL dash;			/* draw this contour with dashed lines? */
	BOOL label;			/* label this contour? */
}
Level;

typedef struct plot		/* plot description */
{
	double x0, y0;        /* coords of bottom left corner */
	double x1, y1;        /* coords of top right corner */
	double xu, yu;        /* half the real dimensions of cells */
	double lowest_shade;		/* shade of lowest Z value (0 -> black) */
	double highest_shade;		/* shade of highest Z value (1 -> white) */
	int type_size;		/* type size (in pts) of labels */
	short nz;                     /* number of contour levels */
	Level *levels;		/* array of contour levels */
}
Plot;

typedef enum			/* directions for GetNext motion */
{
	North,
	NorthEast,
	East,
	SouthEast,
	South,
	SouthWest,
	West,
	NorthWest,
	NumDirections			/* number of directions (probably 8) */
}
Direction;

typedef enum
{
	Horizontal, // contour passes below current point
	Vertical // contour passes to the left of current
}
Attitude;

#define NOP(d)  ((d) % NumDirections)

typedef struct state
{
	Attitude attitude;            /* Vertical or Horizontal? */
	BOOL to_label;                /* we want to label this contour */
	Direction direction;          /* direction of motion */
	short ii;                     /* X coord of current point on path */
	short jj;                     /* Y coord of current point on path */
}
State;

#define MAX_LAB 200             /* max number of labels before dumping */
typedef struct label
{
	double xlab[MAX_LAB];          /* list of X coords of labels */
	double ylab[MAX_LAB];          /* list of Y coords of labels */
	short klab[MAX_LAB];          /* list of label values for xlab, ylab */
	short escapement[MAX_LAB];    /* angle for CreateFont() function */
	short nlab;                   /* number of labels printed so far */
}
Label;

#define ABS(x)  (x >= 0 ? x : -x)

//*************************************************************************
// class:  CContourPlot
// stores information about a contour plot anywhere in any window, and
// knows how to interact with a ZimsGrid object to grid a list of (x, y, z)
// values. Also knows how to draw its own contour (also zoomed)
// It creates a local copy of the raw data
//*************************************************************************

class CContourPlot// : CObject
{
public:
	BOOL InitRawData(int n, double *x, double *y, double *z);
	void CleanUp();
	BOOL ResampleData(double rx1, double ry1, double rx2, double ry2);
	void InitResampledData(CZimsGrid *zimsgrid);
	BOOL DrawContourPlot(CDC *dc, int left, int top, int right, int bottom);
	void DrawAxisLabels(CDC *dc, int left, int top, int right, int bottom);
	void DrawWellLabels(CDC *dc, int left, int top, int right, int bottom);
	void DrawBox(CDC *dc, int left, int top, int right, int bottom, CZimsGrid *grid);
	void SetContourLevels(int num, Level *kp);
	void SetContourLabels(int num, double *x, double *y, char **lab);
	void SetContourLabels(int num, double *x, double *y, char *opt, char **lab);
	BOOL SetContourOption(int option, int intvalue);
	BOOL SetContourOption(int option, long longvalue);
	BOOL SetContourOption(int option, BOOL flag, double floatvalue);
	int  ContourOptionsDialog(int option, showspecialstuff_type *showspecialstuff=NULL);
	int  GetLabelOptions() { return labelling; }
	BOOL GetColourShading() { return ColorShading; }
	void CContourPlot::ForceContSetToClose(void);

private:
	void DrawContourKey(Plot *pp);
	void DrawPlot (Plot *pp, Data *dp);
	void AllocTerrain (short nx, short ny, Terrain *tp);
	void FreeTerrain (short nx, short ny, Terrain *tp);
	void MapTerrain (short k, Plot *pp, Data *dp, Terrain *tp);
	BOOL GetNext (short nx, short ny, Terrain *tp, State *s);
	void MainLoop (Plot *pp, Data *dp);
	void DrawLabels (double type_size, Label *lp, Level *kp);
	void DrawContour (Attitude start, Direction direction, short i, short j,
		short k, Label *lp, Plot *pp, Data *dp, Terrain *tp);
	COLORREF InterpolateHSV(double val, double min, double max);
	BOOL upside;
	BOOL normalizegradients;
	double normgradfac;

public:
	CContourPlot(CVelmanDoc *pDoc);
	~CContourPlot();
	/*
	~CContourPlot()
	{
	CleanUp();
	}*/

private:
	CVelmanDoc *pDoc;
	BOOL HaveData, HaveResampled, HaveIndividualLevels;
	BOOL HaveDownsampledCopy;
	BOOL CanAdjustCorseness;
	BOOL MayDeleteGrid;
	double **rawxy;
	double *rawz;
	int npts;
	CZimsGrid *grid;
	int NumRows, NumCols;
	int numuserdeflevels;
	Level *userdeflevels;
	double *labelx, *labely;
	char *labelopt;
	char **labeltext;
	int labelnum;
	int labelling;	// whether automatic, user, or both
	double x1, y1, x2, y2; // min/max grid coords
	CVelmanPalette  *velpal; // custom colour palette for this plot.
public:
	double zmin, zmax;	// max. range of values
	int dotsize;		// size of dots next to label.
private:
	int ysize;		// size of output window in y direction
	int NumLevels;
	int StdLineThickness;
	int NthLineThickness;
	int EveryNthThick;
	int EveryNthLabel;
	int LabelTypeSize, KeyTypeSize;
	int ShadeLevels;
	int coarseness;
	BOOL dokey;
	int keyx, keyy;
	BOOL AutoKeyTypeSize;
	CDC *pDC;
	COLORREF linecol;

	double smallest_maxmin;

	//  static member variables used to preserve contour options
	//  between invocations of the CContourPlot class.

	static int ColourPalette;  // Controls choice of colour palette.
	static int RequestedNumLevels;
	static COLORREF textcol;
	static BOOL ColorShading;
	static int NumLabels;   // controls how many labels will be plotted
};
