#include "VelmanDoc.h"
#include "resource.h"	// needed for importpr.h to function
#include "ProgressDlg.h"	// needed to update "progress" dialog box
#include "fft.h"
#ifndef VM64
#include "gridio.h"		// needed for EarthVision library functions
#endif
#include "helper.h"
#include "mainfrm.h"
#include <afxtempl.h>
#include <string.h>
#include <io.h>
#include <stdio.h>
#include <sys/stat.h>
#include <errno.h>
#ifdef VELMAN_UNIX
#include <sys/types.h>
#include <unistd.h>
#endif

void _drffti(int n, double *wsave, int *ifac);

extern int cp3int_parmlen;

// **********************************************************************
// Subroutine  : ClearContents()
// Arguments   : none
// Use         : Destructor of CZimsGrid
// Returns     : none
// **********************************************************************
void CZimsGrid::ClearContents(void)
{
	if(grid)
		free_dmatrix(grid, 0, rows-1, 0, columns-1);

	if(indexgrid)
		free_imatrix(indexgrid, 0, rows-1, 0, columns-1);

	if(c)
		free_dvector(c, 1, 0);

	if(faultgrid)
		faultgrid->Delete(this);
}

// **********************************************************************
// Subroutine  : Init()
// Arguments   : nrows:     number of rows
//               ncolumns   number of columns
// Use         : Fill the data structures
// Returns     : TRUE if OK, FALSE if Error
// **********************************************************************
BOOL CZimsGrid::Init(int nrows, int ncolumns, CVelmanDoc *document,
	UBYTE ntype, double nznon)
{
	if(nrows<=0 || ncolumns<=0) return FALSE;
	rows = nrows;
	columns = ncolumns;
	SetType(ntype);
	SetHorizon(-1);
	znon = nznon;
	pDoc = document;
	if(!(grid = dmatrix(0, rows-1, 0, columns-1))) return FALSE;
	SetUnit();
	return TRUE;
}

// **********************************************************************
// Subroutine  : WriteLimits()
// Arguments   : *x, *y : pointers to arrays with min and max values
// Use         : Sets and checks x and y limits of the grid
// Returns     : none
// **********************************************************************
void CZimsGrid::WriteLimits(double *x, double *y)
{
	for(int i=0;i<2;i++)
	{
		xlim[i] = x[i];
		ylim[i] = y[i];
	}
	dx = (xlim[1]-xlim[0])/(columns-1);
	dy = (ylim[1]-ylim[0])/(rows-1);
}

//***********************************************************************
// Subroutine	:			Read()
// Arguments	:			infile : reference to binary input file
// Use        :     Read binary file of CZimsGrid
// Returns    :     sucess
//***********************************************************************
#ifdef VELMAN_UNIX
BOOL CZimsGrid::Read(ifstream &infile, CVelmanDoc *document)
#else
BOOL CZimsGrid::Read(std::ifstream &infile, CVelmanDoc *document)
#endif
{
	int length, ntype, nid, nhor;
	char namebuf[128];
	pDoc = document;
	infile.read((char *) &length, sizeof(int));
	infile.read(namebuf, length*sizeof(char));
	infile.read((char *) &nid, sizeof(int));
	infile.read((char *) &ntype, sizeof(UBYTE));
	infile.read((char *) &nhor, sizeof(int));
	infile.read((char *) &rows, sizeof(int));
	infile.read((char *) &columns, sizeof(int));
	infile.read((char *) &znon, sizeof(double));
	infile.read((char *) &avg, sizeof(double));
	infile.read((char *) &znonvals, sizeof(int));
	infile.read((char *) xlim, 2*sizeof(double));
	infile.read((char *) ylim, 2*sizeof(double));
	infile.read((char *) &unit_timepower, sizeof(int));
	infile.read((char *) &unit_distpower, sizeof(int));
	infile.read((char *) &faultid, sizeof(int));
	dx = (xlim[1]-xlim[0])/(columns - 1);
	dy = (ylim[1]-ylim[0])/(rows - 1);
	SetName(namebuf);
	SetType(ntype);
	SetHorizon(nhor);
	SetID(nid);
	if(faultid >= 0)
	{
		faultgrid = pDoc->faultcube.GetAt(faultid);
		faultgrid->Attach(this);
	}
	if(AfxGetApp()->GetProfileInt("Debug", "EnableSwapping", 0))
	{
		//// store the position of the grid data in the gridpos variable
		//gridpos = infile.tellg();
		//// just ignore the data for now and set the grid pointer accordingly
		//infile.seekg((streamoff) rows*columns*sizeof(double), ios::cur);
		//grid = NOT_READ;
	}
	else
	{
		// or just read the data in normally
		gridpos = 0L;
		if(!(grid = dmatrix(0, rows-1, 0, columns-1))) return(FALSE);
		infile.read((char *) grid[0], rows*columns*sizeof(double));
	}
	return(TRUE);
}

//***********************************************************************
// Subroutine	:			Write()
// Arguments	:			outfile : reference to binary output file
// Use        :     Write binary file of CZimsGrid
// Returns    :     sucess
//***********************************************************************
BOOL CZimsGrid::Write(ofstream &outfile)
{
	int length = name.GetLength()+1;

	outfile.write((char *) &length, sizeof(int));
	outfile.write((const char *) name, length*sizeof(char));
	outfile.write((char *) &gridid, sizeof(int));
	outfile.write((char *) &type, sizeof(UBYTE));
	outfile.write((char *) &horizon, sizeof(int));
	outfile.write((char *) &rows, sizeof(int));
	outfile.write((char *) &columns, sizeof(int));
	outfile.write((char *) &znon, sizeof(double));
	outfile.write((char *) &avg, sizeof(double));
	outfile.write((char *) &znonvals, sizeof(int));
	outfile.write((char *) xlim, 2*sizeof(double));
	outfile.write((char *) ylim, 2*sizeof(double));
	outfile.write((char *) &unit_timepower, sizeof(int));
	outfile.write((char *) &unit_distpower, sizeof(int));
	outfile.write((char *) &faultid, sizeof(int));
	outfile.write((char *) grid[0], rows*columns*sizeof(double));
	return TRUE;
}

//***********************************************************************
// Subroutine	:		  WriteZimsFile(void), wrapper
// Arguments  :     void
// Use        :     Writes the current Grid to disk as a zims file...
// Returns    :     TRUE - OK
//                  FALSE - Error occured...
//***********************************************************************
BOOL CZimsGrid::WriteZimsFile(int GridType, CString FolderPath)
{
	CString mfdname, tmpname;
	int i, gaga;
	char ten, one;
	BOOL result;

	//get name for mfd file, up to 14 grids written to one file.
	mfdname= pDoc->MFDName + ".mfd";
	tmpname= pDoc->MFDName + ".tmp";

	switch(GridType)
	{
	case 0:
		result = WriteASCII(FolderPath);
		return result;

	case 1:
		result = WriteZimsFile_ZMAP(FolderPath);
		return result;
	case 2:
		i=-1;
		do
		{
			i++;
			one = i%10 + '0';
			ten = i/10 + '0';
			mfdname.SetAt(mfdname.GetLength()-5, one);
			mfdname.SetAt(mfdname.GetLength()-6, ten);
			tmpname.SetAt(tmpname.GetLength()-5, one);
			tmpname.SetAt(tmpname.GetLength()-6, ten);
		} while ((gaga=_access((const char *) (FolderPath + mfdname), 0)) != -1);
		// some files exist go to last file with data
		if(i != 0)
		{
			i--;
			one = i%10 + '0';
			ten = i/10 + '0';
			mfdname.SetAt(mfdname.GetLength()-5, one);
			mfdname.SetAt(mfdname.GetLength()-6, ten);
			tmpname.SetAt(tmpname.GetLength()-5, one);
			tmpname.SetAt(tmpname.GetLength()-6, ten);
			result =  WriteZimsFile_MFD(mfdname, tmpname, FolderPath);
			if(result)
			{
				// change permissions on files to read and write...
#ifdef VELMAN_UNIX
				struct stat filestat;

				stat((const char *) (FolderPath+mfdname), &filestat);
				chmod((const char *) (FolderPath+mfdname), filestat.st_mode |	S_IWUSR | S_IRUSR);
				stat((const char *) (FolderPath+tmpname), &filestat);
				chmod((const char *) (FolderPath+tmpname), filestat.st_mode |	S_IWUSR | S_IRUSR);
#else
				_chmod((const char *) (FolderPath+mfdname),_S_IREAD|_S_IWRITE);
				_chmod((const char *) (FolderPath+tmpname),_S_IREAD|_S_IWRITE);
#endif
				_unlink((const char *) (FolderPath+mfdname));
				rename((const char *) (FolderPath+tmpname),
					(const char *) (FolderPath+mfdname));
			}
			return result;
		}
		else
		{
			result = WriteZimsFile_MFD(CString(""), mfdname, FolderPath);
			return result;
		}
	case 3:
		result = WriteZimsFile_CP3(FolderPath);
		return result;
	case 4:

		result = WriteZimsFile_CP3int(FolderPath);
		return result;
	case 5:

		result = WriteZimsFile_EV(FolderPath);
		return result;
	case 6:
		result = WriteZimsFile_ZmapISO(FolderPath);
		return result;
	default:
		pDoc->Error("No valid grid file type", MSG_ERR);
		return FALSE;
	}
	return FALSE;
}

//	writes out a ZMAP Iso format file
BOOL CZimsGrid::WriteZimsFile_ZmapISO(CString FolderPath)
{
	CString	xmin, xmax, ymin, ymax;
	double	nullVal;
	int		xcol, yrow, nperline, fieldwidth, ndecpts, i, j, n;
	CString cname = GetName();
	ofstream	gridfile;
	CString filename = FolderPath + CMainFrame::GetNewFileName(FolderPath, GetName(), ".dat");

	//	set up grid parameters
	xmin.Format("%.2f", GetXlimits()[0]);
	xmax.Format("%.2f", GetXlimits()[1]);
	ymin.Format("%.2f", GetYlimits()[0]);
	ymax.Format("%.2f", GetYlimits()[1]);
	xcol = GetColumns();
	yrow = GetRows();
	nullVal = GetZnon();

	//	write header
	gridfile.open(filename);
	if ( gridfile.fail() )
	{
		AfxMessageBox("Attempt to open file : " + filename + " failed.\nExport aborted.", MB_OK );
		return FALSE;
	}
	gridfile << "!" << endl;
	gridfile << "!    Isomap grid format generated by VelocityManager" << endl;
	gridfile << "!    Grid name : " << cname << endl;
	gridfile << "!" << endl;
	nperline = 5;		// no. of values per line (as output in line below)
	gridfile << "@Grid " << cname <<", GRID, 5, HEADER" << endl;
	fieldwidth = 15;
	ndecpts    = 7;
	gridfile << fieldwidth << ", " << nullVal << ",  , " << ndecpts << ", 1" << endl;
	gridfile << yrow << ",  " << xcol << ",  ";
	gridfile << xmin << ",  " << xmax << ",  ";
	gridfile << ymin << ",  " << ymax << endl;
	gridfile << "  0.0,  0.0,  0.0" << endl;
	gridfile << "@" << endl;  // last line of header

	//	write data
	gridfile.precision(ndecpts);
	gridfile << setiosflags(ios::showpoint); // | ios::scientific
	for ( j = 0; j < xcol; j++ )
	{
		n=0;
		for ( i = 0; i < yrow; i++ )
		{
			float val = (float) GetGrid()[i][j];

			gridfile << setw(fieldwidth) << val;

			n++;
			if ( n == nperline || i == yrow-1 )
			{
				n = 0;
				gridfile << endl;
			}
		}
	}
	
	gridfile << "" << endl;	// last line of file @ removed from between " "
	gridfile.close();

	return TRUE;
}

//	Writes out an EarthVision file format.
BOOL CZimsGrid::WriteZimsFile_EV(CString FolderPath)
{
#ifndef VM64
	// earthvision variables
	char *desc;
	float *grid;
	int xcol, yrow;
	double xmin, xmax, ymin, ymax;
	float nullVal;
	char *datFile, *datField, *vfltFile, *nvfltFile;
	int status;
	int projection, zone, units, zUnits, pUnits;
	double GCTPparms[15];
	// ---
	int		i, j;
	char	fname[256];

	CString filename = FolderPath + CMainFrame::GetNewFileName(FolderPath, GetName(), ".grd");

	//	Initialise EV variables
	desc = NULL;	// we don't use these
	datFile = NULL;
	datField = NULL;
	vfltFile = NULL;
	nvfltFile = NULL;
	projection = -1;	// these are all recommended value for 'unused'
	zone = 0;
	units = -1;
	zUnits = -1;
	pUnits = -1;
	for (i=0; i<15; i++)
		GCTPparms[i] = 0.0;

	//	set up grid parameters; size etc
	xmin = (GetXlimits()[0]);
	xmax = (GetXlimits()[1]);
	ymin = (GetYlimits()[0]);
	ymax = (GetYlimits()[1]);
	xcol = GetColumns();
	yrow = GetRows();
	nullVal = (float) GetZnon();

	//	set up filename to write to
	//name = FolderPath + GetName() + ".2grd";	// we always write 2D grids
	strcpy(fname, (const char *)filename);

	//	copy data

	grid = new float[yrow*xcol];
	for (i=0; i<yrow; i++)
		for (j=0; j<xcol; j++)
			grid[j+i*xcol] = (float) GetGrid()[yrow-1-i][j];

	//	write file

	status = gridWrite2d (fname, desc,
		grid, xcol, yrow,
		xmin, xmax, ymin, ymax,
		nullVal,
		datFile, datField,
		vfltFile, nvfltFile,
		projection, zone, units, GCTPparms, zUnits);
	if ( status )
	{
		pDoc->Error("Error opening file " + name + " for writing.\n"
			"Did you remove the project directory?", MSG_ERR );
		delete(grid);
		return FALSE;
	}

	//	free memory
	delete(grid);

	return TRUE;

#else
	pDoc->Error("Earthvision format not supported in 64bit version of VelocityManager.", MSG_ERR );
	return FALSE;
#endif

}

//***********************************************************************
// Subroutine	:		  WriteZimsFile(void), ZMAP version
// Arguments  :     void
// Use        :     Writes the current Grid to disk as a zims file...
// Returns    :     TRUE - OK
//                  FALSE - Error occured...
//***********************************************************************
BOOL CZimsGrid::WriteZimsFile_ZMAP(CString FolderPath)
{
	ofstream zimsdata, zimsheader;
	_zims_header header;
	_zims_grid hgrid;
	float **fltarray;
	int i, j;
	char date[10], time[10];
	CString zimsname, zimsnameh, cname;
	zimsname = FolderPath + CMainFrame::GetNewFileName(FolderPath, GetName(), ".gd");
	zimsnameh = FolderPath + CMainFrame::GetNewFileName(FolderPath, GetName(), ".gh");

#ifdef VELMAN_UNIX
	zimsdata.open(zimsname);
	zimsheader.open(zimsnameh);
#else
	zimsdata.open(zimsname, ios::binary);
	zimsheader.open(zimsnameh, ios::binary);
#endif
	if(zimsdata.fail())
	{
		pDoc->Error("Could not open " + zimsname + ".gd"
			"\nDid you remove the grid database directory?", MSG_ERR);
		return FALSE;
	}
	if(zimsheader.fail())
	{
		pDoc->Error("Could not open " + zimsname + ".gh"
			"\nDid you remove the grid database directory?", MSG_ERR);
		zimsdata.close();
		return FALSE;
	}
	hgrid.nrows   = (long int) rows;
	hgrid.ncolumns = (long int) columns;
	hgrid.xmin = (double) xlim[0];
	hgrid.xmax = (double) xlim[1];
	hgrid.ymin = (double) ylim[0];
	hgrid.ymax = (double) ylim[1];
	hgrid.gxinc = (float) (hgrid.xmax-hgrid.xmin)/(hgrid.ncolumns-1);
	hgrid.gyinc = (float) (hgrid.ymax-hgrid.ymin)/(hgrid.nrows-1);
	hgrid.unused1 = (ZIMSWORD) GetType();
	hgrid.zero = 0;
	hgrid.xband = hgrid.yband = 0.0f;
	hgrid.reach = (float) (sqrt((xlim[1]-xlim[0])*(xlim[1]-xlim[0])+
		(ylim[0]-ylim[0])*(ylim[1]-ylim[0]))/2.0);
	hgrid.gmax = (float) CalcMaxVal();
	hgrid.gmin = (float) CalcMinVal();
	hgrid.znon = (float) GetZnon();
	hgrid.unused1 = 0l;
	for(i=0;i<12;i++)
		hgrid.unused2[i] = 0l;
	for(i=0;i<20;i++)
		header.filename[i] = ' ';
	strncpy(header.filename, (const char *) GetName(), GetName().GetLength());
	header.filetype = 1;
	_strdate(date);
	_strtime(time);
	sscanf(time, "%d%*c%d", &header.ndxdat_hr, &header.ndxdat_min);
	sscanf(date, "%d%*c%d%*c%d", &header.ndxdat_mon, &header.ndxdat_day,
		&header.ndxdat_yr);
	header.ndxnxd = (long int) columns+1;
	header.ndxnxh = (long int) 4;
	header.ndxprm = (long int) 3;
	header.ndxfld = (long int) 0;
	header.msc = (long int) 0;
	header.open = (long int) 0;
	header.ndxlen = (long int) hgrid.nrows;
	strncpy(header.filename1, "\000\000\000\000\000\000\000\000\000\000"
		"\000\000\000\000\000\000\000\000\000\000", 20);
	header.filetype1 = 0;
	strncpy(header.filename2, "\000\000\000\000\000\000\000\000\000\000"
		"\000\000\000\000\000\000\000\000\000\000", 20);
	header.filetype2 = 0;
	for(i=0;i<14;i++)
		header.unused1[i] = 0l;
	for(i=0;i<2;i++)
		header.unused2[i] = 0l;
	for(i=0;i<18;i++)
		header.unused3[i] = 0l;
#ifdef VELMAN_UNIX
	InvertData((char *)&header, zims_header_sec);
	InvertData((char *)&hgrid, zims_grid_sec);
#endif
	zimsheader.write((char *) &header, sizeof(_zims_header));
	zimsheader.write((char *) &hgrid, sizeof(_zims_grid));
	TRACE("filename : %s\n", header.filename);          /* 1-5: file's long name */
	TRACE("filetype : %ld\n", header.filetype);
	TRACE("ndxdat_day : %ld\n", header.ndxdat_day); /* 7: file creation - day */
	TRACE("ndxdat_mon : %ld\n", header.ndxdat_mon); /* 8: file creation - month */
	TRACE("ndxdat_yr : %ld\n", header.ndxdat_yr);  /* 9: file creation - year */
	TRACE("ndxdat_hr : %ld\n", header.ndxdat_hr);  /* 10: file creation - hour */
	TRACE("ndxdat_min : %ld\n", header.ndxdat_min); /* 11: file creation - minute */
	TRACE("ndxnxd : %ld\n", header.ndxnxd);     /* 12: rec # for next data record */
	TRACE("ndxnxh : %ld\n", header.ndxnxh);     /* 13: rec # for next data header record */
	TRACE("ndxprm : %ld\n", header.ndxprm);     /* 14: rec # of parameter record */
	TRACE("ndxfld : %ld\n", header.ndxfld);     /* 15: rec # of data fields beginning position */
	TRACE("msc : %ld\n", header.msc);        /* 16: */
	TRACE("open : %ld\n", header.open);       /* 17: */
	TRACE("ndxlen : %ld\n", header.ndxlen);     /* 18: */
	TRACE("filename1 : %s\n", header.filename1);      /* 33-37: support filename #1 */
	TRACE("filetype1 : %ld\n", header.filetype1);          /* 38: support file #1's type */
	TRACE("filename2 : %s\n", header.filename2);      /* 33-37: support filename #2 */
	TRACE("filetype2 : %ld\n", header.filetype2);          /* 38: support file #2's type */
	TRACE("zero : %ld, %lf\n", hgrid.zero, (double) hgrid.zero);       /* 1: numeric value 0.0 */
	TRACE("nrows : %ld\n", hgrid.nrows);      /* 2: # of rows */
	TRACE("ncolumns : %ld\n", hgrid.ncolumns);   /* 3: # of columns */
	TRACE("xmin : %lf\n", hgrid.xmin);       /* 5-6: xmin of grid */
	TRACE("xmax : %lf\n", hgrid.xmax);       /* 7-8: xmax of grid */
	TRACE("ymin : %lf\n", hgrid.ymin);       /* 9-10: ymin of grid */
	TRACE("ymax : %lf\n", hgrid.ymax);       /* 11-12: ymax of grid */
	TRACE("gxinc : %f\n", hgrid.gxinc);      /* 13: x increment */
	TRACE("gyinc : %f\n", hgrid.gyinc);      /* 14: y increment */
	TRACE("xband : %f\n", hgrid.xband);      /* 15: x expansion of collection region over AOI */
	TRACE("yband : %f\n", hgrid.yband);      /* 16: y expansion of collection region over AOI */
	TRACE("reach : %f\n", hgrid.reach);      /* 17: data collection radius */
	TRACE("znon : %f\n", hgrid.znon);       /* 18: znon value */
	TRACE("gmax : %f\n", hgrid.gmax);       /* 19: zmax of grid */
	TRACE("gmin : %f\n", hgrid.gmin);       /* 20: zmin of grid */
	fltarray = matrix(0, columns-1, 0, rows-1);
	Stamp();
	for(i=0;i<rows;i++)
		for(j=0;j<columns;j++)
			fltarray[j][i] = (float) GetGrid()[i][j];
#ifdef VELMAN_UNIX
	int number_grid_sec[4]={ -1, 4, 0, 0};   // -1 will be overwritten in a sec
	number_grid_sec[0]=rows*columns;
	InvertData((char *)fltarray[0], number_grid_sec);
#endif
	zimsdata.write((char *) fltarray[0], sizeof(float)*rows*columns);
	free_matrix(fltarray, 0, columns-1, 0, rows-1);
	return TRUE;
}

