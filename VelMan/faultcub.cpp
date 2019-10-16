#include "VelmanDoc.h"
#include "resource.h"	// needed for importpr.h to function
#include "ProgressDlg.h"	// needed to update "progress" dialog box
#include <string.h>
#include <io.h>
#include <stdio.h>

// **********************************************************************
// Subroutine  : Dump()
// Arguments   : outfile : ASCII file to dump to
// Use         : dump contents of line to ASCII file
// Returns     : void
// **********************************************************************
void CFaultLine::Dump(ofstream &outfile)
{
	outfile << "(" << x0 << ", " << y0 << ") ---> ("
		<< x1 << ", " << y1 << ")\n";
}

//***********************************************************************
// Subroutine	:		  Write()
// Arguments	:			outfile : reference to binary output file
// Use        :     Write internal format faultline to disk
// Returns    :     none
//***********************************************************************
void CFaultLine::Write(ofstream &outfile)
{
	outfile.write((char *) &x0, sizeof(double));
	outfile.write((char *) &x1, sizeof(double));
	outfile.write((char *) &y0, sizeof(double));
	outfile.write((char *) &y1, sizeof(double));
}

//***********************************************************************
// Subroutine	:		  Read()
// Arguments	:			infile : reference to binary input file
// Use        :     Read internal format faultline from disk
// Returns    :     none
//***********************************************************************
void CFaultLine::Read(ifstream &infile)
{
	infile.read((char *) &x0, sizeof(double));
	infile.read((char *) &x1, sizeof(double));
	infile.read((char *) &y0, sizeof(double));
	infile.read((char *) &y1, sizeof(double));
}

// checks if two lines cross, returns FALSE if they do
BOOL CFaultLine::operator != (CFaultLine &otherline)
{
	double m0, m1, b0, b1;
	double c0, c1;

	//Find the slope and intersect for line 2
	if (otherline.GetX1() != otherline.GetX0())  // check if line 2 is vertical
	{
		m1 = (otherline.GetY1()-otherline.GetY0())/(otherline.GetX1()-otherline.GetX0());
		b1 = otherline.GetY0()-m1*otherline.GetX0();

		// see if points of line 1 lie on either side of line 2
		c0 = (GetY0() - m1*GetX0() - b1)
			*(GetY1() - m1*GetX1() - b1);
	}
	else
	{
		c0 = (GetX1() - otherline.GetX1())*(GetX0() - otherline.GetX0());
	}

	if (c0 < 0.0)  // if that is the case then do the same for the other line
	{
		if (GetX1() != GetX0())  // check if line 1 is vertical
		{
			m0 = (GetY1()-GetY0())/(GetX1()-GetX0());
			b0 = GetY0()-m0*GetX0();

			c1 = (otherline.GetY0() - m0*otherline.GetX0() - b0)
				*(otherline.GetY1() - m0*otherline.GetX1() - b0);
		}
		else
		{
			c1 = (otherline.GetX1() - GetX1())*(otherline.GetX0() - GetX0());
		}

		if (c1 < 0.0) return FALSE;
	}

	return TRUE;
}

// checks if two lines cross, returns TRUE if they do
BOOL CFaultLine::operator == (CFaultLine &otherline)
{
	double m0, m1, b0, b1;
	double c0, c1;

	//Find the slope and intersect for line 2
	if (otherline.GetX1() != otherline.GetX0())  // check if line 2 is vertical
	{
		m1 = (otherline.GetY1()-otherline.GetY0())/(otherline.GetX1()-otherline.GetX0());
		b1 = otherline.GetY0()-m1*otherline.GetX0();

		// see if points of line 1 lie on either side of line 2
		c0 = (GetY0() - m1*GetX0() - b1)
			*(GetY1() - m1*GetX1() - b1);
	}
	else
	{
		c0 = (GetX1() - otherline.GetX1())*(GetX0() - otherline.GetX0());
	}

	if (c0 < 0.0)  // if that is the case then do the same for the other line
	{
		if (GetX1() != GetX0())  // check if line 1 is vertical
		{
			m0 = (GetY1()-GetY0())/(GetX1()-GetX0());
			b0 = GetY0()-m0*GetX0();

			c1 = (otherline.GetY0() - m0*otherline.GetX0() - b0)
				*(otherline.GetY1() - m0*otherline.GetX1() - b0);
		}
		else
		{
			c1 = (otherline.GetX1() - GetX1())*(otherline.GetX0() - GetX0());
		}

		if (c1 < 0.0) return TRUE;
	}

	return FALSE;
}

