// forward declaration of document class and other classes referred to in this file
class CVelmanDoc;
class CShotLine;
class CShotPt;
class CProgressMonit;

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
#include <math.h>
#include <float.h>
#include <time.h>

// user includes needed
#include "global.h"
#include "zimscube.h"
#include "cmntypes.h"
#include "nrutil.h"

#ifndef __STDAFX_H__
#include "stdafx1.h"
#endif
#include "Funclib.h"	// Added by ClassView

// user defined constants for CSeismicData
#define CSD_MAXLEN                 5120
#define CSD_TYPELINELENGTH		        5
#define CSD_RMSRESAMPLEINTERVALS      4
#define CSD_2DRESAMPLEINTERVALS       5
#define CSD_MINIBUF		      	      128
#define CSD_IMPOSSIBLE_SHOTPOINT	32000

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

// rough content of a grid file header (for "Grid database view" dialog)
struct GridFileHeaderInfo
{
	int rows, columns;		// extent of grid
	double xmin, xmax, ymin, ymax;	// min/max coords
	int size;						// size of grid data in KB
};

//*************************************************************************
// Class:				CDummyReadSeisdata
// Is used for the "Test RMS file" function; only the first few lines
// of an RMS file are read and this class can store line code, shotpoint number,
// velocity, time, coord so that this data can be rendered on screen and the user
// can decide whether this looks good.
//*************************************************************************
class CDummyReadSeisdata : public CObject
{
public:
	void AddPt(CString line, int pointid, double newvel, double newtime,
		double newx=CSD_DONTKNOW, double newy=CSD_DONTKNOW)
	{
		Line.Add(line);
		sprintf(buf, "%d", pointid);
		Shotpt.Add(buf);
		sprintf(buf, "%6.1f", newvel);
		vel.Add(buf);
		sprintf(buf, "%6.1f", newtime);
		time.Add(buf);
		if(newx==CSD_DONTKNOW)
			sprintf(buf, "?");
		else
			sprintf(buf, "%6.1f", newx);
		x.Add(buf);
		if(newy==CSD_DONTKNOW)
			sprintf(buf, "?");
		else
			sprintf(buf, "%6.1f", newy);
		y.Add(buf);
	}
	CString GetLine(int index) { return Line.GetAt(index); }
	CString GetId(int index) { return Shotpt.GetAt(index); }
	CString GetTime(int index) { return time.GetAt(index); }
	CString GetVel(int index) { return vel.GetAt(index); }
	CString GetX(int index) { return x.GetAt(index); }
	CString GetY(int index) { return y.GetAt(index); }
	CString GetItem(int index, int which)  // a comfy wrapper
	{
		switch(which)
		{
		case 0:return GetLine(index);
		case 1:return GetId(index);
		case 2:return GetTime(index);
		case 3:return GetVel(index);
		case 4:return GetX(index);
		case 5:return GetY(index);
		default:return GetLine(index);
		}
	}
	int GetSize() { return Line.GetSize(); }
	void CleanUp()
	{
		Line.RemoveAll();
		Shotpt.RemoveAll();
		vel.RemoveAll();
		time.RemoveAll();
		x.RemoveAll();
		y.RemoveAll();
	}
	char Filtertype;
private:
	char buf[64];
	CStringArray Line, Shotpt, vel, time, x, y;
};

//*************************************************************************
// Class:				CSeismicData
// Arguments:   none
// Description:	Contains all functionality to read and write seismic data
// Variables:   mainfile :    main seismic data file (ASCII)
//              suppfile :    supplimentary seismic data file
//              filterfile :  ASCII file with filter descriptions
//              pDoc :  pointer to application
//              inputbuffer : memory reserved for input line
//*************************************************************************
class CSeismicData : public CObArray {
	friend void UnPad(char *string, int length);
	friend void Pad(char *string, int length);
	friend void zero_mem(char *ptr, int length);
	friend void InvertData(char *data, int section[]);

private:

#ifdef VELMAN_UNIX
	ifstream datafile;
	ifstream suppfile;
	ifstream filterfile;
#else
	std::ifstream datafile;
	std::ifstream suppfile;
	std::ifstream filterfile;
#endif
	long DataFileLength;

	CVelmanDoc *pDoc;

	BOOL HaveInit;
	double VelFactor;  // to convert between m/s and ft/s
	double illegal;
	double smat[2][2];
	double shift[2];
	char typeline[CSD_TYPELINELENGTH];

	char *GetString(char *inputbuffer, int start, int end);
	double GetDouble(char *inputbuffer, int start, int end);
	char *KillWhite(char *str);
	CString &KillWhite(CString &str);
	void ExpandTabs(char *inputbuffer);
	int	ExtractLineNumber(CString &name);
	BOOL TooClose(CShotPt *timept, CShotPt *cmpt, double radius);
	BOOL TooClose(double x, double y, double radius);

