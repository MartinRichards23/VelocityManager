#ifdef VELMAN_UNIX
#include <fstream.h>
#include <iostream.h>
#include <iomanip.h>
#else
#include <fstream>
#include <iostream>
#include <iomanip>
#endif
#include <ctype.h>
#include <math.h>
#include <float.h>
#include <time.h>
#include <assert.h>

#ifndef VELMAN_UNIX
using namespace std;
#endif

#include "nrutil.h"
#include "distmat.h"
#include "cmntypes.h"
#include "global.h"
#include "faultcub.h"
#include "helper.h"
#include "zims_rec.h"
#include "unixmfd.h"
#include "unixcp3.h"
#include "tpswrap.h"

typedef double ** doublePointerPointer;  // keep Unix compiler happy

#ifndef __STDAFX_H__
#include "stdafx.h"
#endif

// possible types of rms data
#define ZIMS_TYPE_POLY        4
#define ZIMS_TYPE_LINE        2
#define ZIMS_TYPE_GRID        1
#define CP3_TYPE_GRID        -6
#define CP3_TYPE_LINE        -7
#define CP3_TYPE_FAULT       -8

// parameters for the resampling of the rms data
#define ZIMS_RMSINTERVAL             200.0
#define ZIMS_MINRESAMPLEHORIZONS      10
#define ZIMS_RMSRESAMPLEINTERVALS	    6

// number of digits to round an imported grid down to
#define ZIMS_ROUNDIMPORTEDGRIDS_DOWNTO 1000.0 // 10^x means x digits precision

// ways of autocorrecting a grid (see CZimsGrid::AutoCorrect)
#define ZIMS_AUTOCORRECT_SMART 0
#define ZIMS_AUTOCORRECT_HIGHERZNON 1
#define ZIMS_AUTOCORRECT_LOWERZNON 2
#define ZIMS_AUTOCORRECT_BOTHZNON 3

// Decides which interpolation routine is used in CZimsGrid::Evaluate
// at the moment a tps-type evaluation
#ifndef  ZIMS_TPSEVAL
#define  ZIMS_TPSEVAL
#endif

// this stuff needed for MFD file handling, defined in helper.cxx
extern int meta_file_head_sec[];
extern int file_head_sec[];
extern int file_info_sec[];
extern int three_numbers_sec[];
extern int zims_grid_sec[];
extern int zims_para_sec[];
extern int zims_field_sec[];
extern int one_word_sec[];
extern int one_dword_sec[];
extern int zims_header_sec[];

// this stuff needed for CP3 file handling, defined in helper.cxx
extern int cp3_file_head_sec[];
extern int cp3_grid_sec[];
extern int cp3_control_sec[];
extern int cp3_fault_sec[];

// defines for differentiation routine
#define CON  1.4
#define CON2 (CON*CON)
#define NTAB 10
#define SAFE 2.0

//	parameter block for CP3int format grid file
//	can't make this part of the class structure because of the way
//	velman creates exportable grids. Also, we always want to make sure
//	(for now at least), that we have the latest header from grids that
//	velman imports.
extern char *cp3int_parm;

//*************************************************************************
// Class:				CZimsGrid
// Arguments:   none
// Description:	store and manipulate resampled Seismic data
// Variables:   rows, columns, horizons  :  dimension of the grid
//              xlim, ylim  : minima and maxima of the grid
//              ***grid     : effective grid values
//              names       : name for each horizon
//*************************************************************************

class CZimsGrid : public CObject
{
	friend CZimsGrid * operator + (CZimsGrid &arg1, CZimsGrid &arg2);
	friend CZimsGrid * operator - (CZimsGrid &arg1, CZimsGrid &arg2);
	friend CZimsGrid * operator * (CZimsGrid &arg1, CZimsGrid &arg2);
	friend CZimsGrid * operator / (CZimsGrid &arg1, CZimsGrid &arg2);

	friend void UnPad(char *string, int length);
	friend void Pad(char *string, int length);
	friend void zero_mem(char *ptr, int length);
	friend void InvertData(char *data, int section[]);

private :
	CVelmanDoc *pDoc;
	CFaultGrid *faultgrid;
	int faultid;

	CString name;
	char unitname[16], type_name[16];

	int **indexgrid;
	double **grid;
	int gridid;
	int znonvals;

	double **p, *c;
	double avg;
	UBYTE type;
	streampos gridpos;
	time_t last_used;

	BOOL ValidPoint(int ni, int nj);
	BOOL PlacePoint(double x, double y, int mi, int mj, int *vector);
	int *GetInterpolationPoints(int &points, double x, double y);
	double *Interpolate(int order, int points, int ni, int nj);
	BOOL MakeRawName(CString &grawname);

	BOOL WriteZimsFile_ZMAP(CString);
	BOOL WriteZimsFile_MFD(CString &filename0, CString &filename1, CString);
	BOOL WriteZimsFile_CP3(CString);
	BOOL WriteZimsFile_CP3int(CString);
	BOOL WriteZimsFile_EV(CString);
	BOOL WriteZimsFile_ZmapISO(CString);

	double dfridr(double x, double y, double hx, double hy, double *err);