//***********************************************************************
// Subroutine	:		  WriteZimsFile(void), MFD version
// Arguments  :     void
// Use        :     Writes the current Grid to disk as a zims file...
// Returns    :     TRUE - OK
//                  FALSE - Error occured...
//***********************************************************************
BOOL CZimsGrid::WriteZimsFile_MFD(CString &filename0, CString &filename1, CString FolderPath)
{
#ifdef VELMAN_UNIX
	ifstream oldzimsfile;
	ofstream newzimsfile;
#else
	std::ifstream oldzimsfile;
	std::ofstream newzimsfile;
#endif
	meta_file_head *metahead = NULL;
	file_head *filehead = NULL;
	file_info *fileinfo = NULL;
	zims_grid *gridinfo = NULL;
	float *fgrid = NULL;
	char *buffer = NULL;
	int datakeys, keys, *key = NULL;
	int hnum = -1, currblock = -1, new_off = -1;
	int coff, currdatakey, currkeys, totalkeys, keyblocks;
	char one, ten;
	BOOL firstkey;
	time_t tp;
	tm *_tm;
	int i0, i, j, k;
	buffer = new char[ZIMS_BLOCK];
#ifdef VELMAN_UNIX
	newzimsfile.open(FolderPath + filename1);
#else
	newzimsfile.open(FolderPath + filename1, ios::binary);
#endif
	if(newzimsfile.fail())
	{
		pDoc->Error("Could not open new file " + filename1+
			"\nDid you remove the grid database directory?", MSG_ERR);
		return FALSE;
	}
	// if filename0 is present - append to old file...
	if(!filename0.IsEmpty())
	{
#ifdef VELMAN_UNIX
		oldzimsfile.open(FolderPath + filename0);
#else
		oldzimsfile.open(FolderPath + filename0, ios::binary);
#endif
		if(oldzimsfile.fail())
		{
			pDoc->Error("Could not open old file " + filename0, MSG_ERR);
			return FALSE;
		}
		// read the old header block
		oldzimsfile.read(buffer, ZIMS_BLOCK);
		currblock = 0;
		// metahead pointer into correct place
		metahead = (meta_file_head *) buffer;
		// Invert read data header
#ifndef VELMAN_UNIX
		InvertData((char *) metahead, meta_file_head_sec);
#endif
		// find new header number
		hnum = metahead->filenum;
		metahead->filenum++;
	}
	else
	{
		zero_mem(buffer, ZIMS_BLOCK);
		currblock = 0;
		metahead = (meta_file_head *) buffer;
		metahead->filenum = 1;
		metahead->meta_next = 0;
		hnum = 0;
		strncpy(metahead->filename, (const char *) filename1, 24);
		Pad(metahead->filename, 24);
		metahead->four = 4;
		metahead->endoff = 2; // headers starting at 0x800
	}
	// This is the number of keys required for the raw data only
	datakeys = (GetRows()*GetColumns()*sizeof(float)+
		ZIMS_BLOCK-1)/ZIMS_BLOCK;
	// This is where the new data will be written too
	new_off = metahead->endoff - 1;
	// add new data length to end_of_file offset
	// datakeys + keyblocks + file_head + grid_head
	keyblocks = 1;
	totalkeys = datakeys+2;
	if(totalkeys > ZIMS_BLOCK/sizeof(int)-2)
	{
		totalkeys -= ZIMS_BLOCK/sizeof(int)-2;
		keyblocks += 1+totalkeys/(ZIMS_BLOCK/sizeof(int)-4);
	}
	metahead->endoff += datakeys+keyblocks+2;
	// see if new header is in the next data block (0..14)
	// This has to be put to another file then by recursively calling WriteZimsFile()...
	if(hnum > 14)
	{
		j = filename1.Find('.');
		i = (int) 10*(filename1[j-2] - '0') + filename1[j-1] - '0' + 1;
		one = i%10 + '0';
		ten = i/10 + '0';
		filename0.SetAt(j-2, ten);
		filename0.SetAt(j-1, one);
		filename1.SetAt(j-2, ten);
		filename1.SetAt(j-1, one);
		delete buffer;
		return WriteZimsFile_MFD(CString(""), filename1, FolderPath);
	}
	// point filehead to correct location in current block
	// one is for the meta_file
	filehead = (file_head *) buffer+hnum+1;
	// fill the filehead structure
	strncpy(filehead->dataname, (const char *) GetName(), 24);
	Pad(filehead->dataname, 24);
	filehead->datatype   = ZIMS_TYPE_GRID;
	filehead->res0       = 0;
	filehead->block0     = 4;
	filehead->off3       = 0; // offset to 3numbers
	filehead->block2     = GetColumns()*GetRows()*sizeof(float)/ZIMS_BLOCK;
	filehead->res2       = GetColumns()*GetRows() -
		ZIMS_BLOCK*filehead->block2/sizeof(float);
	filehead->header_off = new_off+1; // This points to the keys header
	filehead->five       = 5;
	filehead->meta_head  = 1; // always refer to block 0
	// filehead structure is completed -> invert
	// metahead structure is completed -> invert
#ifndef VELMAN_UNIX
	InvertData((char *) filehead, file_head_sec);
	InvertData((char *) metahead, meta_file_head_sec);
#endif
	// read and write until we are at the new data location
	while(currblock < new_off)
	{
		newzimsfile.write(buffer, ZIMS_BLOCK);
		if(!filename0.IsEmpty())
		{
			oldzimsfile.read(buffer, ZIMS_BLOCK);
			long count = (long)oldzimsfile.gcount();
			if(count < ZIMS_BLOCK)
				zero_mem(buffer+count, ZIMS_BLOCK-count);
		}
		else
			zero_mem(buffer, ZIMS_BLOCK);
		currblock++;
	}
	// define inversion structures
#ifndef VELMAN_UNIX
	int key_sec[3];
	int fgrid_sec[3];
	key_sec[1] = sizeof(int);
	key_sec[2] = 0;
	fgrid_sec[0] = ZIMS_BLOCK/sizeof(float);
	fgrid_sec[1] = sizeof(float);
	fgrid_sec[2] =  0;
#endif
	// Write keys list.
	keys = datakeys+2; // These are all real keys: datakeys + info + grid
	key = ivector(0, ZIMS_BLOCK/sizeof(int)-1);
	zero_mem((char *) key, ZIMS_BLOCK);
	firstkey = TRUE;
	coff = new_off;
	do
	{
		// currkeys contains the actual number of keys
		if(firstkey)
		{
			currkeys = (keys>ZIMS_BLOCK/sizeof(int)-2)? ZIMS_BLOCK/sizeof(int)-2:keys;
			key[0] = currkeys+2;
		}
		else
		{
			currkeys = (keys>ZIMS_BLOCK/sizeof(int)-4)? ZIMS_BLOCK/sizeof(int)-4:keys;
			key[0] = currkeys+4;
		}
		// This points to the next keyring
		key[1] = (keys>ZIMS_BLOCK/sizeof(int)-2)? coff+currkeys+2:0;
		// invert length and next key
#ifndef VELMAN_UNIX
		key_sec[0] = 2;
		InvertData((char *) key, key_sec);
#endif
		// these are the info + first data keys, they are written only once
		if(firstkey)
		{
			for(i=2;i<currkeys+2;i++)
				key[i] = coff+i;
			for(;i<ZIMS_BLOCK/sizeof(int);i++)
				key[i] = 0;
			// invert info + data keys
#ifndef VELMAN_UNIX
			key_sec[0] = ZIMS_BLOCK/sizeof(int)-2;
			InvertData((char *) (key+2), key_sec);
#endif
		}
		// These are the nth data keys, write as many of them as you can
		else
		{
			for(i=4;i<currkeys+4;i++)
				key[i] = coff+i-2;
			for(;i<ZIMS_BLOCK/sizeof(int);i++)
				key[i] = 0;
			// invert only data keys
#ifndef VELMAN_UNIX
			key_sec[0] = ZIMS_BLOCK/sizeof(int)-4;
			InvertData((char *) (key+4), key_sec);
#endif
		}
		// write key block to disk...
		newzimsfile.write((char *) key, ZIMS_BLOCK);
		currblock++;
		currdatakey = 0;
		if(firstkey)
		{
			// get the system date and time
			time(&tp);
			_tm = localtime((const time_t *) &tp);
			// complete the fileinfo structure in order
			fileinfo = (file_info *) buffer;
			strncpy(fileinfo->dataname, (const char *) GetName(), 24);
			Pad(fileinfo->dataname, 24);
			fileinfo->datatype = ZIMS_TYPE_GRID;
			fileinfo->one = 1;
			strncpy(fileinfo->filename, (const char *) filename1, 24);
			Pad(fileinfo->filename, 24);
			strftime(fileinfo->date, 21, "%d  %b %y  %H  %M", (const tm *) _tm);
			fileinfo->ident = 1;
			fileinfo->twoeight0 = 0x28;
			fileinfo->twoeight1 = 0x28;
			// fileinfo structure is completed -> invert
#ifndef VELMAN_UNIX
			InvertData((char *) fileinfo, file_info_sec);
#endif
			// write file_info block to disk and zero old block
			newzimsfile.write(buffer, ZIMS_BLOCK);
			zero_mem(buffer, ZIMS_BLOCK);
			currblock++;
			currdatakey++;
			// complete the zims_grid structure in order
			gridinfo = (zims_grid *) buffer;
			gridinfo->zero=0;
			gridinfo->nrows=GetRows();
			gridinfo->ncolumns=GetColumns();
			gridinfo->xmin=xlim[0];
			gridinfo->xmax=xlim[1];
			gridinfo->ymin=ylim[0];
			gridinfo->ymax=ylim[1];
			gridinfo->gxinc=(float) (xlim[1]-xlim[0])/(GetColumns()-1);
			gridinfo->gyinc=(float) (ylim[1]-ylim[0])/(GetRows()-1);
			gridinfo->xband=(float) (xlim[1]-xlim[0]);
			gridinfo->yband=(float) (ylim[1]-ylim[0]);
			gridinfo->reach=(float) sqrt(gridinfo->xband*gridinfo->xband+
				gridinfo->yband*gridinfo->yband);
			gridinfo->znon=(float) GetZnon();
			gridinfo->gmax=(float) CalcMaxVal();
			gridinfo->gmin=(float) CalcMinVal();
			// gridinfo structure is completed -> invert
#ifndef VELMAN_UNIX
			InvertData((char *) gridinfo, zims_grid_sec);
#endif
			// write grid_info block to disk and zero old block
			newzimsfile.write(buffer, ZIMS_BLOCK);
			zero_mem(buffer, ZIMS_BLOCK);
			currblock++;
			currdatakey++;
		}
		// initialize float buffer - but once only
		if(firstkey)
		{
			fgrid = (float *) buffer;
			j=0;
			i0=0;
		}
		k=0;
		// write floating data to disk
		Stamp();
		while(j<GetColumns() && currdatakey<currkeys)
		{
			i=i0;
			while(i<GetRows() && currdatakey<currkeys)
			{
				if(k < ZIMS_BLOCK/sizeof(float))
				{
					fgrid[k] = (float) GetGrid()[i][j];
					i++;
					k++;
				}
				else
				{
#ifndef VELMAN_UNIX
					InvertData(buffer, fgrid_sec);
#endif
					newzimsfile.write(buffer, ZIMS_BLOCK);
					zero_mem(buffer, ZIMS_BLOCK);
					currblock++;    // increment block count
					currdatakey++;  // increment data key count
					k=0;
				}
			}
			if(currdatakey<currkeys)
			{
				j++;
				i0=0;
			}
		}
		// remember position for next data
		i0=i;
		// update loop counters
		keys -= currkeys;
		coff += currkeys+1;
		firstkey = FALSE;
		// do until all keys have been written
	} while(keys > 0);
	// write last block to disk
#ifndef VELMAN_UNIX
	InvertData(buffer, fgrid_sec);
#endif
	newzimsfile.write(buffer, ZIMS_BLOCK);
	zero_mem(buffer, ZIMS_BLOCK);
	currblock++;
	delete buffer;
	free_ivector(key, 0, ZIMS_BLOCK/sizeof(int)-1);
	return TRUE;
}

//***********************************************************************
// Subroutine	:		  WriteZimsFile(void), CP3 version
// Arguments  :     void
// Use        :     Writes the current Grid to disk as a zims file...
// Returns    :     TRUE - OK
//                  FALSE - Error occured...
//***********************************************************************
BOOL CZimsGrid::WriteZimsFile_CP3(CString FolderPath)
{
	CString filename = FolderPath + CMainFrame::GetNewFileName(FolderPath, GetName(), ".svs");

	ofstream cp3file;
	cp3_file_head filehead;
	cp3_grid cp3grid;
	float *cp3col;
	CP3WORD cp3colsize;
	int i, j, znonnum, cp3realsize;
	char dataname[128];

	// define column byte reversal block
#ifndef VELMAN_UNIX
	int cp3col_sec[3];
	cp3col_sec[1] = sizeof(float);
	cp3col_sec[2] = 0;
#endif

#ifdef VELMAN_UNIX
	cp3file.open(filename);
#else
	cp3file.open(filename, ios::binary);
#endif
	if(cp3file.fail())
	{
		pDoc->Error("Error opening file " + filename + " for writing."
			"\nDid you remove the grid database directory?", MSG_ERR);
		return FALSE;
	}
	filehead.ifile = 1;
	filehead.ivnum = 1;
	filehead.nsubs = 0;
	filehead.itype = CP3_TYPE_GRID;
#ifndef VELMAN_UNIX
	InvertData((char *) &filehead, cp3_file_head_sec);
#endif
	FortranWrite(cp3file, (char *) &filehead, sizeof(cp3_file_head));
	strncpy(dataname, (const char *) GetName(), 128);
	Pad(dataname, 128);
	FortranWrite(cp3file, dataname, 128);
	cp3grid.nrow=GetRows();
	cp3grid.ncol=GetColumns();
	cp3grid.zmin=(float) CalcMinVal();
	cp3grid.zmax=(float) CalcMaxVal();
	cp3grid.xmin=(float) (GetXlimits()[0]);
	cp3grid.xmax=(float) (GetXlimits()[1]);
	cp3grid.ymin=(float) (GetYlimits()[0]);
	cp3grid.ymax=(float) (GetYlimits()[1]);
	cp3grid.xinc=(cp3grid.xmax-cp3grid.xmin)/(cp3grid.ncol-1);
	cp3grid.yinc=(cp3grid.ymax-cp3grid.ymin)/(cp3grid.nrow-1);
	for(i=0;i<56;i++) cp3grid.unused0[i] = 0x00;
	cp3grid.findt=(float) GetZnon();
	for(i=0;i<924;i++) cp3grid.unused1[i] = 0x00;
#ifndef VELMAN_UNIX
	InvertData((char *) &cp3grid, cp3_grid_sec);
#endif
	FortranWrite(cp3file, (char *) &cp3grid, sizeof(cp3_grid));
	// allocate memory for one CP3 column
	cp3col = vector(0, GetRows()-1);
	// write length of zero bias column
	cp3colsize = GetRows();
#ifndef VELMAN_UNIX
	InvertData((char *) &cp3colsize, one_word_sec);
#endif
	FortranWrite(cp3file, (char *) &cp3colsize, sizeof(CP3WORD));
	// zero bias column
	for(i=0;i<GetRows();i++)
		cp3col[i] = 0.0f;
#ifndef VELMAN_UNIX
	cp3col_sec[0]=GetRows();
	InvertData((char *) cp3col, cp3col_sec);
#endif
	FortranWrite(cp3file, (char *) cp3col, GetRows()*sizeof(float));
	// write compressed data column by column
	Stamp();
	for(j=0;j<GetColumns();j++)
	{
		cp3colsize = 0;
		i=0;
		znonnum = 0;
		while(i<GetRows() || znonnum !=0)
		{
			if(i != GetRows() && GetZnon(GetGrid()[i][j]))
			{
				znonnum++;
			}
			else
			{
				switch(znonnum)
				{
				case 0:
					cp3col[cp3colsize] = (float) GetGrid()[i][j];
					cp3colsize++;
					break;
				case 1:
					cp3col[cp3colsize] = (float) GetZnon();
					if(i != GetRows())
					{
						cp3col[cp3colsize+1] = (float) GetGrid()[i][j];
						cp3colsize += 2;
					}
					else
					{
						cp3colsize++;
					}
					znonnum = 0;
					break;
				default:
					cp3col[cp3colsize] = (float) -GetZnon();
					cp3col[cp3colsize+1] = (float) znonnum;
					if(i != GetRows())
					{
						cp3col[cp3colsize+2] = (float) GetGrid()[i][j];
						cp3colsize += 3;
					}
					else
					{
						cp3colsize += 2;
					}
					znonnum = 0;
				}
			}
			i++;
		}
		// save real size in case it gets inverted
		// then write the column size record
		cp3realsize = cp3colsize;
#ifndef VELMAN_UNIX
		InvertData((char *) &cp3colsize, one_word_sec);
#endif
		FortranWrite(cp3file, (char *) &cp3colsize, sizeof(CP3WORD));
		// write this compressed column to disk
#ifndef VELMAN_UNIX
		cp3col_sec[0]=cp3realsize;
		InvertData((char *) cp3col, cp3col_sec);
#endif
		FortranWrite(cp3file, (char *) cp3col, cp3realsize*sizeof(float));
	}
	// write last column as zero length
	cp3colsize = 0;
#ifndef VELMAN_UNIX
	InvertData((char *) &cp3colsize, one_word_sec);
#endif
	FortranWrite(cp3file, (char *) &cp3colsize, sizeof(CP3WORD));
	// free memory
	free_vector(cp3col, 0, GetRows()-1);
	return TRUE;
}

