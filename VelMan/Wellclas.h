#include <afxtempl.h>

// forward declaration of document class
class CVelmanDoc;
class CHorizon;
class CMeasurePt;

// standard includes needed
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

// user includes needed
#include "global.h"
#include "cmntypes.h"

#ifndef __STDAFX_H__
#include "stdafx.h"
#endif

// ways of dealing with pseudo velocities
#define Pseudo_None 1
#define Pseudo_Deviated 2
#define Pseudo_All 3

//user defined constants
#define MPT_STATUS_INVALID 0x80
#define MPT_STATUS_SCRL_EXCLUDE 0x20
#define MPT_STATUS_USER_EXCLUDE 0x10
#define MPT_STATUS_MUSTNOTREPAIR_TIME 0x08
#define MPT_STATUS_MUSTNOTREPAIR_DEPTH 0x04
#define MPT_STATUS_CALC_DEPTH 0x02
#define MPT_STATUS_CALC_TIME 0x01

#define MPT_MUSTNOTREPAIR_ITEM -9999
#define MPT_ILLEGAL ZIMS_VAL_ZNON
#define MPT_INVALID ZIMS_VAL_ZNON
#define MPT_MINSECS 10.0
#define MPT_PMIN 0.0
#define MPT_PMAX 1.0e12
#define MPT_DMIN -1.0e7
#define MPT_DMAX 1.0e7
#define MPT_TMIN -1.0e7
#define MPT_TMAX 1.0e7

#define HORL_VALIDLINE 0x02
#define HORL_EOF 0x08
#define HORL_EXCLUDE 2
#define HORL_MAXLEN 4096
#define HORL_WELLMAXLEN 64
#define HORL_DIVIDER '\t'
#define HORL_MAXCHUNK 1024
#define HORL_NAMEMAX 64

#define WELL_STATUS_DEVIATED 0x01

// flags in "HowToHandleWellResiduals"
#define WELL_MODEL_MULTIPLICATIVERESIDUALS 0x01 // additive/multiplicative/hybrid residuals
#define WELL_MODEL_NORESIDUALSIFDESELECTED 0x02 // ignore residuals of deselected wells
#define WELL_MODEL_NORESIDUALSATALL 0x04 // no residual gridding at all
#define WELL_MODEL_ADDITIVERESIDUALS 0x08

// Functions to convert from the model fit a, b parameters to Vo, K which
// is what we display to the users.
double ab2V(double a, double b);
double ab2K( double a, double b );
double VK2a( double V, double K );
double VK2b( double V, double K );

//*************************************************************************
// Class: CHorizonList
// Arguments: none
// Description: Object derived from CObjArray which contains an array
// of all horizons.
// Variables: surveyfile : ifstream object with raw positional data
// wellfile : ifstream object with raw time/depth data
// wellnames : array of all wellnames
// pDoc : pointer to document
// inputbuffer : pointer of buffer for input file
// excludestring : array of strings with prompt program to
// ignore this particular input line
//*************************************************************************
class CHorizonList: public CObArray
{
private:
#ifdef VELMAN_UNIX
	ifstream surveyfile;
	ifstream wellfile;
#else
	std::ifstream surveyfile;
	std::ifstream wellfile;
#endif
	CVelmanDoc *pDoc;
	char *inputbuffer;
	long SurveyFileLength;
	long numberOfBytesRead;

	CString excludestring[HORL_EXCLUDE];
	CString Switch2VerticalString;
	CString Switch2DeviatedString;

	BOOL PrimaryDatabase; // FALSE for database with secondary wells

#ifdef VELMAN_UNIX
	UBYTE GetNewLine(ifstream &file, BOOL compressSpaces);
#else
	UBYTE GetNewLine(std::ifstream &file, BOOL compressSpaces);
#endif
	UBYTE GetMemNewLine(char **memfile);
	BOOL CheckData(void);
	void Clear();
	BOOL CheckASCIIFile(CString &filename);

public:
	CStringArray wellnames; // names of wells
	CArray<double, double> wellXsurf;
	CArray<double, double> wellYsurf;
	CDWordArray wellstatus;