	int rows, columns;
	double xlim[2], ylim[2];
	double dx, dy;
	double znon;

	int horizon;
	int unit_timepower;		// power of t in the units of this grid
	int unit_distpower;		// power of x in the units of this grid

public:

	CZimsGrid(CVelmanDoc *doc) : CObject()
	{
		rows = columns = 0;
		xlim[0] = xlim[1] = ylim[0] = ylim[1] = CSD_ILLEGAL;
		dx = dy = CSD_ILLEGAL;
		type = 0;
		horizon = gridid = -1;
		grid = NULL; indexgrid = NULL; c = NULL; p = NULL;
		znon = ZIMS_VAL_ZNON;
		pDoc = doc;
		gridpos = 0L;
		faultgrid = NULL;
		faultid = -1;
		znonvals = -1;
		avg = -FLT_MAX;
		unit_timepower=unit_distpower=0;
		time(&last_used);
	}

	CZimsGrid(void) : CObject()
	{
		rows = columns = 0;
		xlim[0] = xlim[1] = ylim[0] = ylim[1] = CSD_ILLEGAL;
		dx = dy = CSD_ILLEGAL;
		type = 0;
		horizon = gridid = -1;
		grid = NULL; indexgrid = NULL; c = NULL; p = NULL;
		znon = ZIMS_VAL_ZNON;
		pDoc = NULL;
		gridpos = 0L;
		faultgrid = NULL;
		faultid = -1;
		znonvals = -1;
		avg = -FLT_MAX;
		unit_timepower=unit_distpower=0;
		time(&last_used);
	}

	~CZimsGrid() { ClearContents(); }
	void ClearContents(void);
	BOOL Init(int nrows, int ncolumns, CVelmanDoc *document, UBYTE ntype = CSD_TYPE_UNKNOWN, double znon = ZIMS_VAL_ZNON);
	void WriteLimits(double *x, double *y);

	CString &GetName(void) { return name; }
	void SetName(CString newname);
	CVelmanDoc *GetDocument(void) { return pDoc; }
	int GetHorizon(void) { return horizon; }
	void SetHorizon(int nhor);
	int GetID(void) { return gridid; }
	void SetID(int newid);
	int GetFaultID(void) { return faultid; }
	void SetFaultID(int newid) { faultid = newid; }
	int **GetIndexGrid(void) { return indexgrid; }
	double **GetGrid(void);
	int GetRows(void) { return rows; }
	int GetColumns(void) { return columns; }
	double *GetXlimits(void) { return xlim; }
	double *GetYlimits(void) { return ylim; }
	double Getdx(void) { return dx; }
	double Getdy(void) { return dy; }
	BOOL QueryLimits(double x, double y);
	UBYTE GetType(void) { return type; }
	void SetType(UBYTE ntype);
	void SetType(void);
	void SetUnit(void);
	double GetZnon(void) { return znon; }
	BOOL GetZnon(double val);
	int GetZnonVals(void) { return znonvals; }
	void ClearResample(void) { free_dvector(c, 1, 0); c = NULL; free_dmatrix(p, 1, 0, 1, 2); p=NULL; }
	double GetAvg(void) { return avg; }
	int GetUnitTimePower(void) { return unit_timepower; }
	int GetUnitDistPower(void) { return unit_distpower; }
	void SetUnitTimePower(int newpower) { unit_timepower=newpower; }
	void SetUnitDistPower(int newpower) { unit_distpower=newpower; }
	double X(int nj) { assert( xlim[0]+nj*dx <= xlim[1] ); return (xlim[0] + nj*dx); }
	double Y(int ni) { assert( ylim[1]-ni*dy >= ylim[0] ); return (ylim[1] - ni*dy); }
	CFaultGrid *GetFaultGrid(void) { return faultgrid; }
	int FindNewID(void);
	void Link(CFaultGrid *nfltgrid);
	void RoundAllGridnodes();

	void CopyAndDownsample(CZimsGrid &oldgrid);
	int  DownsampleTargetsize();
	BOOL ShouldDownsample() { return(rows*columns>DownsampleTargetsize()); }
	BOOL IsAllNull() { CalcAverage(); return(rows*columns<=znonvals); }

#ifdef VELMAN_UNIX
	BOOL Read(ifstream &infile, CVelmanDoc *document);
	BOOL Write(ofstream &outfile);
#else
	BOOL Read(std::ifstream &infile, CVelmanDoc *document);
	BOOL Write(std::ofstream &outfile);
#endif
	BOOL WriteASCII(CString);
	BOOL WriteZimsFile(int, CString);
	void Stamp(void) { time(&last_used); }