//***********************************************************************
// Subroutine	:		  WriteZimsFile(void), CP3 version, internal
// Arguments  :     void
// Use        :     Writes the current Grid to disk as a zims file...
// Returns    :     TRUE - OK
//                  FALSE - Error occured...
//***********************************************************************
BOOL CZimsGrid::WriteZimsFile_CP3int(CString FolderPath)
{
	// local variables
	int i, j;
	int blklength, val, totallength, itemlength;
	BOOL newformat = FALSE;
	char buffer[6144], *intptr;

	CString filename = FolderPath + CMainFrame::GetNewFileName(FolderPath, GetName(), ".scps");

	time_t now;
	tm *_tm;

	ofstream zimsfile;
#ifndef VELMAN_UNIX
	int cp3col_sec[3];
	cp3col_sec[1] = sizeof(float);
	cp3col_sec[2] = 0;
#endif

	// float grid, transposed
	float **fgrid = NULL;

	// define structures for internal CP3 file
	static char magicnum[0x11] = {"CPS3    rch     "};
	static char lockblk[0x11]  = {"LOCK    cch     "};
	static char descblk[0x641] = {"    "
		"                "
		"                "
		"                "
		"                "
		"                "
		"  "};
	static char mainblk[0x11]  = {"MAIN    csm     "};
	static char subsblk[0x11]  = {"SUBS    csm     "};
	static char en[0x02]       = {"N"};
	static char nameblk[0x50];
	static char colsep[0x02]   = {0x5a, 0x00};
	static char prmblk[0x11]   = {"PARM    ccp     "};
	static char endblk_old[0x11] = {"END     cch     "};
	static char endblk_new[33]   = {"                END     cch     "};	// change to end block
	const int  parm_len_old = 45;
	static CP3intItem cp3item_old[parm_len_old]
	= { {"I1SAFI", "0"},
	{"I2SAFI", "0"},
	{"IASASC", "0"},
	{"INSASC", "0"},
	{"IPSASC", "1"},
	{"ISSSUI", "1"},
	{"ITSAFI", "3"},
	{"ITSATT", "0"},
	{"ITSIAL", "1"},
	{"IZSATT", "0"},
	{"MOSASC", "0"},
	{"MOSIAL", "1"},
	{"NCSNRO", ""},
	{"NDSIAL", "1"},
	{"NRSNRO", ""},
	{"NSSSUI", "INDT"},
	{"NUMREC_", ""},
	{"NUMSUB_", "1"},
	{"PCSIPO(1)", "0.0"},
	{"PCSIPO(10)", "0.0"},
	{"PCSIPO(2)", "0.0"},
	{"PCSIPO(3)", "0.0"},
	{"PCSIPO(4)", "0.0"},
	{"PCSIPO(5)", "0.0"},
	{"PCSIPO(6)", "0.0"},
	{"PCSIPO(7)", "0.0"},
	{"PCSIPO(8)", "0.0"},
	{"PCSIPO(9)", "0.0"},
	{"QCDATE_", ""},
	{"QMDATE_", ""},
	{"QXSASC", "(5e15.8)"},
	{"RASGLA", "-9"},
	{"RBSGLA", "-5"},
	{"RECLEN_", ""},
	{"RISASC", ""},
	/*{"X1SLIM", ""},
	{"X2SLIM", ""},
	{"XISXIN", ""},
	{"Y1SLIM", ""},
	{"Y2SLIM", ""},
	{"YISXIN", ""},
	{"Z1SLIM", ""},
	{"Z2SLIM", ""}, */
	{"U1SLIM", ""},				// 90. x min
	{"U2SLIM", ""},				// x max
	{"XISXIN", ""},
	{"U3SLIM", ""},				// y min
	{"U4SLIM", ""},				// y max
	{"YISXIN", ""},
	{"U5SLIM", ""},				// z min
	{"U6SLIM", ""},				// z max
	{"_GF_PROPERTY_CODE", "Time"},
	{"_UNITS_Z", "s"}	};

	//  this is taken from the parameter block of a typical cps file..
	const int  parm_len = 140;
	static CP3intItem cp3item[parm_len] = {
		{"CIMCLL", "COMPUTED"},		// 0. no idea what this is, some files have numerical values here
		{"CLMCLL", "COMPUTED"},		// no idea what this is
		{"COSMSA(1)", "770.00000"},	// ditto
		{"COSMSA(2)", "1.00000000"},	// ditto
		{"COSMSA(3)", "1.00000000"},	// ditto
		{"COSMSA(4)", "1.00000000"},	// ditto
		{"COSMSA(5)", "1.00000000"},	// ditto
		{"COSMSA(6)", "1.00000000"},	// ditto
		{"DFSMOD", "4.0000000"},		// ditto
		{"DZSMOD", "-5.0000000"},		// ditto
		{"FUSMOD", "100.000000"},		// 10. ditto
		{"IASCST", "2"},				// ditto
		{"ICSIPR", "38"},				// ditto
		{"IDSGBI", "283989"},			// ditto
		{"IDSMOD", "1"},				// ditto
		{"IFSPJM", "1"},				// ditto
		{"IGSPJZ", "23"},				// ditto
		{"IHSHEM", "-1"},				// ditto
		{"IHSKEY", "33992"},			// ditto
		{"IHSSCL", "1"},				// ditto
		{"ILSLTS", "2"},				// 20. ditto
		{"INMPAL", "1"},				// ditto
		{"ISSSUI", "1"},				// ditto
		{"ITMCLA", "1"},				// ditto
		{"ITSMOD", "2"},				// ditto
		{"ITSMSA", "104"},				// ditto
		{"IXSAXE", "0"},				// something to do with an x-axis?
		{"IXSGBI", ""},				// .. try this as x axis spacing.. see IYSGBI
		{"IYSAXE", "0"},				// something to do with y-axis?
		{"IYSGBI", ""},				// We think this might be spacing in y-axis, but if
		//   that's the case why do we have zero for IXSBGI ?
		{"JXSAXE", "0"},				// 30. something to do with x axis again?
		{"JXSGBI", ""},				// think no. of columns
		{"JYSAXE", "0"},				// no idea
		{"JYSGBI", ""},				// think no. of rows
		{"KXSAXE", "1"},				// some property of the x axis no doubt
		{"KXSGBI", "1"},				// ditto
		{"KYSAXE", "1"},				// some property of the y axis
		{"KYSGBI", "-1"},				// ditto
		{"LSMPAL", "0"},				// no idea, but guess 'land surface map colour palette?
		{"LXSAXE", "1"},				// something to do with x axis.. maybe plotting info?
		{"LXSGBI", "1"},				// 40. ditto
		{"LYSAXE", "1"},				// something for y axis.. maybe plotting info?
		{"LYSGBI", "1"},				// ditto
		{"MMSSCL", "0"},				// no idea
		{"MOSLIZ", "0"},				// ditto
		{"MOSMOD", "0"},				// ditto
		{"MOSUSG", "0"},				// ditto
		{"MOSUSZ", "0"},				// ditto
		{"MPSSCL", "0"},				// ditto
		{"NCMCLL", "32768"},			// I think this is no. of colours in 16 bit colour palette
		{"NCSNRO", ""},				// 50. no. of columns
		{"NDSMOD", "1"},				// don't know
		{"NIMCLA", "4"},				// guess some property of the I axis on plot?
		{"NJMCLA", "4"},				// likewise for J axis on plot?
		{"NPSMOD", "8"},				// dunno
		{"NRSNRO", ""},				// no. of rows
		{"NSSSUI", "INDT"},			// dunno
		{"NTSMOD", "16"},				// dunno
		{"NUMREC_", ""},				// no. of records ie. columns
		{"NUMSUB_", "1"},				// dunno
		{"ODMCLA", "90.000000"},		// 60. projection info? origin of projection?
		{"OTMCLA", "0.0"},				// ditto
		{"PROJID_", "-1"},				// choice of map projection?
		// parameters beginning with 'Q' seem to hold character data
		{"QASGBI", ""},				// some default name for data?
		{"QASHUN", "m"},				// units for x axis on plot??
		{"QBSGED", ""},				// who knows what this is.. eg GF:SA69-BZ
		{"QBSHUN", "m"},				// units for y axis on plot??
		{"QC4DAT_", ""},				// a date.. not sure of what exactly
		{"QCSELP", ""},				// some sort of name? e.g. SA69, leave blank
		{"QDSNAM_(1)[1]", "SCR_931608689 [1714183340]"},	//  what is this name for?
		{"QDSNAM_(2)[1]", "null"},		// 70. some sort of name...?
		{"QDSNAM_(3)[1]", "null"},		// ditto
		{"QDSNAM_(4)[1]", "null"},		// ditto
		{"QDSNAM_(5)[1]", "null"},		// ditto
		{"QDSNAM_(6)[1]", "null"},		// ditto
		{"QDSNAM_(7)[1]", "null"},		// ditto
		{"QDSNAM_(8)[1]", "null"},		// ditto
		{"QFSNAM_(1)[1]", "null"},		// ditto
		{"QM4DAT_", ""},	//	a date of some description, not same as QC4DAT, this is later
		{"QPMPAL", "Rainbow"},			// colour palette name
		{"QPSNAM_(1)[1]", "Seabed [315961922]"},	// 80. horizon name?  what's the number?
		{"RASGBI", "0.0"},				// no idea
		{"RASGLA", ""},				// think this is grid node x spacing */
		{"RBSGLA", ""},				// think this is grid spacing in y */
		{"RECLEN_", ""},				// no. of rows
		{"REMRES(1)", "0.38000000"},	// no idea?
		{"REMRES(2)", "0.38000000"},	// ditto
		{"SMSSCL", "0.0"},				// ditto
		{"SPSSCL", "0.0"},				// ditto
		{"SSETID_", "1085750959"},		// guess some sort of ID number??
		{"U1SLIM", ""},				// 90. x min
		{"U2SLIM", ""},				// x max
		{"U3SLIM", ""},				// y min
		{"U4SLIM", ""},				// y max
		{"U5SLIM", ""},				// z min
		{"U6SLIM", ""},				// z max
		{"UASHUM", "1.000000000000"},	// no idea
		{"UBSCMR", "0.0"},				// ditto
		{"UBSHUM", "1.000000000000"},	// ditto
		{"UCSBPR", "0.0"},				// ditto
		{"UDSST1", "0.0"},				// 100. ditto
		{"UESST2", "0.0"},				// ditto
		{"UGSNLT", "0.0"},				// ditto
		{"UHSEQL", "0.0"},				// ditto
		{"UISNQL", "0.0"},				// ditto
		{"UJSSQL", "0.0"},				// ditto
		{"UOSCLT", "0.0"},				// ditto
		{"UPSCLN", "0.0"},				// ditto
		{"UQSAZM", "0.0"},				// ditto
		{"URSFES", "0.0"},				// ditto
		{"USSFNR", "0.0"},				// 110. ditto
		{"UTSSFC", "1.000000000000"},	// ditto
		{"UUSXPV", ""},				// x min
		{"UVSYPV", ""},				// y min
		{"UWSROT", "0.0"},				// no idea
		{"UXSBXS", ""},				// grid spacing in x
		{"UXSGBI", ""},				// x min (again!)
		{"UYSBYS", ""},				// grid spacing in y
		{"UYSGBI", ""},				// y min (again!)
		{"UZSAEL", "0.0"},				// prob. something to do with z axis?
		{"UZSAGS", "0.0"},				// 120. ditto
		{"UZSREA", "0.0"},				// ditto
		{"VSSVSL", "1000.00000"},		// no idea
		{"XISGBI", ""},				// x grid spacing
		{"XISMOD", "COMPUTED"},		// ditto
		{"XISXIN", ""},				// x grid spacing
		{"YISGBI", ""},				// y grid spacing
		{"YISMOD", "COMPUTED"},		// no idea
		{"YISXIN", ""},				// y grid spacing
		{"ZASLIZ", "INDT"},			// something to do with start of z axis?
		{"ZBSLIZ", "INDT"},			// 130. something to do with end of z axis?
		{"_GF_CONTAINER_ID", "37716"},	// some sort of id number? do I change it?
		{"_GF_CONTAINER_NAME", "MVE_01_Seabed"},	// horizon name?
		{"_GF_CONTAINER_TYPE", "Horizon"},			// type of data.. leave as is
		{"_GF_DIMENSION", "Time"},		// define dimension type I guess */
		{"_GF_DI_ID", "-1"},			// dimension id?
		{"_GF_DI_NAME", ""},			// some sort of name for DI .. whatever that is
		{"_GF_PROPERTY_CODE", "Time"},	// property of time dimension coming up
		{"_SM_SUBTYPE", "401"},		// prob. some code associated with a time axis
		{"_UNITS_Z", "ms"},			// units of z axis (s for seconds or 'ms' for millisecs
		// should this be msec or 'ms' for millisecs?  NEED TO CHECK GRID UNIT!!!
	};

	// Create random cp3 internal name for the grid
#ifdef VELMAN_UNIX
	zimsfile.open(filename);
#else
	zimsfile.open(filename, ios::binary);
#endif
	if(zimsfile.fail())
	{
		pDoc->Error("Error opening file " + filename + " for writing."
			"\nDid you remove the grid database directory?", MSG_ERR);
		return FALSE;
	}

	// write magic number header
	zimsfile.write(magicnum, 0x10);

	// this is a 4 byte int. I have no idea about its meaning
	blklength = 4;
#ifndef VELMAN_UNIX
	InvertData((char *) &blklength, one_word_sec);
#endif
	zimsfile.write((char *) &blklength, sizeof(int));

	if ( newformat )
		val = 0xc0c0;
	else
		val = 0x00;			// noted recent cp3 files have this byte sequence
#ifndef VELMAN_UNIX
	InvertData((char *) &val, one_word_sec);
#endif
	zimsfile.write((char *) &val, sizeof(int));

	// write lock block
	zimsfile.write(lockblk, 0x10);

	// write main description block
	// fill the rest of the description block with spaces
	for(i=0x56;i<0x640;i++)
		descblk[i] = 0x20;
	blklength = 0x640;
#ifndef VELMAN_UNIX
	InvertData((char *) &blklength, one_word_sec);
#endif
	zimsfile.write((char *) &blklength, sizeof(int));
	zimsfile.write(descblk, 0x640);
	zimsfile.write(mainblk, 0x10);

	// write total data length and SUBS block
	totallength = 0x55 + GetColumns()*(1+sizeof(int)+GetRows()*sizeof(float));
	blklength = totallength +0x14;
#ifndef VELMAN_UNIX
	InvertData((char *) &blklength, one_word_sec);
#endif
	zimsfile.write((char *) &blklength, sizeof(int));
	zimsfile.write(subsblk, 0x10);

	// write total datalength and N block
	blklength = totallength;
#ifndef VELMAN_UNIX
	InvertData((char *) &blklength, one_word_sec);
#endif
	zimsfile.write((char *) &blklength, sizeof(int));
	zimsfile.write(en, 0x01);

	// write filenameblock length and filename block
	// now fill the name into the nameblk, but first zero entire block;
	for(i=0;i<0x50;i++)
		nameblk[i] = 0x00;
	sprintf(nameblk, "%8d  ->%s", GetColumns()+1, (LPCTSTR) GetName());
	blklength = 0x50;
#ifndef VELMAN_UNIX
	InvertData((char *) &blklength, one_word_sec);
#endif
	zimsfile.write((char *) &blklength, sizeof(int));
	zimsfile.write(nameblk, 0x50);

	// now we start writing the actual floating point data
	// allocate transposed floating point grid...
	fgrid = matrix(0, GetColumns()-1, 0, GetRows()-1);

	// fill floating point grid and transpose as we go along;
	for(i=0;i<GetRows();i++)
		for(j=0;j<GetColumns();j++)
		{
			fgrid[j][i] = GetGrid()[i][j];
			if (fgrid[j][i] = (float) ZIMS_VAL_ZNON )
				fgrid[j][i] = (float) 9.9999926e29;	// 10/07. new file format seems to use this as ZNON
		}									// but param block does not include ZNON parameter so just match the number

		// store number of rows in blklength
		blklength = GetRows()*sizeof(int);
#ifndef VELMAN_UNIX
		InvertData((char *) &blklength, one_word_sec);
		cp3col_sec[0] = GetRows();
#endif

		// now write the grid
		for(j=0;j<GetColumns();j++)
		{
			zimsfile.write(colsep, 0x01);
			zimsfile.write((char *) &blklength, sizeof(int));
#ifndef VELMAN_UNIX
			InvertData((char *) fgrid[j], cp3col_sec);
#endif
			zimsfile.write((char *) fgrid[j], GetRows()*sizeof(float));
		}

		// free the temporary floating point grid
		free_matrix(fgrid, 0, GetColumns()-1, 0, GetRows()-1);

		// after data write parameter block
		zimsfile.write(prmblk, 0x10);
		// first fill the CP3int data base with the values from the grid
		// get the system date and time
		time(&now);
		_tm = localtime((const time_t *) &now);

		//    this is for old parameter block
		if ( ! newformat )
		{
			sprintf(cp3item[12].item_val, "%d", GetColumns());
			sprintf(cp3item[14].item_val, "%d", GetRows());
			sprintf(cp3item[16].item_val, "%d", GetColumns());
			strftime(cp3item[28].item_val, 32, "%d-%b-%Y %H:%M:%S",_tm);
			strftime(cp3item[29].item_val, 32, "%d-%b-%Y %H:%M:%S",_tm);
			sprintf(cp3item[33].item_val, "%d", GetRows());
			sprintf(cp3item[34].item_val, "%lf", GetZnon());
			sprintf(cp3item[35].item_val, "%lf", xlim[0]);
			sprintf(cp3item[36].item_val, "%lf", xlim[1]);
			sprintf(cp3item[37].item_val, "%lf", (xlim[1]-xlim[0])/(GetColumns()-1));
			sprintf(cp3item[38].item_val, "%lf", ylim[0]);
			sprintf(cp3item[39].item_val, "%lf", ylim[1]);
			sprintf(cp3item[40].item_val, "%lf", (ylim[1]-ylim[0])/(GetRows()-1));
			sprintf(cp3item[41].item_val, "%lf", CalcMinVal());
			sprintf(cp3item[42].item_val, "%lf", CalcMaxVal());
		}
		else
		{
			//	New parameter block.
			sprintf(cp3item[31].item_val, "%d", GetColumns());	// JXSGBI
			sprintf(cp3item[50].item_val, "%d", GetColumns());	// NRSNRO
			sprintf(cp3item[58].item_val, "%d", GetColumns());	// NUMREC_
			sprintf(cp3item[33].item_val, "%d", GetRows());		// JYSGBI
			sprintf(cp3item[55].item_val, "%d", GetRows());		// NRSNRO
			sprintf(cp3item[84].item_val, "%d", GetRows());		// RECLEN_
			strftime(cp3item[67].item_val, 32, "%d-%b-%Y %H:%M:%S",_tm);	// QC4DAT_
			strftime(cp3item[78].item_val, 32, "%d-%b-%Y %H:%M:%S",_tm);	// QM4DAT_
			sprintf(cp3item[90].item_val, "%lf", xlim[0]);		// U1SLIM
			sprintf(cp3item[112].item_val, "%lf", xlim[0]);		// UUSPV
			sprintf(cp3item[116].item_val, "%lf", xlim[0]);		// UXSGBI
			sprintf(cp3item[91].item_val, "%lf", xlim[1]);		// U2SLIM
			sprintf(cp3item[92].item_val, "%lf", ylim[0]);		// U3SLIM
			sprintf(cp3item[113].item_val, "%lf", ylim[0]);		// UVSYPV
			sprintf(cp3item[118].item_val, "%lf", ylim[0]);		// UYSGBI
			sprintf(cp3item[93].item_val, "%lf", ylim[1]);		// U4SLIM
			sprintf(cp3item[27].item_val, "%d", (xlim[1]-xlim[0])/(GetColumns()-1));	// IXSGBI
			sprintf(cp3item[82].item_val, "%lf", (xlim[1]-xlim[0])/(GetColumns()-1));	// RASGLA
			sprintf(cp3item[115].item_val, "%lf", (xlim[1]-xlim[0])/(GetColumns()-1));	// UXSBXS
			sprintf(cp3item[123].item_val, "%lf", (xlim[1]-xlim[0])/(GetColumns()-1));	// XISGBI
			sprintf(cp3item[125].item_val, "%lf", (xlim[1]-xlim[0])/(GetColumns()-1));	// XISXIN
			sprintf(cp3item[29].item_val, "%d", (ylim[1]-ylim[0])/(GetRows()-1));		// IYSGBI
			sprintf(cp3item[83].item_val, "%lf", (ylim[1]-ylim[0])/(GetRows()-1));		// RBSGLA
			sprintf(cp3item[117].item_val, "%lf", (ylim[1]-ylim[0])/(GetRows()-1));		// UYSGBI
			sprintf(cp3item[126].item_val, "%lf", (ylim[1]-ylim[0])/(GetRows()-1));		// YISGBI
			sprintf(cp3item[128].item_val, "%lf", (ylim[1]-ylim[0])/(GetRows()-1));		// YISXIN
			sprintf(cp3item[44].item_val, "%lf", CalcMinVal());		// U5SLIM
			sprintf(cp3item[45].item_val, "%lf", CalcMaxVal());		// U6SLIM */
		}

		if ( ! newformat )    //  old header code from here .....
		{
			totallength = 0;
			for(i=0;i<parm_len_old;i++)
			{
				itemlength = strlen(cp3item[i].item_id)+strlen(cp3item[i].item_val)+1;
				blklength = itemlength;
#ifndef VELMAN_UNIX
				InvertData((char *) &blklength, one_word_sec);
#endif
				intptr = (char *) &blklength;
				buffer[totallength++]=intptr[2];
				buffer[totallength++]=intptr[3];
				sprintf(buffer+totallength, "%s=%s", cp3item[i].item_id, cp3item[i].item_val);
				totallength += itemlength;
			}

			itemlength=0x00;
			blklength = itemlength;
#ifndef VELMAN_UNIX
			InvertData((char *) &blklength, one_word_sec);
#endif
			intptr = (char *) &blklength;
			buffer[totallength++]=intptr[2];
			buffer[totallength++]=intptr[3];

			blklength = totallength;
#ifndef VELMAN_UNIX
			InvertData((char *) &blklength, one_word_sec);
#endif
			zimsfile.write((char *) &blklength, sizeof(int));
			zimsfile.write(buffer, totallength);
			zimsfile.write(endblk_old, 0x10);
		}
		else
		{

			blklength = cp3int_parmlen;
#ifndef VELMAN_UNIX
			InvertData((char *) &blklength, one_word_sec);
#endif
			zimsfile.write((char *) &blklength, sizeof(int));
			zimsfile.write(cp3int_parm, cp3int_parmlen);
			zimsfile.write(endblk_new, 33);
		}

		return TRUE;
}