	CHorizonList(void): CObArray()
	{
		inputbuffer = NULL;
		pDoc = NULL;
		excludestring[0]="*"; // if "*" or ";" appears in a line in the well files,
		excludestring[1]=";"; // that line is treated as a comment line.
		Switch2VerticalString="VERTICAL";
		Switch2DeviatedString="DEVIATED";
		PrimaryDatabase=TRUE;
	}

	~CHorizonList();
	BOOL Init(CVelmanDoc *document);

	double CheckTime(double time);
	//BOOL Read(CString surveyfilename, CString wellfilename, int UsePseudoVelocities); // Paths to files, and bool indicating to always use horizon values for well time values in td file
	BOOL ReadNew(CString filename, int UsePseudoVelocities);
	BOOL Read(void);
	BOOL Write(void);
	BOOL WriteASCII(CString FolderPath);
	BOOL WriteASCII2(CString FolderPath);
	void ProcessHorizon(int);
	BOOL RebuildTimeDepth(int hor, BOOL DoDepth, BOOL DoTime);
	void FitLayer(int, CVelmanDoc *);// fit layer exactly, taking excluded points into account
	// (unlike the fitting in the coarse model preview does)
	CHorizon * operator[](int i) { return ((CHorizon *) CObArray::operator[](i)); }
	BOOL IsPrimaryDatabase() { return PrimaryDatabase; }
	BOOL IsSecondaryDatabase() { return !PrimaryDatabase; }
	void SetPrimaryDatabase() { PrimaryDatabase=TRUE; }
	void SetSecondaryDatabase() { PrimaryDatabase=FALSE; }
	void RemoveWell(int index);
};

//*************************************************************************
// Class: CHorizon
// Arguments: none
// Description: Object derived from CObject and CObjArray which contains
// its own name as well as all measurement points in this
// horizon
// Variables: name : name of horizon
// plus variables describing the model chosen for this particular horizon
//*************************************************************************
class CHorizon: public CObArray
{
public:
	CString name;
	BOOL HaveModel;// TRUE if model has been chosen (that choice can be overridden,
	// but we will not open the "Choose model" dialog automatically any more)
	int ModelNumber; // number of model in library that was chosen
	int DistWeight; // kind of distance weighting: 0=linear 1=quadratic
	int ExcludePercent;
	int NumPts; // number of valid measurement points (set by ProcessHorizon)
	BOOL HaveFit; // TRUE if a valid fit has been done and FitA/B are set
	double FitA, FitB; // fitting parameters
	BOOL UseCustomFit; // TRUE: do not perform fit but read out the next two variables
	double CustomFitA, CustomFitB;
	double FitGoodness; // chi-squared of the fit
	double WeightCenterX, WeightCenterY, WeightCenterRad; // for functional weighting
	int WeightDecreaseFunc; // dto.
	BOOL NormalizeGradients;
	double NormGradFactor;
	BOOL HowToHandleWellResiduals;
	BOOL KeepResidualGridAfterDC;

	CHorizon(void): CObArray()
	{
		HaveModel=FALSE; HaveFit=FALSE; ModelNumber=-1;
		WeightCenterX=0.0;WeightCenterY=0.0;
		WeightCenterRad=0.0;WeightDecreaseFunc=-1;
		ExcludePercent=0;DistWeight=0;NumPts=0;
		NormalizeGradients=TRUE;NormGradFactor=50.0;
		// HowToHandleWellResiduals= WELL_MODEL_MULTIPLICATIVERESIDUALS | WELL_MODEL_NORESIDUALSIFDESELECTED; // default to multiplicative
		HowToHandleWellResiduals= WELL_MODEL_NORESIDUALSIFDESELECTED;// default to optimised
		UseCustomFit=FALSE;CustomFitA=0.0;CustomFitB=0.0;
		KeepResidualGridAfterDC=TRUE;
	}
	CHorizon(CString &newname): CObArray()
	{
		name = newname; HaveModel=FALSE; HaveFit=FALSE; ModelNumber=-1;
		WeightCenterX=0.0;WeightCenterY=0.0;
		WeightCenterRad=0.0;WeightDecreaseFunc=-1;
		ExcludePercent=0;DistWeight=0;NumPts=0;
		NormalizeGradients=TRUE;NormGradFactor=50.0;
		// HowToHandleWellResiduals= WELL_MODEL_MULTIPLICATIVERESIDUALS | WELL_MODEL_NORESIDUALSIFDESELECTED; // default to multiplicative
		HowToHandleWellResiduals= WELL_MODEL_NORESIDUALSIFDESELECTED;// default to optimised
		UseCustomFit=FALSE;CustomFitA=0.0;CustomFitB=0.0;
		KeepResidualGridAfterDC=TRUE;
	}