	BOOL ReadColumn(BOOL OnlyDummyRead=FALSE);
	BOOL ReadRow(BOOL OnlyDummyRead=FALSE);
	BOOL IsThatLine(char *inputbuffer, int tagstart, int tagend, char *tag);
	BOOL AddNewPt(CString &linename, int lineid, int shotpt, UBYTE type, double x, double y, double value);
	BOOL AddNewPt(CString &linename, int lineid, int shotpt, UBYTE type, double x, double y, double value , double minRadius);
	CShotLine *FindLine(int lineid);
	CShotLine *FindLine(CString &linename);
#ifdef VELMAN_UNIX
	BOOL GetZimsLine(char *inputbuffer, ifstream &file);
#else
	BOOL GetZimsLine(char *inputbuffer, std::ifstream &file);
#endif

	BOOL MakeDOSName_ZMAP(CString &zimsname, CStringArray &dosname, int &index, int &hnum);
	BOOL MakeDOSName_MFD(CString &zimsname, CStringArray &dosname, int &index, int &hnum);
	BOOL MakeDOSName_CP3(CString &zimsname, CStringArray &dosname, int &index, int &hnum);
	BOOL MakeDOSName_CP3int(CString &zimsname, CStringArray &dosname, int &index, int &hnum);
	BOOL MakeDOSName_EV (CString &zimsname, CStringArray &dosname, int &index, int &hnum );
	BOOL MakeDOSName_ZmapISO(CString &zimsname, CStringArray &dosname, int &index, int &hnum );

	BOOL MakeZimsNameList_ZMAP(CStringArray &outarray, CStringArray &dosname, int type);
	BOOL MakeZimsNameList_MFD(CStringArray &outarray, CStringArray &dosname, int type);
	BOOL MakeZimsNameList_CP3(CStringArray &outarray, CStringArray &dosname, int type);
	BOOL MakeZimsNameList_CP3int(CStringArray &outarray, CStringArray &dosname, int type);
	BOOL MakeZimsNameList_EV (CStringArray &outarray, CStringArray &dosname, int type);
	BOOL MakeZimsNameList_ZmapISO(CStringArray &outarray, CStringArray &dosname, int type);

	BOOL ReadZimsFile_ZmapISO(CString &suppname, int hnum, UBYTE dtype, int horizon);
	BOOL ReadZimsFile_EV (CString &suppname, int hnum, UBYTE dtype, int horizon );
	BOOL ReadZimsFile_ZMAP(CString &suppname, int hnum, UBYTE dtype, int horizon);
	BOOL ReadZimsFile_MFD(CString &filename, int hnum, UBYTE dtype, int horizon);
	BOOL ReadZimsFile_CP3(CString &filename, int hnum, UBYTE dtype, int horizon);
	BOOL ReadZimsFile_CP3int(CString &filename, int hnum, UBYTE dtype, int horizon);

	BOOL ReadZimsFileHeader_ZMAP(CString &suppname, int hnum, UBYTE dtype, int horizon, struct GridFileHeaderInfo *headerinfo);
	BOOL ReadZimsFileHeader_MFD(CString &filename, int hnum, UBYTE dtype, int horizon, struct GridFileHeaderInfo *headerinfo);
	BOOL ReadZimsFileHeader_CP3(CString &filename, int hnum, UBYTE dtype, int horizon, struct GridFileHeaderInfo *headerinfo);
	BOOL ReadZimsFileHeader_CP3int(CString &filename, int hnum, UBYTE dtype, int horizon, struct GridFileHeaderInfo *headerinfo);
	BOOL ReadZimsFileHeader_EV (CString &suppname, struct GridFileHeaderInfo *headerinfo);
	BOOL ReadZimsFileHeader_ZmapISO(CString &suppname, int hnum, UBYTE dtype, int horizon, struct GridFileHeaderInfo *headerinfo);

	BOOL DeleteZimsFile_ZmapISO(CStringArray &dosname, int index);
	BOOL DeleteZimsFile_EV(CStringArray &dosname, int index);
	BOOL DeleteZimsFile_ZMAP(CStringArray &dosname, int index, int hnum);
	BOOL DeleteZimsFile_MFD(CStringArray &dosname, int index, int hnum);
	BOOL DeleteZimsFile_CP3(CStringArray &dosname, int index, int hnum);
	BOOL DeleteZimsFile_CP3int(CStringArray &dosname, int index, int hnum);

