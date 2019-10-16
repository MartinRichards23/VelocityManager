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

#include "nrutil.h"
#include "distmat.h"
#include "cmntypes.h"
#include "global.h"
#include "helper.h"

#ifndef __STDAFX_H__
#include "stdafx.h"
#endif

#define NSTACK 75
#define M 7

class CZimsGrid;

class CFaultLine : public CObject 
{
	friend BOOL operator > (CFaultLine &line, CFaultLine &otherline);
	friend BOOL operator < (CFaultLine &line, CFaultLine &otherline);

private :
	double x0, x1, y0, y1;

public :
	CFaultLine(void) : CObject()
	{
		x0 = CSD_DONTKNOW;
		y0 = CSD_DONTKNOW;
		x1 = CSD_DONTKNOW;
		y1 = CSD_DONTKNOW;
	}
	CFaultLine(double nx0, double ny0, double nx1, double ny1) : CObject()
	{
		x0 = nx0;
		y0 = ny0;
		x1 = nx1;
		y1 = ny1;

		if(GetY1() < GetY0()) Flip();
	}
	~CFaultLine() { }

	double GetX0(void) { return (x0); }
	double GetY0(void) { return (y0); }
	double GetX1(void) { return (x1); }
	double GetY1(void){ return (y1); }

#ifdef VELMAN_UNIX
	void Dump(ofstream &outfile);
	void Read(ifstream &infile);
	void Write(ofstream &outfile);
#else
	void Dump(std::ofstream &outfile);
	void Read(std::ifstream &infile);
	void Write(std::ofstream &outfile);
#endif

	BOOL operator != (CFaultLine &otherline);
	BOOL operator == (CFaultLine &otherline);
	void Flip();
};

class CFaultGrid : public CObArray
{
private :
	CVelmanDoc *pDoc;
	CString name;
	BOOL InDestructor;

	void Swap(int i, int j);

protected :
	double xlim[2], ylim[2];
	double znon;

	int horizon;
	CObArray owner_grid;

public :
	CFaultGrid(CVelmanDoc *doc) : CObArray()
	{
		xlim[0] = xlim[1] = ylim[0] = ylim[1] = CSD_ILLEGAL;
		znon = ZIMS_VAL_ZNON;
		horizon = -1;
		pDoc = doc;
		InDestructor=FALSE;
	}
	CFaultGrid(void) : CObArray()
	{
		xlim[0] = xlim[1] = ylim[0] = ylim[1] = CSD_ILLEGAL;
		znon = ZIMS_VAL_ZNON;
		horizon = -1;
		pDoc = NULL;
		InDestructor=FALSE;
	}
	~CFaultGrid() { InDestructor=TRUE;ClearContents(); }
	void ClearContents(void);
	BOOL Init(CVelmanDoc *pDoc, double nznon = ZIMS_VAL_ZNON);
	void WriteLimits(double *x, double *y);

	void Delete(CZimsGrid *zgrid);
	void Attach(CZimsGrid *zgrid) { owner_grid.Add((CObject *) zgrid); }
	CString &GetName(void) { return name; }
	void SetName(CString newname);
	CVelmanDoc *GetDocument(void) { return pDoc; }
	int GetHorizon(void) { return horizon; }
	double *GetXlimits(void) { return xlim; }
	double *GetYlimits(void) { return ylim; }
	double GetZnon(void) { return znon; }
	CFaultLine *GetAt(int i) { return (CFaultLine *) CObArray::GetAt(i); }

	void AddFaultLine(double x0, double y0, double x1, double y1)
	{
		CObArray::Add(new CFaultLine(x0, y0, x1, y1));
	}
	void AddFaultLine(CFaultLine *faultline)
	{
		CObArray::Add(faultline);
	}
#ifdef VELMAN_UNIX
	BOOL Read(ifstream &infile);
	BOOL Write(ofstream &outfile);
#else
	BOOL Read(std::ifstream &infile);
	BOOL Write(std::ofstream &outfile);
#endif
	BOOL WriteASCII(CString FolderPath);
	BOOL Sort(void);

	CString &operator = (CString nname)
	{
		SetName(nname);
		return name;
	}

	int &operator = (int nhorizon)
	{
		horizon = nhorizon;
		return horizon;
	}

	CFaultLine *operator [] (int index)
	{
		return((CFaultLine *) GetAt(index));
	}
};

class CFaultCube : public CObArray
{
private :
	CVelmanDoc *pDoc;

public :
	CFaultCube(CVelmanDoc *doc) : CObArray() { pDoc = doc; }
	CFaultCube(void) : CObArray() { pDoc = NULL; }
	~CFaultCube(void) { ClearContents(); }
	void ClearContents(void);
	void Init(CVelmanDoc *document) { pDoc = document; }

	CFaultGrid *GetAt(int i) { return (CFaultGrid *) CObArray::GetAt(i); }
	void Add(CFaultGrid *faultgrid, CString name, int horizon);

	BOOL Read(void);
	BOOL Write(void);
	BOOL WriteASCIIInternal(void);

	CFaultGrid *operator [] (int index)
	{
		return((CFaultGrid *) GetAt(index));
	}
};