	~CHorizon();

	void WriteName(CString &newname) { name = newname; }
	CString &GetName(void) { return name; }
	void SetHorizonWeights(int type, int model=0, double x=0.0, double y=0.0,
		double rad=1.0, double downto=0.0, CVelmanDoc *pDoc=NULL);

	CMeasurePt * operator[](int i)
	{
		return ((CMeasurePt *) CObArray::operator[](i));
	}
};

//*************************************************************************
// Class: CMeasurePt
// Arguments: none
// Description: Object derived from CObject which contains positional as
// well as time depth information on a certain measurement
// point.
// It also contains information that is computed (by ProcessHorizon)
// from the original time and depth values
// Variables: x, y = position of measurement point
// depth = depth of horizon
// status = status byte
// mptime, mpdepth = mid point time/depth (computed later)
// vInt = interval velocity (computed later)
// region = region number
// reserved = pointer to reserved extension
//*************************************************************************

class CMeasurePt: public CObject
{
private:
	double x, y, depth, time; // read from file (depth, time = bottom depth and time)
	double mptime, mpdepth; // middle point time and depth (computed)
	double vInt; // interval velocity (computed)
	double dTop, tTop; // top depth and time (computed)
	double dInt, tInt; // interval depth and time (computed)
	double weight; // weight of point in regression (usually 1.0)
	UBYTE status, region;
	double TieTimeDiff, TieDepthDiff, TieIntVelDiff;// when tying a grid to the well,
	// store the encountered difference here
public:
	int wellindex; // index number of well for this point
	CMeasurePt(void): CObject()
	{
		x = y = depth = time = MPT_INVALID;
		TieTimeDiff = TieDepthDiff = TieIntVelDiff = MPT_INVALID;
		mptime=mpdepth=vInt=MPT_INVALID;
		dTop=tTop=dInt=tInt=MPT_INVALID;
		status = 0;
		region = 0;
		weight = 1.0;
	}

	~CMeasurePt() { }

	UBYTE ReadStatus(void) { return(status); }
	void WriteStatus(UBYTE nstatus) { status = nstatus; }

	UBYTE ReadRegion(void) { return(region); }
	void WriteRegion(UBYTE nregion) { region = nregion; }

	double ReadX(void) { return(x); }
	double ReadY(void) { return(y); }
	double ReadDepth(void) { return(depth); }
	double ReadTime(void) { return(time); }
	double ReadvInt(void) { return(vInt); }
	double Readmptime(void) { return(mptime); }
	double Readmpdepth(void) { return(mpdepth); }
	double ReadWeight(void) { return(weight); }
	double ReadZ1(void) { return(dTop); }
	double ReadtTop(void) { return(tTop); }
	double ReadtInt(void) { return(tInt); }
	double ReaddInt(void) { return(dInt); }
	double ReadTieTdiff(void) { return(TieTimeDiff); }
	double ReadTieVdiff(void) { return(TieIntVelDiff); }
	double ReadTieDdiff(void) { return(TieDepthDiff); }

	void ReadValuePair(int, double *, double *);

	void WriteX(double nx) { x = nx; }
	void WriteY(double ny) { y = ny; }
	void WriteDepth(double ndepth) { depth = ndepth; }
	void WriteTime(double ntime) { time = ntime; }
	void WritevInt(double v) { vInt = v; }
	void Writemptime(double t) { mptime = t; }
	void Writempdepth(double d) { mpdepth = d; }
	void WriteWeight(double w) { weight = w; }
	void WritetTop(double t) { tTop=t; }
	void WritetInt(double t) { tInt=t; }
	void WritedInt(double d) { dInt=d; }
	void WritedTop(double z) { dTop=z; }
	void WriteTieTdiff(double d) { TieTimeDiff=d; }
	void WriteTieDdiff(double d) { TieDepthDiff=d; }
	void WriteTieVdiff(double d) { TieIntVelDiff=d; }

	BOOL WriteSurveyData(double nx, double ny, UBYTE newstatus);
	BOOL WriteWellData(double ntime, double ndepth);
};