// compares the y values of two FaultLines
BOOL operator > (CFaultLine &line, CFaultLine &otherline)
{
	// make sure that points are arranged in the correct way.
	if (line.GetY1() < line.GetY0()) line.Flip();
	if (otherline.GetY1() < otherline.GetY0()) otherline.Flip();

	if (line.GetY0() > otherline.GetY0()) return TRUE;
	else return FALSE;
}

// compares the y values of two FaultLines
BOOL operator < (CFaultLine &line, CFaultLine &otherline)
{
	// make sure that points are arranged in the correct way.
	if (line.GetY1() < line.GetY0()) line.Flip();
	if (otherline.GetY1() < otherline.GetY0()) otherline.Flip();

	if (line.GetY0() < otherline.GetY0()) return TRUE;
	else return FALSE;
}

// operator switches coordinate pairs
void CFaultLine::Flip()
{
	double t;

	t = x0;
	x0 = x1;
	x1 = t;

	t = y0;
	y0 = y1;
	y1 = t;
}

// **********************************************************************
// Subroutine  : Init()
// Arguments   :
// Use         : Fill the data structures
// Returns     : TRUE if OK, FALSE if Error
// **********************************************************************
BOOL CFaultGrid::Init(CVelmanDoc *document, double nznon)
{
	horizon = -1;
	znon = nznon;
	pDoc = document;

	return TRUE;
}

// **********************************************************************
// Subroutine  : WriteLimits()
// Arguments   : *x, *y : pointers to arrays with min and max values
// Use         : Sets and checks x and y limits of the grid
// Returns     : none
// **********************************************************************
void CFaultGrid::WriteLimits(double *x, double *y)
{
	for (int i=0; i<2; i++)
	{
		xlim[i] = x[i];
		ylim[i] = y[i];
	}
}

// **********************************************************************
// Subroutine  : Delete()
// Arguments   : none
// Use         : tells the faultgrids that one zimsgrid has dropped it
//               will erase the faultgrid if that was the last one
// Returns     : void
// **********************************************************************
void CFaultGrid::Delete(CZimsGrid *zgrid)
{
	// First clear the faultgrid in the grid which deleted it
	zgrid->Link(NULL);

	// search for the grid in the owner_grid array and remove it
	for (int i=0; i<owner_grid.GetSize(); i++)
	{
		if ((CZimsGrid *) owner_grid[i] == zgrid)
			owner_grid.RemoveAt(i);
	}

	// if no grids reference the faultgrid, delete it
	if (owner_grid.GetSize() == 0)
		ClearContents();
}

// **********************************************************************
// Subroutine  : SetName()
// assign this name to the grid. If any other grid has got the same name,
// assign a "version number" in the form  gridname=newname Vxx, where xx is
// some unique number
// **********************************************************************
void CFaultGrid::SetName(CString newname)
{
	int i = 0;
	int latestversion=0;
	CFaultCube *faultcube=&pDoc->faultcube;
	CFaultGrid *thegrid;
	CString gridname;
	CString pattern=newname+"_V";	// Remove space for unix filename
	int patternlength=pattern.GetLength();
	char buf[256];

	// go through all other faultgrids...
	for (i=0; i<faultcube->GetSize(); i++)
	{
		thegrid=faultcube->GetAt(i);
		// names are equal if a) gridname=newname or b) gridname=newname+version numb.
		gridname=thegrid->GetName();
		if (gridname==newname)
			latestversion=1;
		else
		{
			if (gridname.Left(patternlength)==pattern)
				latestversion=max(latestversion,
				atoi((const char *)gridname.Mid(patternlength)));
		}
	}
	if (latestversion==0)
		name=newname;  // no other grid found
	else
	{
		sprintf(buf, "%s_V%d", (const char *)newname, latestversion+1);
		name=(CString)buf;
	}
}

//***********************************************************************
// Subroutine	:			WriteASCII()
// Arguments	:			Folder to put file in
// Use        :     Write ASCII Dump file of CFaultGrid
// Returns    :     sucess
//***********************************************************************
BOOL CFaultGrid::WriteASCII(CString FolderPath)
{
	CString filename = FolderPath + pDoc->BaseName + ".lin";
	ofstream outfile;

#ifdef VELMAN_UNIX
	outfile.open((const char *)filename);
#else
	outfile.open((const char *)filename, ios::binary);
#endif

	if(outfile.fail())
		return FALSE;

	CFaultLine *currentline;

	outfile << "Name :\t" << name  << endl
		<< "XLim :\t" << xlim[0] << "\tto\t" << xlim[1] << endl
		<< "YLim :\t" << ylim[0] << "\tto\t" << ylim[1] << endl
		<< "NULL :\t" << znon << "\n\n";

	for (int i=0; i<GetSize(); i++)
	{
		currentline = GetAt(i);
		currentline->Dump(outfile);
	}

	outfile << "\n\n";
	return TRUE;
}