//***********************************************************************
// Subroutine	:			WriteASCII()
// Arguments	:			Folder to put file in
// Use        :     Write ASCII Dump file of CZimsGrid
// Returns    :     sucess
//***********************************************************************
BOOL CZimsGrid::WriteASCII(CString FolderPath)
{
	CString filename = FolderPath + CMainFrame::GetNewFileName(FolderPath, GetName(), ".xls");
	ofstream outfile;

#ifdef VELMAN_UNIX
	outfile.open((const char *)filename);
#else
	outfile.open((const char *)filename, ios::binary);
#endif

	if(outfile.fail())
	{
		pDoc->Error("Error writing file: " + GetName(), MSG_ERR);
		return FALSE;
	}

	int r, col;

	Stamp();
	outfile << "Name :\t" << name << "\tID: " << GetID() << endl
		<< "Type :\t" << (int) GetType() << "\tAverage :\t" <<  avg	<<endl
		<< "Rows :\t" << rows << "\tColumns :\t" << columns << endl
		<< "XLim :\t" << xlim[0] << "\tto\t" << xlim[1] << endl
		<< "YLim :\t" << ylim[0] << "\tto\t" << ylim[1] << endl
		<< "NULL :\t" << znon << "\t# NULL values :\t" << znonvals << endl
		<< "t power:\t" << unit_timepower << "\tx power:" << unit_distpower << "\n\n";
	for(col=0;col<columns;col++)
		outfile << "\t" <<xlim[0]+dx*col;
	for(r=0;r<rows;r++)
	{
		outfile << endl << ylim[1]-dy*r;
		for(col=0;col<columns;col++)
		{
			outfile << "\t" << GetGrid()[r][col];
		}
	}
	outfile << endl << endl;
	return TRUE;
}

//***********************************************************************
// Subroutine	:			Transpose()
// Arguments	:			none
// Use        :     Transposes the grid in CZimsGrid
// Returns    :     none
//***********************************************************************
void CZimsGrid::Transpose(void)
{
	double **tgrid;
	int i, j, n, nn, i0, j0, d, itemp;
	double temp, temp2;
	unsigned char **igrid;

	igrid = new unsigned char *[columns];
	igrid[0] = new unsigned char[columns*rows];

	for(j=1;j<columns;j++)
	{
		igrid[j] = igrid[j-1]+rows;
	}

	for(j=0;j<columns;j++)
	{
		for(i=0;i<rows;i++)
		{
			igrid[j][i]=0x00;
		}
	}

	tgrid = new double *[columns];
	tgrid[0] = grid[0];

	for(j=1;j<columns;j++)
	{
		tgrid[j] = tgrid[j-1]+rows;
	}

	nn = rows*columns;
	n=0;
	i0=0;
	j0=0;

	do
	{
		i = i0 = n%rows;
		j = j0 = n/rows;
		temp = grid[i0][j0];
		do
		{
			temp2 = tgrid[j][i];
			tgrid[j][i] = temp;
			igrid[j][i] = 0x01;
			temp = temp2;
			d = (j*rows+i);
			j = d%columns;
			i = d/columns;
		} while((i!=i0) || (j!=j0));
		tgrid[j][i] = temp;
		igrid[j][i] = 0x01;
		while((n < nn) && (igrid[0][n])) n++;
	} while(n < nn);
	if(igrid)
	{
		delete(igrid[0]);
		delete(igrid);
	}
	if(grid) delete(grid);
	grid = tgrid;
	itemp = columns;
	columns = rows;
	rows = itemp;
	for(i=0;i<2;i++)
	{
		temp = ylim[i];
		ylim[i] = xlim[i];
		xlim[i] = temp;
	}
}

//***********************************************************************
// Subroutine :	    IndexGrid()
// Arguments  :     void
// Use        :     Indexes the grid, finds the average and number of
//                  znon pts and does a rough linear spline to get rid
//					of the znon values...
// Returns    :			void
//***********************************************************************
void CZimsGrid::IndexGrid(void)
{
	int j, k, n;
	int pos[4];
	int maxdim = (rows > columns) ? rows:columns;
	double x0, x1, y0, y1;
	BOOL notfound = TRUE;
	Stamp();
	// indexgrid : 0 if OK, 1 if Znon, -1 if TPSplined...
	indexgrid = imatrix(0, rows-1, 0, columns-1);
	// counting znons for progress bar...
	// fill indexgrid
	for(j=0;j<rows;j++)
		for(k=0;k<columns;k++)
			if(GetZnon(GetGrid()[j][k]))
				indexgrid[j][k] = 1;
			else
				indexgrid[j][k] = 0;
	// Now do a rough linear spline for the znon values...
	for(j=0;j<rows;j++)
	{
		for(k=0;k<columns;k++)
		{
			// This values is znon!
			if(indexgrid[j][k] == 1)
			{
				// reset all loop parameters
				n=1;
				notfound = TRUE;
				pos[0] = pos[1] = pos[2] = pos[3] = 0;
				// Do this loop until a point left/right || up/down of this znon point
				// has been found which is valid...
				do
				{
					if(!pos[0] && (j-n) >= 0 && !indexgrid[j-n][k])
						pos[0] = n;
					if(!pos[1] && (j+n) < rows && !indexgrid[j+n][k])
						pos[1] = n;
					if(!pos[2] && (k-n) >= 0 && !indexgrid[j][k-n])
						pos[2] = n;
					if(!pos[3] && (k+n) < columns && !indexgrid[j][k+n])
						pos[3] = n;
					// found a point above and below or left right...
					if((pos[0] && pos[1]) || (pos[2] && pos[3])) notfound = FALSE;
					// Could not find either...
					if(n >= maxdim) break;
					n++;
				} while(notfound);
				// no adjacent set of points were found - set the point to the average...
				if(notfound)
					GetGrid()[j][k] = avg;
				// Here adjacent points were found - therefore do a linear spline...
				else
				{
					if(pos[0] && pos[1])
					{
						y0 = GetGrid()[j-pos[0]][k];
						y1 = GetGrid()[j+pos[1]][k];
						x0 = (double) pos[0];
						x1 = (double) pos[1]+pos[0];
					}
					else
					{
						y0 = GetGrid()[j][k-pos[2]];
						y1 = GetGrid()[j][k+pos[3]];
						x0 = (double) pos[2];
						x1 = (double) pos[3]+pos[2];
					}
					GetGrid()[j][k] = y0 + (y1-y0)*x0/x1;
				}
			}
		}
	}
	CalcAverage();
}

//***********************************************************************
// Subroutine	:			RandomIndexGrid()
// Arguments  :     void
// Use        :     Indexes the grid, invents some values for those nodes
//									that prior to this used to be ZNON values. (for contouring,
//									where the indexed grid notes will later be blanked out on
//								  screen anyway)
// Returns    :			void
//***********************************************************************
void CZimsGrid::RandomIndexGrid(void)
{
	int j, k;
	// indexgrid : 0 if OK, 1 if Znon, -1 if TPSplined...
	indexgrid = imatrix(0, rows-1, 0, columns-1);
	// counting znons for progress bar...
	Stamp();
	// fill indexgrid, and set the according grid nodes to the average
	for(j=0;j<rows;j++)
	{
		for(k=0;k<columns;k++)
		{
			if(GetZnon(GetGrid()[j][k]))
			{
				indexgrid[j][k] = 1;
				GetGrid()[j][k]=avg;
			}
			else
				indexgrid[j][k] = 0;
		}
	}
	CalcAverage();
}

//***********************************************************************
// Subroutine	:			UnIndexGrid()
// Arguments  :     void
// Use        :     Puts Znons back into the grid at the indexed points
//                  and returns memory for the indexgrid...
// Returns    :			void
//***********************************************************************
void CZimsGrid::UnIndexGrid(void)
{
	if(!indexgrid)
		return;

	Stamp();

	for(int i=0;i<rows;i++)
	{
		for(int j=0;j<columns;j++)
		{
			if(indexgrid[i][j] == 1)
				GetGrid()[i][j] = GetZnon();
		}
	}

	delete indexgrid[0];
	delete indexgrid;
	indexgrid = NULL;
	CalcAverage();
}

//***********************************************************************
// Subroutine	:			CalcAverage()
// Arguments  :     none
// Use        :     Calculates the average grid value...
// Returns    :			the average grid value
//***********************************************************************
double CZimsGrid::CalcAverage(void)
{
	int area = rows*columns;
	Stamp();
	avg = 0.0;
	znonvals = 0;

	for(int i=0;i<rows;i++)
	{
		for(int j=0;j<columns;j++)
		{
			if(GetZnon(GetGrid()[i][j]))
				znonvals++;
			else
				avg += GetGrid()[i][j];
		}
	}

	if(znonvals == area)
		avg=GetZnon();
	else
		avg /= (area-znonvals);
	return avg;
}

//***********************************************************************
// Subroutine	:			CalcMaxVal()
// Arguments  :     none
// Use        :     Calculates the maximum grid value...
// Returns    :			the maximum grid value
//***********************************************************************
double CZimsGrid::CalcMaxVal(void)
{
	double max = -DBL_MAX;
	Stamp();

	for(int i=0;i<rows;i++)
	{
		for(int j=0;j<columns;j++)
		{
			if(GetZnon(GetGrid()[i][j]))
				continue;
			if(GetGrid()[i][j] > max)
				max = GetGrid()[i][j];
		}
	}

	return max;
}

//***********************************************************************
// Subroutine	:			CalcMinVal()
// Arguments  :     none
// Use        :     Calculates the minimum grid value...
// Returns    :			the minimum grid value
//***********************************************************************
double CZimsGrid::CalcMinVal(void)
{
	double min = DBL_MAX;
	Stamp();

	for(int i=0;i<rows;i++)
	{
		for(int j=0;j<columns;j++)
		{
			if(!GetZnon(GetGrid()[i][j]) && GetGrid()[i][j] < min)
				min = GetGrid()[i][j];
		}
	}

	return min;
}

// Function to find null values in grid and overwrite them with
// the input constant value.
void CZimsGrid::OverwriteNULL(double val)
{
	int	i, j;

	Stamp();
	for ( i=0; i<rows; i++ )
	{
		for ( j=0; j<columns; j++ )
		{
			if ( GetZnon(GetGrid()[i][j]) )
				GetGrid()[i][j] = val;
		}
	}

	// recompute grid statistics
	CalcMaxVal();
	CalcMinVal();
	CalcAverage();
}

//***********************************************************************
// Subroutine	:			CleanGrid ()
// Arguments  :     points = the max number of ZNONS between two valid
//                  points that we use to interpolate intermediate points
//					Interpolation is linear.
//					if points<0, we kill ALL Znons, albeit in a rather
//					strict manner, we call IndexGrid()
// Use        :     Goes through all grid and cleans it of all znon
//                  values except those that do not have valid neighbours
//					nearby.
// Returns    :		1 if successful
//                  0 if not
//***********************************************************************
BOOL CZimsGrid::CleanGrid(int points)
{
	if(points<0)
	{
		IndexGrid();
		return TRUE;
	}
	double grad;
	BOOL foundgood;
	int i, j, k, l;
	Stamp();
	// indexgrid : 0 if OK, 1 if Znon, -1 if TPSplined...
	if(indexgrid)	free_imatrix(indexgrid, 0, rows-1, 0, columns-1);
	indexgrid = imatrix(0, rows-1, 0, columns-1);
	// counting znons for progress bar...
	// fill indexgrid
	for(j=0;j<rows;j++)
	{
		for(k=0;k<columns;k++)
		{
			if(GetZnon(GetGrid()[j][k]))
				indexgrid[j][k] = 1;
			else
				indexgrid[j][k] = 0;
		}
	}

	// Now do a rough linear spline for the znon values...
	// ..first horizontally. Never interpolate across more than "points" NULL values
	for(i=0;i<rows;i++)
	{
		for(j=1;j<columns-1;j++)
		{
			// found a ZNON ?
			if(indexgrid[i][j])
			{
				// go right further until we find the next good point
				foundgood=FALSE;
				for(k=j+1;k<columns;k++)
				{
					if(!indexgrid[i][k])
					{
						foundgood=TRUE;
						break;
					}
				}
				// did we find one and was it close enough?
				if(foundgood && (k-j)<=points && !indexgrid[i][j-1])
				{
					// can do the interpolation. compute gradient
					grad=(GetGrid()[i][k]-GetGrid()[i][j-1])/(k-(j-1));
					for(l=j;l<k;l++)
					{
						GetGrid()[i][l]=GetGrid()[i][j-1]+(l-(j-1))*grad;
						indexgrid[i][l]=0; // reset NULL flag
					}
				}
				j=k; // go on searching from here
			}
		}
	}
	// now vertically
	for(j=0;j<columns;j++)
	{
		for(i=1;i<rows-1;i++)
		{
			// found a ZNON ?
			if(indexgrid[i][j])
			{
				// go down further until we find the next good point
				foundgood=FALSE;
				for(k=i+1;k<rows;k++)
				{
					if(!indexgrid[k][j])
					{
						foundgood=TRUE;
						break;
					}
				}
				// did we find one and was it close enough?
				if(foundgood && (k-i)<=points && !indexgrid[i-1][j])
				{
					// can do the interpolation. compute gradient
					grad=(GetGrid()[k][j]-GetGrid()[i-1][j])/(k-(i-1));
					for(l=i;l<k;l++)
					{
						GetGrid()[l][j]=GetGrid()[i-1][j]+(l-(i-1))*grad;
						indexgrid[l][j]=0; // reset NULL flag
					}
				}
				i=k; // go on searching from here
			}
		}
	}
	CalcMaxVal();
	CalcMinVal();
	CalcAverage();
	return TRUE;
}

//***********************************************************************
// Subroutine	:			LowPassFilter()
// Arguments  :     limit = width of filter (0=broad..100=narrow)
// Use        :     Low pass filters the grid - using the data in
//                  CHorizon as a cutoff value...
// Returns    :     void.
//***********************************************************************
void CZimsGrid::LowPassFilter(double lowpasslimit)
{
	double fc2 = (double) 0.01*((rows-1)*(rows-1)+(columns-1)*(columns-1))
		*(100.0-lowpasslimit);
	double f2, w;

	for(int i=0;i<rows;i++)
	{
		for(int j=0;j<columns;j++)
		{
			f2 = (double) i*i + j*j;
			w = 1.0 - f2/fc2;
			GetGrid()[i][j] = (w > 0)? w*GetGrid()[i][j]:0.0;
		}
	}
}

//***********************************************************************
// Subroutine	:			PatchInSomeOldValues()
// Arguments  :     original = the original grid before any filtering
// Use        :     After low pass filtering a grid and FFTing it back into
//									real space, this patches in some values of the very
//									original grid (which always still exists because we only
//									smooth copies of the original grids)
//			To avoid edge effects, we patch in the original values at the outer
//			border, then for some thick stretch we mix original and new values
//      with the weight of the new values going linearly up like a "ramp",
//      and for the most part (the inner bits of the grid) we just just the
//      new values
// Returns    :     void.
//***********************************************************************
void CZimsGrid::PatchInSomeOldValues(CZimsGrid *original)
{
	int startramp, endramp;
	int i, j, outerframe, innerframe;
	double rampsize, ramp;
	if((AfxGetApp()->GetProfileInt("Debug", "ForgetTheRamp", 0))==1)
	{
		pDoc->Log("Ignoring ramp altogether", LOG_BRIEF);
		return;
	}
	startramp=AfxGetApp()->GetProfileInt("Debug", "FFTRampStart", 50);
	endramp=AfxGetApp()->GetProfileInt("Debug", "FFTRampEnd", 75);
	outerframe=(min(rows, columns)*startramp)/1000;
	innerframe=(min(rows, columns)*endramp)/1000;
	rampsize=(double)(innerframe-outerframe);
	if(rampsize<1.0)
		innerframe=-100;	// just do not do it if the ramp is too short
	// (all "innerframe" if's further down will now fail)
	double **orgvals=original->GetGrid();
	original->Stamp();
	for(i=0;i<rows;i++)
	{
		for(j=0;j<columns;j++)
		{
			// are we in the outer frame? Then just copy original values
			if(i<outerframe || (rows-i)<=outerframe ||
				j<outerframe || (columns-j)<=outerframe)
				GetGrid()[i][j]=orgvals[i][j];
		}
	}

	// inner frame; row gives ramp height
	for(i=outerframe;i<innerframe;i++)
	{
		ramp=1.0-(innerframe-i)/rampsize;
		for(j=i;j<columns-i;j++)
		{
			GetGrid()[i][j]=ramp*orgvals[i][j]+(1.0-ramp)*GetGrid()[i][j];
			GetGrid()[rows-i-1][j]=ramp*orgvals[rows-i-1][j]+(1.0-ramp)*GetGrid()[rows-i-1][j];
		}
	}

	// inner frame; column gives ramp height
	for(j=outerframe;j<innerframe;j++)
	{
		ramp=(innerframe-j)/rampsize;
		for(i=j;i<rows-j;i++)
		{
			GetGrid()[i][j]=ramp*orgvals[i][j]+(1.0-ramp)*GetGrid()[i][j];
			GetGrid()[i][columns-j-1]=ramp*orgvals[i][columns-j-1]+(1.0-ramp)*GetGrid()[i][columns-j-1];
		}
	}
	return;
}

//***********************************************************************
// Subroutine	:			FFT2D()
// Arguments  :     direction : 1 forward
//                             -1 backward
// Use        :     Fast Fourier Transforms the grid
// Returns    :     TRUE if successful, FALSE if not...
//***********************************************************************
BOOL CZimsGrid::FFT2D(int direction)
{
	int i, j;
	double *wsave;
	int *ifac;
	double nfact = 1.0;
	Stamp();
	// Check if there are znons in the grid, stop if there are...
	if(GetZnonVals() != 0) {
		pDoc->Error("Cannot FFT grid with NULL Values", MSG_ERR);
		return(FALSE);
	}
	// Cannot have more than 16 prime factors (impossible anyway...)
	ifac = ivector(0, 15);
	// allocate wsave vector for calculation...
	wsave = dvector(0, 2*columns+14);

	// If you apply normalisation after the forward transform (see code below),
	// in order to better relate wave amplitudes to features seen in the original
	// grid then you must undo that normalisation before the reverse FFT. This code
	// now commented out as it's better to do it outside of this function in the
	// user code. Note, did have some issues with this on some grids, most likely
	// because PatchInSomeOldValues was doing something ad-hoc though I never got
	// to the bottom of it. gc.
	/*if ( direction < 0 )
	{
	nfact = columns*rows;
	for(i=0;i<rows;i++)
	for(j=0;j<columns;j++)
	GetGrid()[i][j] *= nfact;
	for(i=1;i<rows-1;i++)
	for(j=1;j<columns-1; j++)
	GetGrid()[i][j] *= 2.0;
	}*/

	// initialize FFT sequence...
	drffti(columns, wsave, ifac);
	// Perform 1DFFT's on every grid row...
	for(i=0;i<rows;i++)
	{
		if(direction >= 0)
			drfftf(columns, GetGrid()[i], wsave, ifac);
		else
			drfftb(columns, GetGrid()[i], wsave, ifac);
	}
	free_dvector(wsave, 0, 2*columns+14);

	// Transpose the matrix...
	Transpose();		// nb. transpose does the swap of no. of variables rows and columns
	wsave = dvector(0, 2*columns+14);
	drffti(columns, wsave, ifac);
	// Again perform FFT transform now on columns
	for(i=0;i<rows;i++)
	{
		if(direction >= 0)
			drfftf(columns, GetGrid()[i], wsave, ifac);
		else
			drfftb(columns, GetGrid()[i], wsave, ifac);
	}
	// Transpose back again...
	Transpose();

	// Normalise.
	// All elements are normalized by 1/N.M where N is the
	// number of array elements in x and M is the number of
	// array elements in y. Must do this for reverse transform
	// to ensure we get back to original grid.
	if ( direction < 0 )
	{
		nfact = 1.0/(columns*rows);
		for(i=0;i<rows;i++)
			for(j=0;j<columns;j++)
				GetGrid()[i][j] *= nfact;
	}

	// normalize wave components. Useful for relating
	// wave component amplitudes to features seen in original
	// data. Commented out as this is not the appropriate place.
	// Better to do it after this function to keep this as original
	// code. However, left here for documentation purposes. See
	// supporting documetnation about normalisation for transforms.

	/*if ( direction > 0 )
	{
	for (i = 1; i < rows-1; i++ )
	for (j=1; j < columns-1; j++ )
	GetGrid()[i][j] *= 2.0;
	}*/

	// free temporary memory again...
	free_ivector(ifac, 0, 15);
	free_dvector(wsave, 0, 2*columns+14);
	return(TRUE);
}