	double FindCPSVar(const char *fieldname, char *dataindex, int length, BOOL *status);

public:
	CSeismicData(void) : CObArray()
	{
		pDoc = NULL;
		HaveInit=FALSE;

		smat[0][0] = 1.0;
		smat[0][1] = 0.0;
		smat[1][0] = 0.0;
		smat[1][1] = 1.0;

		shift[0] = 0.0;
		shift[1] = 0.0;
	}

	~CSeismicData() { ClearContents(); }

	void Init(CVelmanDoc *document, double illval = CSD_ILLEGAL);
	BOOL IsInitialized() { return HaveInit; }
	void ClearContents(UBYTE type = CSD_TYPE_ANY);
	void ClearMostContents(void);

	void Convert2GlobalCoordinates(void);
	BOOL FillMatrix(
		int line0, int spt0, double x0, double y0,
		int line1, int spt1, double x1, double y1,
		int line2, int spt2, double x2, double y2);

	BOOL Read(void);
	BOOL Read(CString dataname, CString filtername, CString filter, CString suppname = "", BOOL OnlyDummyRead=FALSE);
	BOOL Write(void);
	BOOL WriteASCIIInternal(void);

	BOOL MakeDOSName(CString &zimsname, CStringArray &dosname, int &index, int &hnum);
	BOOL MakeZimsNameList(CStringArray &outarray, CStringArray &dosname, int type);
	BOOL ReadZimsFile(CString suppname, int hnum, UBYTE dtype, int horizon=-1);
	BOOL ReadZimsFileHeader(CString suppname, int hnum, UBYTE dtype, int horizon=-1, struct GridFileHeaderInfo *headerinfo=NULL);
	BOOL DeleteZimsFile(CString gridname);
	BOOL ExistZimsFile(CString &filename, int type);

	BOOL Interpolate2DShotLines(UBYTE type, BOOL ShowWarnings);
	BOOL DoDixEquation(void);
	int ResampleShotpoints(CZimsGrid **timegrid, int horizons, CObArray &intvelpts);
	BOOL CleanInternalDataBase(void);

	void SetIllegal(double nillegal) {illegal = nillegal; }
	CShotLine *& operator [](int i) { return((CShotLine *&) CObArray::operator[](i)); }
	CDummyReadSeisdata dummyseisdata;
};

//*************************************************************************
// Class:				CShotLine
// Arguments:   none
// Description:	represents a shot line and contains an array of all shot
//              points on that shot line
// Variables:   name :   CString containing name of shot line
//*************************************************************************
class CShotLine : public CObArray
{
private:
	CString name;
	int lineid;

public:
	CShotLine(CString &nname, int newid) : CObArray()
	{
		name = nname;
		lineid = newid;
	}
	~CShotLine();

	CString &GetName(void) { return (name);}
	CShotPt *FindSPoint(int sptid, UBYTE type = CSD_TYPE_ANY);
	int GetSPoint(int sptid);

	CShotPt * operator [] (int i) { return((CShotPt *) GetAt(i)); }
	operator int() { return lineid; }
};

//*************************************************************************
// Class:				CDataPt
// Arguments:   ndata : item of data
// Description:	represents a data point
// Variables:   data  :  value of data stored at this x, y
//*************************************************************************
class CDataPt : public CObject
{
private :
	double data;

public:
	CDataPt(double ndata = CSD_ILLEGAL) : CObject() { data = ndata; }

	operator double() { return data; }
	double &operator = (double ndata) { return(data = ndata); }
};

//*************************************************************************
// Class:				CShotPt
// Arguments:   nx, ny : position of shot point
//              ntype  : type of data in this point
// Description:	represents a shot point and contains an arraz of all data
//              points (CDataPt) at that x, y location
// Variables:   x, y      :  doubles containing global position
//              type     :  type of data stored at that location. This
//                          can be different types of velocity or time
//*************************************************************************
class CShotPt : public CObArray
{
private:
	int id;
	double x, y;
	UBYTE type;

public:
	CShotPt(int nid, UBYTE ntype) : CObArray()
	{
		id = nid;
		x = CSD_DONTKNOW;

		y = CSD_DONTKNOW;
		type = ntype;
	}

	~CShotPt();

	operator int() { return id; }
	operator UBYTE() { return type; }
	double operator [](int i) { return((double) *((CDataPt *) GetAt(i))); }

	int GetID(void) { return id; }
	UBYTE GetType(void) { return type;}
	double GetX(void) { return x; }
	double GetY(void) { return y; }

	void PutX(double nx)
	{
		if(x == CSD_DONTKNOW)
			x = nx;
	}
	void PutY(double ny)
	{
		if(y == CSD_DONTKNOW)
			y = ny;
	}
};

