// well point interrogation

#include "VelmanDoc.h"

//***********************************************************************
// Subroutine	:			ClearContents()
// Arguments	:			none
// Use        :     Return memory allocated by CWellPointList
// Returns    :     none
//***********************************************************************
void CWellPointList::ClearContents(void)
{
	int i = 0;

	while(i < GetSize()) 
	{
		delete (CWellPoint *) GetAt(i++);
	}

	RemoveAll();
}

//***********************************************************************
// Subroutine	:	 ReadFromFile()
// Arguments:    name of file to read from
// Use:          reads x, y, depth and maybe also well names from a file
// Returns:      number of successfully read points
//
// File format: TAB delimited columns in ASCII format
// first line MIGHT contain description of the order of columns, containing
// the characters "x" "y" and "d" in the first three columns in any order.
// That defines which column contains what. Default: x=col 1, y=col 2, d=col 3
// All other lines: Either entries x TAB y TAB d (or whatever order was
// defined in the first line), or an entry   nnnnn: in the first column (must
// end with a colon in that case), defining the name of the well that the
// following lines belong to. This is optional, if there is no name defined,
// a blank name is used. If a name is defined, that well name is used for
// all following lines up to the definition of the next name.
//***********************************************************************
int CWellPointList::ReadFromFile(const char *filename)
{
	char buf[512];
	CString line;
	CString wellname="";
	char col1, col2, col3;
	int icol1, icol2, icol3;
	int xcol, ycol, dcol;
	int nEntries=0;
	double val[3];
	CWellPoint *wellpt;

#ifdef VELMAN_UNIX
	ifstream file(filename);
#else
	std::ifstream file(filename);
#endif
	if(!file.good())
	{
		sprintf(buf, "Cannot open well point information file '%s'.", filename);
		pDoc->Error(buf, MSG_ERR);
		return 0;
	}

	// read the first line -- it should contain a description of how the
	// columns are ordered. If it does not, it might already be the first
	// data line. (A description line is optional)
	if(!GetNewLine(file, line))
	{
		sprintf(buf, "Well point information file '%s' is empty.", filename);
		pDoc->Error(buf, MSG_ERR);
		file.close();
		return 0;
	}

	// does the first line contain ordering information?
	sscanf((const char *)line, "%c\t%c\t%c", &col1, &col2, &col3);
	icol1=(int)(toupper(col1));
	icol2=(int)(toupper(col2));
	icol3=(int)(toupper(col3));
	// the three standard columns where the data comes from
	xcol=0;
	ycol=1;
	dcol=2;
	// does the first line specify a different order?
	if(strchr("XYD", icol1) && strchr("XYD", icol2) && strchr("XYD", icol3) &&
		icol1!=icol2 && icol1!=icol3 && icol2!=icol3)
	{
		// we have a special order!
		xcol=(icol1=='X' ? 0 : (icol2=='X' ? 1 : 2));
		ycol=(icol1=='Y' ? 0 : (icol2=='Y' ? 1 : 2));
		dcol=(icol1=='D' ? 0 : (icol2=='D' ? 1 : 2));
		// and we need to read a new line!
		if(!GetNewLine(file, line))
		{
			sprintf(buf, "Well point information file '%s' is empty.", filename);
			pDoc->Error(buf, MSG_ERR);
			file.close();
			return 0;
		}
	}

	// now read all lines until the end of file
	do
	{
		// does the line specify the name of the following wells?
		if(line.Right(1)==":")
		{
			// yes -- extract it from the line and continue
			wellname=line.Left(line.GetLength()-1);
			continue;
		}
		// else read out three numbers and assign them
		if(sscanf((const char *)line, "%lf%lf%lf", &val[0], &val[1], &val[2])==3)
		{
			// create a new well point
			wellpt=new CWellPoint(val[xcol], val[ycol], val[dcol], wellname);
			Add(wellpt);
			nEntries++;
		}
	} while(GetNewLine(file, line));

	file.close();

	if(nEntries==0)
	{
		sprintf(buf, "Well point information file '%s' does not follow format"
			" specifications. No data read.", filename);
		pDoc->Error(CString(buf), MSG_ERR);
	}
	else
	{
		sprintf(buf, "Read %d entries from the well point file '%s'", nEntries, filename);
		pDoc->Log(CString(buf), LOG_STD);
	}
	SortByName();
	return nEntries;
}