// this function computes the wave amplitudes from the input FFT grid.
// note that you MUST run a forward FFT on the grid before using this function.
// If you don't you'll get complete rubbish.
// A wave amplitude in 1D is computed by:
//    Ai = 0.5 * sqrt( wr*wr + wi*wi )
//  where wr is the real component of the wave i and wi is the imaginary
//  component of the wave.  For a 2D transform, we need to transpose
//  as in the FFT.
//  Wave amplitudes are computed in place, therefore grid data on the incoming
//  grid is destroyed.
//  A word on how the FFT components are stored. data(0)(0) has the constant
//	term and is untouched by this routine. The FFT is done column based, that is,
//  in the x direction of the grid. One slightly confusing issue is that velman
//  stores the x direction as increases with respect to the increasing column index.
//  However, the y direction is stored decreasing with respect to the increasing row index.
//  Also note that cosine term is stored first, followed by the cosine term, for each wave.
//  Therefore after the wave amplitudes have been computed, only 1/4 of the data grid, from
//  i = 0 to rows/2, j = 0 to cols/2 contains any useful data.
//  Also note that no normalisation is done in this routine. The wave components from the forward
//	transform are not normalized until after the reverse transform is done. If you want to
//  relate the amplitudes to features in the original grid you probably want to do some
//  normalisation. See comments in FFT2D and supporting velman docs.
void CZimsGrid::FFT2Amp(void)
{
	double **data = GetGrid();
	int i, j, k;

	// compute x direction wave amplitudes
	for ( i = 0; i<rows; i++ )
	{
		for ( j = 1; j<columns/2; j++ )		// n.b. we are working with cos/sin coeff pairs
		{
			k = 2*(j-1);
			data[i][j] = 0.5 * sqrt( data[i][k+1]*data[i][k+1] + data[i][k+2]*data[i][k+2] );
		}
	}
	Transpose();
	data = GetGrid();
	// compute y direction wave amplitudes
	for ( i = 0; i<rows/2; i++ )	//	 only half the number of rows because we've reduced data above
	{
		for ( j = 1; j<columns/2; j++ )
		{
			k = 2*(j-1);
			data[i][j] = 0.5 * sqrt( data[i][k+1]*data[i][k+1] + data[i][k+2]*data[i][k+2] );
		}
	}
	Transpose();

	// zero out rest of grid
	for ( i = rows/2; i<rows; i++ )
		for (j = columns/2; j<columns; j++ )
			GetGrid()[i][j] = 0.0;

	// statistics
	CalcAverage();
	CalcMinVal();
	CalcMaxVal();
}

//***********************************************************************
// Subroutine	:	Resample()
// Arguments	:	n:   number of points to resample
//                  stepsize: steps between points resampled with the tps
//                            and points resapled by 2D cubic spline
//					f:   array[1..n] contains functional values to be
//                       resampled
//                  p:   positions p[1..n][1..2] contains x and y pos
//                       of the functional values
//					normgrad: if TRUE, normalize gradients of grid by adding
//					          fake points outside the grid which are set to the
//							  average as computed by ReasonableAverage() in DISTMAT.CPP
//					normgradfac: if normgrad=TRUE, the fake poitns are added
//					             at a distance of normgradfac*MINSIZE away from each grid
//								 ide. MINSIZE=the smaller of the two grid extends.
// Use        :     Resamples the irregular values into a regular grid
//                  storing it in the grid data-base
// Returns    :     none
//***********************************************************************
BOOL CZimsGrid::Resample(int n, int stepsize, double *f, double **np, BOOL normgrad, double normgradfac)
{
	int i = 0, j = 0, k = 0, l = 0;
	int offset = 0, tpsrows = 0, tpscolumns = 0;
	double *x1a, *x2a, **y2a, **fg, **fs, *inputval;
	Point *zz = NULL;
	double _avg = 0; // the "average" value that we patch in outside the grid
	double dist = 0; // distance at which to patch in the extra point
	double xwidth=(xlim[1]-xlim[0]), ywidth=(ylim[1]-ylim[0]);
	double xdist = 0, ydist = 0, tpsdx = 0, tpsdy = 0;
	BOOL TooClose = FALSE;
	int splinepts = 0;
	int	fasttps = AfxGetApp()->GetProfileInt("ArithmeticParams", "FastTPS", 0);

	// Time stamp the grid
	Stamp();

	TRACE("Resample Entered, normgradfac = %lf\n", normgradfac);
	// if 0 input points, fill grid with ZNONs and return FALSE
	if(n<1)
	{
		(*this) = GetZnon();
		return FALSE;
	}

	// Get the number of points to be splined across from the .ini file
	if(stepsize < 0)
	{  // less than zero is always called from RMS processing
		stepsize = -stepsize;
		splinepts =AfxGetApp()->GetProfileInt("ArithmeticParams", "RMSResampleSplinePts", 8);
	}
	else
	{ // larger than zero stepsize is from the well tying
		splinepts =AfxGetApp()->GetProfileInt("ArithmeticParams", "WellResampleSplinePts", 8);
	}

	// Check that the number of splinepts is valid - 0 has a special meaning...
	if(splinepts < 5 && splinepts != 0)
	{
		pDoc->Log("Not enough splinepoints for Resample, increased to 5", LOG_BRIEF);
		splinepts = 5;
	}

	// Define gridsize for the TPS
	// if the grids are too small decrease stepsize until
	// its small enough
	stepsize += 1;
	do
	{
		stepsize--;
		offset = (rows-1)%stepsize? 2:1;
		tpsrows = (rows-1)/stepsize+offset;
		offset = (columns-1)%stepsize? 2:1;
		tpscolumns = (columns-1)/stepsize+offset;
	} while(stepsize != 1 && (tpsrows < splinepts ||
		tpscolumns < splinepts));

	// Find the distance between the TPS points
	tpsdx = dx*stepsize;
	tpsdy = dy*stepsize;

	if(!c)
	{
		// find the tps coefficients...
		// do we later put in some extra points?
		if(normgrad)
		{
			p = dmatrix(1, n+8, 1, 2);
			inputval=dvector(1, n+8);
		}
		else
		{
			p = dmatrix(1, n, 1, 2);
			inputval=dvector(1, n);
		}

		// Check that there are no illegal points present
		for(i=1, j=1;i<=n;i++)
		{
			if(f[i] != CSD_DONTKNOW && f[i] != CSD_ILLEGAL && !GetZnon(f[i]))
			{
				p[j][1] = np[i][1];
				p[j][2] = np[i][2];
				inputval[j] = f[i];
				j++;
			}
		}
		// correct the number of points
		n=j-1;

		// Check that any point is not closer than a certain distance to any other point
		for(i=1, j=1;i<=n;i++)
		{
			// check all other points whether we are getting uncomfortably close
			TooClose=FALSE;
			for(k=i+1;k<=n;k++)
			{
				xdist=p[i][1]-p[k][1];
				ydist=p[i][2]-p[k][2];
				if(xdist*xdist+ydist*ydist<=FLT_MIN)
				{
					TooClose=TRUE;
					TRACE("\nthrowing out at (%f,%f)", p[i][1], p[i][2]);
					break;
				}
			}

			// if too close - then throw it out...
			if(!TooClose)
			{
				p[j][1] = p[i][1];
				p[j][2] = p[i][2];
				inputval[j]=inputval[i];
				j++;  // j counts points actually used
			}
		}
		// correct the number of points
		n=j-1;		

		// get min / max values to plot our fake points around
		// start by making dimensions the dimensions of the grid
		double MaxX = xlim[1]; 
		double MinX = xlim[0];
		double MaxY = ylim[1];
		double MinY = ylim[0];

		// now see if points are greater/less than that
		for(i=1;i<=n;i++)
		{
			MaxX = max(p[i][1], MaxX);
			MinX = min(p[i][1], MinX);

			MaxY = max(p[i][2], MaxY);
			MinY = min(p[i][2], MinY);
		}

		// now fake some extra data points
		// for the gradient normalization
		// this prevents extrapolation errors
		if(normgrad)
		{
			_avg=FindAverageValue(n, &inputval[1]); // FindAvg() thinks arrays start at [0]

			// make sure normgradfact is between 100.0 and 0.0
			if(normgradfac > 100.0) 
				normgradfac = 100.0;
			else if(normgradfac < 0.0)
				normgradfac = 0.0;

			double distFactor = 1.0-normgradfac/100.0;
			// find corresponding distance
			dist=min(xwidth, ywidth) * distFactor;

			// set them to the average value
			for(i=n+1;i<=n+8;i++)
				inputval[i]=_avg;

			// now fake their positions
			for(i=1;i<=3;i++)
				p[n+i][1]=MinX-0.99*dist;	// pts 1..3 to left of grid
			for(i=4;i<=5;i++)
				p[n+i][1]=MinX+xwidth/2.0;	 // pts 4..5 in middle
			for(i=6;i<=8;i++)
				p[n+i][1]=MaxX+0.99*dist;	 // pts 6..8 to right of grid

			p[n+1][2]=MaxY+0.99*dist;
			p[n+2][2]=MinY+ywidth/2.0;
			p[n+3][2]=MinY-0.99*dist;

			p[n+4][2]=MaxY+0.99*dist;
			p[n+5][2]=MinY-0.99*dist;

			p[n+6][2]=MaxY+0.99*dist;
			p[n+7][2]=MinY+ywidth/2.0;
			p[n+8][2]=MinY-0.99*dist;

			n+=8;		// finally use this revised number of points
		}
		else
		{
			// This is to make sure all coords are always < 1.0
			dist = min(xwidth, ywidth)*0.01;
		}
		// renormalize all coordinates to between 0.0 to 1.0
		for(i=1;i<=n;i++)
		{
			p[i][1] = (p[i][1]-xlim[0]+dist)/(xwidth+2*dist);
			p[i][2] = (p[i][2]-ylim[0]+dist)/(ywidth+2*dist);
		}

		c = interpolate_tps(p, n, inputval, fasttps);
	}

	// This is the point vector of the points to be evaluated at...
	zz = pvector(1, tpsrows*tpscolumns);

	// put the coords into the pointvector
	k=1;
	for(i=0;i<tpsrows;i++)
	{
		for(j=0;j<tpscolumns;j++)
		{
			if(j == tpscolumns-1) // is this the last column?
				zz[k].x = (dist+xwidth)/(xwidth+2*dist); // yes - x coord of last column
			else
				zz[k].x = (dist+tpsdx*j)/(xwidth+2*dist); // no - current x coord

			if(i == tpsrows-1) // is this the last row?
				zz[k].y = dist/(ywidth+2*dist); // yes - ycoord of last row
			else
				zz[k].y = (ywidth+dist-tpsdy*i)/(ywidth+2*dist); // no - current y coord

			// increment point value
			k++;
		}
	}

	// now the values are evaluated at a smaller grid by TPS
	fg = GEvaluateGridTPS(p, c, n, zz, tpsrows, tpscolumns, fasttps);

	// give back memory of the TPS;
	ClearResample();
	free_dvector(inputval, 1, 0);
	free_pvector(zz, 1, tpsrows*tpscolumns);

	// Set entire grid to ZNON first
	(*this) = GetZnon();

	// Copy double precision data into the double grid of the horizon
	// and into another double array for the 2D spline...
	for(i=0;i<tpsrows;i++)
	{
		if(i == tpsrows-1)
			k = rows-1;
		else
			k = i*stepsize;

		for(j=0;j<tpscolumns;j++)
		{
			if(j == tpscolumns-1)
				l = columns-1;
			else
				l = j*stepsize;

			GetGrid()[k][l] = fg[i+1][j+1];
		}
	}
	// make sure that the area splined together is not too large as compared to the grid size
	if(3*(splinepts-1)*stepsize > rows || 3*(splinepts-1)*stepsize > columns) splinepts = 0;

	// if stepsize == 1 then we need not do any more splining
	if(stepsize != 1)
	{
		if(splinepts != 0)
		{
			// allocate space for x and y coordinates...
			x2a = dvector(1, splinepts+1);      // this is the different x coords
			x1a = dvector(1, splinepts+1);      // and the y coords in the gridsection
			// allocate space for functional values...
			fs = dmatrix(1, splinepts+1, 1, splinepts+1);
			// allocate space for the second derivatives...
			y2a = dmatrix(1, splinepts+1, 1, splinepts+1);

			// move gridsection of stepsize*stepsize over complete array...
			for(i=0;i<tpsrows-1;i+=splinepts)
			{
				for(j=0;j<tpscolumns-1;j+=splinepts)
				{
					// Check if spline has gone beyond defined array
					if(i+splinepts+1 >= tpsrows)
						i=tpsrows-splinepts-1;
					if(j+splinepts+1 >= tpscolumns)
						j=tpscolumns-splinepts-1;

					// copy functional values into submatrix for 2D spline
					// and generate vectors of x and y coordinates for grid section...
					for(k=1;k<=splinepts+1;k++)
					{
						if(i+k == tpsrows)
							x1a[k] = (double) rows-1;
						else
							x1a[k] = (double) stepsize*(i+k-1);
						for(l=1;l<=splinepts+1;l++)
						{
							if(j+l == tpscolumns)
								x2a[l] = (double) columns-1;
							else
								x2a[l] = (double) stepsize*(j+l-1);

							fs[k][l] = fg[i+k][j+l];
						}
					}

					// find second derivatives matrix...
					splie2(x2a, fs, splinepts+1, splinepts+1, y2a);

					// Find the other points by bicubic spline...
					for(k=0;k<=stepsize*splinepts && i*stepsize+k < rows;k++)
					{
						for(l=0;l<=stepsize*splinepts && j*stepsize+l < columns;l++)
						{
							if(GetZnon(GetGrid()[i*stepsize+k][j*stepsize+l]))
							{
								splin2(x1a, x2a, fs, y2a, splinepts+1, splinepts+1, i*stepsize+k,
									j*stepsize+l, &GetGrid()[i*stepsize+k][j*stepsize+l]);
							}
						}
					}

				}
			}
			// return the memory needed for the bicubic spline...
			free_dmatrix(fs, 1, splinepts+1, 1, splinepts+1);
			free_dmatrix(y2a, 1, splinepts+1, 1, splinepts+1);
			free_dvector(x2a, 1, splinepts+1);
			free_dvector(x1a, 1, splinepts+1);
		}
		else
		{ // spline using the whole grid.
			y2a = dmatrix(1, tpsrows, 1, tpscolumns);
			x1a = dvector(1, tpsrows);
			x2a = dvector(1, tpscolumns);

			// fill the y coordinate first (down the rows)
			for(i=0;i<tpsrows;i++)
			{
				if(i == tpsrows-1)
					x1a[i+1] = (double) rows-1;
				else
					x1a[i+1] = (double) i*stepsize;
			}

			// then the x coordinate (across the columns)
			for(j=0;j<tpscolumns;j++)
			{
				if(j == tpscolumns-1)
					x2a[j+1] = (double) columns-1;
				else
					x2a[j+1] = (double) j*stepsize;
			}

			// find 2nd derivative matrix...
			splie2(x2a, fg, tpsrows, tpscolumns, y2a);

			// Find the values at all points where none have been determined by TPS
			for(i=0;i<rows;i++)
			{
				for(j=0;j<columns;j++)
				{
					if(GetZnon(GetGrid()[i][j]))
						splin2(x1a, x2a, fg, y2a, tpsrows, tpscolumns, i, j, &GetGrid()[i][j]);
				}
			}

			// Free the memory for the 2D spline
			free_dmatrix(y2a, 1, tpsrows, 1, tpscolumns);
			free_dvector(x1a, 1, tpsrows);
			free_dvector(x2a, 1, tpscolumns);
		}
	}

	// Free the matrix with the direct TPS points
	free_dmatrix(fg, 1, tpsrows, 1, tpscolumns);
	CalcAverage();

	return(TRUE);
}

//***********************************************************************
// Subroutine	:		  SetHorizon()
// Arguments  :     void
// Use        :     sets a new horizon for grid
// Returns    :     void
//***********************************************************************
void CZimsGrid::SetHorizon(int nhor)
{
	// set new horizon
	horizon = nhor;

	// Now check if this grid has already been added under the
	// old horizon
	if(gridid != -1)
	{
		// if yes then find a new id for it
		SetID(FindNewID());
	}
}

//***********************************************************************
// Subroutine	:		  SetID()
// Arguments  :     void
// Use        :     sets a new gridid
// Returns    :     void
//***********************************************************************
void CZimsGrid::SetID(int newid)
{
	// set new id
	gridid = newid;
}

//***********************************************************************
// Subroutine	:		  SetType()
// Arguments  :     void
// Use        :     sets a new gridtype
// Returns    :     void
//***********************************************************************
void CZimsGrid::SetType(UBYTE ntype)
{
	// set new type
	type = ntype;

	// Check if grid had already been added with another type
	if(gridid != -1)
		SetID(FindNewID());
}

//***********************************************************************
// Subroutine	:		  SetType()
// Arguments  :     void
// Use        :     sets the gridtype according to the grid unit
// Returns    :     void
//***********************************************************************
void CZimsGrid::SetType(void)
{
	SetType(CSD_TYPE_OTHER);
	if(unit_timepower == 1 && unit_distpower == 0) SetType(CSD_TYPE_TIME);
	if(unit_timepower == -1 && unit_distpower == 1) SetType(CSD_TYPE_INTVEL);
	if(unit_timepower == 0 && unit_distpower == 1) SetType(CSD_TYPE_DEPTH);
}

//***********************************************************************
// Subroutine	:		  SetUnit()
// Arguments  :     void
// Use        :     sets the gridunit according to the grid type
// Returns    :     void
//***********************************************************************
void CZimsGrid::SetUnit(void)
{
	switch(GetType())
	{
	case CSD_TYPE_TIME :
		unit_timepower = 1;
		unit_distpower = 0;
		break;
	case CSD_TYPE_RMSVEL :
	case CSD_TYPE_INTVEL :
	case CSD_TYPE_AVGVEL :
		unit_timepower = -1;
		unit_distpower = 1;
		break;
	case CSD_TYPE_DEPTH :
		unit_timepower = 0;
		unit_distpower = 1;
		break;
	case CSD_TYPE_RESIDUAL :
		unit_timepower = 0;
		unit_distpower = 1;
		break;
	}
}

//***********************************************************************
// Subroutine	:			GetZnon()
// Arguments  :     val = value to be checked for znon
// Use        :     checks if the value val is within 1% of znon
// Returns    :     TRUE if test was positive
//                  FALSE if not
//***********************************************************************
BOOL CZimsGrid::GetZnon(double val)
{
	if(znon > 0)
	{
		if(val > 0.995*znon && val < 1.005*znon)
			return(TRUE);
	}
	else
	{
		if(val < 0.995*znon && val > 1.005*znon)
			return(TRUE);
	}
	return(FALSE);
}

//***********************************************************************
// Subroutine	:			Link()
// Arguments  :     nftlgrid, faultgrid to attach to zimsgrid
// Use        :     associated a faultgrid with a zimsgrid
// Returns    :     void
//***********************************************************************
void CZimsGrid::Link(CFaultGrid *nfltgrid)
{
	int i = 0;

	if(nfltgrid != NULL)
	{
		for ( i=0;i<pDoc->faultcube.GetSize();i++)
		{
			if(nfltgrid == pDoc->faultcube[i])
				break;
		}
		faultid = i;
		faultgrid = nfltgrid;
		nfltgrid->Attach(this);
	}
	else
	{
		faultid = -1;
		faultgrid = NULL;
	}
}