//***********************************************************************
// Subroutine	:		  Read()
// Arguments	:			infile : reference to binary input file
// Use        :     Read internal format faultlines from disk
// Returns    :     success
//***********************************************************************
BOOL CFaultGrid::Read(ifstream &infile)
{
	int length, i;
	char namebuf[128];
	CFaultLine *thisline;

	infile.read((char *) &length, sizeof(int));
	infile.read(namebuf, length*sizeof(char));
	name = (const char *) namebuf;
	infile.read((char *) &horizon, sizeof(int));
	infile.read((char *) xlim, 2*sizeof(double));
	infile.read((char *) ylim, 2*sizeof(double));
	infile.read((char *) &length, sizeof(int));

	for (i=0; i<length; i++)
	{
		thisline = new CFaultLine;
		thisline->Read(infile);
		Add(thisline);
	}

	return TRUE;
}

//***********************************************************************
// Subroutine	:		  Write()
// Arguments	:			outfile : reference to binary output file
// Use        :     Write internal format faultlines to disk
// Returns    :     success
//***********************************************************************
BOOL CFaultGrid::Write(ofstream &outfile)
{
	int length = name.GetLength()+1;

	outfile.write((char *) &length, sizeof(int));
	outfile.write((const char *) name, length*sizeof(char));
	outfile.write((char *) &horizon, sizeof(int));
	outfile.write((char *) xlim, 2*sizeof(double));
	outfile.write((char *) ylim, 2*sizeof(double));
	length = GetSize();
	outfile.write((char *) &length, sizeof(int));

	for (int i=0; i<length; i++)
		GetAt(i)->Write(outfile);

	return TRUE;
}

//***********************************************************************
// Subroutine	:			ClearContents()
// Arguments	:			none
// Use        :     Return memory allocated by CFaultGrid
// Returns    :     none
//***********************************************************************
void CFaultGrid::ClearContents(void)
{
	int i = 0;

	// Erase all pointers to faultgrids in the zimsgrids
	// do not do so if we are simply destructing the entire data structure
	if (!InDestructor) // this flag is set to TRUE in ~CFaultGrid only
	{
		for (i=0; i<owner_grid.GetSize(); i++)
			((CZimsGrid *) owner_grid[i])->Link(NULL);
	}
	owner_grid.RemoveAll();

	// Then remove the FaultLines
	i=0;
	while (i < GetSize()) delete GetAt(i++);
	RemoveAll();
}

//***********************************************************************
// Subroutine	:			Swap()
// Arguments	:			none
// Use        :     Swap faultlines i and j in the faultgrid
// Returns    :     none
//***********************************************************************
void CFaultGrid::Swap(int i, int j)
{
	CFaultLine *temp;

	temp = GetAt(i);
	SetAt(i, GetAt(j));
	SetAt(j, temp);
}

//***********************************************************************
// Subroutine	:			Sort()
// Arguments	:			none
// Use        :     QuickSort the faultcube in order of lowest Y0
// Returns    :     none
//***********************************************************************
BOOL CFaultGrid::Sort(void)
{
	long i, n=GetSize(), ir=n-1, j, k, l=0;
	int jstack=0, *istack;
	CFaultLine *a;
	CWaitCursor wait;

	istack = ivector(0, NSTACK-1);
	for (;;)
	{
		if (ir-l<M)
		{
			for (j=l+1; j<=ir; j++)
			{
				a = (*this)[j];
				for (i=j-1; i>=0; i--)
				{
					if (*(*this)[i] < *a) break;
					SetAt(i+1, GetAt(i));
				}
				SetAt(i+1, a);
			}
			if (jstack == 0) break;
			ir=istack[jstack--];
			l=istack[jstack--];
		}
		else
		{
			k=(l+ir) >> 1;
			Swap(k, l+1);
			if (*(*this)[l+1] > *(*this)[ir])
			{
				Swap(l+1, ir);
			}
			if (*(*this)[l] > *(*this)[ir])
			{
				Swap(l, ir);
			}
			if (*(*this)[l+1] > *(*this)[l])
			{
				Swap(l+1, l);
			}
			i=l+1;
			j=ir;
			a=(*this)[l];
			for (;;)
			{
				do i++;
				while (*(*this)[i] < *a);
				do j--;
				while (*(*this)[j] > *a);
				if (j<i) break;
				Swap(i, j);
			}
			SetAt(l, GetAt(j));
			SetAt(j, a);
			jstack += 2;
			if (jstack > NSTACK) return FALSE;
			if (ir-i+1 >= j-l)
			{
				istack[jstack] = ir;
				istack[jstack-1]=i;
				ir=j-1;
			}
			else
			{
				istack[jstack]=j-1;
				istack[jstack-1]=l;
				l=i;
			}
		}
	}
	free_ivector(istack, 0, NSTACK-1);
	return TRUE;
}