//***********************************************************************
// Subroutine	:	 WriteToFile()
// Arguments:    name of file to write to
// Use:          writes all x, y, depth, time and maybe also well names to a file
// Returns:      number of successfully written points
//***********************************************************************
int CWellPointList::WriteToFile(const char *filename)
{
	int i = 0;
	CWellPoint *wellpt;
	char buf[512];

	ofstream file(filename);
	CString wellname;

	if(!file.good())
	{
		sprintf(buf, "Cannot export time conversion file: "
			"Could not open file '%s'", filename);
		pDoc->Error(buf, MSG_ERR);
		return 0;
	}

	file << "Time conversion file for project '" << pDoc->Title << "'" << endl;
	file << endl;
	file << "x\ty\ttime\tdepth" << endl;

	SortByName();
	for(i=0;i<GetSize();i++)
	{
		wellpt=(CWellPoint *)GetAt(i);
		if(wellpt->status & WLPT_STATUS_INVALID)
			continue;
		if(wellpt->name != wellname)
		{
			wellname=wellpt->name;
			file << (const char *)wellname << ":" << endl;
		}
		sprintf(buf, "%-7.0f\t%-7.0f\t", wellpt->x, wellpt->y);
		file << buf;
		if(wellpt->status & WLPT_STATUS_HAVETIME)
		{
			sprintf(buf, "%-7.2f", wellpt->t);
			file << buf;
		}
		else
			file << "?";
		sprintf(buf, "\t%-7.2f", wellpt->d);
		file << buf << endl;
	}
	file << endl << "All times are two-way milliseconds.";
	return 0;
}

//***********************************************************************
// Subroutine	:	 CompareAndSwap()
// Arguments:		pos=position to be compared with its following neighbour
// Use:				   compare two consecutive entries in the well point list and
//							 swap them if they are not in order. Sort them by well names,
//							and within each well sort them by increasing depth.
//							 nor a comment line (=it does not start with "*" or ";")
// Returns:     TRUE if we had to swap (order not yet perfect)
//***********************************************************************
BOOL CWellPointList::CompareAndSwap(int pos)
{
	CWellPoint *thispt, *nextpt, *temp;
	thispt=(CWellPoint *)GetAt(pos);
	nextpt=(CWellPoint *)GetAt(pos+1);

	int string_compare=thispt->name.CompareNoCase((const char *)nextpt->name);
	if( string_compare> 0 || string_compare==0 && thispt->d > nextpt->d)
	{
		temp = (CWellPoint *)GetAt(pos);
		SetAt(pos, GetAt(pos+1));
		SetAt(pos+1, temp);
		return TRUE;
	}
	return FALSE;
}

//***********************************************************************
// Subroutine	:	 GetNewLine()
// Arguments:		file=open input stream, line=line that is read
// Use:				   read the next line from a stream that is neither empty
//							 nor a comment line (=it does not start with "*" or ";")
// Returns:      FALSE if file end reached, TRUE otherwise
//							and the read line in "line"
//***********************************************************************
#ifdef VELMAN_UNIX
BOOL CWellPointList::GetNewLine(ifstream &file, CString &line)
#else
BOOL CWellPointList::GetNewLine(std::ifstream &file, CString &line)
#endif
{
	char buf[256];

	line="";
	if(file.eof())
		return FALSE;

	do
	file.getline(buf, 256);
	while(!file.eof() && (buf[0]==';' || buf[0] =='*' || buf[0]==0));

	line=buf;
	return TRUE;
}

//***********************************************************************
// Subroutine	:	 SortByName()
// Use:				   sort the well point database by well names, then by depths
//***********************************************************************
void CWellPointList::SortByName()
{
	BOOL bNotDone = TRUE;

	while (bNotDone)
	{
		bNotDone = FALSE;
		for(int pos = 0;pos < GetUpperBound();pos++)
			bNotDone |= CompareAndSwap(pos);
	}
}

//***********************************************************************
// Subroutine	:	 TimeConvert()
// Arguments:		pointer to the zimscube containing all necessary grids
// Use:				  time convert a depth value
// Returns:      FALSE if file end reached, TRUE otherwise
//							and the read line in "line"
//***********************************************************************
void CWellPoint::TimeConvert(CWellPointList *wellpointlist, CZimsCube *zimscube)
{
	// reset flag
	status &= ~WLPT_STATUS_HAVETIME;

	if(status & WLPT_STATUS_INVALID)
		return;

	t=zimscube->TimeConvertDepthPoint(x, y, d,
		wellpointlist->interp_method,
		wellpointlist->extrap_gradient_percent);

	if(t<1.0E29)
		status|=WLPT_STATUS_HAVETIME;
}