//***********************************************************************
// Arguments  :     normgrad, normgradfac: see Resample()
//									(they just get passed on)
//					changename: if TRUE (this is the default), "_T" appended
//								to grid name
// Use        :     ties the grid to the well data of appropriate type
// Returns    :     TRUE if successful
//                  FALSE if not
//***********************************************************************
BOOL CZimsGrid::TieGrid(BOOL normgrad, double normgradfac, BOOL changename)
{
	CHorizon *chorizon;
	CMeasurePt *cmpt;
	CZimsGrid *corrgrid = new CZimsGrid(pDoc);
	CString msg, orig_name;
	int j = 0, n = 0, stepsize = 0;
	double x = 0, y = 0, val = 0, vreal = 0, *f;
	double writeback = 0;

	// read .ini file entry
	stepsize=AfxGetApp()->GetProfileInt("ArithmeticParams", "WellResampleStep", 2);

	if(type != CSD_TYPE_TIME && type != CSD_TYPE_INTVEL && type != CSD_TYPE_DEPTH)
	{
		pDoc->Error("There is no appropriate data in the well database\n"
			"to tie grid "+GetName(), MSG_ERR);
		return FALSE;
	}
	if(horizon >= pDoc->horizonlist.GetSize())
	{
		pDoc->Error("No well data avaliable to apply to grid " +
			GetName(), MSG_ERR);
		return FALSE;
	}
	if(horizon == -1)
	{
		pDoc->Error("This grid is not assigned to any horizon, so it is unclear what well"
			" data to use for tying.\nUse the 'Designate' grid tool to assign this grid to a particular"
			" horizon.", MSG_ERR);
		return FALSE;
	}

	pDoc->Log("Tying grid '"+GetName()+"' to well data.", LOG_STD);
	chorizon = pDoc->horizonlist[horizon];
	if(chorizon->GetSize()==0)
	{
		pDoc->Error("No points found to tie "+GetName(), MSG_WARN);
		return FALSE;
	}

	if(p)
	{
		free_dmatrix(p, 1, 0, 1, 0);
		p=NULL;
	}

	// do the actual tying a few times
	int numtie=AfxGetApp()->GetProfileInt("ArithmeticParams", "IterativeTies", 1);
	for(int counter=0; counter<numtie; counter++)
	{
		p = dmatrix(1, chorizon->GetSize(), 1, 2);
		f = dvector(1, chorizon->GetSize());
		*corrgrid << *this;
		n=0;
		for(j=0;j<chorizon->GetSize();j++)
		{
			if(!(cmpt = (*chorizon)[j])) 
				continue;

			// don't tie to invalid data
			if(cmpt->ReadStatus() & MPT_STATUS_INVALID)
				continue;

			// don't tie to values that are to be re-calculated
			/*if(cmpt->ReadStatus() & MPT_STATUS_CALC_TIME)
				continue;*/

			x = cmpt->ReadX();
			y = cmpt->ReadY();

			// don't tie when invalid coordinates
			if(x == MPT_INVALID || y == MPT_INVALID)
				continue;

			switch(type)
			{
			case CSD_TYPE_TIME:
				vreal = cmpt->ReadTime();
				break;
			case CSD_TYPE_INTVEL:
				vreal = cmpt->ReadvInt();
				break;
			case CSD_TYPE_DEPTH:
				vreal = cmpt->ReadDepth();
				break;
			}
			if(vreal == MPT_INVALID || vreal==MPT_ILLEGAL)
			{
				writeback = MPT_INVALID;
			}
			else
			{
				val = Evaluate(x, y);
				writeback = GetZnon(val) ? MPT_INVALID : (vreal-val);
				if(!GetZnon(val))
				{
					p[n+1][1] = x;
					p[n+1][2] = y;
					f[n+1] = vreal - val;
					n++;
				}
			}
			// store difference between orig. grid and well value in well database
			switch(type)
			{
			case CSD_TYPE_TIME:
				if(counter==0)
					cmpt->WriteTieTdiff(writeback);
				else if(cmpt->ReadTieTdiff() != MPT_INVALID)
					cmpt->WriteTieTdiff(cmpt->ReadTieTdiff()+writeback);
				break;
			case CSD_TYPE_INTVEL:
				if(counter==0)
					cmpt->WriteTieVdiff(writeback);
				else if(cmpt->ReadTieTdiff() != MPT_INVALID)
					cmpt->WriteTieVdiff(cmpt->ReadTieVdiff()+writeback);
				break;
			case CSD_TYPE_DEPTH:
				if(counter==0)
					cmpt->WriteTieDdiff(writeback);
				else if(cmpt->ReadTieTdiff() != MPT_INVALID)
					cmpt->WriteTieDdiff(cmpt->ReadTieDdiff()+writeback);
				break;
			}
		}

		// if no points to tie to at all, output error and give up
		if(n<1)
		{
			pDoc->Error("No valid well data point found to tie grid '"+
				GetName()+"'. Tying is skipped.", MSG_WARN);
			free_dmatrix(p, 1, chorizon->GetSize(), 1, 2);
			p=NULL;
			free_dvector(f, 1, chorizon->GetSize());
			f=NULL;
			return FALSE;
		}

		// if three wells or less to tie to, switch on normalisation at >=80%
		if(n<4)
		{
			if(!normgrad)
			{
				msg="Less than four wells found for tying grid '"+GetName()+"'.\n"
					"The tying options you selected have been overriden:";
				if(!normgrad)
				{
					normgrad=TRUE;
					msg+="\n  - switched on gradient normalisation";
				}
				pDoc->Error(msg, MSG_WARN);
			}
		}

		corrgrid->Resample(n, stepsize, f, p, normgrad, normgradfac);
		corrgrid->ClearResample();
		*this += *corrgrid;
		free_dmatrix(p, 1, chorizon->GetSize(), 1, 2);
		p=NULL;
		free_dvector(f, 1, chorizon->GetSize());
		f=NULL;
	}

	if(changename)
	{
		orig_name = name;
		name = name + CString("_T");
	}

	// If the timeslices are to be kept for INTVEL_GRIDS
	// Then Add them to the cube
	if(AfxGetApp()->GetProfileInt("ArithmeticParams", "KeepCorrGrid", 1) &&
		GetType() == CSD_TYPE_TIME)
	{
		corrgrid->CopyZnon(*this);
		pDoc->zimscube.Add(corrgrid, orig_name + CString("_C"), (int) *(this));
	}
	else if(GetType() == CSD_TYPE_INTVEL && pDoc->zimscube.keeptieslices
		== TRUE)
	{
		corrgrid->CopyZnon(*this);
		pDoc->zimscube.Add(corrgrid, orig_name + CString("_C"), (int) *(this));
	}
	else
	{
		delete corrgrid;
	}

	return TRUE;
}

//***********************************************************************
// Function:    ValidPoint()
// Return:      TRUE if point is within grid else FALSE
// Args:        ni, nj  : point to be looked at
// Use:         Checks if a point is neither out of bounds or znon
//***********************************************************************
BOOL CZimsGrid::ValidPoint(int ni, int nj)
{
	if(ni < 0 || ni >= GetRows()) return FALSE;
	if(nj < 0 || nj >= GetColumns()) return FALSE;
	return TRUE;
}

//***********************************************************************
// Function:    PlacePoint()
// Return:      TRUE if point was accepted, else FALSE
//              if TRUE, the coords in vector[0] and vector[1]
// Args:        ni, nj  :  upper left corner of square in which the point
//                        is to be found
//              mi, mj  :  coordinates of evaluation point to be tested
// Use:         accepts or rejects an evaluationpoint depending on
//               whether the point is accross a faultline or not...
//***********************************************************************
BOOL CZimsGrid::PlacePoint(double x, double y, int mi, int mj, int *vector)
{
	BOOL cross = FALSE;
	CFaultLine *currline, *faultline;
	int i, ni, nj;
	// Get left upper corner
	nj = (int) ((x-xlim[0])/dx);
	ni = (int) ((ylim[1]-y)/dy);
	if(HasFaultGrid())
	{ // if there is a faultgrid we also have to check intersections
		// make a line out of the two points griven
		currline = new CFaultLine(x, y, X(mj), Y(mi));
		// Check all relevant lines in the faultgrid
		for(i=0;i<faultgrid->GetSize();i++)
		{
			faultline = (*faultgrid)[i];
			if(currline->GetY1() < faultline->GetY0()) continue; // They cannot possibly cross
			if(currline->GetY0() > faultline->GetY1()) continue; // as the y's don't intersect
			if(*currline == *faultline)
			{ // check if the two lines cross
				cross = TRUE;
				break;
			}
		}
		delete currline;
	}

	if(!cross)
	{
		vector[0] = mi;
		vector[1] = mj;
		return TRUE;
	}
	return FALSE;
}

//***********************************************************************
// Function:    GetInterpolationPoints()
// Return:      pointer to coordinates of points in the form
//              x0, y0, x1, y1...xn-1, yn-1
//              points = actual number of points found
// Args:        points :  number of points to be used in interpolation
//              ni, nj  :  upper left corner of square in which the point
//                        is to be found
// Use:         finds the correct number of points in the grid closest
//              to the interpolation point
//***********************************************************************
int *CZimsGrid::GetInterpolationPoints(int &points, double x, double y)
{
	int n = 0, dn = 0, i = 0, j = 0, di = 0, dj = 0, i2 = 0, j2 = 0;
	int *pvector = NULL;
	int pointsfound = 0;
	BOOL evalok, goodpoint;
	static int maxznons = -1;
	static int iwidth = -1;
	int **igrid = NULL;
	char buffer[128];
	int ni = 0, nj = 0, zi = 0, zj = 0;
	Stamp();

	// Get left upper corner
	nj = (int) ((x-xlim[0])/dx);
	ni = (int) ((ylim[1]-y)/dy);

	// Get maximum allowed znons from .INI file
	if(maxznons == -1)
		maxznons = AfxGetApp()->GetProfileInt("ArithmeticParams", "MaxZnons", 9);

	// Get the width of the index grid used and make sure its odd...
	if(iwidth == -1)
	{
		iwidth = AfxGetApp()->GetProfileInt("ArithmeticParams", "IndexWidth", 7);
		if(iwidth%2 == 0) iwidth++;
	}

	// allocate indexgrid and fill with znons
	// and znon shadows
	igrid = imatrix(0, iwidth-1, 0, iwidth-1);
	for(i=0;i<iwidth;i++)
		for(j=0;j<iwidth;j++)
			igrid[i][j] = 0;
	// valid pts are given 0, znons 1, shadows 2 and out of range 3
	for(i=0;i<iwidth;i++)
	{
		for(j=0;j<iwidth;j++)
		{
			zi = ni - iwidth/2 + i;
			zj = nj - iwidth/2 + j;

			if (!ValidPoint(zi, zj))
			{
				igrid[i][j] = 3;
			}
			else if(GetZnon(GetGrid()[zi][zj]))
			{
				if(i < iwidth/2) di = -1;
				else if(i == iwidth/2) di = 0;
				else if(i > iwidth/2) di = 1;
				if(j < iwidth/2) dj = -1;
				else if(j == iwidth/2) dj = 0;
				else if(j > iwidth/2) dj = 1;
				for(i2=i;i2>=0 && i2<iwidth;i2+=di)
				{
					for(j2=j;j2>=0 && j2<iwidth;j2+=dj)
					{
						igrid[i2][j2] = 2;
						if(dj == 0) break;
					}
					if(di == 0) break;
				}
				igrid[i][j] = 1;
			}
		}
	}
	// allocate memory for point vector
	pvector = ivector(0, 2*points-1);
	// Now search in concentric squares around the point
	// to be evaluated in a star shaped pattern
	pointsfound = 0;
	goodpoint = FALSE;
	evalok = TRUE;

	if(igrid[iwidth/2][iwidth/2] == 0)
	{
		pvector[pointsfound*2] = ni;
		pvector[pointsfound*2+1] = nj;
		pointsfound++;
		goodpoint = TRUE;
	}
	else if (maxznons <= 1)
		evalok = FALSE;

	for(n=1;n<=iwidth/2;n++)
	{
		for(dn=0;dn<=n;dn++)
		{
			if(pointsfound < points && dn != n && igrid[iwidth/2+dn][iwidth/2+n] == 0 &&
				PlacePoint(x, y, ni+dn, nj+n, pvector+2*pointsfound))
			{
				pointsfound++;
				goodpoint = TRUE;
			}
			if(pointsfound < points && dn != 0 && igrid[iwidth/2+dn][iwidth/2-n] == 0 &&
				PlacePoint(x, y, ni+dn, nj-n, pvector+2*pointsfound))
			{
				pointsfound++;
				goodpoint = TRUE;
			}
			if(pointsfound < points && dn != 0 && igrid[iwidth/2-dn][iwidth/2+n] == 0 &&
				PlacePoint(x, y, ni-dn, nj+n, pvector+2*pointsfound))
			{
				pointsfound++;
				goodpoint = TRUE;
			}
			if(pointsfound < points && dn != n && igrid[iwidth/2-dn][iwidth/2-n] == 0 &&
				PlacePoint(x, y, ni-dn, nj-n, pvector+2*pointsfound))
			{
				pointsfound++;
				goodpoint = TRUE;
			}
			if(pointsfound < points && dn != 0 && igrid[iwidth/2+n][iwidth/2+dn] == 0 &&
				PlacePoint(x, y, ni+n, nj+dn, pvector+2*pointsfound))
			{
				pointsfound++;
				goodpoint = TRUE;
			}
			if(pointsfound < points && dn != n && igrid[iwidth/2+n][iwidth/2-dn] == 0 &&
				PlacePoint(x, y, ni+n, nj-dn, pvector+2*pointsfound))
			{
				pointsfound++;
				goodpoint = TRUE;
			}
			if(pointsfound < points && dn != n && igrid[iwidth/2-n][iwidth/2+dn] == 0 &&
				PlacePoint(x, y, ni-n, nj+dn, pvector+2*pointsfound))
			{
				pointsfound++;
				goodpoint = TRUE;
			}
			if(pointsfound < points && dn != 0 && igrid[iwidth/2-n][iwidth/2-dn] == 0 &&
				PlacePoint(x, y, ni-n, nj-dn, pvector+2*pointsfound))
			{
				pointsfound++;
				goodpoint = TRUE;
			}
		}
		if(goodpoint == FALSE && (2*n+1)*(2*n+1) >= maxznons)
		{
			evalok = FALSE;
			break;
		}
	}

	// Make sure there are enough valid points in the grid, if not, log it.
	if(pointsfound < points)
	{
		/*if(AfxGetApp()->GetProfileInt("Debug", "ForMyEyesOnly", 0))
		{
			sprintf(buffer, "Could only find %d instead of %d valid points to evaluate at (%d,%d)", pointsfound, points, ni, nj);
			pDoc->Log(buffer, LOG_DETAIL);
		}*/
		points = pointsfound;
	}

	// return memory and points
	if(igrid) 
		free_imatrix(igrid, 0, iwidth-1, 0, iwidth-1);

	// need at least three points to evaluate
	if(points < 3)
	{
		free_ivector(pvector, 0, 1);
		return(NULL);
	}

	// return vector of points...
	if(evalok)
	{
		i=INT_MAX; i2=INT_MIN;
		j=INT_MAX; j2=INT_MIN;
		// Check if all points found lie along a line
		for(n=0;n<points;n++)
		{
			if(pvector[n*2  ] < i)  i  = pvector[n*2];
			if(pvector[n*2  ] > i2) i2 = pvector[n*2];
			if(pvector[n*2+1] < j)  j  = pvector[n*2+1];
			if(pvector[n*2+1] > j2) j2 = pvector[n*2+1];
		}
		// test if lower and upper limits are equal
		if(i==i2 || j==j2)
		{
			free_ivector(pvector, 0, 1);
			return(NULL);
		}
		else return(pvector);
	}
	else
	{
		free_ivector(pvector, 0, 2*points-1);
		return(NULL);
	}
}

//***********************************************************************
// Function:    Interpolate()
// Return:      *a = pointer to coefficient matrix [order+1][order+1]
// Args:        order  :  order of interpolation
//              points :  number of points to be used in interpolation
//              ni, nj  :  upper left corner of square in which the point
//                        is to be found
// Use:         sets up all matrices for interpolation
//***********************************************************************
double *CZimsGrid::Interpolate(int order, int points, int ni, int nj)
{
	register int i, j, k, l, m, n;
	int bigorder = (order+1)*(order+1);
	double val, valx, valy, valxn, valyn, d;
	double *a, **_c, **C;
	int *indx;
	int *ptcoord;
	Stamp();
	// Get coords of next "points" points
	ptcoord = GetInterpolationPoints(points, ni, nj);
	// allocate memory for matrices and vectors
	a = dvector(0, bigorder-1);
	C = dmatrix(1, bigorder, 1, bigorder);
	_c = dmatrix(0, 2*order, 0, 2*order);
	indx = ivector(1, bigorder);
	// zero matrix and vector elements
	for(k=0;k<bigorder;k++)
		a[k] = 0.0;

	for(m=0;m<=2*order;m++)
	{
		for(n=0;n<=2*order;n++)
		{
			_c[n][m] = 0.0;
		}
	}
	// Find all elements in the form SUM(fi*xi^n*yi^n)
	for(k=0;k<points;k++)
	{
		i = ptcoord[2*k];
		j = ptcoord[2*k+1];
		val = GetGrid()[i][j];
		valx = xlim[0] + dx*j;
		valy = ylim[1] - dy*i;
		valxn = valyn = 1.0;
		n=0;
		m=0;
		do
		{
			do
			{
				a[n+m*(order+1)] += val*valxn*valyn;
				valxn *= valx;
				n++;
			} while(n<=order);
			valxn = 1.0;
			valyn *= valy;
			m++;
			n=0;
		} while(m<=order);
	}
	// Find all elements in the form SUM(xi^n+N*yi^n+M)
	for(k=0;k<points;k++)
	{
		i = ptcoord[2*k];
		j = ptcoord[2*k+1];
		valx = xlim[0] + dx*j;
		valy = ylim[1] - dy*i;
		valxn = valyn = 1.0;
		n=0;
		m=0;
		do
		{
			do
			{
				_c[m][n] += valxn*valyn;
				valxn *= valx;
				n++;
			} while(n<=(2*order));
			valxn = 1.0;
			valyn *= valy;
			m++;
			n=0;
		} while(m<=(2*order));
	}
	// reorganize 4-tensor into large square matrix for equation solving
	for(m=0;m<bigorder;m++)
	{
		for(n=0;n<bigorder;n++)
		{
			k = n%(order+1);
			l = n/(order+1);
			i = m%(order+1);
			j = m/(order+1);
			C[m+1][n+1] = _c[l+j][k+i];
		}
	}
	// invert equations and back substitute
	ludcmp(C, bigorder, indx, &d);
	lubksb(C, bigorder, indx, a-1);
	// free memory used
	free_dmatrix(C, 1, bigorder, 1, bigorder);
	free_dmatrix(_c, 0, 2*order, 0, 2*order);
	free_ivector(indx, 1, bigorder);
	free_ivector(ptcoord, 0, 2*points-1);
	return (a);
}

//***********************************************************************
// QueryLimits( x, y )
// Checks to see if the pt defined by the input is within the area of
// interest ie. the grid.
// args; x & y coords of point to be checked
// returns: FALSE if within grid, FALSE if outside
//***********************************************************************
BOOL CZimsGrid::QueryLimits( double x, double y )
{
	if ( x > xlim[1] )
		return FALSE;
	else if ( x < xlim[0] )
		return FALSE;
	else if ( y > ylim[1] )
		return FALSE;
	else if ( y < ylim[0] )
		return FALSE;
	else
		return TRUE;
}

//***********************************************************************
// Subroutine	:			Evaluate()
// Arguments  :     x : x coordinate of point to be evaluated
//                  y : y coordinate of point to be evaluated
// Use        :     calculates the interpolated value of a point not on
//                  the grid by 1/r2 weighting
// Returns    :     val - if within grid
//                  znon if not
//***********************************************************************
double CZimsGrid::Evaluate(double x, double y)
{
#ifdef ZIMS_TPSEVAL
	int ni, nj, i, pts;
	static int points = -1;
	static int fast = AfxGetApp()->GetProfileInt("ArithmeticParams", "FastTPS", 0);
	int *pcoords;
	double **realcoords = NULL, *cvec = NULL, *f = NULL;
	double z[2], val;
	Stamp();

	// Get points from .INI file
	if(points == -1)
		points = AfxGetApp()->GetProfileInt("ArithmeticParams", "EvaluatePoints", 10);

	// out of range?
	if( !QueryLimits(x, y) )
		return (GetZnon());
	// This is the upper left hand corner
	nj = (int) ((x-xlim[0])/dx);
	ni = (int) ((ylim[1]-y)/dy);
	// store coords in a vector
	z[0] = x;
	z[1] = y;
	// get neighboring points
	pts = points;
	pcoords = GetInterpolationPoints(pts, x, y);
	// if pcoords == NULL then not enough valid points where found
	if(!pcoords)
	{
		return(GetZnon());
	}
	else
	{
		realcoords = dmatrix(1, pts, 1, 2);
		f = dvector(1, pts);
		// fill tps arrays
		for(i=0;i<pts;i++)
		{
			realcoords[i+1][1] = pcoords[2*i+1]*dx+xlim[0];
			realcoords[i+1][2] = ylim[1] - pcoords[2*i]*dy;
			f[i+1] = GetGrid()[pcoords[2*i]][pcoords[2*i+1]];
		}
		// find coefficients and evaluate spline
		cvec = interpolate_tps(realcoords, pts, f, fast);
		val = evaluate_point_tps(realcoords, cvec, pts, z-1);
		// free memory areas
		free_dmatrix(realcoords, 1, pts, 1, 2);
		free_dvector(f, 1, pts);
		free_dvector(cvec, 1, pts+3);
		free_ivector(pcoords, 0, 2*pts-1);
		return(val);
	}
#else
	int ni, nj, i, j, k;
	double *a, val = 0.0, xn, yn;
	static int order = -1;
	static int points = -1;
	// out of range?
	if(x > xlim[1] || x < xlim[0] ||
		y > ylim[1] || y < ylim[0])
		return (GetZnon());
	// This is the upper left hand corner
	nj = (int) ((x-xlim[0])/dx);
	ni = (int) ((ylim[1]-y)/dy);
	// Get parameters from .INI file...
	if(order == -1)
	{
		order = AfxGetApp()->GetProfileInt("ArithmeticParams", "EvaluateOrder", 1);
		points = AfxGetApp()->GetProfileInt("ArithmeticParams","EvaluatePoints", 10);

		if(points < (order+1)*(order+1))
		{
			pDoc->Log("Increased # of points in Evaluate, to small for current order", LOG_BRIEF);
			points = (order+1)*(order+1);
		}
	}
	// Find coefficient vector for this point
	a = Interpolate(order, points, ni, nj);
	// evaluate 2d polynomial at the given point
	yn = 1.0;
	k=0;
	for(i=0;i<=order;i++)
	{
		xn = 1.0;
		for(j=0;j<=order;j++)
		{
			val += a[k++]*xn*yn;
			xn *= x;
		}
		yn *= y;
	}
	// free memory for coefficients
	free_dvector(a, 0, 1);
	return (val);
#endif
}