	void OverwriteNULL(double val);
	void Transpose(void);
	BOOL FFT2D(int direction);
	void FFT2Amp(void);
	void IndexGrid(void);
	void RandomIndexGrid(void);
	void UnIndexGrid(void);
	BOOL CleanGrid(int points);
	void LowPassFilter(double);
	void PatchInSomeOldValues(CZimsGrid *original);
	BOOL Resample(int n, int stepsize, double *f, double **p, BOOL normgrad=FALSE, double normgradfac=50.0);
	double CalcAverage(void);
	double CalcMaxVal(void);
	double CalcMinVal(void);
	double Evaluate(double x, double y);
	BOOL TieGrid(BOOL normgrad=FALSE, double normgradfac=50.0, BOOL changename=TRUE);
	void ConvertToMilliseconds();
	void ApplyCutoff(double min, double max, BOOL UseMinMax);
	void ApplyCutoffAverage(double percent, bool UseMinMax);
	void AutoCorrect(int how, CZimsGrid *withRespectTo);
	void LimitGrid(CZimsGrid *withRespectTo);
	char *GetUnitName(void);
	char *GetTypeName(void);
	void SmoothMovingWindow(CZimsGrid *fromgrid, int windowsize);
	void SmoothMovingWindow2(CZimsGrid *fromgrid, double length, int minSmoothNodes);
	void UseProperZnon(void);
	void CopyZnon(CZimsGrid &othergrid);
	double *FindNormal(double x, double y);

	BOOL HasFaultGrid(void) 
	{
		if(faultgrid)
			return TRUE;
		else return FALSE;
	}

	operator int () { return GetHorizon(); }
	operator double () { return GetAvg(); }
	operator doublePointerPointer() { return GetGrid(); }

	operator CFaultGrid *() { return faultgrid; }
	BOOL operator == (CZimsGrid &other);
	BOOL operator != (CZimsGrid &other);
	int operator < (CZimsGrid &other);
	CZimsGrid & operator = (CZimsGrid &oldgrid);
	CZimsGrid & operator = (double val);
	CZimsGrid & operator << (CZimsGrid &oldgrid);
	CZimsGrid & operator += (CZimsGrid &arg1);
	CZimsGrid & operator -= (CZimsGrid &arg1);
	CZimsGrid & operator *= (CZimsGrid &arg1);
	CZimsGrid & operator /= (CZimsGrid &arg1);
	CZimsGrid & operator *= (double val);
	CZimsGrid & operator /= (double val);
};

//*************************************************************************
// Class:				CZimsCube
// Arguments:   none
// Description:	Array of resampled Seismic horizons
// Variables:   none
//*************************************************************************
class CZimsCube : public CObArray
{
private:
	CVelmanDoc *pDoc;

	BOOL CreateGrids(void);
	BOOL SplineAvgHorizons(CZimsGrid **&timegrid, CZimsGrid **&rawvelgrid, CZimsGrid **&rmshorizon,
		int horizons, int rmshorizons, double rmsinterval);

public:
	BOOL keeptieslices;

	CZimsCube(CVelmanDoc *document) : CObArray() 
	{
		keeptieslices = FALSE;
		pDoc = document; 
	}
	CZimsCube(void) : CObArray() 
	{
		keeptieslices = FALSE;
		pDoc = NULL; 
	}
	~CZimsCube(void) { ClearContents(); }

	void Init(CVelmanDoc *document) { pDoc = document; }
	void ClearContents(void);

	BOOL Read(void);
	BOOL ReadASCIIZimsGrid(CString suppname);
	BOOL Write(void);
	BOOL WriteASCIIInternal(void);
	BOOL WriteZimsFiles(void);

	BOOL DoDixEquation(void);
	BOOL DepthConvert(int ProjectScope, int layer);

	CZimsGrid *FindHorizon(int horizon, UBYTE type, int gridid);
	BOOL FindHorizon(UBYTE type);
	CObArray *FindHorizon(int horizon, UBYTE type);
	int FindHorizon(CZimsGrid *grid);

	int FindNewID(int horizon, UBYTE type);
	int DesignateGrid(CZimsGrid *newgrid);
	BOOL RemoveAt(CZimsGrid *horizon);
	int Add(CZimsGrid *grid, CString name, int horizon);
	int FillListboxWithGridnames(CListBox *pLB, CWordArray &entries, BOOL Timegridsonly=FALSE);
	int CanDepthConvertLayer(int layer);
	int CanDepthConvertSecondaryTimegrid(CZimsGrid *grid, int numlayers);
	int CanDepthConvertSecondaryTimeslice(int numlayers);
	BOOL DepthConvertSecondaryTimegrids(int numgrids, int *gridindex, int interp_method, int extrap_gradient_percent, BOOL rebuild_avgvels, int Layers);
	BOOL DepthConvertSecondaryTimeslice(double value, int interp_method, int extrap_gradient_percent, BOOL rebuild_avgvels);
	BOOL VelocityConvertSecondaryTimeslice(double value, int interp_method, int extrap_gradient_percent);
	double TimeConvertDepthPoint(double x, double y, double depth, int interp_method, int extrap_gradient_percent);
	BOOL RayDepth( CZimsGrid** raydepthgrids, CZimsGrid** raypregrids,
		CZimsGrid** timegrids,  CZimsGrid** reb_timegrids, CZimsGrid** intvelgrids,
		CZimsGrid** xoffsetgrids, CZimsGrid** yoffsetgrids, CZimsGrid** anglegrids,
		double anglelimit, double regridbinsize);

	CZimsGrid *operator [] (int index) 
	{
		return((CZimsGrid *) GetAt(index));
	}
};