//***********************************************************************
// Subroutine	:			ClearContents()
// Arguments	:			none
// Use        :     Return memory allocated by CFaultGrid
// Returns    :     none
//***********************************************************************
void CFaultCube::ClearContents(void)
{
	int i = 0;
	while (i < GetSize())
	{
		delete GetAt(i++);
	}
	RemoveAll();
}

//***********************************************************************
// Subroutine	:		  Add()
// Arguments	:			faultgrid:  pointer to faultgrid
//                  name:       name of faultgrid
//                  horizon:    horizon number of faulgrid
// Use        :     Add a faultgrid into the faultcube
// Returns    :     void
//***********************************************************************

void CFaultCube::Add(CFaultGrid *faultgrid, CString name, int horizon)
{

	if (!name.IsEmpty())
		*faultgrid = name;
	if (horizon != -1)
		*faultgrid = horizon;

	CObArray::Add(faultgrid);
}

//***********************************************************************
// Subroutine	:		  Read()
// Arguments	:			infile : reference to binary input file
// Use        :     Read internal format faultcube from disk
// Returns    :     success
//***********************************************************************
BOOL CFaultCube::Read(void)
{
	CString filename = pDoc->TargetDir+pDoc->BaseName+".zln";
#ifdef VELMAN_UNIX
	ifstream infile((const char *) filename, ios::in);
#else
	std::ifstream infile((const char *) filename, ios::in|ios::binary);
#endif
	int i, size;

	CFaultGrid *fgrid;

	// ClearContents before reading new data base...
	ClearContents();

	if (infile.fail())
	{
		pDoc->Error("Could not read faultcube data file " + filename, MSG_ERR);
		return FALSE;
	}

	infile.read((char *) &size, sizeof(int));

	i=0;
	while (i < size)
	{
		if (pDoc->m_pProgress)
			pDoc->m_pProgress->ChangePercentage((i*20)/size);

		fgrid = new CFaultGrid(pDoc);
		if (!(fgrid->Read(infile)))
			return(FALSE);

		CObArray::Add(fgrid);
		i++;
	}

	return TRUE;
}

//***********************************************************************
// Subroutine	:		  Write()
// Arguments	:			outfile : reference to binary output file
// Use        :     Write internal format faultcube to disk
// Returns    :     success
//***********************************************************************
BOOL CFaultCube::Write(void)
{
	CString filename = pDoc->TargetDir+pDoc->BaseName+".zln";
#ifdef VELMAN_UNIX
	ofstream outfile((const char *) filename);
#else
	ofstream outfile((const char *) filename, ios::binary);
#endif
	int i, size=GetSize();

	if (outfile.fail())
	{
		pDoc->Error("Could not write faultline data file " + filename, MSG_ERR);
		return FALSE;
	}

	outfile.write((char *) &size, sizeof(int));

	i=0;
	while (i < size)
	{
		if (pDoc->m_pProgress)
			pDoc->m_pProgress->ChangePercentage((i*20)/size);

		if (!(((*this)[i])->Write(outfile)))
			return FALSE;

		i++;
	}

	return TRUE;
}

//***********************************************************************
// Subroutine	:			WriteASCIIInternal()
// Arguments	:			none
// Use        :     Write ASCII file of CFaultCube
// Returns    :     sucess
//***********************************************************************
BOOL CFaultCube::WriteASCIIInternal(void)
{
	int i=0;
	while (i < GetSize())
	{
		// change progress monitor if there is any. Changes in the range 90%..100%
		if (pDoc->m_pProgress)
			pDoc->m_pProgress->ChangePercentage(90+(i*10)/GetSize());
		if (!(GetAt(i++)->WriteASCII(pDoc->GetExportDir()))) return FALSE;
	}
	return TRUE;
}