//***********************************************************************
// Subroutine	:			ConvertToMilliseconds()
// multiply every value by 1000 t0 convert from seconds to milliseconds
// Returns    :     void
//***********************************************************************
void CZimsGrid::ConvertToMilliseconds()
{
	for(int i=0;i<rows;i++)
	{
		for(int j=0;j<columns;j++)
		{
			//ignore null values
			if(GetGrid()[i][j] == GetZnon())
				continue;

			double value = GetGrid()[i][j];
			GetGrid()[i][j] = value * 1000;
		}
	}	
}

//***********************************************************************
// Subroutine	:			ApplyCutOff()
// Arguments  :     min, max : limits between which grid values are kept, UseMinMax - True to set clipped values
// to min/max of range as appropriate, otherwise set to null
// Use        :     Sets all values outside range to Znon
// Returns    :     void
//***********************************************************************
void CZimsGrid::ApplyCutoff(double min, double max, BOOL UseMinMax)
{
	Stamp();
	if(UseMinMax)
	{
		for(int i=0;i<rows;i++)
		{
			for(int j=0;j<columns;j++)
			{
				//ignore null values
				if(GetGrid()[i][j] == GetZnon())
					continue;

				if(GetGrid()[i][j] < min)
				{
					GetGrid()[i][j] = min;
				}
				else if(GetGrid()[i][j] > max)
				{
					GetGrid()[i][j] = max;
				}
			}
		}
	}
	else
	{
		for(int i=0;i<rows;i++)
		{
			for(int j=0;j<columns;j++)
			{
				if(GetGrid()[i][j] < min || GetGrid()[i][j] > max)
				{
					GetGrid()[i][j] = GetZnon();
				}
			}
		}

	}
}

int sort(const void *x, const void *y)
{
	return (int)(*(double*)x - *(double*)y);
}

//***********************************************************************
// Subroutine	:			ApplyCutOff()
// Arguments  :     Same as above, but uses % instead of absolute values
// Use        :     Sets all values outside range to Znon
// Returns    :     void
//***********************************************************************
void CZimsGrid::ApplyCutoffAverage(double percent, bool UseMinMax)
{
	int totalPoints = rows * columns;
	double* points = new double[totalPoints];
	int usefulPoints = 0;

	for(int i=0;i<rows;i++)
	{
		for(int j=0;j<columns;j++)
		{
			//ignore null values
			if(GetGrid()[i][j] == GetZnon())
				continue;

			points[usefulPoints] = grid[i][j];

			usefulPoints++;
		}
	}	

	qsort(points, usefulPoints, sizeof(double), sort);

	// number of values % represents
	int values = int((percent / 100) * usefulPoints);
	double min=0, max=0;

	// get the min and max vlaues at last
	min = points[values];
	max = points[usefulPoints - values];

	// finally do the actual cutoff
	ApplyCutoff(min, max, UseMinMax);
}

// **********************************************************************
// Subroutine  : CopyZnon()
// Arguments   : the grid to copy the Znons from
// Use         : fills all values which are Znons in the other grid with znons
//               in this grid.
// Returns     : none
// **********************************************************************
void CZimsGrid::CopyZnon(CZimsGrid &othergrid)
{
	// Check if grids actually refer to the same area
	// and also have the same extent
	if(*this != othergrid) return;

	// now copy the Znons from one grid to the other grid
	for(int i=0;i<othergrid.GetRows();i++)
	{
		for(int j=0;j<othergrid.GetColumns();j++)
		{
			if(othergrid.GetZnon(othergrid.GetGrid()[i][j]))
				GetGrid()[i][j] = GetZnon();
		}
	}
}

//***********************************************************************
// Subroutine	:			operator =
// Arguments  :     oldgrid : The old grid the new grid should be set
//                  equal to.
// Use        :     Copies an entire grid to another grid, deallocating
//                  and allocating memory
// Returns    :     reference to the current grid.
//***********************************************************************
CZimsGrid & CZimsGrid::operator = (CZimsGrid &oldgrid)
{
	Stamp();
	oldgrid.Stamp();

	// Check if grid is filled...
	if(grid) free_dmatrix(grid, 0, rows-1, 0, columns-1);
	ClearResample();

	// Initialize all grid values...
	Init(oldgrid.GetRows(), oldgrid.GetColumns(), oldgrid.GetDocument(),
		oldgrid.GetType(), oldgrid.GetZnon());

	WriteLimits(oldgrid.GetXlimits(), oldgrid.GetYlimits());
	avg=oldgrid.GetAvg();
	znonvals=oldgrid.GetZnonVals();
	SetHorizon((int) oldgrid);
	unit_timepower=oldgrid.GetUnitTimePower();
	unit_distpower=oldgrid.GetUnitDistPower();

	// inherit the faultgrid if it exists
	Link((CFaultGrid *) oldgrid);
	SetFaultID(oldgrid.GetFaultID());

	// copy values over to other grid
	for(int i=0;i<rows;i++)
	{
		for(int j=0;j<columns;j++)
		{
			GetGrid()[i][j] = ((double **)oldgrid)[i][j];
		}
	}

	return(*this);
}

//***********************************************************************
// Subroutine	:			operator <<
// Arguments  :     oldgrid : The old grid the new grid should be set
//                  equal to.
// Use        :     Copies an entire grid to another grid, deallocating
//                  and allocating memory, values in grid are NOT copied
// Returns    :     reference to the current grid.
//***********************************************************************
CZimsGrid & CZimsGrid::operator << (CZimsGrid &oldgrid)
{
	// Check if grid is filled...
	if(grid) free_dmatrix(grid, 0, rows-1, 0, columns-1);
	ClearResample();

	// Initialize all grid values...
	Init(oldgrid.GetRows(), oldgrid.GetColumns(), oldgrid.GetDocument(),
		oldgrid.GetType(), oldgrid.GetZnon());
	WriteLimits(oldgrid.GetXlimits(), oldgrid.GetYlimits());
	SetHorizon((int) oldgrid);
	unit_timepower=oldgrid.GetUnitTimePower();
	unit_distpower=oldgrid.GetUnitDistPower();

	// inherit the faultgrid if it exists
	Link((CFaultGrid *) oldgrid);
	SetFaultID(oldgrid.GetFaultID());

	return(*this);
}

//***********************************************************************
// Subroutine	:			operator =
// Arguments  :     val : constant
// Use        :     sets a grid to a constant
// Returns    :     reference to the current grid.
//***********************************************************************
CZimsGrid & CZimsGrid::operator = (double val)
{
	Stamp();

	for(int i=0;i<rows;i++)
	{
		for(int j=0;j<columns;j++)
		{
			GetGrid()[i][j] = val;
		}
	}

	return (*this);
}

//***********************************************************************
// Subroutine	:			operator *=
// Arguments  :     val : const grid should be multiplied by
// Use        :     multiplies grid by a constant
// Returns    :     reference to *this
//***********************************************************************
CZimsGrid & CZimsGrid::operator *= (double val)
{
	Stamp();

	for(int i=0;i<rows;i++)
	{
		for(int j=0;j<columns;j++)
		{
			if(!GetZnon(GetGrid()[i][j]))
				GetGrid()[i][j] *= val;
		}
	}

	return (*this);
}

//***********************************************************************
// Subroutine   :       operator /=
// Arguments    :       val : const that grid should be multiplied by
// Use          :       multiplies grid by a constant
// Returns      :       reference to *this
//***********************************************************************
CZimsGrid & CZimsGrid::operator /= (double val)
{
	Stamp();

	for (int i=0; i<rows; i++ )
	{
		for (int j=0; j<columns; j++ )
		{
			if ( !GetZnon(GetGrid()[i][j]) )
				GetGrid()[i][j] /= val;
		}
	}

	return (*this);
}

//***********************************************************************
// Subroutine	:			operator +=
// Arguments  :     arg1 references to grids to be added to grid
// Use        :     adds arg1 to grid...
// Returns    :     reference to *this
//***********************************************************************
CZimsGrid & CZimsGrid::operator += (CZimsGrid &arg1)
{
	Stamp();
	arg1.Stamp();

	// check unit consistency
	if((arg1.unit_timepower != unit_timepower) ||
		(arg1.unit_distpower != unit_distpower) )
	{
		pDoc->Error("Cannot add grids with different units", MSG_ERR);
		return *this;
	}
	// check type consistency
	if((arg1.GetType() != GetType()))
	{
		pDoc->Error("Cannot add grids of different type", MSG_ERR);
		return *this;
	}
	// check extent.
	if(arg1 != *this)
	{
		pDoc->Error("Cannot add grids of different extent", MSG_ERR);
		return *this;
	}
	// add grids into resultgrid...
	for(int i=0;i<rows;i++)
	{
		for(int j=0;j<columns;j++)
		{
			if(arg1.GetZnon(((double **)arg1)[i][j]) || GetZnon(GetGrid()[i][j]))
				GetGrid()[i][j] = GetZnon();
			else
				GetGrid()[i][j] += ((double **)arg1)[i][j];
		}
	}

	CalcAverage();
	return *this;
}

//***********************************************************************
// Subroutine	:			operator -=
// Arguments  :     arg1 references to grids to be subtracted from grid
// Use        :     subtracts arg1 from grid...
// Returns    :     reference to *this
//***********************************************************************
CZimsGrid & CZimsGrid::operator -= (CZimsGrid &arg1)
{
	Stamp();
	arg1.Stamp();

	// check unit consistency
	if((arg1.unit_timepower != unit_timepower) ||
		(arg1.unit_distpower != unit_distpower) )
	{
		pDoc->Error("Cannot subtract grids with different units", MSG_ERR);
		return *this;
	}
	// check type consistency
	if((arg1.GetType() != GetType()))
	{
		pDoc->Error("Cannot subtract grids of different type", MSG_ERR);
		return *this;
	}
	// check extent.
	if(arg1 != *this)
	{
		pDoc->Error("Cannot subtract grids of different extent", MSG_ERR);
		return *this;
	}
	// sub grids into resultgrid...
	for(int i=0;i<rows;i++)
	{
		for(int j=0;j<columns;j++)
		{
			if(arg1.GetZnon(((double **)arg1)[i][j]) || GetZnon(GetGrid()[i][j]))
				GetGrid()[i][j] = GetZnon();
			else
				GetGrid()[i][j] -= ((double **)arg1)[i][j];
		}
	}

	CalcAverage();

	return *this;
}

//***********************************************************************
// Subroutine	:			operator *=
// Arguments  :     arg1 references to grids to be multiplied to grid
// Use        :     multiplies arg1 to grid...
// Returns    :     reference to *this
//***********************************************************************
CZimsGrid & CZimsGrid::operator *= (CZimsGrid &arg1)
{
	Stamp();
	arg1.Stamp();

	// check extent.
	if(arg1 != *this)
	{
		pDoc->Error("Cannot multiply grids of different extent", MSG_ERR);
		return *this;
	}
	// set units and type correcty
	unit_timepower += arg1.unit_timepower;
	unit_distpower += arg1.unit_distpower;
	SetType();

	// mult grids into resultgrid...
	for(int i=0;i<rows;i++)
	{
		for(int j=0;j<columns;j++)
		{
			if(fabs(((double **) arg1)[i][j]) < FLT_MIN || fabs(GetGrid()[i][j]) < FLT_MIN)
				GetGrid()[i][j] = 0.0;
			else if(arg1.GetZnon(((double **)arg1)[i][j]) || GetZnon(GetGrid()[i][j]))
				GetGrid()[i][j]= GetZnon();
			else
				GetGrid()[i][j] *= ((double **)arg1)[i][j];
		}
	}

	CalcAverage();

	return *this;
}

//***********************************************************************
// Subroutine	:			operator /=
// Arguments  :     arg1 references to grid to be divided by
// Use        :     divides grid by arg1
// Returns    :     reference to *this
//***********************************************************************
CZimsGrid & CZimsGrid::operator /= (CZimsGrid &arg1)
{
	Stamp();
	arg1.Stamp();

	// check extent.
	if(arg1 != *this)
	{
		pDoc->Error("Cannot divide grids of different extent", MSG_ERR);
		return *this;
	}
	// set units and type correcty
	unit_timepower -= arg1.unit_timepower;
	unit_distpower -= arg1.unit_distpower;
	SetType();

	// mult grids into resultgrid...
	for(int i=0;i<rows;i++)
	{
		for(int j=0;j<columns;j++)
		{
			if(arg1.GetZnon(((double **)arg1)[i][j]) || GetZnon(GetGrid()[i][j]) ||
				((double **)arg1)[i][j]==0.0)
				GetGrid()[i][j] = GetZnon();
			else
				GetGrid()[i][j] /= ((double **)arg1)[i][j];
		}
	}

	CalcAverage();

	return *this;
}

//***********************************************************************
// Subroutine	:			operator ==
// Arguments  :     other : grid to be compared to *this
// Use        :     compare the extent of two grids
// Returns    :     TRUE if equal, FALSE if not
//***********************************************************************
BOOL CZimsGrid::operator == (CZimsGrid &other)
{
	// check if dimensions are OK
	if((columns != other.columns) ||
		(rows != other.rows)) return(FALSE);
	// check if extent is OK
	if((xlim[0] != other.xlim[0]) ||
		(xlim[1] != other.xlim[1]) ||
		(ylim[0] != other.ylim[0]) ||
		(ylim[1] != other.ylim[1])) return(FALSE);
	return(TRUE);
}

//***********************************************************************
// Subroutine	:			operator !=
// Arguments  :     other : grid to be compared to *this
// Use        :     compare the extent of two grids
// Returns    :     FALSE if equal, TRUE if not
//***********************************************************************
BOOL CZimsGrid::operator != (CZimsGrid &other)
{
	// check if dimensions are OK
	if((columns != other.columns) ||
		(rows != other.rows)) return(TRUE);
	// check if extent is OK
	if((xlim[0] != other.xlim[0]) ||
		(xlim[1] != other.xlim[1]) ||
		(ylim[0] != other.ylim[0]) ||
		(ylim[1] != other.ylim[1])) return(TRUE);
	return(FALSE);
}

//***********************************************************************
// Subroutine	:			operator <
// Arguments  :     other : grid to be compared to *this
// Use        :     compare all values of two grids, return number of points that
//                  are larger than the point of the other grid.
// Returns    :     number of points NOT <= and not ZNON, hence number of
//                  errorneous points. Return value 0 means all ok
//	                Return value -1 means general inconsistency
//***********************************************************************
int CZimsGrid::operator < (CZimsGrid &other)
{
	int retval=0;

	Stamp();
	other.Stamp();

	// check if dimensions are OK
	if((columns != other.columns) ||
		(rows != other.rows)) return(-1);
	// check if extent is OK
	if((xlim[0] != other.xlim[0]) ||
		(xlim[1] != other.xlim[1]) ||
		(ylim[0] != other.ylim[0]) ||
		(ylim[1] != other.ylim[1])) return(-1);

	for(int i=0;i<rows;i++)
	{
		for(int j=0;j<columns;j++)
		{
			if(!GetZnon(GetGrid()[i][j]) && !other.GetZnon(((double **)other)[i][j]))
			{
				if(GetGrid()[i][j]>((double **)other)[i][j])
					retval++;
			}
		}
	}

	return(retval);
}

//***********************************************************************
// Subroutine	:			operator +
// Arguments  :     arg1, arg2 references to grids to be added
// Use        :     Creates new grid which is the sum of both grids
// Returns    :     POINTER! to the new grid
//***********************************************************************
CZimsGrid * operator + (CZimsGrid &arg1, CZimsGrid &arg2)
{
	CZimsGrid *result;

	arg1.Stamp();
	arg2.Stamp();

	// check unit consistency
	if((arg1.unit_timepower != arg2.unit_timepower) ||
		(arg1.unit_distpower != arg2.unit_distpower) )
	{
		arg1.pDoc->Error("Cannot add grids with different units", MSG_ERR);
		return NULL;
	}
	// check type consistency
	/*if((arg1.GetType() != arg2.GetType()))
	{
		arg1.pDoc->Error("Cannot add grids of different type", MSG_ERR);
		return NULL;
	}*/
	// check extent.
	if(arg1 != arg2)
	{
		arg1.pDoc->Error("Cannot add grids of different extent", MSG_ERR);
		return NULL;
	}
	// initialize new grid...
	result=new CZimsGrid;
	result->Init(arg1.rows, arg1.columns, arg1.pDoc, arg1.GetType(), arg1.GetZnon());
	result->WriteLimits(arg1.GetXlimits(), arg1.GetYlimits());
	result->unit_timepower=arg1.GetUnitTimePower();
	result->unit_distpower=arg1.GetUnitDistPower();
	// inherit the faulgrid if it exists
	result->Link((CFaultGrid *) arg1);
	result->SetFaultID(arg1.GetFaultID());
	// if both grids are of the same horizon -> new grid will also be of that horizon
	if(arg1.GetHorizon() == arg2.GetHorizon())
		result->SetHorizon(arg1.GetHorizon());

	// add grids into resultgrid...
	for(int i=0;i<result->rows;i++)
	{
		for(int j=0;j<result->columns;j++)
		{
			if(arg1.GetZnon(((double **)arg1)[i][j]) || arg2.GetZnon(((double **)arg2)[i][j]))
				((double**) (*result))[i][j] = result->GetZnon();
			else
				((double**)(*result))[i][j] = ((double **)arg1)[i][j] + ((double **)arg2)[i][j];
		}
	}

	result->CalcAverage();
	return result;
}

//***********************************************************************
// Subroutine	:			operator -
// Arguments  :     arg1, arg2 references to grids to be subtracted
// Use        :     Creates new grid which is the difference of both grids
// Returns    :     POINTER! to the new grid
//***********************************************************************
CZimsGrid * operator - (CZimsGrid &arg1, CZimsGrid &arg2)
{
	CZimsGrid *result;

	arg1.Stamp();
	arg2.Stamp();

	// check unit consistency
	if((arg1.unit_timepower != arg2.unit_timepower) ||
		(arg1.unit_distpower != arg2.unit_distpower) )
	{
		arg1.pDoc->Error("Cannot subtract grids with different units", MSG_ERR);
		return(NULL);
	}
	// check type consistency
	/*if((arg1.GetType() != arg2.GetType()))
	{
		arg1.pDoc->Error("Cannot subtract grids of different type", MSG_ERR);
		return(NULL);
	}*/
	// check if dimensions are OK
	if(arg1 != arg2)
	{
		arg1.pDoc->Error("Cannot subtract grids of different extent", MSG_ERR);
		return(NULL);
	}
	// initialize new grid...
	result = new CZimsGrid;
	result->Init(arg1.rows, arg1.columns, arg1.pDoc, arg1.GetType(), arg1.GetZnon());
	result->WriteLimits(arg1.GetXlimits(), arg1.GetYlimits());
	result->unit_timepower=arg1.GetUnitTimePower();
	result->unit_distpower=arg1.GetUnitDistPower();
	// inherit the faulgrid if it exists
	result->Link((CFaultGrid *) arg1);
	result->SetFaultID(arg1.GetFaultID());
	// if both grids are of the same horizon -> new grid will also be of that horizon
	if(arg1.GetHorizon() == arg2.GetHorizon())
		result->SetHorizon(arg1.GetHorizon());

	// sub grids into resultgrid...
	for(int i=0;i<result->rows;i++)
	{
		for(int j=0;j<result->columns;j++)
		{
			if(arg1.GetZnon(((double **)arg1)[i][j]) || arg2.GetZnon(((double **)arg2)[i][j]))
				((double**) (*result))[i][j] = result->GetZnon();
			else
				((double**) (*result))[i][j] = ((double **)arg1)[i][j] - ((double **)arg2)[i][j];
		}
	}

	result->CalcAverage();
	return result;
}

//***********************************************************************
// Subroutine	:			operator *
// Arguments  :     arg1, arg2 references to grids to be multiplied
// Use        :     Creates new grid which is the product of both grids
// Returns    :     POINTER! to the new grid
//***********************************************************************
CZimsGrid * operator * (CZimsGrid &arg1, CZimsGrid &arg2)
{
	CZimsGrid *result;

	arg1.Stamp();
	arg2.Stamp();
	// check if dimensions are OK
	if(arg1 != arg2)
	{
		arg1.pDoc->Error("Cannot multiply grids of different extent", MSG_ERR);
		return(NULL);
	}
	// initialize new grid...
	result=new CZimsGrid;
	result->Init(arg1.rows, arg1.columns, arg1.pDoc, arg1.GetType(), arg1.GetZnon());
	result->WriteLimits(arg1.GetXlimits(), arg1.GetYlimits());
	// set units and type correcty
	result->unit_timepower = arg1.unit_timepower + arg2.unit_timepower;
	result->unit_distpower = arg1.unit_distpower + arg2.unit_distpower;
	result->SetType();
	// inherit the faulgrid if it exists
	result->Link((CFaultGrid *) arg1);
	result->SetFaultID(arg1.GetFaultID());
	// if both grids are of the same horizon -> new grid will also be of that horizon
	if(arg1.GetHorizon() == arg2.GetHorizon())
		result->SetHorizon(arg1.GetHorizon());

	// mult grids into resultgrid...
	for(int i=0;i<result->rows;i++)
	{
		for(int j=0;j<result->columns;j++)
		{
			if(fabs(((double **) arg1)[i][j]) < FLT_MIN || fabs(((double **) arg2)[i][j]) < FLT_MIN)
				((double **)(*result))[i][j] = 0.0;
			else if(arg1.GetZnon(((double **)arg1)[i][j]) || arg2.GetZnon(((double **)arg2)[i][j]))
				((double **)(*result))[i][j] = result->GetZnon();
			else
				((double **)(*result))[i][j] = ((double **)arg1)[i][j] * ((double **)arg2)[i][j];
		}
	}

	result->CalcAverage();
	return result;
}

//***********************************************************************
// Subroutine	:			operator /
// Arguments  :     arg1, arg2 references to grids to be multiplied
// Use        :     Creates new grid which is the retio of both grids
// Returns    :     POINTER! to the new grid
//***********************************************************************
CZimsGrid * operator / (CZimsGrid &arg1, CZimsGrid &arg2)
{
	CZimsGrid *result = NULL;

	arg1.Stamp();
	arg2.Stamp();
	// check if dimensions are OK
	if(arg1 != arg2)
	{
		arg1.pDoc->Error("Cannot divide grids of different extent", MSG_ERR);
		return(NULL);
	}
	// initialize new grid...
	result=new CZimsGrid;
	result->Init(arg1.rows, arg1.columns, arg1.pDoc, arg1.GetType(), arg1.GetZnon());
	result->WriteLimits(arg1.GetXlimits(), arg1.GetYlimits());
	// set units and type correcty
	result->unit_timepower = arg1.unit_timepower - arg2.unit_timepower;
	result->unit_distpower = arg1.unit_distpower - arg2.unit_distpower;
	result->SetType();
	// inherit the faulgrid if it exists
	result->Link((CFaultGrid *) arg1);
	result->SetFaultID(arg1.GetFaultID());
	// if both grids are of the same horizon -> new grid will also be of that horizon
	if(arg1.GetHorizon() == arg2.GetHorizon())
		result->SetHorizon(arg1.GetHorizon());

	// div grids into resultgrid...
	for(int i=0;i<result->rows;i++)
	{
		for(int j=0;j<result->columns;j++)
		{
			if(arg1.GetZnon(((double **)arg1)[i][j]) || arg2.GetZnon(((double **)arg2)[i][j]) ||
				(fabs(((double**)arg2)[i][j]) < 10*DBL_MIN) )
				((double**)(*result))[i][j] = result->GetZnon();
			else
				((double**)(*result))[i][j] = ((double **)arg1)[i][j] / ((double **)arg2)[i][j];
		}
	}

	result->CalcAverage();
	return result;
}

//***********************************************************************
// Subroutine	:			AutoCorrect()
// Arguments  :     withRespectTo: The grid above the one which is to autocorrect
//                  itself.
// Use        :     AutoCorrects a grid, i.e.:
//									how=0 (GRID_AUTOCORRECT_SMATRT) sets each of the values
//									that is LESS
//                  than the corresponding entry in the "withRespectTo" grid to the
//                  value that that grid has there. If time grids overlap, they can be
//                  corrected (bent downwards) with this method.
//									how=1: GRID_AUTOCORRECT_LOWERZERO, HIGHERZERO, BOTHZERO:
//                  sets the conflicting nodes of the lower or higher or both grids
//                  to ZNON
// Returns    :     Nothing, but modifies the grid data
//***********************************************************************
void CZimsGrid::AutoCorrect(int how, CZimsGrid *withRespectTo)
{
	// Check if compared grids are actually have the same extent
	if((*this) == (*withRespectTo))
	{
		Stamp();
		withRespectTo->Stamp();
		double compareznon=withRespectTo->GetZnon();
		for(int i=0;i<rows;i++)
		{
			for(int j=0;j<columns;j++)
			{
				if(!withRespectTo->GetZnon(withRespectTo->GetGrid()[i][j]) && !GetZnon(GetGrid()[i][j])
					&& withRespectTo->GetGrid()[i][j]>GetGrid()[i][j])
				{
					if(how==ZIMS_AUTOCORRECT_SMART)
						GetGrid()[i][j] = max(GetGrid()[i][j], withRespectTo->GetGrid()[i][j]);
					if(how & ZIMS_AUTOCORRECT_HIGHERZNON)
						withRespectTo->GetGrid()[i][j] = compareznon;
					if(how & ZIMS_AUTOCORRECT_LOWERZNON)
						GetGrid()[i][j] = znon;
					// case "ZIMS_AUTOCORRECT_BOTHZNON" handled as both if(hor &..) will flag
				}
			}
		}

		CalcAverage();
		withRespectTo->CalcAverage();
	}
}

void CZimsGrid::SetName(CString newname)
{
	// assign this name to the grid. If any other grid has got the same name,
	// assign a "version number" in the form  gridname=newname_Vxx, where xx is
	// some unique number
	// go through all the other grids
	int latestversion = 0;
	CZimsCube *zimscube=&pDoc->zimscube;
	CZimsGrid *thegrid;
	CString gridname;
	CString pattern=newname+"_V";		// Replaced space with _
	int patternlength = pattern.GetLength();
	char buf[256];

	for(int i=0;i<zimscube->GetSize();i++)
	{
		thegrid=(CZimsGrid *)(zimscube->GetAt(i));

		// Check that you are not comparing the grid to itself
		if(thegrid == this) continue;

		// names are equal if a) gridname=newname or b) gridname=newname+version numb.
		gridname=thegrid->GetName();
		if(newname == gridname)
			latestversion = max(latestversion, 1);
		else if(pattern == gridname.Left(patternlength))
			latestversion=max(latestversion,
			atoi((const char *)gridname.Mid(patternlength)));
	}

	if(latestversion==0)
		name = newname;  // no other grid found
	else
	{
		sprintf(buf, "%s_V%d", (const char *)newname, latestversion+1);
		name=(CString)buf;
	}
}

//***********************************************************************
// Subroutine	:			LimitGrid()
// Arguments  :     withRespectTo: The grid the ZNONs of which are to be copied
// Use        :     limits a grid by setting all those points to ZNON that
//                  are ZNON in the "withRespectTo" grid. Useful for clipping
//                  bits out of a depth grid, looking at some original time grid
//									full of ZNONs
// Returns    :     Nothing, but modifies the grid data
//***********************************************************************
void CZimsGrid::LimitGrid(CZimsGrid *withRespectTo)
{
	Stamp();
	withRespectTo->Stamp();

	for(int i=0;i<rows;i++)
	{
		for(int j=0;j<columns;j++)
		{
			if(withRespectTo->GetZnon(withRespectTo->GetGrid()[i][j]))
				GetGrid()[i][j] = znon;
		}
	}

	CalcAverage();
}

//***********************************************************************
// Subroutine	:	CopyAndDownsample()
// Arguments  :     oldgrid: Grid to be copied
// Use        :     copies every n-th node from oldgrid, so that this grid
//					will contain a downsampled copy with a maximum number
//					of nodes that is read from the INI file (see ShouldDownsample)
// Returns    :     Nothing, but modifies the grid data
//***********************************************************************
void CZimsGrid::CopyAndDownsample(CZimsGrid &oldgrid)
{
	int i, ii, j, jj;
	Stamp();
	oldgrid.Stamp();

	// Check if grid is filled...
	if(grid) free_dmatrix(grid, 0, rows-1, 0, columns-1);
	ClearResample();
	// do we need to downsample, and if so, how far?
	int oldrows=oldgrid.GetRows();
	int oldcols=oldgrid.GetColumns();
	int oldsize=oldrows*oldcols;
	int targetsize=DownsampleTargetsize();
	int factor=1;
	if(oldsize>targetsize)
	{
		while(oldsize/(factor*factor)>targetsize)
		{
			factor*=2;
		}
	}
	// Initialize all grid values...
	int newrows, newcols;
	newrows=oldrows/factor;
	newcols=oldcols/factor;
	Init(newrows, newcols, oldgrid.GetDocument(),
		oldgrid.GetType(), oldgrid.GetZnon());
	WriteLimits(oldgrid.GetXlimits(), oldgrid.GetYlimits());
	SetHorizon((int) oldgrid);
	unit_timepower=oldgrid.GetUnitTimePower();
	unit_distpower=oldgrid.GetUnitDistPower();

	// copy values over to other grid
	for(i=0, ii=0;ii<newrows;i+=factor, ii++)
	{
		for(j=0, jj=0;jj<newcols;j+=factor, jj++)
		{
			GetGrid()[ii][jj] = ((double **)oldgrid)[i][j];
		}
	}
}

//***********************************************************************
// Subroutine	:			DonwnsampleTargetsize()
// Use        :     finds size down to which grids should be downsampled
//									if they are too large for contouring. It's in the range x
//									(INI-File entry) up to 4*x-1
// Returns    :     Nothing, but modifies the grid data
//***********************************************************************
int CZimsGrid::DownsampleTargetsize()
{
	// read INI value, make it into range 10, 000...75, 000
	int minsize=AfxGetApp()->GetProfileInt("Contouring", "DownSampleThreshold", 25000);
	minsize=max(minsize, 1000);
	minsize=min(minsize, 75000);
	return 4*minsize-1;
}

//***********************************************************************
// Subroutine	:			GetUnitName()
// Use        :     return name of grid unit
//					So far, only does m/ft, msec, m/msec, ft/msec or "" (unknown)
// Returns    :     pointer to string (must not be modified!!!)
//***********************************************************************
char *CZimsGrid::GetUnitName()
{
	char buf2[32];
	// buf2=generic unit of distance
	strcpy(buf2, (pDoc->Units & UNIT_OUTPUTINFEET) ? "feet" : "metres");
	unitname[0]=0;
	if(unit_timepower==1 && unit_distpower==0)
		strcpy(unitname, "msec");
	if(unit_timepower==-1 && unit_distpower==1)
		sprintf(unitname, "%s/sec", buf2);
	if(unit_timepower==0 && unit_distpower==1)
		strcpy(unitname, buf2);
	return unitname;
}

//***********************************************************************
// Subroutine	:			GetTypeName()
// Use        :     return type of grid as a string
// Returns    :     pointer to string (must not be modified!!!)
//***********************************************************************
char *CZimsGrid::GetTypeName()
{
	switch(GetType())
	{
	case CSD_TYPE_TIME :
		strcpy(type_name, "Time");
		break;
	case CSD_TYPE_RMSVEL :
	case CSD_TYPE_INTVEL :
	case CSD_TYPE_AVGVEL :
		strcpy(type_name, "Vel.");
		break;
	case CSD_TYPE_DEPTH :
		strcpy(type_name, "Depth");
		break;
	default:
		strcpy(type_name, "Grid");
	}
	return type_name;
}

//Same as SmoothMovingWindow but takes the length in metres rather than number of nodes
void CZimsGrid::SmoothMovingWindow2(CZimsGrid *fromgrid, double length, int minSmoothNodes)
{
	double *ext = fromgrid->GetXlimits();
	double nodeSpacing = (ext[1]-ext[0])/(fromgrid->GetColumns()-1);

	int nodes = int(length / nodeSpacing);

	if(nodes < minSmoothNodes)
		nodes = minSmoothNodes;

	SmoothMovingWindow(fromgrid, nodes);
}

//***********************************************************************
// Subroutine	:			SmoothMovingWindow()
// Arguments  :     fromgrid: Grid to be copied
//									windowsize: odd (!) sidelength of moving window
// Use        :     copies everything from "fromgrid" into this grid
//									(which is created if necessary) and smoothes it by means
//									of a running average
//									Prior to calling this, one HAS to do something along the
//									lines of  (*newgrid)=(*fromgrid) to copy grid extend etc.
// Returns    :     Nothing, but modifies the grid data
//***********************************************************************
void CZimsGrid::SmoothMovingWindow(CZimsGrid *fromgrid, int windowsize)
{
	int i = 0, j = 0, ii = 0, jj = 0;
	// make window size odd
	windowsize=2*(windowsize/2)+1;
	int wsize=windowsize/2;  // extend of window to either side of actual point
	int thiswsize = 0; // wsize for current point, smaller than wsize along edges
	double wgtsum = 0;		// sum up weights for running weighted average
	double thiswgt = 0;
	double sum = 0;   // to sum up weighted average
	Stamp();
	fromgrid->Stamp();

	// Check if grid is filled...
	if(grid) 
		free_dmatrix(grid, 0, rows-1, 0, columns-1);

	ClearResample();

	if(!(grid = dmatrix(0, rows-1, 0, columns-1)))
	{
		rows=0;
		columns=0;
		return;
	}

	double **orgvals=fromgrid->GetGrid();

	// computing the weights is quite expensive, we do it once and for all here
	double **weights=dmatrix(0, windowsize-1, 0, windowsize-1);

	i=wsize;
	j=i;

	for(ii=0;ii<=2*wsize;ii++)
	{
		for(jj=0;jj<=2*wsize;jj++)
		{
			weights[ii][jj]=1.0-sqrt((double)(ii-i)*(ii-i)+(jj-j)*(jj-j))/(sqrt(2.0)*(wsize+1));
		}
	}

	for(i=0;i<rows;i++)
	{
		for(j=0;j<columns;j++)
		{
			if(GetZnon(orgvals[i][j]))
				GetGrid()[i][j]=znon;
			else
			{
				wgtsum=0.0;  // will at least go up to 1 as the actual point is always there
				sum=0.0;
				// compute size of this current window. If we are close to an edge, it
				// might be smaller than wsize.
				thiswsize=min(wsize, i);
				thiswsize=min(thiswsize, j);
				thiswsize=min(thiswsize, rows-1-i);
				thiswsize=min(thiswsize, columns-1-j);
				for(ii=i-thiswsize;ii<=i+thiswsize;ii++)
				{
					for(jj=j-thiswsize;jj<=j+thiswsize;jj++)
					{
						if(!GetZnon(orgvals[ii][jj]))
						{
							// weight decays linearly with distance from (i, j)
							thiswgt=weights[ii-i+wsize][jj-j+wsize];
							wgtsum+=thiswgt;
							sum+=thiswgt*orgvals[ii][jj];
						}
					}
				}

				GetGrid()[i][j]=sum/wgtsum;
			}
		}	
	}

	free_dmatrix(weights, 0, windowsize-1, 0, windowsize-1);
}

//***********************************************************************
// Subroutine	:			UseProperZnon()
// Use        :     sets the ZNON value to 1E30, and changes the grid
//									accordingly if necessary
// Returns    :     Nothing, but modifies the grid data
//***********************************************************************
void CZimsGrid::UseProperZnon()
{
	Stamp();

	for(int i=0;i<rows;i++)
	{
		for(int j=0;j<columns;j++)
		{
			if(GetZnon(GetGrid()[i][j]))
				GetGrid()[i][j]=ZIMS_VAL_ZNON;
		}
	}

	znon=ZIMS_VAL_ZNON;
}

//***********************************************************************
// Subroutine	:			RoundAllGridnodes()
// Use        :     Rounds all grid nodes to 3 decimal places to avoid
//									problems if two "almost" equal nodes in different grids appear
//									different and cause precision problems
//***********************************************************************
void CZimsGrid::RoundAllGridnodes()
{
	return; //!!
	for(int i=0;i<rows;i++)
	{
		for(int j=0;j<columns;j++)
		{
			if(!GetZnon(GetGrid()[i][j]))
				GetGrid()[i][j]=
				((long int)((GetGrid()[i][j])*ZIMS_ROUNDIMPORTEDGRIDS_DOWNTO+0.5))/
				ZIMS_ROUNDIMPORTEDGRIDS_DOWNTO;
		}
	}
}

//***********************************************************************
// Subroutine	:			MakeRawName()
// Use        :     This function created the raw name for internal memory
//                  management for the each ZimsGrid()
// Returns    :     success
//***********************************************************************
BOOL CZimsGrid::MakeRawName(CString &grawname)
{
	char rawname[13];

	// Check if we are dealing with a legal grid...
	if(GetID() < 0 || GetHorizon() < -1) return FALSE;
	// Now make the internal grid name
	sprintf(rawname, "g%3d%2x%2x.swp", GetID(), GetType(), GetHorizon()+0x80);
	for(int i=0;i<13;i++)
		if(isspace(rawname[i])) rawname[i] = '_';
	// copy it to the output CString
	grawname = CString(rawname);
	return TRUE;
}

//***********************************************************************
// Subroutine	:			GetGrid()
// Use        :     This function returns a pointer to the grid and
//                  swaps it in if necessary
// Returns    :     pointer to grid...
//***********************************************************************
double **CZimsGrid::GetGrid(void)
{
	return grid;
}

//***********************************************************************
// Subroutine :		FindNewID(void)
// Use        :     This function will find a new id for a grid depending
//                  on its type and horizon
// Returns    :     the new ID
//***********************************************************************
int CZimsGrid::FindNewID(void)
{
	int newid;

	newid = pDoc->zimscube.FindNewID(GetHorizon(), GetType());

	return newid;
}

//***********************************************************************
// Subroutine :		  FindNormal()
// Use        :     This function will find the normal to a surface at a
//                  certain location x, y
// Returns    :     a vector 0..2 of the normal
//***********************************************************************
double *CZimsGrid::FindNormal(double x, double y)
{
	// define the original guess for the width of the search interval
	double hx = 3.0*dx, hy = 3.0*dy;
	static double err;
	double *norm = dvector(0, 2), ln, x1, y1;
	int i = 0;

	//  Adjust point at which we compute derivative if we are too close
	//  to the boundary. We assume that the derivative does not change
	//  significantly over the last 3 points. This will be ok as long as
	//  the surface is reasonably smooth. Data points at the edge tend to
	//  be suspect anyway.

	if ( x-hx < xlim[0] )
		x1 = xlim[0] + hx;
	else if ( x+hx > xlim[1] )
		x1 = xlim[1] - hx;
	else
		x1 = x;
	if ( y-hy < ylim[0] )
		y1 = ylim[0] + hy;
	else if ( y+hy > ylim[1] )
		y1 = ylim[1] - hy;
	else
		y1 = y;

	norm[0] = -dfridr(x1, y1, hx, 0.0, &err);
	norm[1] = -dfridr(x1, y1, 0.0, hy, &err);
	norm[2] = 1.0;

	if(GetZnon(-norm[0]) || GetZnon(-norm[1]))
	{
		free_dvector(norm, 0, 2);
		return NULL;
	}

	ln = sqrt(norm[0]*norm[0]+norm[1]*norm[1]+1.0);

	for(i=0;i<2;i++)
		norm[i] /= ln;

	return norm;
}

//***********************************************************************
// Subroutine :		  dfridr
// Use        :     This function will find the derivative of a 3D
//                  function along a certain direction defined by
//                  a point (x, y) and a direction hx, hy
// Returns    :     the value of df/dr
//***********************************************************************
double CZimsGrid::dfridr(double x, double y, double hx, double hy, double *err)
{
	double errt, fac, hhx, hhy, hh, **a, ans = 0;
	double res1, res2;

	if(hx == 0.0 && hy == 0.0)
		nrerror("hx or hy must be non zero in dfridr.");

	a=dmatrix(1, NTAB, 1, NTAB);

	hhx=hx; hhy=hy;
	hh=sqrt(hhx*hhx+hhy*hhy);

	res1 = Evaluate(x+hhx, y+hhy);
	res2 = Evaluate(x-hhx, y-hhy);

	if(GetZnon(res1) || GetZnon(res2))
	{
		free_dmatrix(a, 1, NTAB, 1, NTAB);
		return(GetZnon());
	}

	a[1][1]=(res1-res2)/(2.0*hh);

	*err = GetZnon();

	for(int i=2;i<=NTAB;i++)
	{
		// succesively smaller step along the chosen direction.
		hhx /= CON; hhy /= CON; hh /= CON;

		res1 = Evaluate(x+hhx, y+hhy);
		res2 = Evaluate(x-hhx, y-hhy);

		if(GetZnon(res1) || GetZnon(res2))
		{
			free_dmatrix(a, 1, NTAB, 1, NTAB);
			return(GetZnon());
		}

		a[1][i]=(res1-res2)/(2.0*hh);

		fac = CON2;

		// no more function evalulations are needed here.
		double  zz;         // this is only used to avoid a compiler bug.
		for(int j=2;j<=i;j++)
		{
			zz = (a[j-1][i]*fac-a[j-1][i-1])/(fac-1.0);
			a[j][i] = zz;
			fac=CON2*fac;
			errt=DMAX(fabs(a[j][i]-a[j-1][i]), fabs(a[j][i]-a[j-1][i-1]));

			if(errt < *err)
			{
				*err=errt;
				ans = zz;
			}
		}

		// check if new iteration made things much worse...
		if(fabs(a[i][i]-a[i-1][i-1]) >= SAFE*(*err))
		{
			free_dmatrix(a, 1, NTAB, 1, NTAB);
			return(ans);
		}
	}

	free_dmatrix(a, 1, NTAB, 1, NTAB);
	return(ans);
}
