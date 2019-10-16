#include "VelmanDoc.h"
#include "resource.h" // needed for importpr.h to function
#include "ProgressDlg.h" // needed to update "progress" dialog box
#include "cmntypes.h" // needed for ZIMS_VAL_ZNON
#ifndef VM64
#include "gridio.h" // EarthVision supplied header for reading/writing
// their files. Note only for 32bit.
#endif
#include "helper.h"
#include "shlwapi.h"
#include <string.h>

//#ifndef VELMAN_UNIX
//using namespace std;
//#endif

//***********************************************************************
// Subroutine : GetString()
// Arguments : start : column to start reading string
// end : column to end reading string
// Use : read patrial string from inputbuffer
// please don't forget to delete pointer afterwards
// columns are counted starting with 1 *not* 0
// Returns : string with 0x00 termination
//***********************************************************************
char *CSeismicData::GetString(char *inputbuffer, int start, int end)
{
	char *tempc = new char[end - start + 2];
	tempc[end - start + 1] = 0x00;
	strncpy(tempc, (const char *)inputbuffer + start - 1, end - start + 1);
	return(tempc);
}

//***********************************************************************
// Subroutine : ExtractLineNumber()
// Arguments : name = name of shotline
// Use : Extractrs the linenumber from the shotline name
// Returns : integer of shotline name
//***********************************************************************
int CSeismicData::ExtractLineNumber(CString &name)
{
	CString temp = "";

	for (int i = 0; i < name.GetLength(); i++)
	{
		if (isdigit(name[i])) temp += name[i];
	}

	if (temp.GetLength() == 0)
		return(-1);
	else
		return(atoi(temp));
}

//***********************************************************************
// Subroutine : GetDouble()
// Arguments : start : column to start reading string
// end : column to end reading string
// Use : read double value from inputbuffer
// Returns : doubleing point value
//***********************************************************************
double CSeismicData::GetDouble(char *inputbuffer, int start, int end)
{
	char *tempc = GetString(inputbuffer, start, end);
	double tempf = (double)atof(tempc);
	delete tempc;
	return(tempf);
}

//***********************************************************************
// Subroutine : KillWhite()
// Arguments : str : character string
// Use : returns character string without white spaces and
// all capitals
// Returns : char *
//***********************************************************************
char *CSeismicData::KillWhite(char *str)
{
	int i = 0, j = 0;
	do
	{
		if (!isspace(str[i]))
			str[j++] = toupper(str[i]);
	} while (str[i++]);
	return(str);
}

CString &CSeismicData::KillWhite(CString &str)
{
	int i = 0, j = 0;
	LPTSTR buffer = str.GetBuffer(0);
	do
	{
		if (!isspace(buffer[i]))
			buffer[j++] = toupper(buffer[i]);
	} while (buffer[i++]);
	return(str);
}

//***********************************************************************
// Subroutine : ExpandTabs()
// Arguments :
// Use : expands Tabs in inputbuffer into equivalent number of spaces
// all capitals
// Returns :
//***********************************************************************
void CSeismicData::ExpandTabs(char *inputbuffer)
{
	char *buf;
	int j, k, numblank;
	size_t i;
	// copy inputbuffer into different auxiliary buffer, if possible
	if (!(buf = new char[CSD_MAXLEN]))
		return;
	strcpy(buf, inputbuffer);
	for (j = 0, i = 0; i < strlen(buf); i++)
	{
		if (buf[i] != '\t')
			inputbuffer[j++] = buf[i];
		else
		{
			numblank = j % 8;
			if (numblank == 0) numblank = 8;
			for (k = 0; k < numblank; k++)
				inputbuffer[j++] = ' ';
		}
	}
	inputbuffer[j] = 0;
}

//***********************************************************************
// Parameters : timept : first point to compare
// cmpt : second point to compare
// radius : square of minimal radius
// Use : checks whether two points are within a certain radius
// Returns : TRUE if points are within this radius
//***********************************************************************
BOOL CSeismicData::TooClose(CShotPt *timept, CShotPt *cmpt, double radius)
{
	double x = timept->GetX() - cmpt->GetX();
	double y = timept->GetY() - cmpt->GetY();
	double dist = x * x + y * y;

	if (dist < radius)
		return(TRUE);
	else
		return(FALSE);
}

//***********************************************************************
// Parameters : x, y, radius (note radius should be squared)
// Use : checks whether x/y are within radius of existing points
// Returns : TRUE if points are within this radius
//***********************************************************************
BOOL CSeismicData::TooClose(double x, double y, double radius)
{
	double dist;
	CShotLine *shotline;
	CShotPt *timept;

	if (radius <= 0)
		return FALSE;

	for (int i = 0; i < GetSize(); i++)
	{
		shotline = (*this)[i];
		int shotpts = shotline->GetSize();

		for (int j = 0; j < shotpts; j++)
		{
			timept = (CShotPt *)shotline->GetAt(j);

			double xx = timept->GetX() - x;
			double yy = timept->GetY() - y;

			dist = xx * xx + yy * yy;
			if (dist < radius)
				return TRUE;
		}
	}

	return FALSE;
}

//***********************************************************************
// Subroutine : FillMatrix()
// linen : identification string of line
// sptn : integer identifiying shot point
// xn, yn : global coords of pt n
// Use : calculate matrix and shift for
// survey to global coords
// Returns : TRUE if successful conversion
//***********************************************************************
BOOL CSeismicData::FillMatrix(int line0, int sptid0, double x0, double y0,
	int line1, int sptid1, double x1, double y1,
	int line2, int sptid2, double x2, double y2)
{
	double **a, **b;
	if (line0 == 0 || line1 == 0 || line2 == 0 || sptid0 == 0 || sptid1 == 0 || sptid2 == 0)
	{
		pDoc->Error("You have to enter three complete pairs of coordinates!", MSG_ERR);
		return FALSE;
	}
	a = dmatrix(1, 6, 1, 6);
	b = dmatrix(1, 6, 1, 1);
	a[1][1] = sptid0; a[1][2] = line0; a[1][3] = 0.0; a[1][4] = 0.0; a[1][5] = 1.0; a[1][6] = 0.0;
	a[2][1] = 0.0; a[2][2] = 0.0; a[2][3] = sptid0; a[2][4] = line0; a[2][5] = 0.0; a[2][6] = 1.0;
	a[3][1] = sptid1; a[3][2] = line1; a[3][3] = 0.0; a[3][4] = 0.0; a[3][5] = 1.0; a[3][6] = 0.0;
	a[4][1] = 0.0; a[4][2] = 0.0; a[4][3] = sptid1; a[4][4] = line1; a[4][5] = 0.0; a[4][6] = 1.0;
	a[5][1] = sptid2; a[5][2] = line2; a[5][3] = 0.0; a[5][4] = 0.0; a[5][5] = 1.0; a[5][6] = 0.0;
	a[6][1] = 0.0; a[6][2] = 0.0; a[6][3] = sptid2; a[6][4] = line2; a[6][5] = 0.0; a[6][6] = 1.0;
	b[1][1] = x0;
	b[2][1] = y0;
	b[3][1] = x1;
	b[4][1] = y1;
	b[5][1] = x2;
	b[6][1] = y2;
	if (!gaussj(a, 6, b, 1))
	{
		pDoc->Error("Please pick 3 points that do not lie on a line.", MSG_ERR);
		return FALSE;
	}
	smat[0][0] = b[1][1];
	smat[0][1] = b[2][1];
	smat[1][0] = b[3][1];
	smat[1][1] = b[4][1];
	shift[0] = b[5][1];
	shift[1] = b[6][1];
	free_dmatrix(a, 1, 6, 1, 6);
	free_dmatrix(b, 1, 6, 1, 1);
	return TRUE;
}

//***********************************************************************
// Subroutine : ReadRow()
// Arguments : OnlyDummyRead: Do not add new data to seisdata, but
// just to dummyseisdata (it's just a test read)
// Use : Read Seismic Data from row filter to int data base
// Returns : void
//***********************************************************************
BOOL CSeismicData::ReadRow(BOOL OnlyDummyRead)
{
	int i = 0;
	// if we only do a test import, find out how many lines we want
	int MaxLinesNeeded = -1;
	if (OnlyDummyRead)
		MaxLinesNeeded = AfxGetApp()->GetProfileInt("RMS Import", "SampleImportLines", 25);
	double rmsRadius = (double)AfxGetApp()->GetProfileInt("ArithmeticParams", "RMSRadius", 150);
	rmsRadius *= rmsRadius;
	int tooCloseCount = 0;

	/* Check typeline for further information: 2nd character will contain type
	of datafile: 1=regular grid, x/y info in file 2=regular grid, x/y info
	input by user 3=irregular shot lines (which need not be linear!), x/y
	info comes from ZIMS file
	*/
	char inputbuffer[CSD_MAXLEN];
	char subtype = typeline[1];
	// say something useful in the "Import progress" dialog
	pDoc->m_pProgress->ChangeText2("Analysing filter");
	if (subtype<'1' || subtype>'3')
	{
		pDoc->Error("Error in filter (TYPELINE field): Wrong subtype given, must be 1, 2 or 3", MSG_ERR);
		return FALSE;
	}
	// remember filter type for a better "Test import" output
	if (OnlyDummyRead)
		dummyseisdata.Filtertype = subtype;
	/* Read further info from filterfile. Each "tag" is of the format
	from;to;xxxxx from, to=column numbers (zero based) of where in line tag
	is found, xxxx=actual tag (as many characters as necessary)
	if additional numbers are required, they come after the ;to;, as for the
	"new shotpoint" tag:
	from;to;x_from;x_to;y_from;y_to;xxxx
	*/
	// Tag to identify new shotline
	// from;to;linecodestart;linecodeend;linenostart;linenoend;TAG
	int newlinetagstart, newlinetagend;
	int linecodestart, linecodeend;
	int linenostart, linenoend;
	int percent, oldpercent = -5;
	char newlinetag[32];
	BOOL XShotPtFmt; // TRUE if shotline and shotpoint are given on the same input line
	filterfile.getline(inputbuffer, CSD_MAXLEN - 1);
	sscanf(inputbuffer, "%d;%d;%d;%d;%d;%d;%s", &newlinetagstart, &newlinetagend,
		&linecodestart, &linecodeend, &linenostart, &linenoend, newlinetag);
	XShotPtFmt = (newlinetagstart == -1);// that is the case if tag starts at column -1
	/* Tag to identify new shotpoint
	from;to;ptnostartcol;ptnoendcol;xstartcol;xendcol;ystartcol;yendcol;TAG
	If the file type is not 1, the values for x_from.. etc are arbitrary, but
	have to be there nevertheless. They should all be zero, hence
	from;to;nostart;noend;0;0;0;0;TAG
	*/
	/* If the shotline and shotpoint information are both given on the same input
	file line, this is a XShotPt line instead of a ShotPt line (see format descr.),
	and we have to use a different format. */
	int newpttagstart, newpttagend;
	int newptnostart, newptnoend;
	int newptxstart, newptxend;
	int newptystart, newptyend;
	char newpttag[32];
	filterfile.getline(inputbuffer, CSD_MAXLEN - 1);
	if (XShotPtFmt)
	{
		sscanf(inputbuffer, "%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%s", &newpttagstart, &newpttagend,
			&newptnostart, &newptnoend,
			&linecodestart, &linecodeend, &linenostart, &linenoend,
			&newptxstart, &newptxend, &newptystart, &newptyend, newpttag);
	}
	else
	{
		sscanf(inputbuffer, "%d;%d;%d;%d;%d;%d;%d;%d;%s", &newpttagstart, &newpttagend,
			&newptnostart, &newptnoend,
			&newptxstart, &newptxend, &newptystart, &newptyend, newpttag);
	}
	if (subtype == '1' && (newptxstart == -1 || newptystart == -1))
	{
		pDoc->Error("Error in filter (SHOTPT field): Where do I find coordinates?", MSG_ERR);
		return FALSE;
	}
	// Tag to identify new shotlist (at a given line and point)
	// a shotlist is a list of data values for a given shotpoint on a given line
	// this list of data contains velocities, times, and maybe more data that can be
	// ignored. num_before, num_between, num_after are used to skip data before, between,
	// or after the relevant two data items, and "order" tells us in which of the two
	// possible orders velocity and time are given.
	// from;to;liststart;number_of_values_before;num_between;number_after;order;type;TAG
	// the pattern can be repeated, e.g. if velocity, time, vel, time, vel, time,
	// the entries would be from;to;start;0;0;0;V;R;TAG
	// "2" stands for times to be taken *2
	// order: "T"=times first, "V"=velocity first
	// "liststart" is the column number at which all data begins
	int newlisttagstart, newlisttagend;
	int newlistdatastart;
	int valuesbefore, valuesbetween, valuesafter;
	char order, timetype = 'x';
	char newlisttag[32];
	double timefactor;
	filterfile.getline(inputbuffer, CSD_MAXLEN - 1);
	sscanf(inputbuffer, "%d;%d;%d;%d;%d;%d;%c;%c;%s", &newlisttagstart, &newlisttagend,
		&newlistdatastart, &valuesbefore, &valuesbetween, &valuesafter, &order, &timetype, newlisttag);
	switch (timetype)
	{
	case '1':
		timefactor = 2.0;
		break;
	default:
		timefactor = 1.0;
	}
	// Tags to identify lines that can be ignored (there can be several of those)
	// from;to;TAG (if from==-1, "to" and "TAG" are ignored, and there are no
	// lines that may be ignored in the data file)
	int numberignoretags;
	int *ignoretagstart, *ignoretagend;
	char **ignoretag;
	// find out how many ignorable line types there are
	filterfile.getline(inputbuffer, CSD_MAXLEN - 1);
	sscanf(inputbuffer, "%d", &numberignoretags);
	if (numberignoretags > 0)
	{
		ignoretagstart = new int[numberignoretags];
		ignoretagend = new int[numberignoretags];
		ignoretag = new char *[numberignoretags];
		for (i = 0; i < numberignoretags; i++)
		{
			ignoretag[i] = new char[32];
			filterfile.getline(inputbuffer, CSD_MAXLEN - 1);
			sscanf(inputbuffer, "%d;%d;%s", &ignoretagstart[i], &ignoretagend[i], ignoretag[i]);
		}
	}
	// now read the first lines from the file. If they are ignorable lines, ignore them. Do so
	// until file ends or the first real line is encountered.
	// the rest of the file is lines indicating new shotlines, lines indicating new
	// shot points, data lines and maybe ignorable lines.
	CString CurrentLine;
	int CurrentShotpt, CurrentLineNo;
	double vel, time, x, CurrentX, CurrentY;
	char *ptr, *ptr2, minibuf[CSD_MINIBUF];
	BOOL DidIgnore;
	// read file line by line
	datafile.getline(inputbuffer, CSD_MAXLEN - 1);
	while (!(datafile.fail() || datafile.eof()))
	{
		if (inputbuffer[0] == 0)
		{
			datafile.getline(inputbuffer, CSD_MAXLEN - 1);
			continue;
		}
		// can we ignore the line?
		DidIgnore = FALSE;
		for (i = 0; i < numberignoretags; i++)
		{
			if (IsThatLine(inputbuffer, ignoretagstart[i], ignoretagend[i], ignoretag[i]))
			{
				DidIgnore = TRUE;
				break;
			}
		}
		if (DidIgnore)
		{
			datafile.getline(inputbuffer, CSD_MAXLEN - 1);
			continue;
		}
		// is it the beginning of a new shotline? then find out its name
		if (IsThatLine(inputbuffer, newlinetagstart, newlinetagend, newlinetag))
		{
			memset(minibuf, 0, CSD_MINIBUF); // so that it is ASCIIZ in any case
			strncpy(minibuf, &inputbuffer[linecodestart - 1], linecodeend - linecodestart + 1);
			// sk126 KillWhite(minibuf); // we do not want whitespace in there
			CurrentLine = minibuf;
			// set current line number only in the case of 3d files w/o coords
			if (subtype == '2')
			{
				memset(minibuf, 0, CSD_MINIBUF); // so that it is ASCIIZ in any case
				strncpy(minibuf, &inputbuffer[linenostart - 1], linenoend - linenostart + 1);
				CurrentLineNo = atoi(minibuf);
			}
			else
				CurrentLineNo = -1;
			// also change percentage in the "Import progress" dialog
			pDoc->m_pProgress->ChangePercentage((datafile.tellg() * 90) / DataFileLength);
			datafile.getline(inputbuffer, CSD_MAXLEN - 1);
			continue;
		}
		// is it the beginning of a new shotpoint? then find out its number and pos.
		// it might be a ShotPt or an XShotPt line.
		if (IsThatLine(inputbuffer, newpttagstart, newpttagend, newpttag))
		{
			memset(minibuf, 0, CSD_MINIBUF); // so that it is ASCIIZ in any case
			strncpy(minibuf, &inputbuffer[newptnostart - 1], newptnoend - newptnostart + 1);
			CurrentShotpt = atoi(minibuf);
			// if this is an XShotPt line, the shotline name is also given in this line
			if (XShotPtFmt)
			{
				memset(minibuf, 0, CSD_MINIBUF); // so that it is ASCIIZ in any case
				strncpy(minibuf, &inputbuffer[linecodestart - 1], linecodeend - linecodestart + 1);
				CurrentLine = minibuf;
				// set current line number only in the case of 3d files w/o coords
				if (subtype == '2')
				{
					memset(minibuf, 0, CSD_MINIBUF); // so that it is ASCIIZ in any case
					strncpy(minibuf, &inputbuffer[linenostart - 1], linenoend - linenostart + 1);
					CurrentLineNo = atoi(minibuf);
				}
				else
					CurrentLineNo = -1;
			}
			switch (subtype)
			{
			case '1': // coords are in file
				memset(minibuf, 0, CSD_MINIBUF); // so that it is ASCIIZ in any case
				strncpy(minibuf, &inputbuffer[newptxstart - 1], newptxend - newptxstart + 1);
				CurrentX = (double)atof(minibuf);
				memset(minibuf, 0, CSD_MINIBUF); // so that it is ASCIIZ in any case
				strncpy(minibuf, &inputbuffer[newptystart - 1], newptyend - newptystart + 1);
				CurrentY = (double)atof(minibuf);
				break;
			case '2': // 3d-file, coords are calculated
			case '3': // 2d-file, coords come from elsewhere
				CurrentX = CSD_DONTKNOW;
				CurrentY = CSD_DONTKNOW;
				break;
			}
			// also change percentage in the "Import progress" dialog
			percent = (datafile.tellg() * 90) / DataFileLength;
			if (percent >= oldpercent + 1)
			{
				pDoc->m_pProgress->ChangePercentage(percent);
				oldpercent = percent;
			}
			datafile.getline(inputbuffer, CSD_MAXLEN - 1);
			continue;
		}
		// otherwise it had better be a data line -- but we'll ignore others anyway
		if (IsThatLine(inputbuffer, newlisttagstart, newlisttagend, newlisttag))
		{
			// OK - read doubles from this line and try to fit them in. Endless loop
			// that can be ended by "break"
			ptr = &inputbuffer[newlistdatastart - 1]; // beginning of data
			do
			{
				// skip all data BEFORE relevant velocity data.
				for (i = 0; i < valuesbefore; i++)
					strtod(ptr, &ptr);
				// read value itself. Store it if read was successful
				vel = (double)strtod(ptr, &ptr2);
				if (ptr == ptr2)
					break; // this line has been done
				ptr = ptr2;
				// read ignorable values between the two
				for (i = 0; i < valuesbetween; i++)
					strtod(ptr, &ptr);
				// read second relevant value
				time = (double)strtod(ptr, &ptr2);
				if (ptr == ptr2)
					break; // this line has been done
				ptr = ptr2;
				for (i = 0; i < valuesafter; i++)
					strtod(ptr, &ptr);
				// maybe we read the two the wrong way round, and time came first!
				if (order == 'T')
				{
					x = vel;
					vel = time;
					time = x;
				}
				vel *= VelFactor;

				//finally add the points if not too close to any existing points				
				if (OnlyDummyRead)
				{
					dummyseisdata.AddPt(CurrentLine, CurrentShotpt, vel, time*timefactor, CurrentX, CurrentY);

					if (dummyseisdata.GetSize() > MaxLinesNeeded)
						return FALSE;
				}
				else
				{
					AddNewPt(CurrentLine, CurrentLineNo, CurrentShotpt, CSD_TYPE_RMSVEL, CurrentX, CurrentY, vel, rmsRadius);
					AddNewPt(CurrentLine, CurrentLineNo, CurrentShotpt, CSD_TYPE_TIME, CurrentX, CurrentY, time*timefactor, rmsRadius);
				}

			} while (TRUE);

			datafile.getline(inputbuffer, CSD_MAXLEN - 1);
			continue; // process next line
		}
		datafile.getline(inputbuffer, CSD_MAXLEN - 1);
	}
	if (numberignoretags > 0)
	{
		delete ignoretagstart;
		delete ignoretagend;
		for (i = 0; i < numberignoretags; i++)
			delete ignoretag[i];
		delete ignoretag;
	}
	if (OnlyDummyRead)
		return FALSE;// always FALSE if we just fake it, so we do not check for
	// sec or msec afterwards etc.
	else
		return TRUE;
}

//***********************************************************************
// Subroutine : ReadColumn()
// Arguments : OnlyDummyRead: Do not add new data to seisdata, but
// just to dummyseisdata (it's just a test read)
// Use : Read Seismic Data from column filter to int data base
// Returns : void
//***********************************************************************
BOOL CSeismicData::ReadColumn(BOOL OnlyDummyRead)
{
	int i = 0;
	BOOL DidIgnore;
	// if we only do a test import, find out how many lines we want
	int MaxLinesNeeded = -1;
	if (OnlyDummyRead)
		MaxLinesNeeded = AfxGetApp()->GetProfileInt("RMS Import", "SampleImportLines", 25);
	double rmsRadius = (double)AfxGetApp()->GetProfileInt("ArithmeticParams", "RMSRadius", 150);
	rmsRadius *= rmsRadius;

	char inputbuffer[CSD_MAXLEN];
	int percent, oldpercent = -5;
	// say something useful in the "Import progress" dialog
	pDoc->m_pProgress->ChangeText2("Analysing filter");
	char subtype = typeline[1];
	if (subtype<'1' || subtype>'3')
	{
		pDoc->Error("Error in filter (TYPELINE field): "
			"Wrong subtype given, must be 1, 2 or 3", MSG_ERR);
		return FALSE;
	}
	// remember filter type for a better "Test import" output
	if (OnlyDummyRead)
		dummyseisdata.Filtertype = subtype;
	// Expand Tabs into spaces?
	BOOL DoTabs = (typeline[2] == 'T' || typeline[3] == 'T');
	// read further filter information in next few lines
	// line on where to find what information
	int linestart, lineend, linenostart, linenoend, ptstart, ptend, velstart, velend;
	char minibuf[CSD_MINIBUF];
	CString CurrentLine = "";
	int CurrentShotpt = CSD_IMPOSSIBLE_SHOTPOINT, CurrentLineNo;
	double vel, time, CurrentX = CSD_DONTKNOW, CurrentY = CSD_DONTKNOW;
	int timestart, timeend;
	char timetype = 'x';
	double timefactor;
	filterfile.getline(inputbuffer, CSD_MAXLEN - 1);
	sscanf(inputbuffer, "%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%c", &linestart, &lineend, &linenostart,
		&linenoend, &ptstart, &ptend, &velstart, &velend, &timestart, &timeend, &timetype);
	switch (timetype)
	{
	case '1':
		timefactor = 2.0;
		break;
	default:
		timefactor = 1.0;
	}
	// line on where to find x/y coordinates (only for type 1)
	int xstart, xend, ystart, yend;
	filterfile.getline(inputbuffer, CSD_MAXLEN - 1);
	if (typeline[1] == '1')
		sscanf(inputbuffer, "%d;%d;%d;%d", &xstart, &xend, &ystart, &yend);
	// lines on how ignorable data lines look like
	int numberignoretags;
	int *ignoretagstart, *ignoretagend;
	char **ignoretag;
	// find out how many ignorable line types there are
	filterfile.getline(inputbuffer, CSD_MAXLEN - 1);
	sscanf(inputbuffer, "%d", &numberignoretags);
	if (numberignoretags > 0)
	{
		ignoretagstart = new int[numberignoretags];
		ignoretagend = new int[numberignoretags];
		ignoretag = new char *[numberignoretags];
		for (i = 0; i < numberignoretags; i++)
		{
			ignoretag[i] = new char[32];
			filterfile.getline(inputbuffer, CSD_MAXLEN - 1);
			sscanf(inputbuffer, "%d;%d;%s", &ignoretagstart[i], &ignoretagend[i], ignoretag[i]);
		}
	}
	if (subtype == '2')// 3d-file w/o coord info in it: get coord system later
	{
		// Each shotline has various x-values on various shotpoints, but same y-value.
		// x and y start at (0, 0). Each line and each point increment y or x by 1.0
		CurrentX = CSD_DONTKNOW;
		CurrentY = CSD_DONTKNOW;
	} //sk126 XY position are set to unknown as they will later be completed by
	// Convert2GLobal()
	while (!(datafile.fail() || datafile.eof()))
	{
		datafile.getline(inputbuffer, CSD_MAXLEN - 1);
		if (inputbuffer[0] == 0)
			continue; // empty line
		if (DoTabs) ExpandTabs(inputbuffer);
		// can we ignore the line?
		DidIgnore = FALSE;
		for (i = 0; i < numberignoretags; i++)
		{
			if (IsThatLine(inputbuffer, ignoretagstart[i], ignoretagend[i], ignoretag[i]))
			{
				DidIgnore = TRUE;
				break;
			}
		}
		if (DidIgnore)
			continue;
		// otherwise extract information
		// linecode
		memset(minibuf, 0, CSD_MINIBUF); // so that it is ASCIIZ in any case
		strncpy(minibuf, &inputbuffer[linestart - 1], lineend - linestart + 1);
		// sk126 KillWhite(minibuf);

		CurrentLine = minibuf;
		// set current line number only in the case of 3d files w/o coords
		if (subtype == '2')
		{
			memset(minibuf, 0, CSD_MINIBUF); // so that it is ASCIIZ in any case
			strncpy(minibuf, &inputbuffer[linenostart - 1], linenoend - linenostart + 1);
			CurrentLineNo = atoi(minibuf);
		}
		else
			CurrentLineNo = -1;

		// shotpoint number
		memset(minibuf, 0, CSD_MINIBUF); // so that it is ASCIIZ in any case
		strncpy(minibuf, &inputbuffer[ptstart - 1], ptend - ptstart + 1);
		// is this another shotpoint number than the last one? if we make our own
		// coordinates, this means we have to increment x

		CurrentShotpt = atoi(minibuf);
		// velocity
		memset(minibuf, 0, CSD_MINIBUF); // so that it is ASCIIZ in any case
		strncpy(minibuf, &inputbuffer[velstart - 1], velend - velstart + 1);
		vel = (double)atof(minibuf);
		vel *= VelFactor;
		// time
		memset(minibuf, 0, CSD_MINIBUF); // so that it is ASCIIZ in any case
		strncpy(minibuf, &inputbuffer[timestart - 1], timeend - timestart + 1);
		time = (double)atof(minibuf)*timefactor;
		// maybe even coordinates
		if (subtype == '1')
		{
			memset(minibuf, 0, CSD_MINIBUF); // so that it is ASCIIZ in any case
			strncpy(minibuf, &inputbuffer[xstart - 1], xend - xstart + 1);
			CurrentX = (double)atof(minibuf);
			memset(minibuf, 0, CSD_MINIBUF); // so that it is ASCIIZ in any case
			strncpy(minibuf, &inputbuffer[ystart - 1], yend - ystart + 1);
			CurrentY = (double)atof(minibuf);
		}

		//finally add the points if not too close to any existing points		
		if (OnlyDummyRead)
		{
			dummyseisdata.AddPt(CurrentLine, CurrentShotpt, vel, time, CurrentX, CurrentY);

			if (dummyseisdata.GetSize() > MaxLinesNeeded)
				return FALSE;
		}
		else
		{
			AddNewPt(CurrentLine, CurrentLineNo, CurrentShotpt, CSD_TYPE_RMSVEL, CurrentX, CurrentY, vel, rmsRadius);
			AddNewPt(CurrentLine, CurrentLineNo, CurrentShotpt, CSD_TYPE_TIME, CurrentX, CurrentY, time, rmsRadius);
		}


		// also change percentage in the "Import progress" dialog
		percent = (datafile.tellg() * 100) / DataFileLength;
		if (percent >= oldpercent + 1)
		{
			pDoc->m_pProgress->ChangePercentage(percent);
			oldpercent = percent;
		}
	}
	if (numberignoretags > 0)
	{
		delete ignoretagstart;
		delete ignoretagend;
		for (i = 0; i < numberignoretags; i++)
			delete ignoretag[i];
		delete ignoretag;
	}
	if (OnlyDummyRead)
		return FALSE;// always FALSE if we just fake it, so we do not check for
	// sec or msec afterwards etc.
	else
		return TRUE;
}

//***********************************************************************
// Subroutine : IsThatLine()
// Arguments : tagstart, tagend: position of tag in a
// line read into inputbuffer. tag=the actual tag.
// Use : Checks whether line bears the given tag
// Returns : TRUE if that is the case. If tagstart==-1, there IS
// no such tag defined, and FALSE is returned
// Note : In filter descriptions, positions are 1-based!
//***********************************************************************
BOOL CSeismicData::IsThatLine(char *inputbuffer, int tagstart, int tagend, char *tag)
{
	if (tagstart == -1) return FALSE; // no tag defined
	tagstart--;
	tagend--; // because they are given 1-based, not 0-based
	return (strncmp(&inputbuffer[tagstart], tag, tagend - tagstart + 1) == 0);
}

BOOL CSeismicData::AddNewPt(CString &linename, int lineid, int sptid, UBYTE type, double x, double y, double value, double minRadius)
{
	CShotLine *line;
	CShotPt *spoint;
	CDataPt *dpoint;
	int i = 0;
	char buffer[128];

	// First filter line...
	if (linename != "") KillWhite(linename);

	// if lineid doesn exist, extract it.
	if (lineid == -1)
	{
		lineid = ExtractLineNumber(linename);
	}
	// otherwise - if it exists and there is no number in the line
	// then add it to the linenumber
	else if (ExtractLineNumber(linename) == -1)
	{
		sprintf(buffer, "%d", lineid);
		linename += buffer; // add lineid to the linename
	}

	// Try both shotline name and then shotline number to find the correct point
	if (!((line = FindLine(linename)) != NULL || (line = FindLine(lineid)) != NULL))
	{
		if (!(line = new CShotLine(linename, lineid)))
		{
			pDoc->Error("Could not allocate new ShotLine", MSG_ERR);
			return FALSE;
		}

		Add(line);
	}

	// try to find shotpoint - if nonexistent, create it
	if (!(spoint = line->FindSPoint(sptid, type)))
	{
		if (!(spoint = new CShotPt(sptid, type)))
		{
			pDoc->Error("Could not allocate new ShotPoint", MSG_ERR);
			return FALSE;
		}

		// check its not going to be too close to any exising
		if (minRadius > 0)
		{
			double dist;
			CShotLine *shotline;
			CShotPt *timept;

			for (int i = 0; i < GetSize(); i++)
			{
				shotline = (*this)[i];
				int shotpts = shotline->GetSize();

				for (int j = 0; j < shotpts; j++)
				{
					timept = (CShotPt *)shotline->GetAt(j);

					// only compare against same type
					if (type != timept->GetType())
						continue;

					double xx = timept->GetX() - x;
					double yy = timept->GetY() - y;

					dist = xx * xx + yy * yy;
					if (dist < minRadius)
						return FALSE; // its too close
				}
			}
		}

		i = 0;
		// Find place where new shotpoint will fit into the array
		while (i < line->GetSize() && (*line)[i]->GetID() < sptid) i++;
		// Add shotpoint ontp the end of into the correct space
		line->InsertAt(i, spoint, 1);
	}

	// Put X, Y information into the shotpoint
	spoint->PutX(x);
	spoint->PutY(y);

	// No use of putting in data value as it is not known
	if (value == CSD_DONTKNOW)
		return TRUE;

	// Add new data point
	if (!(dpoint = new CDataPt(value)))
	{
		pDoc->Error("Could not allocate new DataPoint", MSG_ERR);
		return FALSE;
	}

	spoint->Add(dpoint);
	return TRUE;
}

//***********************************************************************
// Arguments : linename : CString containing ShotLineName
// shotpt : number of ShotPoint
// type : type of shot point
// x, y : position of shotpoint
// value : data item to be stored
// Use : Put a measurement point into the internal data base
// Returns : void
//***********************************************************************
BOOL CSeismicData::AddNewPt(CString &linename, int lineid, int sptid, UBYTE type, double x, double y, double value)
{
	return AddNewPt(linename, lineid, sptid, type, x, y, value, -1);
}

//***********************************************************************
// Subroutine : FindLine()
// Arguments : lineid : integer containing ShotLineID
// Use : Find a line by id in the internal database
// Returns : pointer to CShotLine or NULL
//***********************************************************************
CShotLine *CSeismicData::FindLine(int lineid)
{
	CShotLine *line;
	int i = 0;

	// cannot find with illegal lineid
	if (lineid == -1) return NULL;

	while (i < GetSize())
	{
		line = (CShotLine *)GetAt(i);
		if ((*line) == lineid)
			return line;
		i++;
	}
	return NULL;
}

//***********************************************************************
// Subroutine : FindLine()
// Arguments : linename : CString containing ShotLineName
// Use : Find a line by name in the int. database
// Returns : pointer to CShotLine or NULL
//***********************************************************************
CShotLine *CSeismicData::FindLine(CString &linename)
{
	CShotLine *line;
	int i = 0;

	// cannot find empty linename
	if (linename == "") return NULL;

	while (i < GetSize())
	{
		line = (CShotLine *)GetAt(i);
		if (line->GetName() == linename)
			return line;
		i++;
	}
	return NULL;
}

//***********************************************************************
// Subroutine : GetZimsLine()
// Arguments : file : reference to ASCII input file
// Use : Reads next relevant Zims line into memory
// Returns : TRUE if next line was a data line
// FALSE if next line was a comment
//***********************************************************************
BOOL CSeismicData::GetZimsLine(char *inputbuffer, ifstream &file)
{
	BOOL ret = TRUE;
	file.getline(inputbuffer, CSD_MAXLEN - 1);
	while (inputbuffer[0] == '!')
	{
		ret = FALSE;
		file.getline(inputbuffer, CSD_MAXLEN - 1);
	}
	return ret;
}

//***********************************************************************
// Subroutine : Convert2GlobalCoordinates()
// Arguments : none
// Use : Transform the internal database into global coords
// Returns : none
//***********************************************************************
void CSeismicData::Convert2GlobalCoordinates(void)
{
	CShotLine *sline;
	CShotPt *spoint;
	double x, y;
	int spid, slid, i, j;
	for (i = 0; i < GetSize(); i++)
	{
		sline = (*this)[i];
		slid = (int)(*sline);
		for (j = 0; j < sline->GetSize(); j++)
		{
			spoint = (*sline)[j];
			spid = (int)(*spoint);
			x = smat[0][0] * spid + smat[0][1] * slid + shift[0];
			y = smat[1][0] * spid + smat[1][1] * slid + shift[1];
			spoint->PutX(x);
			spoint->PutY(y);
		}
	}
}

//***********************************************************************
// Subroutine : Init()
// Arguments : document : pointer to current CDocument
// illval : external illegal value
// Use : Allocates memory and initializes CSeismicData
// Returns : none
//***********************************************************************
void CSeismicData::Init(CVelmanDoc *document, double illval)
{
	pDoc = document;
	illegal = illval;
	ClearContents();
	// Allocate memory for input line
	HaveInit = TRUE;
}

//***********************************************************************
// Subroutine : ClearContents()
// Arguments : none
// Use : deletes all contents of the seismicdata object
// Returns : none
//***********************************************************************
void CSeismicData::ClearContents(UBYTE type)
{
	int i, j;
	CShotPt *shotpt;
	CShotLine *shotline;

	if (type == CSD_TYPE_ANY)
	{
		i = 0;
		while (i < GetSize())
		{
			delete (CShotLine *)GetAt(i++);
		}
		RemoveAll();
	}
	else
	{
		i = 0;
		while (i < GetSize())
		{
			shotline = (CShotLine *)GetAt(i);
			j = 0;
			while (j < shotline->GetSize())
			{
				shotpt = (CShotPt *)(*shotline)[j];
				if (shotpt->GetType() & type)
				{
					shotline->RemoveAt(j);
					delete shotpt;
				}
				j++;
			}
			i++;
		}
	}
}

//***********************************************************************
// Subroutine : ClearMostContents()
// Arguments : none
// Use : deletes all contents of the seismicdata object, apart
// from the (then empty) shotpoints themselvers. Then one
// can still draw a seismic map, but not compute anything
// Returns : none
//***********************************************************************
void CSeismicData::ClearMostContents(void)
{
	int i, j, k;
	CShotPt *spt;
	CShotLine *sline;

	for (i = 0; i < GetSize(); i++)
	{
		sline = (CShotLine *)GetAt(i);
		for (j = 0; j < sline->GetSize(); j++)
		{
			spt = (CShotPt *)sline->GetAt(j);
			for (k = 0; k < spt->GetSize(); k++)
				delete (CDataPt *)spt->GetAt(k);
			spt->RemoveAll();
		}
	}
}

//***********************************************************************
// Subroutine : Read()
// Arguments : dataname : name of main data file
// filtername : name of filter definition file
// filter : name of filter to be used for this file
// suppname : name of supplementary data file
// OnlyDummyRead: Do not add new data to seisdata, but
// just to dummyseisdata (it's just a test read)
// Use : Read Seismic Data into internal data base
// Returns : void
//***********************************************************************
BOOL CSeismicData::Read(CString dataname, CString filtername, CString filter, CString suppname, BOOL OnlyDummyRead)
{
	int lines, retval;
	char type;
	char inputbuffer[CSD_MAXLEN];
	// if we only perform a "dummy" type read, remove the stuff we might have
	// read so far
	dummyseisdata.CleanUp();
	// open main datafile
	datafile.open(dataname);
	if (datafile.fail())
	{
		pDoc->Error("Could not open seismic data file " + dataname, MSG_ERR);
		return FALSE;
	}
	pDoc->Log("Opened seismic data file " + dataname, LOG_STD);
	// find out length of data file for percentage output
	// (note that percentage output for
	datafile.seekg(0L, ios::end);
	DataFileLength = datafile.tellg();
	datafile.seekg(0L, ios::beg);
	// open filterfile
	filterfile.open(filtername);
	if (filterfile.fail())
	{
		pDoc->Error("Could not open filter definition file " + filtername, MSG_ERR);
		datafile.close();
		datafile.clear();
		return FALSE;
	}
	pDoc->Log("Opened filter definition file " + filtername, LOG_DETAIL);

	// Any filter at all?
	if (!filter.IsEmpty())
	{
		while (!filterfile.eof())
		{
			filterfile.getline(inputbuffer, CSD_MAXLEN - 1);
			// check if filter is in file and where it is
			if (inputbuffer == filter)
			{
				pDoc->Log("Using RMS file filter " + filter, LOG_STD);
				// Get length and type
				memset(typeline, 0, CSD_TYPELINELENGTH);
				filterfile.getline(typeline, CSD_TYPELINELENGTH - 1);
				type = toupper(typeline[0]);
				filterfile.getline(inputbuffer, CSD_MAXLEN - 1);
				lines = atoi(inputbuffer);
				// set unit flag
				BOOL VelInFeet = (pDoc->Units & UNIT_FEET_RMSVELOCITY) ? TRUE : FALSE;
				VelFactor = 1.0;
				if (pDoc->Units & UNIT_OUTPUTINFEET)
				{
					if (!VelInFeet) VelFactor = 1.0 / METERS_PER_FOOT;
				}
				else
				{
					if (VelInFeet) VelFactor = METERS_PER_FOOT;
				}
				if (VelInFeet)
					pDoc->Log("RMS file read in feet/s", LOG_STD);
				else
					pDoc->Log("RMS file read in m/s", LOG_STD);
				//execute corresponding reading routine
				switch (type)
				{
				case 'R':
					retval = ReadRow(OnlyDummyRead);
					datafile.close();
					datafile.clear();
					filterfile.close();
					filterfile.clear();
					if (retval)
						goto CheckForMilliSecs;
					else
						return FALSE;
				case 'C':
					retval = ReadColumn(OnlyDummyRead);
					datafile.close();
					datafile.clear();
					filterfile.close();
					filterfile.clear();
					if (retval)
						goto CheckForMilliSecs;
					else
						return FALSE;
				default:
					pDoc->Error("Error in filter (TYPELINE field): Wrong basic filter format.", MSG_ERR);
					datafile.close();
					datafile.clear();
					filterfile.close();
					filterfile.clear();
					return FALSE;
				}
			}
		}
		pDoc->Error("Could not find specified filter", MSG_ERR);
		return FALSE;
	}
	else
	{
		pDoc->Error("No 'standard' filters defined - internal error?", MSG_ERR);
	}
	// check whether times are in secs or millisecs. Only check the first shotpoint
	// on the first shotline, or rather start there and go for the first point
	// that actually contains a list of times and velocities
CheckForMilliSecs: // jumps here after successful import
	CShotLine * line;
	int CheckingLineNo = 0;
	BOOL FoundAValidPoint = FALSE;
	CShotPt *pt;
	CDataPt *data;
	double time, maxtime = -1.0;
	int ptno, i, j, k;
	int MinSecs = AfxGetApp()->GetProfileInt("RMS Import", "MSecThreshold", 100);
	// find first shotpoint data list of type "TIME" on first shotline
	while (!FoundAValidPoint && CheckingLineNo < GetSize())
	{
		line = (CShotLine *)GetAt(CheckingLineNo);
		if (line != NULL)
		{
			ptno = 0;
			while (ptno < line->GetSize() && (pt = (CShotPt *)line->GetAt(ptno++)) != NULL)
			{
				// is it a TIME point with some data in it?
				if (pt->GetType() == CSD_TYPE_TIME && pt->GetSize() > 0)
				{
					for (j = 0; j < pt->GetSize(); j++)
					{
						data = (CDataPt *)(pt->GetAt(j));
						maxtime = max(maxtime, (double)*data);
					}
					FoundAValidPoint = TRUE;
					break; // leave the entire set of loops
				}
			}
		}
		CheckingLineNo++;
	}
	// now draw the conclusion if we found any!
	if (FoundAValidPoint)
	{
		if (maxtime < (double)MinSecs)
		{
			pDoc->Log("RMS time values assumed in seconds, transforming to milliseconds", LOG_BRIEF);

			for (i = 0; i < GetSize(); i++)
			{
				line = (CShotLine *)GetAt(i);
				for (j = 0; j < line->GetSize(); j++)
				{
					pt = (CShotPt *)line->GetAt(j);
					if (pt->GetType() == CSD_TYPE_TIME)
					{
						for (k = 0; k < pt->GetSize(); k++)
						{
							data = (CDataPt *)pt->GetAt(k);
							time = (double)*data;
							*data = time * 1000.0;
						}
					}
				}
			}
		}
		else
			pDoc->Log("RMS time values assumed in milliseconds", LOG_DETAIL);
	}
	else // warning if marker was not set
	{
		pDoc->Error("Could not determine whether times in RMS file are given"
			" in seconds or milliseconds.\n\nAssuming milliseconds.", MSG_WARN);
	}

	return TRUE;
}

//***********************************************************************
// Subroutine : ExistZimsFile()
// Arguments : filename : zims name to look in database for
// type : type of file to check for
// Use : checks for zimsfile in database
// Returns : success / failure
//***********************************************************************
BOOL CSeismicData::ExistZimsFile(CString &filename, int type)
{
	CStringArray zimsname;
	char buf[256];

	pDoc->FillListOfDOSnames();
	MakeZimsNameList(zimsname, pDoc->ListOfDOSnames, type);

	for (int i = 0; i < zimsname.GetSize(); i++)
	{
		strcpy(buf, zimsname[i]);
		if (pDoc->GridFileType == 1)
			UnPad(buf, strlen(buf));
		if (!strcmp(buf, (const char *)filename))
			return TRUE;
	}

	return FALSE;
}

//***********************************************************************
// Subroutine : DeleteZimsFile(), wrapper
// Arguments : gridname=the grid to delete
// Use : Deletes ZimsFile in any format
// Returns : success / failure
//***********************************************************************
BOOL CSeismicData::DeleteZimsFile(CString gridname)
{
	CWaitCursor wait;
	int index, hnum;
	pDoc->FillListOfDOSnames();
	if (!(pDoc->seismicdata.MakeDOSName(gridname, pDoc->ListOfDOSnames, index, hnum)))
		return FALSE;
	switch (pDoc->GridFileType)
	{
	case 1:
		return DeleteZimsFile_ZMAP(pDoc->ListOfDOSnames, index, hnum);
	case 2:
		return DeleteZimsFile_MFD(pDoc->ListOfDOSnames, index, hnum);
	case 3:
		return DeleteZimsFile_CP3(pDoc->ListOfDOSnames, index, hnum);
	case 4:
		return DeleteZimsFile_CP3int(pDoc->ListOfDOSnames, index, hnum);
	case 5:
		return DeleteZimsFile_EV(pDoc->ListOfDOSnames, index);
	case 6:
		return DeleteZimsFile_ZmapISO(pDoc->ListOfDOSnames, index);
	default:
		pDoc->Error("No valid grid file type", MSG_ERR);
		return FALSE;
	}
}

// DeleteZimsFile_EV : deletes EarthVision file
BOOL CSeismicData::DeleteZimsFile_EV(CStringArray &filename, int index)
{
	_unlink((const char *)pDoc->ZimsDir + *(filename[index]));
	pDoc->FillListOfDOSnames();
	return TRUE;
}

// Deletes ZMAP ISO format files
BOOL CSeismicData::DeleteZimsFile_ZmapISO(CStringArray &filename, int index)
{
	_unlink((const char *)pDoc->ZimsDir + *(filename[index]));
	pDoc->FillListOfDOSnames();
	return TRUE;
}

//***********************************************************************
// Subroutine : DeleteZimsFile(), ZMAP version
// Arguments : dosname : array of all files containing grid data
// hnum : record number to be deleted
// index : index to filename in dosname array
// Use : Deletes ZimsFile in any format
// Returns : success / failure
//***********************************************************************
BOOL CSeismicData::DeleteZimsFile_ZMAP(CStringArray &dosname, int index, int hnum)
{
	char buf[256];
	BOOL success;
	// copy filename into buffer
	strcpy(buf, (const char *)(pDoc->ZimsDir + (dosname[index])));
	success = (_unlink(buf) == 0);
	// make grid data name out of grid header file name
	buf[strlen(buf) - 1] = 'd';
	success &= (_unlink(buf) == 0);
	pDoc->FillListOfDOSnames();
	return success;
}

//***********************************************************************
// Subroutine : DeleteZimsFile(), MFD version
// Arguments : dosname : array of all files containing grid data
// hnum : record number to be deleted
// index : index to filename in dosname array
// Use : Deletes ZimsFile in any format
// Returns : success / failure
//***********************************************************************
BOOL CSeismicData::DeleteZimsFile_MFD(CStringArray &dosname, int index, int hnum)
{
	meta_file_head metahead;
	file_head filehead;
	fstream mfdfile;
	int files, i;
	int meta_pos[128], meta_files[128];
#ifdef VELMAN_UNIX
	mfdfile.open(pDoc->ZimsDir + *(dosname[index]), ios::in);
#else
	mfdfile.open(pDoc->ZimsDir + *(dosname[index]), ios::binary);
#endif
	// Get the positions and numbers of files for all headers
	i = 0;
	meta_pos[0] = 0;
	do
	{
		mfdfile.seekg(meta_pos[i] * ZIMS_BLOCK, ios::beg);
		mfdfile.read((char *)&metahead, sizeof(meta_file_head));
#ifndef VELMAN_UNIX
		InvertData((char *)&metahead, meta_file_head_sec);
#endif
		// check that file is in correct format
		if (metahead.four != 4)
			break;
		meta_files[i] = metahead.filenum;
		i++;
		meta_pos[i] = metahead.meta_next - 1;
	} while (meta_pos[i] != -1);
	// Find which one of the headers contains this datafile
	i = -1;
	do
	{
		i++;
		hnum -= meta_files[i];
	} while (hnum > 0);
	hnum += meta_files[i];
	// Find this metablock and subtract one from the filenum
	mfdfile.seekg(meta_pos[i] * ZIMS_BLOCK, ios::beg);
	mfdfile.read((char *)&metahead, sizeof(meta_file_head));
#ifndef VELMAN_UNIX
	InvertData((char *)&metahead, meta_file_head_sec);
#endif
	// check that file is in correct format
	if (metahead.four != 4)
		return TRUE;
	metahead.filenum--;
	files = metahead.filenum;
	// No more data remains...
	if (metahead.filenum == 0 && i == 0 && meta_pos[1] == -1)
	{
		_unlink((const char *)pDoc->ZimsDir + *(dosname[index]));
		pDoc->FillListOfDOSnames();
		return TRUE;
	}
	// go back to meta_file_header and write it
	mfdfile.seekg(-(int) sizeof(meta_file_head), ios::cur);
#ifndef VELMAN_UNIX
	InvertData((char *)&metahead, meta_file_head_sec);
#endif
	mfdfile.write((char *)&metahead, sizeof(meta_file_head));
	// goto next file_head block
	mfdfile.seekg(hnum * sizeof(file_head), ios::cur);
	// bring all following headers up by one
	for (i = files - hnum; i > 0; i--)
	{
		mfdfile.seekg(sizeof(file_head), ios::cur);
		mfdfile.read((char *)&filehead, sizeof(file_head));
		mfdfile.seekg(-2 * ((int) sizeof(file_head)), ios::cur);
		mfdfile.write((const char *)&filehead, sizeof(file_head));
	}
	return TRUE;
}

//***********************************************************************
// Subroutine : DeleteZimsFile(), CP3 version
// Arguments : dosname : array of all files containing grid data
// hnum : record number to be deleted
// index : index to filename in dosname array
// Use : Deletes ZimsFile in any format
// Returns : success / failure
//***********************************************************************
BOOL CSeismicData::DeleteZimsFile_CP3(CStringArray &dosname, int index, int hnum)
{
	_unlink((const char *)pDoc->ZimsDir + *(dosname[index]));
	pDoc->FillListOfDOSnames();
	return TRUE;
}

//***********************************************************************
// Subroutine : DeleteZimsFile(), CP3 version, internal version
// Arguments : dosname : array of all files containing grid data
// hnum : record number to be deleted
// index : index to filename in dosname array
// Use : Deletes ZimsFile in any format
// Returns : success / failure
//***********************************************************************
BOOL CSeismicData::DeleteZimsFile_CP3int(CStringArray &dosname, int index, int hnum)
{
	_unlink((const char *)pDoc->ZimsDir + *(dosname[index]));
	pDoc->FillListOfDOSnames();
	return TRUE;
}

//***********************************************************************
// Subroutine : ReadZimsFile(), wrapper
// Arguments : filename : full name of file to be loaded
// hnum : record number to be read
// Use : Read Seismic Data into internal data base from a
// Zims datafile
// Returns : success / failure
//***********************************************************************
BOOL CSeismicData::ReadZimsFile(CString suppname, int hnum, UBYTE dtype, int horizon)
{
	CWaitCursor wait;
	switch (pDoc->GridFileType)
	{
	case 1:
		return ReadZimsFile_ZMAP(suppname, hnum, dtype, horizon);
	case 2:
		return ReadZimsFile_MFD(suppname, hnum, dtype, horizon);
	case 3:
		return ReadZimsFile_CP3(suppname, hnum, dtype, horizon);
	case 4:
		return ReadZimsFile_CP3int(suppname, hnum, dtype, horizon);
	case 5:
		return ReadZimsFile_EV(suppname, hnum, dtype, horizon); // added 01/04
	case 6:
		return ReadZimsFile_ZmapISO(suppname, hnum, dtype, horizon); // added 03/04
	default:
		pDoc->Error("No valid grid file type", MSG_ERR);
		return FALSE;
	}
}

//***********************************************************************
// Subroutine : ReadZimsFileHeader(), wrapper
// Arguments : filename : full name of file to be loaded
// hnum : record number to e read
// Use : Read info about Seismic Data into structure from a
// Zims datafile
// Returns : void
//***********************************************************************
BOOL CSeismicData::ReadZimsFileHeader(CString suppname, int hnum, UBYTE dtype,
	int horizon, struct GridFileHeaderInfo *headerinfo)
{
	if (!headerinfo)
		return FALSE;
	switch (pDoc->GridFileType)
	{
	case 1:
		return ReadZimsFileHeader_ZMAP(suppname, hnum, dtype, horizon, headerinfo);
	case 2:
		return ReadZimsFileHeader_MFD(suppname, hnum, dtype, horizon, headerinfo);
	case 3:
		return ReadZimsFileHeader_CP3(suppname, hnum, dtype, horizon, headerinfo);
	case 4:
		return ReadZimsFileHeader_CP3int(suppname, hnum, dtype, horizon, headerinfo);
	case 5:
		return ReadZimsFileHeader_EV(suppname, headerinfo);
	case 6:
		return ReadZimsFileHeader_ZmapISO(suppname, hnum, dtype, horizon, headerinfo);
	default:
		pDoc->Error("No valid grid file type", MSG_ERR);
		return FALSE;
	}
}

// Earthvision files.
// Will read an entire EarthVision format file; .2grd and .3grd.
// Only the header information will be returned.
BOOL CSeismicData::ReadZimsFileHeader_EV(CString &suppname, GridFileHeaderInfo *headerinfo)
{
#ifndef VM64

	// EarthVision variables
	char *desc;
	float *grid;
	int xcol, yrow, zlev;
	double xmin, xmax, ymin, ymax, zmin, zmax;
	float nullVal;
	char *datFile, *datField, *vfltFile, *nvfltFile;
	int status;
	int projection, zone, units, zUnits, pUnits;
	double GCTPparms[15];
	//---
	char filename[256];

	// turn off variables we don't want returned
	grid = NULL; // only reading header info
	desc = NULL;
	datFile = NULL;
	datField = NULL;
	vfltFile = NULL;
	nvfltFile = NULL;

	suppname = pDoc->ZimsDir + suppname;
	strcpy(filename, (const char *)suppname);

	if (strstr(suppname, ".2grd"))
	{

		status = gridRead2d(filename, &desc,
			&grid, &xcol, &yrow,
			&xmin, &xmax, &ymin, &ymax,
			&nullVal,
			&datFile, &datField,
			&vfltFile, &nvfltFile,
			&projection, &zone, &units, GCTPparms, &zUnits);
		if (status)
			pDoc->Error("Warning, an error occurred trying to read " + suppname, MSG_ERR);
	}
	else if (strstr(suppname, ".3grd"))
	{

		status = gridRead3d(filename, &desc,
			&grid, &xcol, &yrow, &zlev,
			&xmin, &xmax, &ymin, &ymax, &zmin, &zmax,
			&nullVal,
			&datFile, &datField,
			&projection, &zone, &units, GCTPparms,
			&zUnits, &pUnits);
		if (status)
			pDoc->Error("Warning, an error occurred trying to read " + suppname, MSG_ERR);
	}
	else
	{
		pDoc->Error("Could not open seismic file " + suppname, MSG_ERR);
		return FALSE;
	}

	// copy info from file into header structure
	headerinfo->rows = yrow;
	headerinfo->columns = xcol;
	headerinfo->size = (int)((float)xcol*(float)yrow*4.0 / 1024.0);
	headerinfo->xmin = xmin;
	headerinfo->xmax = xmax;
	headerinfo->ymin = ymin;
	headerinfo->ymax = ymax;

	return TRUE;

#else
	pDoc->Error("Earthvision format is not supported in the 64bit version of VelocityManager", MSG_ERR);
	return FALSE;
#endif

}

// Read header to Zmap ISO grid files
BOOL CSeismicData::ReadZimsFileHeader_ZmapISO(CString &suppname, int hnum, UBYTE dtype,
	int horizon, struct GridFileHeaderInfo *headerinfo)
{
	char fname[256], inputbuf[CSD_MAXLEN];
	ifstream infile;
	int fieldwidth, nrows, ncols;
	double znon, xmin, xmax, ymin, ymax;
	char *p;

	suppname = pDoc->ZimsDir + suppname;
	strcpy(fname, (const char *)suppname);

	// open file and read the header
	// remove the commas from the input string so the sscanf works properly
	infile.open(fname);
	while (!infile.eof())
	{
		do
		{
			infile.getline(inputbuf, CSD_MAXLEN - 1);
		} while (inputbuf[0] == '!' || inputbuf[0] == '@');// skip comments
		// read header
		while (p = strchr(inputbuf, ','))
			*p = ' ';
		sscanf(inputbuf, "%d %lf", &fieldwidth, &znon);
		infile.getline(inputbuf, CSD_MAXLEN - 1);
		while (p = strchr(inputbuf, ','))
			*p = ' ';
		sscanf(inputbuf, "%d %d %lf %lf %lf %lf", &nrows, &ncols, &xmin, &xmax, &ymin, &ymax);
		break;
	}

	// copy info into header structure
	headerinfo->rows = nrows;
	headerinfo->columns = ncols;
	headerinfo->size = (int)((float)ncols*(float)nrows*4.0 / 1024.0); // don't we store as doubles??
	headerinfo->xmin = xmin;
	headerinfo->xmax = xmax;
	headerinfo->ymin = ymin;
	headerinfo->ymax = ymax;

	return TRUE;
}

//***********************************************************************
// Subroutine : ReadZimsFileHeader_ZMAP(), ZMAP version
// Arguments : filename : full name of file to be loaded
// hnum : record number to be read
// Use : Read Seismic Data header into structure from a
// Zims datafile
// Returns : void
//***********************************************************************
BOOL CSeismicData::ReadZimsFileHeader_ZMAP(CString &suppname, int hnum, UBYTE dtype,
	int horizon, struct GridFileHeaderInfo *headerinfo)
{
	_zims_header header;
	CString name;
	int extp, hcurr;
	long int ignorebytes = 0;
	char namebuffer[21];
	_zims_para zpara;
	_zims_grid zgrid;
#ifdef VELMAN_UNIX
	int number_grid_sec[4] = { -1, 4, 0, 0 }; // -1 will be overwritten before use
#endif
	namebuffer[20] = 0x00;
	suppname = pDoc->ZimsDir + suppname;
	extp = suppname.Find('.');
	TRACE("ReadZimsFile entered with hnum %d, suppname %s\n", hnum, (const char *)suppname);
	if (extp != -1)
		suppname.GetBufferSetLength(extp + 2);
#ifdef VELMAN_UNIX
	suppfile.open(suppname + "h");
#else
	suppfile.open(suppname + "h", ios::binary);
#endif
	if (suppfile.fail())
	{
		pDoc->Error("Could not open seismic header file " + suppname + "H", MSG_ERR);
		return FALSE;
	}
	// Find relevant header in suppfile
	for (hcurr = 0; hcurr < hnum; hcurr++)
	{
		suppfile.read((char *)&header, sizeof(_zims_header));
#ifdef VELMAN_UNIX
		InvertData((char *)&header, zims_header_sec);
#endif
		suppfile.seekg((header.ndxprm - 1)*ZIMS_RECORD_LENGTH, ios::beg);
		// count bytes in zimsfile in order to skip
		switch (header.filetype)
		{
		case ZIMS_TYPE_GRID: // skip grid section
			suppfile.read((char *)&zgrid, sizeof(_zims_grid));
#ifdef VELMAN_UNIX
			InvertData((char *)&zgrid, zims_grid_sec);
#endif
			ignorebytes += zgrid.nrows*zgrid.ncolumns * sizeof(double);
			break;
		case ZIMS_TYPE_LINE: // skip line section
			suppfile.read((char *)&zpara, sizeof(_zims_para));
#ifdef VELMAN_UNIX
			InvertData((char *)&zpara, zims_para_sec);
#endif
			ignorebytes += zpara.nflds*zpara.nwrds*zpara.nrecs * sizeof(ZIMSWORD);
			break;
		default: // no valid filetype
			pDoc->Error("Zims file: " + suppname + " has unknown filetype", MSG_ERR);
			suppfile.close();
			return FALSE;
		}
		suppfile.seekg((header.ndxnxh - 1)*ZIMS_RECORD_LENGTH);
	}
	// read new header and seekg to parameter record
	suppfile.read((char *)&header, sizeof(_zims_header));
#ifdef VELMAN_UNIX
	InvertData((char *)&header, zims_header_sec);
#endif
	suppfile.seekg((header.ndxprm - 1)*ZIMS_RECORD_LENGTH, ios::beg);
	if (header.filetype != ZIMS_TYPE_GRID)
	{
		suppfile.close();
		return FALSE;
	}
	suppfile.read((char *)&zgrid, sizeof(_zims_grid));
	suppfile.close();
#ifdef VELMAN_UNIX
	InvertData((char *)&zgrid, zims_grid_sec);
#endif
	headerinfo->rows = (int)zgrid.nrows;
	headerinfo->columns = (int)zgrid.ncolumns;
	headerinfo->size = (int)(zgrid.nrows*zgrid.ncolumns * 4 / 1024);
	headerinfo->xmin = zgrid.xmin;
	headerinfo->xmax = zgrid.xmax;
	headerinfo->ymin = zgrid.ymin;
	headerinfo->ymax = zgrid.ymax;
	return TRUE;
}

//***********************************************************************
// Subroutine : ReadZimsFile_ZMAP(), ZMAP version
// Arguments : filename : full name of file to be loaded
// hnum : record number to be read
// Use : Read Seismic Data into internal data base from a
// Zims datafile
// Returns : void
//***********************************************************************
BOOL CSeismicData::ReadZimsFile_ZMAP(CString &suppname, int hnum, UBYTE dtype, int horizon)
{
	_zims_header header;
	CZimsGrid *zimsgrid;
	CString name;
	int i, j, spt, extp, gridsize, hcurr, nline;
	long int ignorebytes = 0;
	char *pointer, namebuffer[21];
	_zims_field *field = NULL;
	double x, y, data, xl[2], yl[2], **grid;
	double x0, x1, y0, y1, oldsegment, segment = -1.0;
	float **fptr;
	ifstream zimsfile;
	char inputbuffer[CSD_MAXLEN];
	_zims_para zpara;
	_zims_grid zgrid;
	CFaultGrid *faultgrid = NULL;
#ifdef VELMAN_UNIX
	int number_grid_sec[4] = { -1, 4, 0, 0 }; // -1 will be overwritten before use
#endif
	namebuffer[20] = 0x00;
	suppname = pDoc->ZimsDir + suppname;
	TRACE("ReadZimsFile entered with hnum %d, suppname %s\n", hnum, (const char *)suppname);
	extp = suppname.Find('.');
	if (extp != -1)
		suppname.GetBufferSetLength(extp + 2);
#ifdef VELMAN_UNIX
	suppfile.open(suppname + "h");
#else
	suppfile.open(suppname + "h", ios::binary);
#endif
	if (suppfile.fail())
	{
		pDoc->Error("Could not open seismic header file " + suppname + "H", MSG_ERR);
		return FALSE;
	}
	pDoc->Log("Opened zims seismic header file " + suppname + "H", LOG_STD);
#ifdef VELMAN_UNIX
	zimsfile.open(suppname + "d");
#else
	zimsfile.open(suppname + "d", ios::binary);
#endif
	if (zimsfile.fail())
	{
		suppfile.close();
		pDoc->Error("Could not open seismic data file " + suppname + "D", MSG_ERR);
		return FALSE;
	}
	pDoc->Log("Opened zims seismic data file " + suppname + "D", LOG_STD);

	// Find relevant header in suppfile
	for (hcurr = 0; hcurr < hnum; hcurr++)
	{
		suppfile.read((char *)&header, sizeof(_zims_header));
#ifdef VELMAN_UNIX
		InvertData((char *)&header, zims_header_sec);
#endif
		suppfile.seekg((header.ndxprm - 1)*ZIMS_RECORD_LENGTH, ios::beg);
		// count bytes in zimsfile in order to skip
		switch (header.filetype)
		{
		case ZIMS_TYPE_GRID: // skip grid section
			suppfile.read((char *)&zgrid, sizeof(_zims_grid));
#ifdef VELMAN_UNIX
			InvertData((char *)&zgrid, zims_grid_sec);
#endif
			ignorebytes += zgrid.nrows*zgrid.ncolumns * sizeof(double);
			break;
		case ZIMS_TYPE_LINE: // skip line section
			suppfile.read((char *)&zpara, sizeof(_zims_para));
#ifdef VELMAN_UNIX
			InvertData((char *)&zpara, zims_para_sec);
#endif
			ignorebytes += zpara.nflds*zpara.nwrds*zpara.nrecs * sizeof(ZIMSWORD);
			break;
		default: // no valid filetype
			pDoc->Error("Zims file: " + suppname + " has unknown filetype", MSG_ERR);
			suppfile.close();
			return FALSE;
		}
		suppfile.seekg((header.ndxnxh - 1)*ZIMS_RECORD_LENGTH);
	}
	zimsfile.seekg(ignorebytes, ios::beg);
	// read new header and seekg to parameter record
	suppfile.read((char *)&header, sizeof(_zims_header));
#ifdef VELMAN_UNIX
	InvertData((char *)&header, zims_header_sec);
#endif
	suppfile.seekg((header.ndxprm - 1)*ZIMS_RECORD_LENGTH, ios::beg);
	// Unpad the filename
	UnPad(header.filename, 20);
	TRACE("filename : %s\n", header.filename); /* 1-5: file's long name */
	TRACE("filetype : %ld\n", header.filetype);
	TRACE("ndxdat_day : %ld\n", header.ndxdat_day); /* 7: file creation - day */
	TRACE("ndxdat_mon : %ld\n", header.ndxdat_mon); /* 8: file creation - month */
	TRACE("ndxdat_yr : %ld\n", header.ndxdat_yr); /* 9: file creation - year */
	TRACE("ndxdat_hr : %ld\n", header.ndxdat_hr); /* 10: file creation - hour */
	TRACE("ndxdat_min : %ld\n", header.ndxdat_min); /* 11: file creation - minute */
	TRACE("ndxnxd : %ld\n", header.ndxnxd); /* 12: rec # for next data record */
	TRACE("ndxnxh : %ld\n", header.ndxnxh); /* 13: rec # for next data header record */
	TRACE("ndxprm : %ld\n", header.ndxprm); /* 14: rec # of parameter record */
	TRACE("ndxfld : %ld\n", header.ndxfld); /* 15: rec # of data fields beginning position */
	TRACE("msc : %ld\n", header.msc); /* 16: */
	TRACE("open : %ld\n", header.open); /* 17: */
	TRACE("ndxlen : %ld\n", header.ndxlen); /* 18: */
	TRACE("filename1 : %s\n", header.filename1); /* 33-37: support filename #1 */
	TRACE("filetype1 : %ld\n", header.filetype1); /* 38: support file #1's type */
	TRACE("filename2 : %s\n", header.filename2); /* 33-37: support filename #2 */
	TRACE("filetype2 : %ld\n", header.filetype2); /* 38: support file #2's type */
	switch (header.filetype)
	{
	case ZIMS_TYPE_GRID: // grid data file
		suppfile.read((char *)&zgrid, sizeof(_zims_grid));
#ifdef VELMAN_UNIX
		InvertData((char *)&zgrid, zims_grid_sec);
#endif
		TRACE("zero : %ld, %lf\n", zgrid.zero, (double)zgrid.zero); /* 1: numeric value 0.0 */
		TRACE("nrows : %ld\n", zgrid.nrows); /* 2: # of rows */
		TRACE("ncolumns : %ld\n", zgrid.ncolumns); /* 3: # of columns */
		TRACE("xmin : %lf\n", zgrid.xmin); /* 5-6: xmin of grid */
		TRACE("xmax : %lf\n", zgrid.xmax); /* 7-8: xmax of grid */
		TRACE("ymin : %lf\n", zgrid.ymin); /* 9-10: ymin of grid */
		TRACE("ymax : %lf\n", zgrid.ymax); /* 11-12: ymax of grid */
		TRACE("gxinc : %f\n", zgrid.gxinc); /* 13: x increment */
		TRACE("gyinc : %f\n", zgrid.gyinc); /* 14: y increment */
		TRACE("xband : %f\n", zgrid.xband); /* 15: x expansion of collection region over AOI */
		TRACE("yband : %f\n", zgrid.yband); /* 16: y expansion of collection region over AOI */
		TRACE("reach : %f\n", zgrid.reach); /* 17: data collection radius */
		TRACE("znon : %f\n", zgrid.znon); /* 18: znon value */
		TRACE("gmax : %f\n", zgrid.gmax); /* 19: zmax of grid */
		TRACE("gmin : %f\n", zgrid.gmin);/* 20: zmin of grid */
		// create new grid
		zimsgrid = new CZimsGrid(pDoc);
		// Initialize new grid and allocate memory for zimsgrid...
		if (!zimsgrid->Init((int)zgrid.nrows, (int)zgrid.ncolumns, pDoc, dtype, zgrid.znon))
		{
			pDoc->Error("Error allocating new Zims grid " + CString(header.filename), MSG_ERR);
			if (zimsgrid) delete zimsgrid;
			suppfile.close();
			return FALSE;
		}
		// get pointer to zimsgrid
		grid = zimsgrid->GetGrid();
		zimsgrid->Stamp();
		// read extrema from file
		xl[0] = (double)zgrid.xmin; xl[1] = (double)zgrid.xmax;
		yl[0] = (double)zgrid.ymin; yl[1] = (double)zgrid.ymax;
		zimsgrid->WriteLimits(xl, yl);
		// read FORTRAN zimsfile and transpose
		gridsize = ((int)zgrid.nrows)*((int)zgrid.ncolumns);
		// read data into float array as zimsfiles are float data...
		fptr = matrix(0, zgrid.ncolumns - 1, 0, zgrid.nrows - 1);
		zimsfile.read((char *)fptr[0], gridsize * sizeof(float));
		// transfer data into double array and return memory...
#ifdef VELMAN_UNIX
		number_grid_sec[0] = zgrid.nrows*zgrid.ncolumns;
		InvertData((char *)fptr[0], number_grid_sec);
#endif
		// Take the transpose as we are working with FORTRAN grid
		for (i = 0; i < zgrid.nrows; i++)
			for (j = 0; j < zgrid.ncolumns; j++)
				grid[i][j] = fptr[j][i];
		free_matrix(fptr, 0, zgrid.ncolumns - 1, 0, zgrid.nrows - 1);
		zimsgrid->CalcAverage();
		// add grid with the correct name and horizon
		// add grid to Zimscube.
		zimsgrid->UseProperZnon();
		zimsgrid->RoundAllGridnodes();
		pDoc->zimscube.Add(zimsgrid, CString(header.filename), horizon);
		break;
	case ZIMS_TYPE_POLY:
	case ZIMS_TYPE_LINE: // line data file
		suppfile.read((char *)&zpara, sizeof(_zims_para));
#ifdef VELMAN_UNIX
		InvertData((char *)&zpara, zims_para_sec);
#endif
		// if this is polygon data, allocate the polygon grid
		// and store it inside the faultcube
		if (header.filetype == ZIMS_TYPE_POLY)
		{
			faultgrid = new CFaultGrid(pDoc);
			faultgrid->Init(pDoc, zpara.znon);
			faultgrid->WriteLimits(&zpara.xmin, &zpara.ymin);
			pDoc->faultcube.Add(faultgrid, CString(header.filename), horizon);
		}
		field = new _zims_field[zpara.nflds];
		suppfile.seekg((header.ndxfld - 1)*ZIMS_RECORD_LENGTH);
		suppfile.read((char *)field, (int)zpara.nflds * sizeof(_zims_field));
#ifdef VELMAN_UNIX
		for (i = 0; i < zpara.nflds; i++)
			InvertData((char *)(field + i), zims_field_sec);
		buffer_sec = new int[zpara.nflds * 2 + 2];
		// create array to invert fields
		for (j = 0; j < zpara.nflds + 1; j++)
		{
			if (field[j].fldtype != 0x16)
			{
				buffer_sec[2 * j] = 1;
				buffer_sec[2 * j + 1] = field[j].nwords * sizeof(ZIMSWORD);
			}
			else
			{
				buffer_sec[2 * j] = field[j].nwords * sizeof(ZIMSWORD);
				buffer_sec[2 * j + 1] = 1;
			}
		}
		buffer_sec[2 * j] = 0;
		buffer_sec[2 * j + 1] = 0;
#endif
		for (i = 0; i < zpara.nrecs; i++)
		{
			if (zimsfile.eof())
			{
				pDoc->Error("Zims file: " + suppname + " data size inconsistency", MSG_ERR);
				suppfile.close();
				zimsfile.close();
				return FALSE;
			}
			zimsfile.read(inputbuffer, (int)zpara.nwrds * sizeof(ZIMSWORD));
#ifdef VELMAN_UNIX
			InvertData(inputbuffer, buffer_sec);
#endif
			x = CSD_DONTKNOW;
			y = CSD_DONTKNOW;
			data = CSD_DONTKNOW;
			name = "";
			spt = 0;
			nline = -1;
			pointer = inputbuffer;
			for (j = 0; j < zpara.nflds; j++)
			{
				switch (field[j].fldtype)
				{
				case 1: // Easting
					if (field[j].nwords * sizeof(ZIMSWORD) == sizeof(double))
						x = (double) *((double *)pointer);
					else
						x = (double) *((float *)pointer);
					break;
				case 2: // Northing
					if (field[j].nwords * sizeof(ZIMSWORD) == sizeof(double))
						y = (double) *((double *)pointer);
					else
						y = (double) *((float *)pointer);
					break;
				case 3: // Data value
					if (field[j].nwords * sizeof(ZIMSWORD) == sizeof(double))
						data = (double) *((double *)pointer);
					else
						data = (double) *((float *)pointer);
					break;
				case 14: // Seismic Shot pt
					if (field[j].nwords * sizeof(ZIMSWORD) == sizeof(double))
						spt = (int)(*((double *)pointer) + 0.3);
					else
						spt = (int)(*((float *)pointer) + 0.3f);
					break;
				case 22: // line name
					strncpy(namebuffer, (const char *)pointer,
						(int)field[j].nwords * sizeof(ZIMSWORD));
					namebuffer[(int)field[j].nwords * sizeof(ZIMSWORD)] = 0x00;
					// sk126 name = KillWhite(namebuffer);
					name = namebuffer;
					break;
				case 25: // line number
					if (field[j].nwords * sizeof(ZIMSWORD) == sizeof(double))
						nline = (int)(*((double *)pointer) + 0.3);
					else
						nline = (int)(*((float *)pointer) + 0.3f);
					break;
				case 35: // Segment ID
					oldsegment = segment;
					if (field[j].nwords * sizeof(ZIMSWORD) == sizeof(double))
						segment = (double) *((double *)pointer);
					else
						segment = (double) *((float *)pointer);
					break;
				default:
					break;
				}
				pointer += field[j].nwords * sizeof(ZIMSWORD);
			}
			if (header.filetype == ZIMS_TYPE_LINE)
			{ // this is shotline data
				if (!name.IsEmpty())
				{
					// sk126 if(nline == -1) nline = ExtractLineNumber(name);
					AddNewPt(name, nline, spt, dtype, x, y, CSD_DONTKNOW);
				}
				else
				{
					pDoc->Error("Zims file: " + suppname + " does not seem to contain shotline data", MSG_ERR);
					suppfile.close();
					return FALSE;
				}
			}
			else
			{ // and this is faultline data...
				if (oldsegment != segment)
				{
					x1 = x;
					y1 = y;
				}
				else
				{
					x0 = x1;
					y0 = y1;
					x1 = x;
					y1 = y;
					if (x0 != CSD_DONTKNOW && y0 != CSD_DONTKNOW &&
						x1 != CSD_DONTKNOW && y1 != CSD_DONTKNOW)
					{
						faultgrid->AddFaultLine(x0, y0, x1, y1);
					}
					else
					{
						pDoc->Error("Zims file: " + CString(header.filename) +
							" does not seem to contain polygon data", MSG_ERR);
						return FALSE;
					}
				}
			}
		}
		break;
	default: // no valid filetype
		pDoc->Error("Zims file: " + suppname + " has unknown filetype", MSG_ERR);
		suppfile.close();
		return FALSE;
	}
	zimsfile.close();
	suppfile.close();
	if (field) delete field;
	return TRUE;
}

//***********************************************************************
// Subroutine : ReadZimsFileHeader_MFD(), MFD version
// Arguments : filename : full name of file to be loaded
// hnum : record number to be read
// Use : Read Seismic Data into structure from a
// Zims datafile
// Returns : void
//***********************************************************************
BOOL CSeismicData::ReadZimsFileHeader_MFD(CString &filename, int hnum,
	UBYTE dtype, int horizon, struct GridFileHeaderInfo *header)
{
	ifstream zimsfile;
	meta_file_head metahead;
	file_head filehead;
	file_info fileinfo;
	int i, keys, newkeys, metanext;
	int *key = NULL, *oldkey = NULL;
#ifdef VELMAN_UNIX
	zimsfile.open(pDoc->ZimsDir + filename);
#else
	zimsfile.open(pDoc->ZimsDir + filename, ios::binary);
#endif
	// Check if something very basic went wrong
	if (zimsfile.fail())
	{
		pDoc->Error("Internal error, file " + filename + " faulty/does not exist...", MSG_ERR);
		return FALSE;
	}
	metanext = 0;
	// check whith meta_file_header we have to go to to get this data...
	do
	{
		zimsfile.seekg(metanext*ZIMS_BLOCK, ios::beg);
		zimsfile.read((char *)&metahead, sizeof(meta_file_head));
#ifndef VELMAN_UNIX
		InvertData((char *)&metahead, meta_file_head_sec);
#endif
		// check that file is in correct format
		if (metahead.four != 4)
			break;
		metanext = metahead.meta_next - 1;
		hnum -= metahead.filenum;
		// something went wrong -- header could not be found
		if (metanext == -1 && hnum >= 0)
		{
			pDoc->Error("Not enough meta_file_headers in file " + filename +
				"to forward to required datafile", MSG_ERR);
			return FALSE;
		}
	} while (hnum >= 0);
	// we have now skipped all previous headers...
	hnum += metahead.filenum;
	// reading the file header for this subfile
	zimsfile.seekg(hnum*ZIMS_RECORD_LENGTH, ios::cur);
	zimsfile.read((char *)&filehead, ZIMS_RECORD_LENGTH);
	// invert data if reading with INTEL
#ifndef VELMAN_UNIX
	InvertData((char *)&filehead, file_head_sec);
#endif
	UnPad(filehead.dataname, 24);
	// reading the key header of the subfile
	zimsfile.seekg((filehead.header_off - 1)*ZIMS_BLOCK, ios::beg);
	// read all keys for this file
#ifndef VELMAN_UNIX
	int key_sec[3];
#endif
	keys = 0;
	do
	{
		zimsfile.read((char *)&newkeys, sizeof(int));
#ifndef VELMAN_UNIX
		InvertData((char *)&newkeys, one_word_sec);
#endif
		// newkeys counts itself - so ignore that
		newkeys--;
		// save old key list
		oldkey = key;
		// read the new keys into the key array
		if (keys)
		{
			// allocate new keylist of the correct size
			key = ivector(0, keys + newkeys - 4);
			// copy old keys over to new array...
			for (i = 0; i < keys; i++)
				key[i] = oldkey[i];
			// read new next_key into key[0]
			zimsfile.read((char *)key, sizeof(int));
			// invert it
#ifndef VELMAN_UNIX
			InvertData((char *)key, one_word_sec);
#endif
			// file_head and grid_head come twice, so ignore and just read data
			zimsfile.ignore(2 * sizeof(int));
			zimsfile.read((char *)(key + keys), (newkeys - 3) * sizeof(int));
			// invert the newly read keys
#ifndef VELMAN_UNIX
			key_sec[0] = newkeys - 3;
			key_sec[1] = sizeof(int);
			key_sec[2] = 0;
			InvertData((char *)(key + keys), key_sec);
#endif
			// free old keys list
			free_ivector(oldkey, 0, keys - 1);
			// new total number of keys...
			keys += newkeys - 3;
		}
		else
		{
			// allocate new keylist of the correct size
			key = ivector(0, keys + newkeys - 1);
			// read 1st key list into key array
			zimsfile.read((char *)(key + keys), newkeys * sizeof(int));
			// invert the entire array
#ifndef VELMAN_UNIX
			key_sec[0] = newkeys;
			key_sec[1] = sizeof(int);
			key_sec[2] = 0;
			InvertData((char *)key + keys, key_sec);
#endif
			// new total number of keys
			keys += newkeys;
		}
		// goto the next block...
		if (key[0] != 0)
			zimsfile.seekg((key[0] - 1)*ZIMS_BLOCK, ios::beg);
	} while (key[0] != 0);
	// read the file_info structure at key[1]
	zimsfile.seekg((key[1] - 1)*ZIMS_BLOCK, ios::beg);
	zimsfile.read((char *)&fileinfo, ZIMS_RECORD_LENGTH);
#ifndef VELMAN_UNIX
	InvertData((char *)&fileinfo, file_info_sec);
#endif
	// Unpad the ASCII fields
	UnPad(fileinfo.dataname, 24);
	UnPad(fileinfo.filename, 24);
	UnPad(fileinfo.date, 20);
	if (filehead.datatype == ZIMS_TYPE_GRID)
	{
		zims_grid gridhead;
		// read the grid descriptor
		zimsfile.seekg((key[2] - 1)*ZIMS_BLOCK, ios::beg);
		zimsfile.read((char *)&gridhead, ZIMS_RECORD_LENGTH);
		// invert data if reading with INTEL
#ifndef VELMAN_UNIX
		InvertData((char *)&gridhead, zims_grid_sec);
#endif
		header->rows = gridhead.nrows;
		header->columns = gridhead.ncolumns;
		header->size = gridhead.nrows*gridhead.ncolumns * 4 / 1024;
		header->xmin = gridhead.xmin;
		header->xmax = gridhead.xmax;
		header->ymin = gridhead.ymin;
		header->ymax = gridhead.ymax;
		zimsfile.close();
		// free key storage space
		free_ivector(key, 0, keys - 1);
		return TRUE;
	}
	else
		zimsfile.close();
	// free key storage space
	free_ivector(key, 0, keys - 1);
	return FALSE;
}

//***********************************************************************
// Subroutine : ReadZimsFile_MFD(), MFD version
// Arguments : filename : full name of file to be loaded
// hnum : record number to be read
// Use : Read Seismic Data into internal data base from a
// Zims datafile
// Returns : void
//***********************************************************************
BOOL CSeismicData::ReadZimsFile_MFD(CString &filename, int hnum,
	UBYTE dtype, int horizon)
{
	ifstream zimsfile;
	meta_file_head metahead;
	file_head filehead;
	file_info fileinfo;
	int i, j, n, k, keys, newkeys, metanext;
	int *key = NULL, *oldkey = NULL;
#ifdef VELMAN_UNIX
	zimsfile.open(pDoc->ZimsDir + filename);
#else
	zimsfile.open(pDoc->ZimsDir + filename, ios::binary);
#endif
	// Check if something very basic went wrong
	if (zimsfile.fail())
	{
		pDoc->Error("Internal error, file " + filename + " faulty/does not exist...", MSG_ERR);
		return FALSE;
	}
	metanext = 0;
	// check which meta_file_header we have to go to to get this data...
	do
	{
		zimsfile.seekg(metanext*ZIMS_BLOCK, ios::beg);
		zimsfile.read((char *)&metahead, sizeof(meta_file_head));
#ifndef VELMAN_UNIX
		InvertData((char *)&metahead, meta_file_head_sec);
#endif
		// check that file is in correct format
		if (metahead.four != 4)
			break;
		metanext = metahead.meta_next - 1;
		hnum -= metahead.filenum;
		// something went wrong -- header could not be found
		if (metanext == -1 && hnum >= 0)
		{
			pDoc->Error("Not enough meta_file_headers in file " + filename +
				"to forward to required datafile", MSG_ERR);
			return FALSE;
		}
	} while (hnum >= 0);
	// we have now skipped all previous headers...
	hnum += metahead.filenum;
	// reading the file header for this subfile
	zimsfile.seekg(hnum*ZIMS_RECORD_LENGTH, ios::cur);
	zimsfile.read((char *)&filehead, ZIMS_RECORD_LENGTH);
	// invert data if reading with INTEL
#ifndef VELMAN_UNIX
	InvertData((char *)&filehead, file_head_sec);
#endif
	UnPad(filehead.dataname, 24);
	// reading the key header of the subfile
	zimsfile.seekg((filehead.header_off - 1)*ZIMS_BLOCK, ios::beg);
	// read all keys for this file
#ifndef VELMAN_UNIX
	int key_sec[3];
#endif
	keys = 0;
	do
	{
		zimsfile.read((char *)&newkeys, sizeof(int));
#ifndef VELMAN_UNIX
		InvertData((char *)&newkeys, one_word_sec);
#endif
		// newkeys counts itself - so ignore that
		newkeys--;
		// save old key list
		oldkey = key;
		// read the new keys into the key array
		if (keys)
		{
			// allocate new keylist of the correct size
			key = ivector(0, keys + newkeys - 4);
			// copy old keys over to new array...
			for (i = 0; i < keys; i++)
				key[i] = oldkey[i];
			// read new next_key into key[0]
			zimsfile.read((char *)key, sizeof(int));
			// invert it
#ifndef VELMAN_UNIX
			InvertData((char *)key, one_word_sec);
#endif
			// file_head and grid_head come twice, so ignore and just read data
			zimsfile.ignore(2 * sizeof(int));
			zimsfile.read((char *)(key + keys), (newkeys - 3) * sizeof(int));
			// invert the newly read keys
#ifndef VELMAN_UNIX
			key_sec[0] = newkeys - 3;
			key_sec[1] = sizeof(int);
			key_sec[2] = 0;
			InvertData((char *)(key + keys), key_sec);
#endif
			// free old keys list
			free_ivector(oldkey, 0, keys - 1);
			// new total number of keys...
			keys += newkeys - 3;
		}
		else
		{
			// allocate new keylist of the correct size
			key = ivector(0, newkeys - 1);
			// read 1st key list into key array
			zimsfile.read((char *)key, newkeys * sizeof(int));
			// invert the entire array
#ifndef VELMAN_UNIX
			key_sec[0] = newkeys;
			key_sec[1] = sizeof(int);
			key_sec[2] = 0;
			InvertData((char *)key, key_sec);
#endif
			// new total number of keys
			keys += newkeys;
		}
		// goto the next block...
		if (key[0] != 0)
			zimsfile.seekg((key[0] - 1)*ZIMS_BLOCK, ios::beg);
	} while (key[0] != 0);
	// read the file_info structure at key[1]
	zimsfile.seekg((key[1] - 1)*ZIMS_BLOCK, ios::beg);
	zimsfile.read((char *)&fileinfo, ZIMS_RECORD_LENGTH);
#ifndef VELMAN_UNIX
	InvertData((char *)&fileinfo, file_info_sec);
#endif
	// Unpad the ASCII fields
	UnPad(fileinfo.dataname, 24);
	UnPad(fileinfo.filename, 24);
	UnPad(fileinfo.date, 20);
	switch (filehead.datatype)
	{
	case ZIMS_TYPE_GRID:
	{
		zims_grid gridhead;
		char *zimsblock = new char[ZIMS_BLOCK];
		CZimsGrid *zimsgrid;
		// create new grid
		zimsgrid = new CZimsGrid(pDoc);
		// read the grid descriptor
		zimsfile.seekg((key[2] - 1)*ZIMS_BLOCK, ios::beg);
		zimsfile.read((char *)&gridhead, ZIMS_RECORD_LENGTH);
		// invert data if reading with INTEL
#ifndef VELMAN_UNIX
		InvertData((char *)&gridhead, zims_grid_sec);
#endif
		// Fill the grid with the now avaliable data
		zimsgrid->Init(gridhead.nrows, gridhead.ncolumns, pDoc, dtype, gridhead.znon);
		zimsgrid->Stamp();
		zimsgrid->WriteLimits(&gridhead.xmin, &gridhead.ymin);
		// now go thru all keys of this subfile and read the data
#ifndef VELMAN_UNIX
		int zblock_sec[3];
		zblock_sec[0] = ZIMS_BLOCK / sizeof(float);
		zblock_sec[1] = sizeof(float);
		zblock_sec[2] = 0;
#endif
		i = 0;
		j = 0;
		for (n = 3; j < gridhead.ncolumns; n++)
		{
			k = 0;
			// check if there are too few keys
			if (n == keys)
			{
				pDoc->Error("Not enough keys for data volume in subfile " +
					CString(filehead.dataname), MSG_ERR);
				return FALSE;
			}
			// find and read the next data block
			zimsfile.seekg((key[n] - 1)*ZIMS_BLOCK, ios::beg);
			zimsfile.read(zimsblock, ZIMS_BLOCK);
			// invert data if reading with INTEL
#ifndef VELMAN_UNIX
			InvertData(zimsblock, zblock_sec);
#endif
			// now copy new block into internal data base
			while (k < ZIMS_BLOCK / sizeof(float))
			{
				((double **)(*zimsgrid))[i][j] = (double) *((float *)(zimsblock + sizeof(float)*k));
				// goto next row
				i++;
				if (i == gridhead.nrows)
				{
					i = 0;
					j++;
				}
				// filled all columns - finished...
				if (j == gridhead.ncolumns) break;
				k++;
			}
		}
		// add grid to Zimscube.
		zimsgrid->CalcAverage();
		zimsgrid->UseProperZnon();
		zimsgrid->RoundAllGridnodes();
		pDoc->zimscube.Add(zimsgrid, CString(filehead.dataname), horizon);
		delete zimsblock;
		break;
	}
	case ZIMS_TYPE_POLY:
	case ZIMS_TYPE_LINE:
	{
		zims_para zpara;
		zims_field *field = NULL;
		CFaultGrid *faultgrid;
		char *buffer = NULL, *pointer = NULL;
		int *buffer_sec = NULL;
		char namebuffer[64];
		CString name;
		double x, y, data;
		int spt, nline;
		double x0, y0, x1, y1, oldsegment, segment = -1.0;
		// line descriptor starts at key[2]
		n = 2;
		// read line descriptor
		zimsfile.seekg((key[n] - 1)*ZIMS_BLOCK, ios::beg);
		zimsfile.read((char *)&zpara, ZIMS_RECORD_LENGTH);
		k = ZIMS_RECORD_LENGTH;
		// invert data if reading with INTEL
#ifndef VELMAN_UNIX
		InvertData((char *)&zpara, zims_para_sec);
#endif
		// if this is polygon data, allocate the polygon grid
		// and store it inside the faultcube
		if (filehead.datatype == ZIMS_TYPE_POLY)
		{
			faultgrid = new CFaultGrid(pDoc);
			faultgrid->Init(pDoc, zpara.znon);
			faultgrid->WriteLimits(&zpara.xmin, &zpara.ymin);
			pDoc->faultcube.Add(faultgrid, CString(filehead.dataname), horizon);
		}
		// allocate memory for fields etc...
		field = new zims_field[zpara.nflds];
		buffer = new char[zpara.nwrds * sizeof(ZIMSWORD)];
		// read field descriptors from keys
		j = 0;
		while (j < (int)zpara.nflds)
		{
			if (ZIMS_BLOCK - k < sizeof(zims_field))
			{
				zimsfile.read((char *)(field)+j * sizeof(zims_field), ZIMS_BLOCK - k);
				n++;
				// Check if there are enough keys for the expected data
				if (n == keys)
				{
					pDoc->Error("Not enough keys for field descriptors in file " +
						CString(filehead.dataname), MSG_ERR);
					return FALSE;
				}
				// else go to new key and read the data from it
				zimsfile.seekg((key[n] - 1)*ZIMS_BLOCK, ios::beg);
				zimsfile.read((char *)(field)+j * sizeof(zims_field) + ZIMS_BLOCK - k,
					sizeof(zims_field) + k - ZIMS_BLOCK);
				k = sizeof(zims_field) + k - ZIMS_BLOCK;
			}
			// There is still enough space in the current key
			// so read the data from it.
			else
			{
				zimsfile.read((char *)(field)+j * sizeof(zims_field),
					sizeof(zims_field));
				k += sizeof(zims_field);
			}
			j++;
		}
		// invert data if reading with INTEL
#ifndef VELMAN_UNIX
		for (i = 0; i < zpara.nflds; i++)
			InvertData((char *)(field + i), zims_field_sec);
		buffer_sec = new int[zpara.nflds * 2 + 2];
		// create array to invert fields
		for (j = 0; j < zpara.nflds; j++)
		{
			if (field[j].fldtype != 0x16)
			{
				buffer_sec[2 * j] = 1;
				buffer_sec[2 * j + 1] = field[j].nwords * sizeof(ZIMSWORD);
			}
			else
			{
				buffer_sec[2 * j] = field[j].nwords * sizeof(ZIMSWORD);
				buffer_sec[2 * j + 1] = 1;
			}
		}
		buffer_sec[2 * j] = 0;
		buffer_sec[2 * j + 1] = 0;
#endif
		// read zimsblocks and put into internal data base
		i = 0;
		k = ZIMS_BLOCK;
		while (i < zpara.nrecs)
		{
			// Check if we have to jump to a new key to read the
			// next data record
			if ((int)(ZIMS_BLOCK - k) < (int)(zpara.nwrds * sizeof(ZIMSWORD)))
			{
				zimsfile.read(buffer, ZIMS_BLOCK - k);
				n++;
				// Check if there are enough keys for the expected data
				if (n == keys)
				{
					pDoc->Error("Not enough keys for data volume in file " +
						CString(filehead.dataname), MSG_ERR);
					return FALSE;
				}
				// else go to new key and read the data from it
				zimsfile.seekg((key[n] - 1)*ZIMS_BLOCK, ios::beg);
				zimsfile.read(buffer + ZIMS_BLOCK - k,
					(int)zpara.nwrds * sizeof(ZIMSWORD) + k - ZIMS_BLOCK);
				k = (int)zpara.nwrds * sizeof(ZIMSWORD) + k - ZIMS_BLOCK;
			}
			// There is still enough space in the current key
			// so read the data from it.
			else
			{
				zimsfile.read(buffer, (int)zpara.nwrds * sizeof(ZIMSWORD));
				k += (int)zpara.nwrds * sizeof(ZIMSWORD);
			}
			// Check if there is enough data...
			if (zimsfile.eof())
			{
				pDoc->Error("ZimsMeta file: " + filename + " data size inconsistency", MSG_ERR);
				return FALSE;
			}
#ifndef VELMAN_UNIX
			InvertData(buffer, buffer_sec);
#endif
			x = CSD_DONTKNOW;
			y = CSD_DONTKNOW;
			data = CSD_DONTKNOW;
			spt = 0;
			nline = -1;
			pointer = buffer;
			for (j = 0; j < zpara.nflds; j++)
			{
				switch (field[j].fldtype)
				{
				case 0x01: // Easting
					if (field[j].nwords * sizeof(ZIMSWORD) == sizeof(double))
						x = (double) *((double *)pointer);
					else
						x = (double) *((float *)pointer);
					break;
				case 0x02: // Northing
					if (field[j].nwords * sizeof(ZIMSWORD) == sizeof(double))
						y = (double) *((double *)pointer);
					else
						y = (double) *((float *)pointer);
					break;
				case 0x03: // Data value
					if (field[j].nwords * sizeof(ZIMSWORD) == sizeof(double))
						data = (double) *((double *)pointer);
					else
						data = (double) *((float *)pointer);
					break;
				case 0x0e: // Seismic Shot pt
					if (field[j].nwords * sizeof(ZIMSWORD) == sizeof(double))
						spt = (int)(*((double *)pointer) + 0.3);
					else
						spt = (int)(*((float *)pointer) + 0.3f);
					break;
				case 0x16: // line name
					strncpy(namebuffer, (const char *)pointer,
						(int)field[j].nwords * sizeof(ZIMSWORD));
					namebuffer[field[j].nwords * sizeof(ZIMSWORD)] = 0x00;
					UnPad(namebuffer, field[j].nwords * sizeof(ZIMSWORD) + 1);
					// sk126 name = KillWhite(namebuffer);
					name = namebuffer;
					break;
				case 0x19: // line number
					if (field[j].nwords * sizeof(ZIMSWORD) == sizeof(double))
						nline = (int)(*((double *)pointer) + 0.3);
					else
						nline = (int)(*((float *)pointer) + 0.3f);
					break;
				case 0x23: // Segment ID
					oldsegment = segment;
					if (field[j].nwords * sizeof(ZIMSWORD) == sizeof(double))
						segment = (double) *((double *)pointer);
					else
						segment = (double) *((float *)pointer);
					break;
				}
				pointer += field[j].nwords * sizeof(ZIMSWORD);
			}
			if (filehead.datatype == ZIMS_TYPE_LINE)
			{
				if (!name.IsEmpty())
				{
					// sk126 if(nline == -1) nline = ExtractLineNumber(name);
					AddNewPt(name, nline, spt, dtype, x, y, CSD_DONTKNOW);
				}
				else
				{
					pDoc->Error("Zims file: " + CString(filehead.dataname) +
						" does not seem to contain shotline data", MSG_ERR);
					return FALSE;
				}
			}
			else
			{
				if (oldsegment != segment)
				{
					x1 = x;
					y1 = y;
				}
				else
				{
					x0 = x1;
					y0 = y1;
					x1 = x;
					y1 = y;
					if (x0 != CSD_DONTKNOW && y0 != CSD_DONTKNOW &&
						x1 != CSD_DONTKNOW && y1 != CSD_DONTKNOW)
					{
						faultgrid->AddFaultLine(x0, y0, x1, y1);
					}
					else
					{
						pDoc->Error("Zims file: " + CString(filehead.dataname) +
							" does not seem to contain polygon data", MSG_ERR);
						return FALSE;
					}
				}
			}
			// a new record has been read.
			i++;
		}
		if (field) delete field;
		if (buffer_sec) delete buffer_sec;
		if (buffer) delete buffer;
		break;
	}
	default: // no valid filetype
		pDoc->Error("Zims file: " + CString(filehead.dataname) +
			" has unknown filetype", MSG_ERR);
		return FALSE;
	}
	// free key storage space
	free_ivector(key, 0, keys - 1);
	return TRUE;
}

//***********************************************************************
// Subroutine : ReadZimsFileHeader_CP3(), CP3 version
// Arguments : filename : full name of file to be loaded
// hnum : record number to be read
// Use : Read Seismic Data into structure from a
// Zims datafile
// Returns : void
//***********************************************************************
BOOL CSeismicData::ReadZimsFileHeader_CP3(CString &filename, int hnum, UBYTE dtype, int horizon, struct GridFileHeaderInfo *header)
{
	ifstream cp3file;
	cp3_file_head filehead;
	char dataname[128];
#ifdef VELMAN_UNIX
	cp3file.open(pDoc->ZimsDir + filename);
#else
	cp3file.open(pDoc->ZimsDir + filename, ios::binary);
#endif
	if (cp3file.fail())
	{
		pDoc->Error("Internal error, file " + filename + " does not exist...",
			MSG_ERR);
		return FALSE;
	}
	FortranRead(cp3file, (char *)&filehead, sizeof(cp3_file_head));
	FortranRead(cp3file, dataname, 128);
	if (cp3file.fail())
	{
		pDoc->Error("Corrupt CP3file : " + filename + " -- cannot read header data ", MSG_ERR);
		return FALSE;
	}
	// invert data if reading with INTEL
#ifndef VELMAN_UNIX
	InvertData((char *)&filehead, cp3_file_head_sec);
#endif
	UnPad(dataname, 128);
	if (filehead.itype == CP3_TYPE_GRID)
	{
		cp3_grid gridhead;
		float **fgrid = NULL, *cp3col = NULL;
		// define column byte reversal block
#ifndef VELMAN_UNIX
		int cp3col_sec[3];
		cp3col_sec[1] = sizeof(float);
		cp3col_sec[2] = 0;
#endif
		FortranRead(cp3file, (char *)&gridhead, sizeof(cp3_grid));
		// invert data if reading with INTEL
#ifndef VELMAN_UNIX
		InvertData((char *)&gridhead, cp3_grid_sec);
#endif
		header->rows = gridhead.nrow;
		header->columns = gridhead.ncol;
		header->size = gridhead.nrow*gridhead.ncol * 4 / 1024;
		header->xmin = (double)gridhead.xmin;
		header->xmax = (double)gridhead.xmax;
		header->ymin = (double)gridhead.ymin;
		header->ymax = (double)gridhead.ymax;
		cp3file.close();
		return TRUE;
	}
	return FALSE;
}

//***********************************************************************
// Subroutine : ReadZimsFileHeader_CP3(), CP3 version, internal
// Arguments : filename : full name of file to be loaded
// hnum : record number to be read
// Use : Read Seismic Data into structure from a
// Zims datafile
// Returns : void
//***********************************************************************
BOOL CSeismicData::ReadZimsFileHeader_CP3int(CString &filename, int hnum,
	UBYTE dtype, int horizon, struct GridFileHeaderInfo *header)
{
	int nextlength;
	char *dataindex = NULL;
	BOOL status = FALSE;

	//open CPS-3 internal data file
#ifdef VELMAN_UNIX
	ifstream cp3file(pDoc->ZimsDir + filename);
#else
	ifstream cp3file(pDoc->ZimsDir + filename, ios::binary);
#endif
	if (cp3file.fail())
	{
		pDoc->Error("Could not open " + filename, MSG_ERR);
		return FALSE;
	}

	// skip over magic number header
	cp3file.seekg(0x10, ios::beg);

	// skip over not used section + LOCK section
	cp3file.read((char *)&nextlength, sizeof(int));
#ifndef VELMAN_UNIX
	InvertData((char *)&nextlength, one_word_sec);
#endif
	cp3file.seekg(nextlength + 0x10, ios::cur);

	// skip over comment section and MAIN section
	cp3file.read((char *)&nextlength, sizeof(int));
#ifndef VELMAN_UNIX
	InvertData((char *)&nextlength, one_word_sec);
#endif
	cp3file.seekg(nextlength + 0x10, ios::cur);

	// skip over SUBS section
	cp3file.read((char *)&nextlength, sizeof(int));
	cp3file.seekg(0x10, ios::cur);

	// skip over the data section
	cp3file.read((char *)&nextlength, sizeof(int));
#ifndef VELMAN_UNIX
	InvertData((char *)&nextlength, one_word_sec);
#endif
	cp3file.seekg(nextlength, ios::cur);

	// skip PARM header and read index data length
	cp3file.seekg(0x10, ios::cur);
	cp3file.read((char *)&nextlength, sizeof(int));
#ifndef VELMAN_UNIX
	InvertData((char *)&nextlength, one_word_sec);
#endif

	// extract data from dataindex section
	dataindex = new char[nextlength];
	cp3file.read(dataindex, nextlength);

	// search through index to find the correct variables
	char *errmsg = NULL;
	header->rows = (int)FindCPSVar("NRSNRO=", dataindex, nextlength, &status);
	if (!status) errmsg = "Problem reading header: could not find rows marker NRSNRO";
	header->columns = (int)FindCPSVar("NCSNRO=", dataindex, nextlength, &status);
	if (!status && !errmsg)
		errmsg = "Problem reading header: could not find columns marker NCSNRO";
	if (status && !errmsg)
		header->size = header->rows*header->columns * sizeof(float) / 1024;
	header->xmin = FindCPSVar("X1SLIM=", dataindex, nextlength, &status);
	if (!status)
		header->xmin = FindCPSVar("U1SLIM=", dataindex, nextlength, &status);
	if (status) // appear to have found a new format .scps file
		pDoc->Log("File : " + filename + " appears to be new style CPS3 file.", LOG_DETAIL);
	if (!status && !errmsg)
		errmsg = "Problem reading header: could not find x limit marker X1SLIM or U1SLIM";
	header->xmax = FindCPSVar("X2SLIM=", dataindex, nextlength, &status);
	if (!status)
		header->xmax = FindCPSVar("U2SLIM=", dataindex, nextlength, &status);
	if (!status && !errmsg)
		errmsg = "Problem reading header: could not find x limit marker X2SLIM or U2SLIM";
	header->ymin = FindCPSVar("Y1SLIM=", dataindex, nextlength, &status);
	if (!status)
		header->ymin = FindCPSVar("U3SLIM=", dataindex, nextlength, &status);
	if (!status && !errmsg)
		errmsg = "Problem reading header: could not find y limit marker Y1SLIM or U3SLIM";
	header->ymax = FindCPSVar("Y2SLIM=", dataindex, nextlength, &status);
	if (!status)
		header->ymax = FindCPSVar("U4SLIM=", dataindex, nextlength, &status);
	if (!status && !errmsg)
		errmsg = "Problem reading header: could not find y limit marker Y2SLIM or U4SLIM";
	if (errmsg)
	{
		pDoc->Log(errmsg, LOG_BRIEF);
		pDoc->Log("Read of file abandoned.", LOG_BRIEF);
		pDoc->Error("The file could not be read because an error occurred."
			"See log file for more details.\n"
			"Reading " + filename + " abandoned.", MSG_ERR);
		return FALSE;
	}

	// free dataindex array
	delete dataindex;

	return TRUE;
}

// Reads an entire EarthVision file and creates a new ZIMS grid from the data.
// Handles both grid data and fault files.
BOOL CSeismicData::ReadZimsFile_EV(CString &suppname, int hnum, UBYTE dtype, int horizon)
{
#ifndef VM64

	// EarthVision variables
	char *desc;
	float *grid;
	int xcol, yrow, zlev;
	double xmin, xmax, ymin, ymax, zmin, zmax;
	float nullVal;
	char *datFile, *datField, *vfltFile, *nvfltFile;
	int status;
	int projection, zone, units, zUnits, pUnits;
	double GCTPparms[15];
	//---
	CString fname;
	char filename[256], gridname[256], *ps;
	CZimsGrid *zimsgrid; // the new grid we write data into
	double xlim[2], ylim[2];
	double **zgrid;
	int offset, i, j;

	fname = pDoc->ZimsDir + suppname;
	strcpy(filename, (const char *)fname);

	// turn off the variables we don't need returned from the EV file
	desc = NULL; // no description
	datFile = NULL; // no details about data files
	datField = NULL;

	if (strstr(suppname, ".2grd") || strstr(suppname, ".3grd"))
	{
		// read in 2D fields
		if (ps = (char *)strstr(suppname, ".2grd"))
		{
			status = gridRead2d(filename, &desc,
				&grid, &xcol, &yrow,
				&xmin, &xmax, &ymin, &ymax,
				&nullVal,
				&datFile, &datField,
				&vfltFile, &nvfltFile,
				&projection, &zone, &units, GCTPparms, &zUnits);
			strncpy(gridname, suppname, ps - suppname);
			gridname[ps - suppname] = '\0';
		}
		else if (ps = (char *)strstr(suppname, ".3grd"))
		{
			// read in 3d fields
			status = gridRead3d(filename, &desc,
				&grid, &xcol, &yrow, &zlev,
				&xmin, &xmax, &ymin, &ymax, &zmin, &zmax,
				&nullVal,
				&datFile, &datField,
				&projection, &zone, &units, GCTPparms,
				&zUnits, &pUnits);
			if (zlev < horizon)
			{
				pDoc->Error("Warning, an error occurred trying to read " + suppname +
					" More horizons were requested than were available in the file.",
					MSG_ERR);
				return FALSE;
			}
			strncpy(gridname, suppname, ps - suppname);
			gridname[ps - suppname] = '\0';
		}
		if (status)
		{
			pDoc->Error("Warning, an error occurred trying to read " + suppname, MSG_ERR);
			return FALSE;
		}
		// now put the data into the grid
		zimsgrid = new CZimsGrid(pDoc);
		zimsgrid->Init(yrow, xcol, pDoc, dtype, nullVal);
		zimsgrid->Stamp();
		xlim[0] = xmin; xlim[1] = xmax;
		ylim[0] = ymin; ylim[1] = ymax;
		zimsgrid->WriteLimits(xlim, ylim);
		zgrid = zimsgrid->GetGrid();

		// copy the actual data in
		// data is ordered as; bottom left as (xmin, ymin), top right (xmax, ymax)
		if (strstr(suppname, ".2grd"))
		{
			for (i = 0; i < yrow; i++)
				for (j = 0; j < xcol; j++)
					zgrid[yrow - 1 - i][j] = grid[j + i * xcol];
		}
		else
		{
			offset = horizon * xcol*yrow; // this assumes horizons start at 0!!
			for (i = 0; i < yrow; i++)
				for (j = 0; j < xcol; j++)
					zgrid[yrow - 1 - i][j] = grid[j + i * xcol + offset];
		}

		// Add grid to Zimscube
		zimsgrid->CalcAverage();
		zimsgrid->UseProperZnon();
		zimsgrid->RoundAllGridnodes();
		pDoc->zimscube.Add(zimsgrid, CString(gridname), horizon); // horizon here is wrong!!!

		// tidy up (free mem. allocated by EV fns)
		free(grid);
		free(vfltFile); free(nvfltFile);

	}
	else if (strstr(suppname, ".vflt"))
	{
		CFaultGrid *faultgrid = NULL;
		ifstream infile;
		char inputbuf[CSD_MAXLEN];
		double x0, x1, y0, y1;
		int fltnum, fltnum1;

		faultgrid = new CFaultGrid(pDoc);
		faultgrid->Init(pDoc, ZIMS_VAL_ZNON);
		pDoc->faultcube.Add(faultgrid, CString(suppname), horizon);

		// Initialise the first fault line
		x0 = CSD_DONTKNOW;
		y0 = CSD_DONTKNOW;
		fltnum = 0;

		// open file and start reading
		infile.open(fname);
		while (!infile.eof())
		{
			do
			{
				infile.getline(inputbuf, CSD_MAXLEN - 1);
			} while (inputbuf[0] == '#');// skip comments
			// extract coords from input, we might have more than one fault per file
			sscanf(inputbuf, "%lf %lf %d", &x1, &y1, &fltnum1);
			if (fltnum1 != fltnum)
				fltnum = fltnum1; // new fault line
			else
				faultgrid->AddFaultLine(x0, y0, x1, y1);
			x0 = x1;
			y0 = y1;
		}
		infile.close();
	}
	else if (strstr(suppname, ".nvflt"))
	{
		CFaultGrid *faultgrid = NULL;
		ifstream infile;
		char inputbuf[CSD_MAXLEN];
		double x0, x1, y0, y1;

		faultgrid = new CFaultGrid(pDoc);
		faultgrid->Init(pDoc, ZIMS_VAL_ZNON);
		pDoc->faultcube.Add(faultgrid, CString(suppname), horizon);

		// Initialise the first fault line
		x0 = CSD_DONTKNOW;
		y0 = CSD_DONTKNOW;

		// open file and start reading
		infile.open(fname);
		while (!infile.eof())
		{
			do// skip comments line at top
			{
				infile.getline(inputbuf, CSD_MAXLEN - 1);
			} while (inputbuf[0] == '#');
			if (strstr(inputbuf, "POLY"))// found the start of a new fault line
			{
				infile.getline(inputbuf, CSD_MAXLEN - 1);
				sscanf(inputbuf, "%lf %lf", &x0, &y0); // set start coords
				infile.getline(inputbuf, CSD_MAXLEN - 1);
			}
			sscanf(inputbuf, "%lf %lf", &x1, &y1);
			faultgrid->AddFaultLine(x0, y0, x1, y1);
			x0 = x1;
			y0 = y1;
		}
		infile.close();
	}
	else
	{
		pDoc->Error("Could not open seismic file " + suppname, MSG_ERR);
		return FALSE;
	}

	return TRUE;

#else

	pDoc->Error("Earthvision format files not supported in 64bit version of VelocityManager", MSG_ERR);
	return FALSE;

#endif
}

// read Zmap ISO format files.
BOOL CSeismicData::ReadZimsFile_ZmapISO(CString &suppname, int hnum, UBYTE dtype, int horizon)
{
	CFaultGrid *fgrid = NULL;
	char filename[256], inputbuf[CSD_MAXLEN];
	CString gridname;
	char cznon[64], fieldtype[8];
	//char *ps;
	ifstream infile;
	int nperline, fieldwidth, decloc, startloc, nrows, ncols;
	double znon, xmin, xmax, ymin, ymax;
	int xflt, yflt, segid, ncount, finx, ftype;
	CZimsGrid *zimsgrid;
	double xlim[2], ylim[2];
	double **zgrid;
	int i, j, fltnum, fltnum1;
	double x0, y0, x1, y1, f;

	strcpy(filename, (const char *)pDoc->ZimsDir + suppname);

	// open file and read the header
	infile.open(filename);
	do
	{
		infile.getline(inputbuf, CSD_MAXLEN - 1);
	} while (inputbuf[0] == '!');// skip comments
	// read header
	// if reading a grid file, the format is: @"format name", "file type", "nodes per line", HEADER (export only)
	// if reading fault file, the format is: @"format name", "file type", "line length", "no. of lines", HEADER
	if (inputbuf[0] == '@')
	{
		// the [] format means read up to but not including the next comma.
		// skip the initial "format name" part.. we don't need it. Also, if the
		// grid name has spaces in it, it makes it hard to read as a string.
		sscanf(strchr(inputbuf, ',') + 1, "%4s, %d", fieldtype, &nperline);
	}
	if (!strncmp(fieldtype, "GRID", 4))
	{
		char *p;

		// GRID file
		// now read the rest of the header
		// remove the commas from the input string so the sscanf works properly
		// note that if we do this the blank string 'cznon' becomes unreadable so
		// just set cznon to be blank. we don't use it anyway.
		infile.getline(inputbuf, CSD_MAXLEN - 1);
		while (p = strchr(inputbuf, ','))
			*p = ' ';
		sscanf(inputbuf, "%d %lf %d %d", &fieldwidth, &znon, &decloc, &startloc);
		cznon[0] = '\0';
		infile.getline(inputbuf, CSD_MAXLEN - 1);
		while (p = strchr(inputbuf, ','))
			*p = ' ';
		sscanf(inputbuf, "%d %d %lf %lf %lf %lf", &nrows, &ncols, &xmin, &xmax, &ymin, &ymax);
		// skip 3rd header line and header delimiter
		infile.getline(inputbuf, CSD_MAXLEN - 1);
		infile.getline(inputbuf, CSD_MAXLEN - 1);

		if (inputbuf[0] != '@')
		{
			AfxMessageBox("Warning, an error occurred whilst reading the file " + suppname +
				". The header format was not as expected. File could not be read.",
				MB_OK);
			return FALSE;
		}

		// now we're in the data section
		zimsgrid = new CZimsGrid(pDoc);
		zimsgrid->Init(nrows, ncols, pDoc, dtype, znon);
		zimsgrid->Stamp();
		xlim[0] = xmin; xlim[1] = xmax;
		ylim[0] = ymin; ylim[1] = ymax;
		zimsgrid->WriteLimits(xlim, ylim);
		zgrid = zimsgrid->GetGrid();

		// read the grid values
		// file ordered such that data is read column by column;
		// innermost loop should be over rows.
		for (j = 0; j < ncols; j++)
			for (i = 0; i < nrows; i++)
				infile >> zgrid[i][j];

		// Add grid to Zimscube
		zimsgrid->CalcAverage();
		zimsgrid->UseProperZnon();
		zimsgrid->RoundAllGridnodes();

		gridname = suppname.Left(suppname.ReverseFind('.')); // remove file extension to make grid name
		pDoc->zimscube.Add(zimsgrid, gridname, horizon);

		// old way of doing it
		//ps = (char *)strstr(suppname, ".dat"); // remove file extension to make grid name
		//strncpy(gridname, suppname, ps-suppname);
		//gridname[ps-suppname] = '\0';
		//pDoc->zimscube.Add(zimsgrid, CString(gridname), horizon);
	}
	else if (!strncmp(fieldtype, "FALT", 4))
	{
		// FAULT file
		// format of header is:
		// field name, field index, field type, rec. number, starting location, ending location,
		// decimal location, ZNON, character znon, printer field width,
		// printer decimal location and sort index.
		// There may be multiple lines of this type in the header; one entry for each field
		// associated with the fault e.g. some faults may only have x, y & seg. id. Other files
		// may have a z value in there too.
		// We need to check the field type as we need the X (type=1), Y (2) and Segment ID (35)

		// should really check that nperline = 1, as that's what the following code assumes.

		xflt = 0; // initialise positions of the fault info we need
		yflt = 0;
		segid = 0;
		ncount = 0;
		infile.getline(inputbuf, CSD_MAXLEN - 1);
		while (inputbuf[0] != '@')
		{
			if (inputbuf[0] == '@') break; // hit end of header line
			ncount++;
			sscanf(strchr(inputbuf, ',') + 1, "%d, %d, %*d, %*d, %*d, %*d, %lf", // skip to first comma
				&finx, &ftype, &znon);
			if (ftype == 1)
				xflt = finx;
			else if (ftype == 2)
				yflt = finx;
			else if (ftype == 35)
				segid = finx;
			infile.getline(inputbuf, CSD_MAXLEN - 1);
		}

		// if we get to here, we've found the end of header marker

		fgrid = new CFaultGrid(pDoc);
		fgrid->Init(pDoc, ZIMS_VAL_ZNON);
		pDoc->faultcube.Add(fgrid, CString(suppname), horizon);

		// initialise the first fault line
		x0 = CSD_DONTKNOW;
		y0 = CSD_DONTKNOW;
		fltnum = 0;

		// now read all fault lines
		// can't use a scanf here because we might have variable
		// values per line
		do
		{
			for (i = 1; i <= ncount; i++)// read each line
			{
				infile >> f;
				if (i == xflt)
					x1 = f;
				else if (i == yflt)
					y1 = f;
				else if (i == segid)
					fltnum1 = (int)f;
			}
			if (fltnum1 != fltnum)
				fltnum = fltnum1;
			else
				fgrid->AddFaultLine(x0, y0, x1, y1);
			x0 = x1;
			y0 = y1;
		} while (!infile.eof());
	}
	else
	{
		AfxMessageBox("Warning, an error occurred whilst reading the file " + suppname +
			". Unrecognised file type in header : " + fieldtype, MB_OK);
		return FALSE;
	}
	infile.close();

	return TRUE;
}

//***********************************************************************
// Subroutine : ReadZimsFile_CP3(), CP3 version
// Arguments : filename : full name of file to be loaded
// hnum : record number to be read
// Use : Read Seismic Data into internal data base from a
// Zims datafile
// Returns : void
//***********************************************************************
BOOL CSeismicData::ReadZimsFile_CP3(CString &filename, int hnum, UBYTE dtype, int horizon)
{
	ifstream cp3file;
	cp3_file_head filehead;
	char dataname[128], buffer[128];
	int namesize;
	int i, j, k;

#ifdef VELMAN_UNIX
	cp3file.open(pDoc->ZimsDir + filename);
#else
	cp3file.open(pDoc->ZimsDir + filename, ios::binary);
#endif

	if (cp3file.fail())
	{
		pDoc->Error("Internal error, file " + filename + " does not exist...", MSG_ERR);
		return FALSE;
	}

	// read the file header and the filename
	FortranRead(cp3file, (char *)&filehead, sizeof(cp3_file_head));
	namesize = FortranRead(cp3file, dataname, 80);

	// Check if this is a CP3 with a weird namefield and Unpad accordingly...
	if (namesize != 0)
	{
		sprintf(buffer, "File %s has nonstandard namefield size", (LPCTSTR)filename);
		pDoc->Log(buffer, LOG_DETAIL);
		UnPad(dataname, namesize);
	}
	else
	{
		UnPad(dataname, 80);
	}

	if (cp3file.fail())
	{
		pDoc->Error("Corrupt CP3file : " + filename + " -- cannot read header data ", MSG_ERR);
		return FALSE;
	}

	// invert data if reading with INTEL
#ifndef VELMAN_UNIX
	InvertData((char *)&filehead, cp3_file_head_sec);
#endif
	switch (filehead.itype)
	{
	case CP3_TYPE_GRID:
	{
		cp3_grid gridhead;
		CP3WORD cp3colsize;
		float **fgrid = NULL, *cp3col = NULL;
		CZimsGrid *zimsgrid;
		int znonnum;
		double xlim[2], ylim[2];

		// allocate zimsgrid structure
		zimsgrid = new CZimsGrid;
		// define column byte reversal block
#ifndef VELMAN_UNIX
		int cp3col_sec[3];
		cp3col_sec[1] = sizeof(float);
		cp3col_sec[2] = 0;
#endif

		FortranRead(cp3file, (char *)&gridhead, sizeof(cp3_grid));

		// invert data if reading with INTEL
#ifndef VELMAN_UNIX
		InvertData((char *)&gridhead, cp3_grid_sec);
#endif

		// convert limits to double
		xlim[0] = (double)gridhead.xmin;
		xlim[1] = (double)gridhead.xmax;
		ylim[0] = (double)gridhead.ymin;
		ylim[1] = (double)gridhead.ymax;
		// Fill zimsgrid with values from the file.
		zimsgrid->Init(gridhead.nrow, gridhead.ncol, pDoc, dtype, gridhead.findt);
		zimsgrid->Stamp(); //!!tp100 I think this makes sense?
		zimsgrid->WriteLimits(xlim, ylim);
		// allocate memory to read floating point numbers
		fgrid = matrix(0, gridhead.ncol - 1, 0, gridhead.nrow - 1);
		cp3col = vector(0, gridhead.nrow - 1);
		// read and ignore z bias values for cp3_grid
		FortranRead(cp3file, (char *)&cp3colsize, sizeof(CP3WORD));

#ifndef VELMAN_UNIX
		InvertData((char *)&cp3colsize, one_word_sec);
#endif

		cp3file.ignore(cp3colsize * sizeof(float) + 8);
		// Now read real columns
		for (j = 0; j < gridhead.ncol; j++)
		{
			FortranRead(cp3file, (char *)&cp3colsize, sizeof(CP3WORD));

#ifndef VELMAN_UNIX
			InvertData((char *)&cp3colsize, one_word_sec);
#endif
			FortranRead(cp3file, (char *)cp3col, cp3colsize * sizeof(float));
			// invert column data if reading with INTEL
#ifndef VELMAN_UNIX
			cp3col_sec[0] = cp3colsize;
			InvertData((char *)cp3col, cp3col_sec);
#endif

			// decompress the grid data...
			k = 0;
			for (i = 0; i < cp3colsize; i++)
			{
				if (cp3col[i] != -gridhead.findt)
				{
					if (k < gridhead.nrow)
						fgrid[j][k] = cp3col[i];
					k++;
				}
				else
				{
					i++;
					znonnum = (int)cp3col[i];
					while (znonnum > 0)
					{
						if (k < gridhead.nrow)
							fgrid[j][k] = gridhead.findt;
						k++;
						znonnum--;
					}
				}
			}
		}

		// store data in the double internal array
		for (i = 0; i < gridhead.nrow; i++)
			for (j = 0; j < gridhead.ncol; j++)
				((double **)(*zimsgrid))[i][j] = (double)fgrid[j][i];

		// add grid to Zimscube.
		zimsgrid->CalcAverage();
		zimsgrid->UseProperZnon();
		zimsgrid->RoundAllGridnodes();
		pDoc->zimscube.Add(zimsgrid, CString(dataname), horizon);

		// free memory
		free_matrix(fgrid, 0, gridhead.ncol - 1, 0, gridhead.nrow - 1);
		free_vector(cp3col, 0, gridhead.nrow - 1);
		break;
	}

	case CP3_TYPE_LINE:
	{
		cp3_control controlhead;
		char subname[256];
		int namestart;
		float *cp3block;
		CString name;
		double x, y, xoff, yoff;
		float soff;
		int spt, lenrec; // nline;
		BOOL first;

		FortranRead(cp3file, (char *)&controlhead, sizeof(cp3_control));

		// Invert data if reading with INTEL
#ifndef VELMAN_UNIX
		InvertData((char *)&controlhead, cp3_control_sec);
		int cp3data_sec[3];
		cp3data_sec[1] = 4;
		cp3data_sec[2] = 0;
#endif
		// check if all independant variables are there
		// return if not.
		if (controlhead.nidatt < 2)
			return FALSE;
		// maximum size of a block is 0x400 records
		cp3block = new float[4 * 256];
		// read and invert the length of the next data record
		FortranRead(cp3file, (char *)&lenrec, sizeof(CP3WORD));
#ifndef VELMAN_UNIX
		InvertData((char *)&lenrec, one_word_sec);
#endif
		for (i = 0; i < filehead.nsubs; i++)
		{
			// 1st record is a name of length 128 (negative length)
			lenrec = -lenrec;
			// ignore the subset name
			FortranRead(cp3file, subname, lenrec);
			UnPad(subname, lenrec);
			for (namestart = 0; subname[namestart] != 0x00; namestart++)
			{
				if (subname[namestart] == '>') break;
			}
			// This is now the proper linename
			// sk126 name = KillWhite(subname+namestart+1);
			name = subname + namestart + 1;
			// sk126 nline = ExtractLineNumber(name);
			spt = 1;// new line so set shotpoint to 1
			// then comes the bias field and data...
			// read first the new length of the record
			FortranRead(cp3file, (char *)&lenrec, sizeof(CP3WORD));
#ifndef VELMAN_UNIX
			InvertData((char *)&lenrec, one_word_sec);
#endif
			// treat first record specially
			first = TRUE;
			// if lenrec < 0 then there is a new subsection
			while (lenrec > 0)
			{
				// read and invert the raw data...
				FortranRead(cp3file, (char *)cp3block, lenrec * sizeof(float));
#ifndef VELMAN_UNIX
				cp3data_sec[0] = lenrec;
				InvertData((char *)cp3block, cp3data_sec);
#endif
				// first record contains all the offsets
				if (first)
				{
					xoff = cp3block[0];
					yoff = cp3block[1];
					// check if there is time/depth
					if (controlhead.nhdatt >= 1)
						soff = cp3block[2];
					j = controlhead.nidatt + controlhead.nhdatt;
					first = FALSE;
				}
				else
					j = 0;
				while (j < lenrec)
				{
					x = cp3block[j] + xoff;
					y = cp3block[j + 1] + yoff;
					if (controlhead.nhdatt >= 1)
						spt = (int)(cp3block[j + 2] + soff + 0.1f);
					else
						spt = -1;
					// add shotpoint
					AddNewPt(name, -1, spt, dtype, x, y, CSD_DONTKNOW);
					j += (controlhead.nidatt + controlhead.nhdatt);
				}
				FortranRead(cp3file, (char *)&lenrec, sizeof(CP3WORD));
#ifndef VELMAN_UNIX
				InvertData((char *)&lenrec, one_word_sec);
#endif
			}
		}
		// free memory
		delete cp3block;
		break;
	}

	case CP3_TYPE_FAULT:
	{
		cp3_fault faulthead;
		CP3WORD lenrec;
		char subname[256];
		int nattr;
		float *cp3block;
		CFaultGrid *faultgrid;
		double xlim[2], ylim[2];
		double x0, x1, y0, y1, xoff, yoff;
		BOOL first;
		FortranRead(cp3file, (char *)&faulthead, sizeof(cp3_fault));
		// invert data if reading with INTEL
#ifndef VELMAN_UNIX
		InvertData((char *)&faulthead, cp3_fault_sec);
		int cp3data_sec[3];
		cp3data_sec[1] = 4;
		cp3data_sec[2] = 0;
#endif
		// as this is polygon data, allocate the polygon grid
		// and store it inside the faultcube
		faultgrid = new CFaultGrid(pDoc);
		faultgrid->Init(pDoc, ZIMS_VAL_ZNON);//!! sk126 this seems not to be stored in file - check
		// convert limits to double
		xlim[0] = (double)faulthead.xmin;
		xlim[1] = (double)faulthead.xmax;
		ylim[0] = (double)faulthead.ymin;
		ylim[1] = (double)faulthead.ymax;
		// write limits and add to the faultcube...
		faultgrid->WriteLimits(xlim, ylim);
		pDoc->faultcube.Add(faultgrid, CString(dataname), horizon);
		// number of attributes per record
		nattr = faulthead.iafatt + 2;
		// maximum size of a block is 0x100 records
		cp3block = new float[nattr * 256];
		// read and invert the length of the next data record
		FortranRead(cp3file, (char *)&lenrec, sizeof(CP3WORD));
#ifndef VELMAN_UNIX
		InvertData((char *)&lenrec, one_word_sec);
#endif
		for (i = 0; i < filehead.nsubs; i++)
		{
			// 1st record is a name of length 128 (negative length)
			lenrec = -lenrec;
			// ignore the subset name
			FortranRead(cp3file, subname, lenrec);
			// then comes the bias field and data...
			FortranRead(cp3file, (char *)&lenrec, sizeof(CP3WORD));
#ifndef VELMAN_UNIX
			InvertData((char *)&lenrec, one_word_sec);
#endif
			// first record must be treated specially
			first = TRUE;
			while (lenrec > 0)
			{
				// read and invert the raw data...
				FortranRead(cp3file, (char *)cp3block, lenrec * sizeof(float));
#ifndef VELMAN_UNIX
				cp3data_sec[0] = lenrec;
				InvertData((char *)cp3block, cp3data_sec);
#endif
				// dont add first point...
				x1 = CSD_DONTKNOW;
				y1 = CSD_DONTKNOW;
				// If this is the first block, get the offsets from
				// the first two data points
				if (first)
				{
					xoff = cp3block[0];
					yoff = cp3block[1];
					j = nattr;
					first = FALSE;
				}
				else
				{
					j = 0;
				}
				// now add all the faultlines in this polygon
				// ignoring the throw and elevation information
				while (j < lenrec)
				{
					x0 = x1;
					y0 = y1;
					x1 = (double)(cp3block[j] + xoff);
					y1 = (double)(cp3block[j + 1] + yoff);
					// elevation and throw are ignored if nattr != 2
					if (x0 != CSD_DONTKNOW && y0 != CSD_DONTKNOW)
						faultgrid->AddFaultLine(x0, y0, x1, y1);
					j += nattr;
				}
				FortranRead(cp3file, (char *)&lenrec, sizeof(CP3WORD));
#ifndef VELMAN_UNIX
				InvertData((char *)&lenrec, one_word_sec);
#endif
			}
		}
		// free memory
		delete cp3block;
		break;
	}
	default: // no valid filetype
		pDoc->Error("CP3 file: " + CString(dataname) + " has unknown filetype", MSG_ERR);
		return FALSE;
	}
	return TRUE;
}

//***********************************************************************
// Subroutine : ReadZimsFile_CP3int(), CP3 version, internal
// Arguments : filename : full name of file to be loaded
// hnum : record number to be read
// Use : Read Seismic Data into internal data base from a
// Zims datafile
// Returns : TRUE or FALSE
//***********************************************************************
BOOL CSeismicData::ReadZimsFile_CP3int(CString &filename, int hnum,
	UBYTE dtype, int horizon)
{
	int nextlength, totallength, parmlen;
	int i, j;
	BOOL status = FALSE;
	char *errmsg = NULL;

	//open CPS-3 internal data file
#ifdef VELMAN_UNIX
	ifstream cp3file(pDoc->ZimsDir + filename);
#else
	ifstream cp3file(pDoc->ZimsDir + filename, ios::binary);
#endif
	if (cp3file.fail())
	{
		pDoc->Error("Could not open " + filename, MSG_ERR);
		return FALSE;
	}

	// skip over magic number header
	cp3file.seekg(0x10, ios::beg);

	// skip over not used section + LOCK section
	cp3file.read((char *)&nextlength, sizeof(int));
#ifndef VELMAN_UNIX
	InvertData((char *)&nextlength, one_word_sec);
#endif
	cp3file.seekg(nextlength + 0x10, ios::cur);

	// skip over comment section and MAIN section
	cp3file.read((char *)&nextlength, sizeof(int));
#ifndef VELMAN_UNIX
	InvertData((char *)&nextlength, one_word_sec);
#endif
	cp3file.seekg(nextlength + 0x10, ios::cur);

	// switch according to filetype
	switch (toupper(filename.GetAt(filename.GetLength() - 4)))
	{
	case 'S':
	{ // this is a grid file
		char dataname[512];
		char *gridname = NULL, *dataindex = NULL;
		int rows, columns;
		CZimsGrid *zimsgrid = NULL;
		float **fgrid = NULL;
		double xlim[2];
		double ylim[2];
		double znon;

		// skip over SUBS section
		cp3file.read((char *)&nextlength, sizeof(int));
		cp3file.seekg(0x10, ios::cur);

		// skip over N char section section and read total datalength
		cp3file.read((char *)&totallength, sizeof(int));
#ifndef VELMAN_UNIX
		InvertData((char *)&totallength, one_word_sec);
#endif
		cp3file.seekg(0x01, ios::cur);

#ifndef VELMAN_UNIX
		int cp3_col_sec[3];
		cp3_col_sec[1] = sizeof(float);
		cp3_col_sec[2] = 0;
#endif

		// This is now the ZimsName
		cp3file.read((char *)&nextlength, sizeof(int));
#ifndef VELMAN_UNIX
		InvertData((char *)&nextlength, one_word_sec);
#endif
		cp3file.read(dataname, nextlength);

		// Read filename and unpad
		j = 0;
		while (j < (int)strlen(dataname))
		{
			if (dataname[j] == '>') break;
			j++;
		}
		gridname = dataname + j + 1;

		// now get number of columns
		cp3file.seekg(0x01, ios::cur);
		cp3file.read((char *)&columns, sizeof(int));
#ifndef VELMAN_UNIX
		InvertData((char *)&columns, one_word_sec);
#endif
		// calculate number of rows and columns from length variables
		rows = (totallength - nextlength - 5) / (1 + sizeof(int) + columns);
		columns /= sizeof(float);
		// back up for read the file now that columns and rows are known
		cp3file.seekg(-0x05, ios::cur);

		// allocate temporary matrix for floating point values
		fgrid = matrix(0, rows - 1, 0, columns - 1);
		// if little endian system, invert byte order
#ifndef VELMAN_UNIX
		cp3_col_sec[0] = columns;
#endif
		// now read the data of the grid
		for (i = 0; i < rows; i++)
		{
			// skip 0x5a byte and record length
			cp3file.seekg(0x05, ios::cur);
			// read column from file...
			cp3file.read((char *)fgrid[i], columns * sizeof(float));
			// if little endian float system invert byte order
#ifndef VELMAN_UNIX
			InvertData((char *)fgrid[i], cp3_col_sec);
#endif
		}
		// skip PARM header and read index data length
		cp3file.seekg(0x10, ios::cur);
		cp3file.read((char *)&nextlength, sizeof(int));
#ifndef VELMAN_UNIX
		InvertData((char *)&nextlength, one_word_sec);
#endif
		// extract data from dataindex section
		dataindex = new char[nextlength];
		parmlen = nextlength;
		cp3file.read(dataindex, nextlength);

		// see which file version we are dealing with. 27/3/00
		// Change happened around early 2000. Old files use {X/Y}, new use {U/V}
		// A much better way to handle this would be test the file version number....
		// ... if only I knew where that was!

		xlim[0] = FindCPSVar("X1SLIM=", dataindex, nextlength, &status);
		if (!status)
			xlim[0] = FindCPSVar("U1SLIM=", dataindex, nextlength, &status);
		if (status) // appear to have found a new format .scps file
			pDoc->Log("File : " + filename + " appears to be new style CPS3 file.", LOG_DETAIL);
		if (!status && !errmsg)
			errmsg = "Problem reading file: could not find x limit marker X1SLIM or U1SLIM";
		xlim[1] = FindCPSVar("X2SLIM=", dataindex, nextlength, &status);
		if (!status)
			xlim[1] = FindCPSVar("U2SLIM=", dataindex, nextlength, &status);
		if (!status && !errmsg)
			errmsg = "Problem reading file: could not find x limit marker X2SLIM or U2SLIM";
		ylim[0] = FindCPSVar("Y1SLIM=", dataindex, nextlength, &status);
		if (!status)
			ylim[0] = FindCPSVar("U3SLIM=", dataindex, nextlength, &status);
		if (!status && !errmsg)
			errmsg = "Problem reading file: could not find y limit marker Y1SLIM or U3SLIM.";
		ylim[1] = FindCPSVar("Y2SLIM=", dataindex, nextlength, &status);
		if (!status)
			ylim[1] = FindCPSVar("U4SLIM=", dataindex, nextlength, &status);
		if (!status && !errmsg)
			errmsg = "Problem reading file: could not find y limit marker Y2SLIM or U4SLIM.";
		znon = FindCPSVar("RISASC=", dataindex, nextlength, &status);
		if (!status)
		{
			znon = 1.0e+30;
			status = TRUE;
			// znon = FindCPSVar(...)
		}
		if (!status && !errmsg)
			errmsg = "Problem reading file: could not find znon marker RISASC.";
		if (errmsg)
		{
			pDoc->Log(errmsg, LOG_BRIEF);
			pDoc->Log("Read of file abandoned.", LOG_BRIEF);
			pDoc->Error("The file could not be read because an error occurred."
				"See log file for more details.\n"
				"Reading " + filename + " abandoned.", MSG_ERR);
			return FALSE;
		}

		// initialize zimsgrid
		zimsgrid = new CZimsGrid;
		// the CPS format is a columns oriented format -- beware
		zimsgrid->Init(columns, rows, pDoc, dtype, znon);

		// copy float to double values and transpose while
		// we are at it...
		for (i = 0; i < rows; i++)
			for (j = 0; j < columns; j++)
				zimsgrid->GetGrid()[j][i] = (double)fgrid[i][j];

		// free temporary matrix
		free_matrix(fgrid, 0, rows - 1, 0, columns - 1);

		// add grid to zimscube
		zimsgrid->WriteLimits(xlim, ylim);
		zimsgrid->CalcAverage();
		zimsgrid->UseProperZnon();
		zimsgrid->RoundAllGridnodes();
		// sk126 -- gridname is read from filename in CPS3 not the internal
		// pDoc->zimscube.Add(zimsgrid, CString(gridname), horizon);
		// comment field
		pDoc->zimscube.Add(zimsgrid, filename.Left(filename.GetLength() - 5), horizon);
		zimsgrid->Stamp();
		//CopyParm(parmlen, dataindex); // only use this if writing new format scsp files. 01/08
		// free temporary data index storage
		delete dataindex;
		break;
	}

	case 'F':
	{ // this is a faultline file
		char dataname[512], subsparm[0x10];
		char *faultname = NULL, *dataindex = NULL;
		int points, recsize, recsize2;
		CFaultGrid *faultgrid = NULL;
		double xlim[2], ylim[2], x0, x1, y0, y1;
		double znon;
		char marker; // Used to check that file contents are as expected.
		int newrecsize; // 18/4/00. See comments at head of this function.
		BOOL newformat = FALSE; // Goes .T. if we've found a new format fault file.
		streampos datastart; // used to note file position at start of data section.

		// read SUBS or PARM section
		cp3file.read((char *)&nextlength, sizeof(int));
#ifndef VELMAN_UNIX
		InvertData((char *)&nextlength, one_word_sec);
#endif
		cp3file.read(subsparm, 0x10);

		// as this is polygon data, allocate the polygon grid
		// and store it inside the faultcube
		faultgrid = new CFaultGrid(pDoc);
		faultgrid->Init(pDoc, ZIMS_VAL_ZNON);

		// Data sections begin with 'SUBS'. Parameter sections begin with 'PARM'
		// Read data sections first.
		while (toupper(subsparm[0]) != 'P')
		{
			// skip over N char section section and read total datalength
			cp3file.read((char *)&totallength, sizeof(int));
#ifndef VELMAN_UNIX
			InvertData((char *)&totallength, one_word_sec);
#endif
			cp3file.seekg(0x01, ios::cur);

			// This is now the ZimsName
			cp3file.read((char *)&nextlength, sizeof(int));
#ifndef VELMAN_UNIX
			InvertData((char *)&nextlength, one_word_sec);
#endif
			cp3file.read(dataname, nextlength);

			// Read filename and unpad
			j = 0;
			while (j < (int)strlen(dataname))
			{
				if (dataname[j] == '>') break;
				j++;
			}
			faultname = dataname + j + 1;

			// now read current fault line
			// Save where we are as we're going to need read forward quite a bit
			// to check which file format we are dealing with.
			datastart = cp3file.tellg();
			cp3file.seekg(0x01, ios::cur);
			cp3file.read((char *)&recsize, sizeof(int));
#ifndef VELMAN_UNIX
			InvertData((char *)&recsize, one_word_sec);
#endif
			// check if record size is as expected
			if ((recsize / sizeof(double)) != 2)
			{
				pDoc->Error("Cannot read faultline data in internal CP3 file : " + filename +
					"\nEither the file is corrupt or this is an unknown filetype.", MSG_ERR);
				return FALSE;
			}

			// In early 2000, we found a new faultline format file had been introduced.
			// In this new format, after each of the pair of 64 bit floats, there was a marker
			// byte 'Z' (0x5A), followed by a 32bit integer (recsize) and 2 32bit floats. These
			// last 2 pair of floats are usually the NULL value (1.0e30) but not always. At
			// present I don't know what they represent.
			// I have not been able to find a way to determine a priori which file format we
			// have. So, we skip the first record and see if we have a 0x58 or 0x5A marker byte.

			// Compute the number of data records assuming old format in case we have only one
			// data point. Here totallength is the total no. of bytes, nextlength is the size of the
			// character section and 5 bytes comes from the marker byte + 4 bytes for the recsize.
			// We divide this by the total length of each record which will be 1 for the marker byte,
			// 4 for sizeof(int), the record size and the record size itself.
			points = (totallength - nextlength - 5) / (1 + sizeof(int) + recsize);

			// Skip forward to see if we have a new format file ie. the 5A marker byte comes next.
			if (points != 1)
			{
				cp3file.seekg(recsize, ios::cur);
				cp3file.read((char *)&marker, sizeof(char));
				if (marker == 'X')
					newformat = FALSE; // old format file
				else if (marker == 'Z')
					newformat = TRUE; // new format file
				else
				{
					pDoc->Error("Unexpected data in CP3 file : " + filename +
						"\nEither the file is corrupt or this format cannot be understood.", MSG_ERR);
					return FALSE;
				}
			}
			else
			{
				newformat = FALSE;
			}

			// If it's a new format file then the number of datapoints has to be
			// recomputed. We assume that the no. of bytes for all the data section is
			// totallength-nextlength-5 (ie. nextlength is length of character header), 5 comes
			// from the 1 byte marker, followed by the 4 byte integer used to hold the value of
			// nextlength. Then divide this by the total size of each record which will be the sum
			// of 2 single byte markers, 2 x 32bit integers for the recsize variables, 2 x 64 bit
			// floats for the x & y values and 2 x 32bit floats for the 32bit floats (the purpose
			// of which are currently unknown).
			if (newformat)
			{
				cp3file.read((char *)&recsize2, sizeof(int));
#ifndef VELMAN_UNIX
				InvertData((char *)&recsize2, one_word_sec);
#endif
				// check recsize is 2x32bit floats
				if ((recsize2 / sizeof(float)) != 2)
				{
					pDoc->Error("Cannot read faultline data in internal CP3 file : " + filename +
						"\nEither the file is corrupt or this is an unknown filetype.", MSG_ERR);
					return FALSE;
				}
				points = (totallength - nextlength - 5) / (1 + sizeof(int) + recsize + 1 + sizeof(int) + recsize2);
			}

			// back up for read the file now that columns and rows are known.
			// should go back to where the first 0x58 byte marker was.
			cp3file.seekg(datastart);

			// initialize loop
			x0 = CSD_DONTKNOW;
			y0 = CSD_DONTKNOW;

			for (j = 0; j < points; j++)
			{
				// now read current fault line
				cp3file.read((char *)&marker, sizeof(char)); // read the '0x58' marker
				cp3file.read((char *)&newrecsize, sizeof(int)); // should get the same as recsize
#ifndef VELMAN_UNIX
				InvertData((char *)&newrecsize, one_word_sec);
#endif
				if (marker != 'X' || newrecsize != recsize)// 'X' = 0x58.
				{
					pDoc->Error("Unexpected data in CP3 file : " + filename +
						"\nEither the file is corrupt or this format cannot be understood.", MSG_ERR);
					return FALSE;
				}
				// read the current point along the faultline
				cp3file.read((char *)&x1, sizeof(double));
				cp3file.read((char *)&y1, sizeof(double));
				// convert byte order if necessary
#ifndef VELMAN_UNIX
				InvertData((char *)&x1, one_dword_sec);
				InvertData((char *)&y1, one_dword_sec);
#endif

				// add new faultline pair
				if (x0 != CSD_DONTKNOW && y0 != CSD_DONTKNOW)
				{
					faultgrid->AddFaultLine(x0, y0, x1, y1);
				}

				// propagate line
				x0 = x1;
				y0 = y1;

				// If reading new format, then check and skip the section with 2x32bit floats.
				if (newformat)
				{
					cp3file.read((char *)&marker, sizeof(char)); // read the '0x5A' marker
					cp3file.read((char *)&newrecsize, sizeof(int)); // should get same as recsize2
#ifndef VELMAN_UNIX
					InvertData((char *)&newrecsize, one_word_sec);
#endif
					if (marker != 'Z' || newrecsize != recsize2)// 'Z' = 0x5A
					{
						pDoc->Error("Unexpected data in CP3 file : " + filename +
							"\nEither the file is corrupt or this format cannot be understood.", MSG_ERR);
						return FALSE;
					}
					cp3file.seekg(recsize2, ios::cur); // skip 2 x 32bit floats
				}
			}

			// read SUBS or PARM section
			cp3file.read(subsparm, 0x10);
		}

		cp3file.read((char *)&nextlength, sizeof(int));
#ifndef VELMAN_UNIX
		InvertData((char *)&nextlength, one_word_sec);
#endif

		// extract data from dataindex section
		dataindex = new char[nextlength];

		// Note as above the file format changed early 2000. Old files use {X/Y}, new
		// use {U/V}.

		cp3file.read(dataindex, nextlength);
		znon = FindCPSVar("RIFASC=", dataindex, nextlength, &status);
		if (!status)
		{
			znon = 1.0e+30;
			status = TRUE;
			// znon = FindCPSVar
		}
		if (!status && !errmsg)
			errmsg = "Problem reading file: could not find znon marker RIFASC.";
		xlim[0] = FindCPSVar("X1FLIM=", dataindex, nextlength, &status);
		if (!status)
			xlim[0] = FindCPSVar("U1FLIM=", dataindex, nextlength, &status);
		if (!status && !errmsg)
			errmsg = "Problem reading file: could not find xlimit marker X1FLIM or U1FLIM.";
		xlim[1] = FindCPSVar("X2FLIM=", dataindex, nextlength, &status);
		if (!status)
			xlim[1] = FindCPSVar("U2FLIM=", dataindex, nextlength, &status);
		if (!status && !errmsg)
			errmsg = "Problem reading file: could not find xlimit marker X2FLIM or U2FLIM.";
		ylim[0] = FindCPSVar("Y1FLIM=", dataindex, nextlength, &status);
		if (!status)
			ylim[0] = FindCPSVar("U3FLIM=", dataindex, nextlength, &status);
		if (!status && !errmsg)
			errmsg = "Problem reading file: could not find ylimit marker Y1FLIM or U3FLIM.";
		ylim[1] = FindCPSVar("Y2FLIM=", dataindex, nextlength, &status);
		if (!status)
			ylim[1] = FindCPSVar("U4FLIM=", dataindex, nextlength, &status);
		if (!status && !errmsg)
			errmsg = "Problem reading file: could not find ylimit marker Y2FLIM or U3FLIM.";
		if (errmsg)
		{
			pDoc->Log(errmsg, LOG_BRIEF);
			pDoc->Log("Read of file abandoned.", LOG_BRIEF);
			pDoc->Error("The file could not be read because an error occurred."
				"See log file for more details."
				"Reading " + filename + " abandoned.", MSG_ERR);
			return FALSE;
		}

		// free temporary data index storage
		delete dataindex;

		// write limits and add to the faultcube...
		faultgrid->WriteLimits(xlim, ylim);
		// Faultgrids as well are added under the filename not the
		// internal grid comment
		// pDoc->faultcube.Add(faultgrid, faultname, horizon);
		pDoc->faultcube.Add(faultgrid, filename.Left(filename.GetLength() - 5), horizon);

		break;
	}

	case 'D':
	{ // this is a shotline location file
		char dataname[512], subsparm[0x10];
		char *linename = NULL, *dataindex = NULL;
		int points, isid, recsize1, recsize2, recsize3;
		double x, y, dsid, depth, xlim[2], ylim[2];

		// read SUBS or PARM section
		cp3file.read((char *)&nextlength, sizeof(int));
		cp3file.read(subsparm, 0x10);

		// read all lines in the data file
		while (toupper(subsparm[0]) != 'P')
		{
			// skip over N char section section and read total datalength
			cp3file.read((char *)&totallength, sizeof(int));
#ifndef VELMAN_UNIX
			InvertData((char *)&totallength, one_word_sec);
#endif
			cp3file.seekg(0x01, ios::cur);

			// This is now the ZimsName
			cp3file.read((char *)&nextlength, sizeof(int));
#ifndef VELMAN_UNIX
			InvertData((char *)&nextlength, one_word_sec);
#endif
			cp3file.read(dataname, nextlength);

			// Read filename and unpad
			j = 0;
			while (j < (int)strlen(dataname))
			{
				if (dataname[j] == '>') break;
				j++;
			}
			linename = dataname + j + 1;
			UnPad(linename, nextlength - j - 1);

			// now read position of current seismic line
			cp3file.seekg(0x01, ios::cur);
			cp3file.read((char *)&recsize1, sizeof(int));
#ifndef VELMAN_UNIX
			InvertData((char *)&recsize1, one_word_sec);
#endif

			// now read depth of current seismic line
			cp3file.seekg(recsize1 + 0x01, ios::cur);
			cp3file.read((char *)&recsize2, sizeof(int));
#ifndef VELMAN_UNIX
			InvertData((char *)&recsize2, one_word_sec);
#endif

			// now read shotline # of current seismic line
			cp3file.seekg(recsize2 + 0x01, ios::cur);
			cp3file.read((char *)&recsize3, sizeof(int));
#ifndef VELMAN_UNIX
			InvertData((char *)&recsize3, one_word_sec);
#endif

			// calculate number of shotpoints and columns from length variables
			points = (totallength - nextlength - 5) / (3 * (1 + sizeof(int)) + recsize1
				+ recsize2 + recsize3);
			recsize1 /= sizeof(double);
			recsize2 /= sizeof(float);
			recsize3 /= sizeof(double);

			// check if record size is as expected
			if (recsize1 != 2 || recsize2 != 1 || recsize3 != 1)
			{
				pDoc->Error("Corrupt internal CP3 file : " + filename +
					" - cannot read seismic data", MSG_ERR);
				return FALSE;
			}

			// back up for read the file now that columns and rows are known
			int ioffset = (int)0x0f + recsize1 * sizeof(double) + recsize2 * sizeof(float);
			cp3file.seekg(-ioffset, ios::cur);

			// read all shotpoint in this shotline
			for (j = 0; j < points; j++)
			{
				// now read current seismic line position data
				cp3file.seekg(0x05, ios::cur);// skip 0x58 id byte and record length
				// read the current point along the faultline
				cp3file.read((char *)&x, sizeof(double));
				cp3file.read((char *)&y, sizeof(double));
				// convert byte order if necessary
#ifndef VELMAN_UNIX
				InvertData((char *)&x, one_dword_sec);
				InvertData((char *)&y, one_dword_sec);
#endif

				// now read current seismic line depth data
				cp3file.seekg(0x05, ios::cur);// skip 0x5a id byte and record length
				// read the current point along the faultline
				cp3file.read((char *)&depth, sizeof(float));
				// convert byte order if necessary
#ifndef VELMAN_UNIX
				InvertData((char *)&depth, one_word_sec);
#endif

				// now read current seismic line shotpoint data
				cp3file.seekg(0x05, ios::cur);// skip 0x44 id byte and record length
				// read the current point along the faultline
				cp3file.read((char *)&dsid, sizeof(double));
				// convert byte order if necessary
#ifndef VELMAN_UNIX
				InvertData((char *)&dsid, one_dword_sec);
#endif
				isid = (int)dsid;

				AddNewPt(CString(linename), -1, isid, dtype, x, y, CSD_DONTKNOW);
			}
			// read SUBS or PARM section
			cp3file.read(subsparm, 0x10);
		}

		cp3file.read((char *)&nextlength, sizeof(int));
#ifndef VELMAN_UNIX
		InvertData((char *)&nextlength, one_word_sec);
#endif

		// extract data from dataindex section
		dataindex = new char[nextlength];
		cp3file.read(dataindex, nextlength);
		xlim[0] = FindCPSVar("X1DLIM=", dataindex, nextlength, &status);
		if (!status) errmsg = "Problem reading file: could not find xlimit marker X1DLIM.";
		xlim[1] = FindCPSVar("X2DLIM=", dataindex, nextlength, &status);
		if (!status && !errmsg)
			errmsg = "Problem reading file: could not find xlimit marker X2DLIM.";
		ylim[0] = FindCPSVar("Y1DLIM=", dataindex, nextlength, &status);
		if (!status && !errmsg)
			errmsg = "Problem reading file: could not find ylimit marker Y1DLIM.";
		ylim[1] = FindCPSVar("Y2DLIM=", dataindex, nextlength, &status);
		if (!status && !errmsg)
			errmsg = "Problem reading file: could not find ylimit marker Y2DLIM.";
		if (errmsg)
		{
			pDoc->Log(errmsg, LOG_BRIEF);
			pDoc->Log("Read of file abandoned.", LOG_BRIEF);
			pDoc->Error("The file could not be read because an error occurred."
				"See log file for more details.\n"
				"Reading " + filename + " abandoned.", MSG_ERR);
			return FALSE;
		}

		// free temporary data index storage
		delete dataindex;
		break;
	}
	}

	return TRUE;
}

//***********************************************************************
// Subroutine : FindCPSVar()
// Arguments : field : character identifier of field
// dataindex : character array of all fields
// length : length of dataindex file
// Use : extracts the field value requested from a CPS
// dataindex
// Returns the double value requested
// If character identifier (field) is not found then
// status is returned FALSE.
// Modified code to return a status to indicate
// whether the variables was found or not. Previously no indication
// was given and velman would carry on regardless instead of aborting
// the import.
//***********************************************************************
double CSeismicData::FindCPSVar(const char *fieldname, char *dataindex,
	int length, BOOL *status)
{
	int i = 0;
	int fieldsize;
	char *cfield = (char *)&fieldsize;
	char currentfield[128];
	double value = ZIMS_VAL_ZNON; // initialise to NULL

	*status = FALSE; // goes TRUE once value has been found

	// search through dataindex for correct field name
	while (i < length)
	{
		// copy field length into size variable and invert if necessary
		cfield[0] = 0x00;
		cfield[1] = 0x00;
		cfield[2] = dataindex[i];
		cfield[3] = dataindex[i + 1];
		i += 2;
#ifndef VELMAN_UNIX
		InvertData((char *)&fieldsize, one_word_sec);
#endif
		strncpy(currentfield, (const char *)dataindex + i, fieldsize);
		currentfield[fieldsize] = 0x00;

		// check if this is the correct field
		if (!strncmp(currentfield, fieldname, strlen(fieldname)))
		{
			value = atof(currentfield + strlen(fieldname));
			*status = TRUE;
			break;
		}
		i += fieldsize;
	}
	return(value);
}

//***********************************************************************
// Subroutine : MakeDOSName(), wrapper
// Arguments : zimsname : internal name in zimsdata base which
// has to be found by this routine
// dosname : CString Array of filenames to be scanned
// for the zimsname
// Use : Makes a DOS Name from a ZimsName
// Returns index : index to dosname array
// hnum : number of the header in the data base
//***********************************************************************
BOOL CSeismicData::MakeDOSName(CString &zimsname, CStringArray &dosname,
	int &index, int &hnum)
{
	switch (pDoc->GridFileType)
	{
	case 1:
		return MakeDOSName_ZMAP(zimsname, dosname, index, hnum);
	case 2:
		return MakeDOSName_MFD(zimsname, dosname, index, hnum);
	case 3:
		return MakeDOSName_CP3(zimsname, dosname, index, hnum);
	case 4:
		return MakeDOSName_CP3int(zimsname, dosname, index, hnum);
	case 5:
		return MakeDOSName_EV(zimsname, dosname, index, hnum); // Earthvision files.
	case 6:
		return MakeDOSName_ZmapISO(zimsname, dosname, index, hnum);
	default:
		pDoc->Error("No valid grid file type", MSG_ERR);
		return FALSE;
	}
}

// Version for Zmap ISO files.
// These files are similar to EarthVision files in that we assume
// the grid name comes from the filename.
BOOL CSeismicData::MakeDOSName_ZmapISO(CString &zimsname, CStringArray &dosname, int &index, int &hnum)
{
	index = -1; // reset to error value

	for (int i = 0; i < dosname.GetSize(); i++)
	{
		// find the file name which matches the grid name.
		// don't do a strstr here as several files might have the same prefix
		// instead, since Zmap ISO files are used by taking the grid name from the
		// filename minus the .dat suffix, put that back on and do a full compare.
		//CString n = ;

		if (dosname[i].CompareNoCase(zimsname + ".dat") == 0)
			index = i;

		//if ( !strcmp(dosname[i], zimsname + ".dat") )
		//	index = i;
	}
	hnum = 0;
	if (index == -1)
		return FALSE; // couldn't match a filename
	else
		return TRUE;
}

// Version for EarthVision files.
BOOL CSeismicData::MakeDOSName_EV(CString &zimsname, CStringArray &dosname, int &index, int &hnum)
{
	// The EarthVision files are quite simple and I can leave the zimsname and dosname as they are
	// and make sure the index and hnum are set sensibly. In our case, they
	// should be set to the start of the array.
	// zimsname is the name of the grid we are looking for.
	// dosname is the array of filenames available in the project.
	// index should be set to the appropriate file
	// hnum should be set to the appropriate header in the file
	// Since for EV files the gridname is in the filename itself, I just need
	// to make sure there is a file of the appropriate name.
	// The header number (hnum) is always zero as I think EV files only have
	// one grid per file.

	index = -1; // reset to error value

	for (int i = 0; i < dosname.GetSize(); i++)
	{
		// find the file name which matches the grid name.
		// need to test for 2D or 3D files.
		if (!strcmp(dosname[i], zimsname + ".2grd"))
			index = i;
		else if (!strcmp(dosname[i], zimsname + ".3grd"))
			index = i;
	}
	hnum = 0;
	if (index == -1)
		return FALSE; // couldn't match a filename
	else
		return TRUE;

}

//**********************************************************************
// Subroutine : MakeDOSName(), ZMAP version
// Arguments : zimsname : internal name in zimsdata base which
// has to be found by this routine
// dosname : CString Array of filenames to be scnned
// for the zimsname
// Use : Makes a DOS Name from a ZimsName
// Returns index : index to dosname array
// hnum : number of the header in the data base
//***********************************************************************
BOOL CSeismicData::MakeDOSName_ZMAP(CString &zimsname, CStringArray &dosname,
	int &index, int &hnum)
{
	int i, hcurr;
	_zims_header header;
	char namebuffer[21];
	namebuffer[20] = 0x00;
	// pad spaces after zimsname
	strncpy(namebuffer, (const char *)zimsname, 20);
	Pad(namebuffer, 20);
	zimsname = namebuffer;
	for (i = 0; i < dosname.GetSize(); i++)
	{
#ifdef VELMAN_UNIX
		ifstream dosfile(pDoc->ZimsDir + dosname[i]);
#else
		ifstream dosfile(pDoc->ZimsDir + dosname[i], ios::binary);
#endif
		if (dosfile.fail())
		{
			pDoc->Error("Internal Error in MakeDOSName - illegal filename", MSG_ERR);
			return FALSE;
		}
		hcurr = 0;
		while (!dosfile.eof())
		{
			dosfile.read((char *)&header, sizeof(_zims_header));
			// Break out of loop if no other header in the file...
			if (dosfile.gcount() == 0) break;
			strncpy(namebuffer, (const char *)header.filename, 20);
			if (zimsname == CString(namebuffer))
			{
				index = i;
				hnum = hcurr;
				return TRUE;
			}
#ifdef VELMAN_UNIX
			InvertData((char *)&header.ndxnxh, one_word_sec);
#endif
			dosfile.seekg((header.ndxnxh - 1)*ZIMS_RECORD_LENGTH);
			hcurr++;
		}
	}
	index = -1;
	hnum = -1;
	return FALSE;
}

//***********************************************************************
// Subroutine : MakeDOSName(), MFD file format version
// Arguments : zimsname : internal name in zimsdata base which
// has to be found by this routine
// dosname : CString Array of filenames to be scanned
// for the zimsname
// Use : Makes a DOS Name from a ZimsName
// Returns index : index to dosname array
// hnum : number of the header in the data base
//***********************************************************************
BOOL CSeismicData::MakeDOSName_MFD(CString &zimsname, CStringArray &dosname,
	int &index, int &hnum)
{
	int i, j, n;
	int metanext;
	meta_file_head metahead;
	file_head filehead;
	char filename[25];
	for (i = 0; i < dosname.GetSize(); i++)
	{
#ifdef VELMAN_UNIX
		ifstream zimsfile(pDoc->ZimsDir + dosname[i]);
#else
		ifstream zimsfile(pDoc->ZimsDir + dosname[i], ios::binary);
#endif
		if (zimsfile.fail())
		{
			pDoc->Error("Internal Error in MakeDOSName - illegal filename " + dosname[i], MSG_ERR);
			index = -1;
			hnum = -1;
			return FALSE;
		}
		n = -1;
		metanext = 0;
		hnum = 0;
		do
		{
			n++;
			zimsfile.seekg(metanext*ZIMS_BLOCK, ios::beg);
			zimsfile.read((char *)&metahead, ZIMS_RECORD_LENGTH);
			// invert data if reading with INTEL
#ifndef VELMAN_UNIX
			InvertData((char *)&metahead, meta_file_head_sec);
#endif
			// check that file is in correct format
			if (metahead.four != 4)
				break;
			// is there another file
			metanext = metahead.meta_next - 1;
			// search through this subfile
			for (j = 0; j < metahead.filenum; j++)
			{
				zimsfile.read((char *)&filehead, ZIMS_RECORD_LENGTH);
				// something has happened so stop
				if (zimsfile.fail() || zimsfile.gcount() < ZIMS_RECORD_LENGTH)
				{
					index = -1;
					hnum = -1;
					return FALSE;
				}
				// invert data if reading with INTEL
#ifndef VELMAN_UNIX
				InvertData((char *)&filehead, file_head_sec);
#endif
				strncpy(filename, (const char *)filehead.dataname, 24);
				filename[24] = ' ';
				UnPad(filename, 25);
				if (!strcmp((const char *)filename,
					(const char *)zimsname))
				{
					index = i;
					hnum += j;
					return TRUE;
				}
			}
			hnum += metahead.filenum;
		} while (metanext != -1);
	}
	index = -1;
	hnum = -1;
	return FALSE;
}

//***********************************************************************
// Subroutine : MakeDOSName(), CP3 file format version
// Arguments : zimsname : internal name in zimsdata base which
// has to be found by this routine
// dosname : CString Array of filenames to be scanned
// for the zimsname
// Use : Makes a DOS Name from a ZimsName
// Returns index : index to dosname array
// hnum : number of the header in the data base
//***********************************************************************
BOOL CSeismicData::MakeDOSName_CP3(CString &zimsname, CStringArray &dosname, int &index, int &hnum)
{
	cp3_file_head filehead;
	char dataname[512];
	int namesize = 0;
	int i = 0;
	for (i = 0; i < dosname.GetSize(); i++)
	{
#ifdef VELMAN_UNIX
		ifstream cp3file(pDoc->ZimsDir + dosname[i]);
#else
		ifstream cp3file(pDoc->ZimsDir + dosname[i], ios::binary);
#endif
		if (cp3file.fail())
		{
			pDoc->Error("Internal Error in MakeDOSName - illegal filename " + dosname[i], MSG_ERR);
			index = -1;
			hnum = -1;
			return FALSE;
		}
		FortranRead(cp3file, (char *)&filehead, sizeof(cp3_file_head));
		namesize = FortranRead(cp3file, dataname, 80);
		if (namesize != 0)
			UnPad(dataname, namesize);
		else
			UnPad(dataname, 80);
		if (!strcmp((const char *)dataname,
			(const char *)zimsname))
		{
			index = i;
			hnum = 0;
			return TRUE;
		}
	}
	index = -1;
	hnum = -1;
	return FALSE;
}

//***********************************************************************
// Subroutine : MakeDOSNameint(), CP3 file format version (internal)
// Arguments : zimsname : internal name in zimsdata base which
// has to be found by this routine
// dosname : CString Array of filenames to be scanned
// for the zimsname
// Use : Makes a DOS Name from a ZimsName
// Returns index : index to dosname array
// hnum : number of the header in the data base
//***********************************************************************
BOOL CSeismicData::MakeDOSName_CP3int(CString &zimsname, CStringArray &dosname,
	int &index, int &hnum)
{
	char dataname[512];
	int nextlength, namesize;
	int i, j;

	// first try to find it by matching the filename
	for (i = 0; i < dosname.GetSize(); i++)
	{
		if (!strncmp((const char *)dosname[i], zimsname, zimsname.GetLength()))
		{
			switch (index)
			{
			case CP3_TYPE_LINE:
			case ZIMS_TYPE_LINE:
				if (toupper(dosname[i].GetAt(dosname[i].GetLength() - 4)) != 'D')
					continue;
				break;
			case CP3_TYPE_FAULT:
			case ZIMS_TYPE_POLY:
				if (toupper(dosname[i].GetAt(dosname[i].GetLength() - 4)) != 'F')
					continue;
				break;
			case CP3_TYPE_GRID:
			case ZIMS_TYPE_GRID:
				if (toupper(dosname[i].GetAt(dosname[i].GetLength() - 4)) != 'S')
					continue;
				break;
			}
			index = i;
			hnum = 0;

			return TRUE;
		}
	}

	// if one cannot find it this way, try by matching the comment
	for (i = 0; i < dosname.GetSize(); i++)
	{
		// open data file
#ifdef VELMAN_UNIX
		ifstream cp3file(pDoc->ZimsDir + dosname[i]);
#else
		ifstream cp3file(pDoc->ZimsDir + dosname[i], ios::binary);
#endif
		if (cp3file.fail())
		{
			pDoc->Error("Could not open " + dosname[i], MSG_ERR);
			return FALSE;
		}

		// skip over magic number header
		cp3file.seekg(0x10, ios::beg);

		// skip over not used section + LOCK section
		cp3file.read((char *)&nextlength, sizeof(int));
#ifndef VELMAN_UNIX
		InvertData((char *)&nextlength, one_word_sec);
#endif
		cp3file.seekg(nextlength + 0x10, ios::cur);

		// skip over comment section and MAIN section
		cp3file.read((char *)&nextlength, sizeof(int));
#ifndef VELMAN_UNIX
		InvertData((char *)&nextlength, one_word_sec);
#endif
		cp3file.seekg(nextlength + 0x10, ios::cur);

		// skip over SUBS section
		cp3file.read((char *)&nextlength, sizeof(int));
		cp3file.seekg(0x10, ios::cur);

		// skip over N char section section
		cp3file.read((char *)&nextlength, sizeof(int));
		cp3file.seekg(0x01, ios::cur);

		// This is now the ZimsName
		cp3file.read((char *)&nextlength, sizeof(int));
#ifndef VELMAN_UNIX
		InvertData((char *)&nextlength, one_word_sec);
#endif
		cp3file.read(dataname, nextlength);

		// Read filename and unpad
		namesize = strlen(dataname);
		j = 0;
		while (j < namesize)
		{
			if (dataname[j] == '>') break;
			j++;
		}
		// compare strings to see if match was found
		if (!strcmp((const char *)dataname + j + 1, zimsname))
		{
			index = i;
			hnum = 0;
			return TRUE;
		}
	}

	// No matching file was found by either matching
	// comment or filename
	index = -1;
	hnum = -1;
	return FALSE;
}

//***********************************************************************
// Subroutine : MakeZimsNameList(), wrapper
// Arguments : dosname : CString Array of all dosnames to be
// scanned for zimsfiles
// type : type of zimsfile to be read
// Use : Read internal .MFD data file
// Returns : outarray : CString Array of zims filenames
//***********************************************************************
BOOL CSeismicData::MakeZimsNameList(CStringArray &outarray,
	CStringArray &dosname, int type)
{
	switch (pDoc->GridFileType)
	{
	case 1:
		return MakeZimsNameList_ZMAP(outarray, dosname, type);
	case 2:
		return MakeZimsNameList_MFD(outarray, dosname, type);
	case 3:
		return MakeZimsNameList_CP3(outarray, dosname, type);
	case 4:
		return MakeZimsNameList_CP3int(outarray, dosname, type);
	case 5: // earthvision files. added dec '03.
		return MakeZimsNameList_EV(outarray, dosname, type);
	case 6:
		return MakeZimsNameList_ZmapISO(outarray, dosname, type);
	default:
		pDoc->Error("No valid grid file type", MSG_ERR);
		return FALSE;
	}
}

// Construct gridnames from Zmap ISO files.
BOOL CSeismicData::MakeZimsNameList_ZmapISO(CStringArray &outarray,
	CStringArray &dosname, int type)
{
	int i, warn;
	static int warnmsg = 0;
	char *ps, ftype[8];
	char gridname[256], fname[256], inputbuf[CSD_MAXLEN];
	ifstream infile;

	outarray.RemoveAll();

	// loop over all files and read the first part
	// of the file to find out what type of file we have
	warn = 0;

	for (i = 0; i < dosname.GetSize(); i++)
	{
		strcpy(fname, (const char *)pDoc->ZimsDir + dosname[i]);

		// We're only interested in files that end in '.dat'
		if ((ps = (char *)StrStrI(dosname[i], ".dat")) == NULL)
			continue;

		// Unfortunately .dat is a common file extension and may
		// for any old file. Need to check carefully below to make sure
		// it's an ASCII format ZMAP ISO file. Dec. 05.
		infile.open(fname);
		while (!infile.eof())
		{
			do
			{
				infile.getline(inputbuf, CSD_MAXLEN - 1);
			} while (inputbuf[0] == '!'); // skip comments in file

			// Now let's make sure this looks like a ZMAP ISO file. If it is,
			// the first non-comment line should begin with a '@'.
			if (inputbuf[0] != '@')
			{
				if (warnmsg == 0)
					AfxMessageBox("Warning, an error occurred whilst reading the file " + dosname[i] +
						". The file has a .dat extension but the header format was not as expected. File could not be read.",
						MB_OK);
				warn = 1;
				infile.close();
				break;
			}

			// look for second text field to find out if it's a grid or fault file
			sscanf(strchr(inputbuf, ',') + 1, "%4s", ftype);

			switch (type)
			{
			case CP3_TYPE_GRID:
			case ZIMS_TYPE_GRID:

				if (!strncmp(ftype, "GRID", 4))
				{
					strncpy(gridname, dosname[i], ps - dosname[i]);
					gridname[ps - dosname[i]] = '\0'; // append NULL to terminate string
					outarray.Add((const char *)gridname);
				}

				break;

			case CP3_TYPE_FAULT:
			case ZIMS_TYPE_POLY:
				// To find fault files we can do this in two different ways
				// either just scan for the files with the right suffixes or
				// check the grid files to see what files they use and then check
				// that those exist.

				if (!strncmp(ftype, "FALT", 4))
				{
					strncpy(gridname, dosname[i], ps - dosname[i]);
					gridname[ps - dosname[i]] = '\0'; // append NULL to terminate string
					outarray.Add((const char *)gridname);
				}

				break;
			} // end of switch

			infile.close();
			break;
		}
	}

	// make sure we only print out warnings for bad files once
	if (warn == 1)
		warnmsg = 1;

	return TRUE;
}

// Function to construct gridnames from EarthVision files.
// These files (unlike the others velman deals with), do not
// have the grid names in the file, so we have to use the filename
// itself and strip off the suffix.
// Incoming list of filenames will include grid files and fault files
BOOL CSeismicData::MakeZimsNameList_EV(CStringArray &outarray, CStringArray &dosname, int type)
{
	int i = 0;
	char *ps;
	char gridname[128];

	outarray.RemoveAll();

	switch (type)
	{
	case CP3_TYPE_GRID:
	case ZIMS_TYPE_GRID:

		for (i = 0; i < dosname.GetSize(); i++)
		{
			// files end in either '.2grd' or '.3grd'
			if (ps = (char *)strstr(dosname[i], ".2grd"))
			{
				strncpy(gridname, dosname[i], ps - dosname[i]);
				gridname[ps - dosname[i]] = '\0'; // append NULL to terminate string
				outarray.Add((const char *)gridname);
			}
			else if (ps = (char *)strstr(dosname[i], ".3grd"))
			{
				strncpy(gridname, dosname[i], ps - dosname[i]);
				gridname[ps - dosname[i]] = '\0';
				outarray.Add((const char *)gridname);
			}
		}

		return TRUE;
		break;

	case CP3_TYPE_FAULT:
	case ZIMS_TYPE_POLY:
		// To find fault files we can do this in two different ways
		// either just scan for the files with the right suffixes or
		// check the grid files to see what files they use and then check
		// that those exist.
		for (i = 0; i < dosname.GetSize(); i++)
		{
			if (ps = (char *)strstr(dosname[i], ".vflt"))
			{
				strncpy(gridname, dosname[i], ps - dosname[i]);
				gridname[ps - dosname[i]] = '\0';
				outarray.Add((const char *)gridname);
			}
			else if (ps = (char *)strstr(dosname[i], ".nvflt"))
			{
				strncpy(gridname, dosname[i], ps - dosname[i]);
				gridname[ps - dosname[i]] = '\0';
				outarray.Add((const char *)gridname);
			}

		} // end of for loop

		return TRUE;

		break;
	} // end of switch

	return TRUE;
}

//***********************************************************************
// Subroutine : MakeZimsNameList(), ZMAP version
// Arguments : dosname : CString Array of all dosnames to be
// scanned for zimsfiles
// type : type of zimsfile to be read
// Use : Read internal .MFD data file
// Returns : outarray : CString Array of zims filenames
//***********************************************************************
BOOL CSeismicData::MakeZimsNameList_ZMAP(CStringArray &outarray, CStringArray &dosname, int type)
{
	ifstream zimsfile;
	_zims_header header;
	char namebuffer[21];
	CString filename;
	int i = 0;
	namebuffer[20] = 0x00;
	outarray.RemoveAll();
	for (i = 0; i < dosname.GetSize(); i++)
	{
		filename = dosname[i];
		TRACE("In MakeZimsList with filename[%d] = %s\n", i, (const char *)filename);
#ifdef VELMAN_UNIX
		zimsfile.clear();
		zimsfile.open(pDoc->ZimsDir + filename);
#else
		zimsfile.clear(); // make sure no error states remain from previous read. 10/08
		zimsfile.open(pDoc->ZimsDir + filename, ios::binary);
#endif
		do
		{
			zimsfile.read((char *)&header, sizeof(_zims_header));
			if (zimsfile.fail())
			{
				if (zimsfile.eof())
				{
					break; // we just reached the end of that file
				}
				else
				{
					pDoc->Error("Error in MakeZimsNameList - corrupt Zims File " +
						dosname[i], MSG_ERR);
					return FALSE;
				}
			}
#ifdef VELMAN_UNIX
			InvertData((char *)&header, zims_header_sec);
#endif
			if (type == header.filetype)
			{
				strncpy(namebuffer, (const char *)header.filename, 20);
				TRACE("In MakeZimsList with namebuffer = %s\n", (const char *)namebuffer);
				outarray.Add((const char *)namebuffer);
			}
			zimsfile.ignore((int)(header.ndxnxh - 3)*ZIMS_RECORD_LENGTH);
		} while (!zimsfile.eof());
		zimsfile.close();
	}
	return(TRUE);
}

//***********************************************************************
// Subroutine : MakeZimsNameList(), MFD version
// Arguments : dosname : CString Array of all dosnames to be
// scanned for zimsfiles
// type : type of zimsfile to be read
// Use : Read internal .MFD data file
// Returns : outarray : CString Array of zims filenames
//***********************************************************************
BOOL CSeismicData::MakeZimsNameList_MFD(CStringArray &outarray, CStringArray &dosname, int type)
{
	meta_file_head metahead;
	file_head filehead;
	int i, j, metanext;
	char filename[25];
	outarray.RemoveAll();
	for (i = 0; i < dosname.GetSize(); i++)
	{
#ifdef VELMAN_UNIX
		ifstream zimsfile(pDoc->ZimsDir + dosname[i]);
#else
		ifstream zimsfile(pDoc->ZimsDir + dosname[i], ios::binary);
#endif
		if (zimsfile.fail())
		{
			pDoc->Error("Could not open " + dosname[i], MSG_ERR);
			return FALSE;
		}
		metanext = 0;
		do
		{
			zimsfile.seekg(metanext*ZIMS_BLOCK, ios::beg);
			zimsfile.read((char *)&metahead, ZIMS_RECORD_LENGTH);
			// invert data if reading with INTEL
#ifndef VELMAN_UNIX
			InvertData((char *)&metahead, meta_file_head_sec);
#endif
			// check that file is in correct format
			if (metahead.four != 4)
				break;
			// see if there is another file_header
			metanext = metahead.meta_next - 1;
			for (j = 0; j < metahead.filenum; j++)
			{
				zimsfile.read((char *)&filehead, ZIMS_RECORD_LENGTH);
				// invert data if reading with INTEL
#ifndef VELMAN_UNIX
				InvertData((char *)&filehead, file_head_sec);
#endif
				strncpy(filename, (const char *)filehead.dataname, 24);
				filename[24] = ' ';
				UnPad(filename, 25);
				if (type == filehead.datatype)
					outarray.Add((const char *)filename);
			}
		} while (metanext != -1);
		zimsfile.close();
	}
	return TRUE;
}

//***********************************************************************
// Subroutine : MakeZimsNameList(), CP3 version
// Arguments : dosname : CString Array of all dosnames to be
// scanned for zimsfiles
// type : type of zimsfile to be read
// Use : Read internal .MFD data file
// Returns : outarray : CString Array of zims filenames
//***********************************************************************
BOOL CSeismicData::MakeZimsNameList_CP3(CStringArray &outarray, CStringArray &dosname, int type)
{
	cp3_file_head filehead;
	char dataname[512];
	int namesize;
	int i, cp3type = -1;
	// Can set the type either with ZIMS or CP3 values
	if (type > 0)
	{
		switch (type)
		{
		case ZIMS_TYPE_GRID:
			cp3type = CP3_TYPE_GRID;
			break;
		case ZIMS_TYPE_LINE:
			cp3type = CP3_TYPE_LINE;
			break;
		case ZIMS_TYPE_POLY:
			cp3type = CP3_TYPE_FAULT;
			break;
		}
	}
	else
	{
		cp3type = type;
	}
	outarray.RemoveAll();

	for (i = 0; i < dosname.GetSize(); i++)
	{
		//open data file
#ifdef VELMAN_UNIX
		ifstream cp3file(pDoc->ZimsDir + dosname[i]);
#else
		ifstream cp3file(pDoc->ZimsDir + dosname[i], ios::binary);
#endif
		if (cp3file.fail())
		{
			pDoc->Error("Could not open " + dosname[i], MSG_ERR);
			return FALSE;
		}
		FortranRead(cp3file, (char *)&filehead, sizeof(cp3_file_head));
		// invert data if reading with INTEL
#ifndef VELMAN_UNIX
		InvertData((char *)&filehead, cp3_file_head_sec);
#endif
		// Read filename and unpad
		namesize = FortranRead(cp3file, dataname, 80);
		if (namesize != 0)
			UnPad(dataname, namesize);
		else
			UnPad(dataname, 80);
		if (filehead.itype == cp3type)
		{
			outarray.Add((const char *)dataname);
		}
	}
	return TRUE;
}

//***********************************************************************
// Subroutine : MakeZimsNameList(), CP3 internal version
// Arguments : dosname : CString Array of all dosnames to be
// scanned for zimsfiles
// type : type of zimsfile to be read
// Use : Read internal .MFD data file
// Returns : outarray : CString Array of zims filenames
//***********************************************************************
BOOL CSeismicData::MakeZimsNameList_CP3int(CStringArray &outarray,
	CStringArray &dosname, int type)
{
	// char dataname[512];
	// int nextlength, namesize;
	int i, j, cp3type = -1;

	// Can set the type either with ZIMS or CP3 values
	if (type > 0)
	{
		switch (type)
		{
		case ZIMS_TYPE_GRID:
			cp3type = CP3_TYPE_GRID;
			break;
		case ZIMS_TYPE_LINE:
			cp3type = CP3_TYPE_LINE;
			break;
		case ZIMS_TYPE_POLY:
			cp3type = CP3_TYPE_FAULT;
			break;
		}
	}
	else
	{
		cp3type = type;
	}
	outarray.RemoveAll();

	for (i = 0; i < dosname.GetSize(); i++)
	{
		// Check that we have the correct filetype
		// else just skip to the next file
		switch (cp3type)
		{
		case CP3_TYPE_GRID:
			if (toupper(dosname[i].GetAt(dosname[i].GetLength() - 4)) != 'S')
				continue;
			break;
		case CP3_TYPE_LINE:
			if (toupper(dosname[i].GetAt(dosname[i].GetLength() - 4)) != 'D')
				continue;
			break;
		case CP3_TYPE_FAULT:
			if (toupper(dosname[i].GetAt(dosname[i].GetLength() - 4)) != 'F')
				continue;
			break;
		}

		j = dosname[i].GetLength() - 5;
		j = (j < 0) ? 0 : j;

		outarray.Add(dosname[i].Left(j));
	}

	return TRUE;
}

//***********************************************************************
// Subroutine : Read()
// Arguments : none
// Use : Read internal .ZMS data file
// Returns : none
//***********************************************************************
BOOL CSeismicData::Read(void)
{
	CString filename = pDoc->TargetDir + pDoc->BaseName + ".zms";
	ifstream infile;
	int i, j, k, nslns, nspts, ndpts, length, percent;
	double x[2], data;
	int id, lineid;
	UBYTE type;
	char buffer[CSD_MAXLEN];
	CShotLine *sline;
	CShotPt *spt;
	CDataPt *dpt;
	CString name;
	// Clear Data base before reading the one from disk...
	ClearContents();
#ifdef VELMAN_UNIX
	infile.open(filename);
#else
	infile.open(filename, ios::binary);
#endif
	if (infile.fail())
	{
		pDoc->Error("Could not read seismic data file " + filename, MSG_ERR);
		return FALSE;
	}
	// pDoc->m_pProgress->ChangeText("Reading internal data base...");
	infile.read((char *)&nslns, sizeof(int));
	SetSize(nslns);
	percent = -5;
	for (i = 0; i < nslns; i++)
	{
		if (pDoc->m_pProgress && (i % 10) == 1)
			pDoc->m_pProgress->ChangePercentage(60 + (i * 20) / nslns);
		infile.read((char *)&nspts, sizeof(int));
		infile.read((char *)&lineid, sizeof(int));
		infile.read((char *)&length, sizeof(int));
		infile.read(buffer, length * sizeof(char));
		name = buffer;
		sline = new CShotLine(name, lineid);
		sline->SetSize(nspts);
		SetAt(i, sline);
		for (j = 0; j < nspts; j++)
		{
			infile.read((char *)&ndpts, sizeof(int));
			infile.read((char *)&id, sizeof(int));
			infile.read((char *)&type, sizeof(UBYTE));
			infile.read((char *)x, sizeof(double) * 2);
			spt = new CShotPt(id, type);
			spt->PutX(x[0]);
			spt->PutY(x[1]);
			spt->SetSize(ndpts);
			sline->SetAt(j, spt);
			for (k = 0; k < ndpts; k++)
			{
				infile.read((char *)&data, sizeof(double));
				dpt = new CDataPt(data);
				spt->SetAt(k, dpt);
			}
		}
	}
	infile.close();
	return TRUE;
}

//***********************************************************************
// Subroutine : Write()
// Arguments : none
// Use : Write internal .ZMS data file
// Returns : none
//***********************************************************************
BOOL CSeismicData::Write(void)
{
	CString filename = pDoc->TargetDir + pDoc->BaseName + ".zms";
	ofstream outfile;
	int i, j, k, nslns, nspts, ndpts, length;
	CShotLine *sline;
	CShotPt *spt;
	double x[2], data;
	int id, lineid, percent, oldpercent = -5;
	UBYTE type;
#ifdef VELMAN_UNIX
	outfile.open(filename);
#else
	outfile.open(filename, ios::binary);
#endif
	if (outfile.fail())
	{
		pDoc->Error("Could not write seismic data file " + filename, MSG_ERR);
		return FALSE;
	}
	nslns = GetSize();
	outfile.write((char *)&nslns, sizeof(int));
	for (i = 0; i < nslns; i++)
	{
		if (pDoc->m_pProgress)
		{
			percent = (i * 100) / nslns;
			if (percent >= oldpercent + 5)
			{
				pDoc->m_pProgress->ChangePercentage(percent);
				oldpercent = percent;
			}
		}
		sline = (CShotLine *)GetAt(i);
		lineid = (int)(*sline);
		nspts = sline->GetSize();
		length = sline->GetName().GetLength() + 1;
		outfile.write((char *)&nspts, sizeof(int));
		outfile.write((char *)&lineid, sizeof(int));
		outfile.write((char *)&length, sizeof(int));
		outfile.write((const char *)sline->GetName(), length * sizeof(char));
		for (j = 0; j < nspts; j++)
		{
			spt = (*sline)[j];
			ndpts = spt->GetSize();
			id = (int)(*spt);
			type = (UBYTE)(*spt);
			x[0] = spt->GetX();
			x[1] = spt->GetY();
			outfile.write((char *)&ndpts, sizeof(int));
			outfile.write((char *)&id, sizeof(int));
			outfile.write((char *)&type, sizeof(UBYTE));
			outfile.write((char *)x, sizeof(double) * 2);
			for (k = 0; k < ndpts; k++)
			{
				data = (double)(*spt)[k];
				outfile.write((char *)&data, sizeof(double));
			}
		}
	}
	outfile.close();
	return TRUE;
}

//***********************************************************************
// Subroutine : WriteASCIIInternal()
// Arguments : none
// Use : ASCII dump of internal data base to disk
// Returns : none
//***********************************************************************
BOOL CSeismicData::WriteASCIIInternal(void)
{
	CString filename = pDoc->TargetDir + pDoc->BaseName + ".ses";
	int i, j, k;
	ofstream outfile(filename);
	CShotLine *sline;
	CShotPt *spt;
	CDataPt *dpt;
	double x, y;
	for (i = 0; i < GetSize(); i++)
	{
		// change progress monitor if there is any. Changes in the range 10%..20%
		if (pDoc->m_pProgress && i % 10)
			pDoc->m_pProgress->ChangePercentage(10 + (i * 20) / GetSize());
		sline = (CShotLine *)GetAt(i);
		outfile << "\nShotLine [" << i << "] : (" << (int)*sline << ") :" << sline->GetName() << endl;
		for (j = 0; j < sline->GetSize(); j++)
		{
			spt = (CShotPt *)sline->GetAt(j);
			x = spt->GetX();
			y = spt->GetY();
			outfile << "\tShotPt [" << j << "] : " << (int)*spt
				<< " at (" << x << "," << y << ")" << " type : " << (int)((UBYTE)*spt) << endl;
			for (k = 0; k < spt->GetSize(); k++)
			{
				dpt = (CDataPt *)spt->GetAt(k);
				outfile << "\t\tDataPt [" << k << "] : " << (double)*dpt << endl;
			}
		}
	}
	return TRUE;
}

//***********************************************************************
// Subroutine : CleanInternalDataBase()
// Arguments : none
// Use : Cleans up the internal data base, throwing out
// all invalid points: Those with information missing or invalid,
// and those reported outside the region defined by timegrids
// (if there are any timegrids imported)
// Returns : Always TRUE
//***********************************************************************
BOOL CSeismicData::CleanInternalDataBase(void)
{
	int k, l, cid, size;
	int spts = 0, slines = 0, discardedspts = 0;
	double *limits;
	double regionX1, regionX2, regionY1, regionY2;
	CZimsGrid *timegrid;
	CWaitCursor wait;
	CShotLine *sline;
	CShotPt *spt, *rmspt;
	char buffer[512];

	// Change mp progress text
	pDoc->m_pProgress->ChangeText("Validating internal data base");
	// go through all shotlines
	for (int i = GetUpperBound(); i >= 0; i--)
	{
		sline = (*this)[i];

		if (GetUpperBound() > 0)
			pDoc->m_pProgress->ChangePercentage(20 - (20 * i) / GetUpperBound());

		for (int j = sline->GetUpperBound(); j >= 0; j--)
		{
			spt = (*sline)[j];
			if ((spt->GetX() == CSD_DONTKNOW) || (spt->GetY() == CSD_DONTKNOW) ||
				(spt->GetSize() == 0))
			{
				delete spt;
				sline->RemoveAt(j);
			}
			else
			{
				if (spt->GetType() == CSD_TYPE_TIME)
					spts++; // count only the time shotpoints, because the velocity ones do not interest the log file reader
			}
		}
		sline->FreeExtra();
		if (sline->GetSize() == 0)
		{
			delete sline;
			RemoveAt(i);
		}
		else
			slines++;
	}

	FreeExtra();
	//sprintf(buffer, "Found %d valid shotlines with %d valid shotpoints", slines, spts);
	sprintf(buffer, "Found %d valid shotpoints", spts);
	pDoc->Log(buffer, LOG_STD);

	if ((timegrid = pDoc->zimscube.FindHorizon(0, CSD_TYPE_TIME, 0)))
	{
		// have a time grid at hand ==> take its coords to define region of interest
		// and throw away all points outside it
		limits = timegrid->GetXlimits();
		regionX1 = limits[0];
		regionX2 = limits[1];
		limits = timegrid->GetYlimits();
		regionY1 = limits[0];
		regionY2 = limits[1];

		for (int i = GetUpperBound(); i >= 0; i--)
		{
			sline = (*this)[i];
			if (GetUpperBound() > 0)
				pDoc->m_pProgress->ChangePercentage(80 - (60 * i) / GetUpperBound());
			for (int j = sline->GetUpperBound(); j >= 0; j--)
			{
				spt = (*sline)[j];
				if (spt->GetX() < regionX1 || spt->GetX() > regionX2 ||
					spt->GetY() < regionY1 || spt->GetY() > regionY2)
				{
					// point is outside grid => outside region of interest. delete it, and
					// only increase "discarded points" counter if it is a TIME point, or
					// else we'd be couting "time" and "rmsvel" points doubly.
					if (spt->GetType() == CSD_TYPE_TIME)
						discardedspts++;
					delete spt;
					sline->RemoveAt(j);
				}
			}

			sline->FreeExtra();
			if (sline->GetSize() == 0)
			{
				delete sline;
				RemoveAt(i);
			}
		}

		// message if something had to be discarded.
		if (discardedspts > 0)
		{
			sprintf(buffer, "%d shotpoints of %d (%d%%) lie outside the region of"
				" interest and have been discarded.", discardedspts, spts,
				(discardedspts * 100) / spts);
			if (discardedspts >= spts)
				strcat(buffer, "\n\nNo shotpoints left in internal database."
					" RMS file and time grids seem incompatible.");
			pDoc->Error(buffer, MSG_WARN);
		}
	}

	// now check that all spoints are increasing in time...
	for (int i = 0; i < GetSize(); i++)
	{
		sline = (*this)[i];
		for (int j = 0; j < sline->GetSize(); j++)
		{
			spt = (*sline)[j];
			// find all time pts
			if (spt->GetType() == CSD_TYPE_TIME)
			{
				cid = (int)(*spt);
				// find the corresponding rms point
				rmspt = sline->FindSPoint(cid, CSD_TYPE_RMSVEL);
				// if this exits then delete extra values
				if (rmspt)
				{
					k = 1; l = 0; size = spt->GetSize();
					while (k < size)
					{
						// if the data is reversed remove both the time and the rmspt
						if ((*spt)[l] >= (*spt)[k])
						{
							delete ((CDataPt *)spt->GetAt(k));
							spt->RemoveAt(k);
							delete ((CDataPt *)rmspt->GetAt(k));
							rmspt->RemoveAt(k);
							size--;
						}
						else
						{
							l = k;
							k++;
						}
					}
					rmspt->FreeExtra();
					spt->FreeExtra();
				}
			}
		}
		pDoc->m_pProgress->ChangePercentage(80 + (10 * (i + 1)) / GetSize());
	}

	// now check that all spoints are increasing in velocity...
	for (int i = 0; i < GetSize(); i++)
	{
		sline = (*this)[i];
		for (int j = 0; j < sline->GetSize(); j++)
		{
			rmspt = (*sline)[j];
			// find all rms pts
			if (rmspt->GetType() == CSD_TYPE_RMSVEL)
			{
				cid = (int)(*rmspt);
				// find the corresponding time point
				spt = sline->FindSPoint(cid, CSD_TYPE_TIME);
				// if this exits then delete extra values
				if (spt)
				{
					k = 1; l = 0; size = rmspt->GetSize();
					while (k < size)
					{
						// if the data is reversed remove both the time and the rmspt
						if ((*rmspt)[l] >= (*rmspt)[k])
						{
							delete ((CDataPt *)rmspt->GetAt(k));
							rmspt->RemoveAt(k);
							delete ((CDataPt *)spt->GetAt(k));
							spt->RemoveAt(k);
							size--;
						}
						else
						{
							l = k;
							k++;
						}
					}
					rmspt->FreeExtra();
					spt->FreeExtra();
				}
			}
		}
		pDoc->m_pProgress->ChangePercentage(90 + (10 * (i + 1)) / GetSize());
	}


	// count total lines and points
	spts = 0;
	slines = 0;
	for (int i = GetUpperBound(); i >= 0; i--)
	{
		sline = (*this)[i];

		for (int j = sline->GetUpperBound(); j >= 0; j--)
		{
			spt = (*sline)[j];
			if (spt->GetType() == CSD_TYPE_TIME)
				spts++;
		}

		slines++;
	}

	// now make sure there aren't too many points
	double SkipLikelihood = 0.0, randomvalue;
	int MaxDerivative = AfxGetApp()->GetProfileInt("RMS Import", "InitialMaxDerivative", 5000);

	if (spts > MaxDerivative)
	{
		SkipLikelihood = (spts - MaxDerivative) / (double)spts;
		srand(0);

		if (AfxGetApp()->GetProfileInt("Debug", "ForMyEyesOnly", 0))
		{
			sprintf(buffer, "Shotpoints total: %d\nInitialMaxDerivative: %d\nSkipping likelihood: %f%%",
				spts, MaxDerivative, 100 * SkipLikelihood);
			pDoc->Log(buffer, LOG_BRIEF);
		}

		// randomly remove excess points
		for (int i = GetUpperBound(); i >= 0; i--)
		{
			sline = (*this)[i];

			for (int j = sline->GetUpperBound(); j >= 0; j--)
			{
				spt = (*sline)[j];
				randomvalue = ((double)rand()) / RAND_MAX;
				if (randomvalue < SkipLikelihood)
				{
					delete spt;
					sline->RemoveAt(j);
				}
			}

			sline->FreeExtra();
			if (sline->GetSize() == 0)
			{
				delete sline;
				RemoveAt(i);
			}
		}
	}

	return TRUE;
}

//***********************************************************************
// Subroutine : Interpolate2DShotLines()
// Arguments : type == type of linedate to interpolate
// ShowWarnings: if TRUE, displays a warning for the first
// shotpoint with problems, and puts all others into logfile
// if FALSE, no warning output (ie.e, when called for a second
// time with a different "type" argument)
// Use : Uses Cubic Splines to interpolate missing points
// along the Shotlines
// Returns : T/F depending on the outcome
//***********************************************************************
BOOL CSeismicData::Interpolate2DShotLines(UBYTE type, BOOL ShowWarnings)
{
	int i, j, k, shotpts, sid, oldsid;
	int emptyspts, ptsresampled, splinenum;
	int startpt, endpt, intsleft, xtraints;
	int ptsize;
	double *x, *y, *t, *dx2, *dy2;
	double cd, ct;
	char buffer[128];
	BOOL toosmall = FALSE;
	UBYTE stype;
	CShotLine *sline;
	CShotPt *spt;
	CObArray emptyspt;
	// Go through all ShotLines to find unknown points
	for (i = 0; i < GetSize(); i++)
	{
		sline = (*this)[i];
		// Get shotline and allocate arrays for computation
		shotpts = sline->GetSize();
		x = new double[3 * shotpts];
		y = x + shotpts;
		t = y + shotpts;
		// Initialize loop counters
		k = 0;
		oldsid = -1;
		// Go through all shotpoints in the shotline
		for (j = 0; j < shotpts; j++)
		{
			spt = (*sline)[j];
			stype = (UBYTE)(*spt);
			// Is the type correct?
			if (stype == type)
			{
				// Get the shotpoint ID and check if the order is correct...
				sid = (int)(*spt);
				if (sid <= oldsid)
				{
					sprintf(buffer, "Order of spts in Interpolate2DShotLine incorrect: line %s",
						(LPCTSTR)sline->GetName());
					pDoc->Error(buffer, MSG_ERR);
					delete x;
					return FALSE;
				}
				oldsid = sid;
				// Is this shotpoint x, y known? If yes move to next point...
				if ((spt->GetX() != CSD_DONTKNOW) && (spt->GetY() != CSD_DONTKNOW))
				{
					// Get x, y and increment time
					x[k] = spt->GetX();
					y[k] = spt->GetY();
					t[k] = (double)sid;
					k++;
				}
				else
					emptyspt.Add(spt);
			}
		}
		// number of empty spts in this shotline
		emptyspts = emptyspt.GetSize();
		// no unknown points in this shotline - continue...
		if (emptyspts == 0)
		{
			delete x;
			continue;
		}
		// ptsize now contains number of known points...
		ptsize = k;
		// Check if there are enough points known in the shotline
		if (ptsize < 4)
		{
			if (!ShowWarnings)
			{
				delete x;
				emptyspt.RemoveAll();
				continue;
			}
			sprintf(buffer, "Only %d known points in line %s", ptsize, (const char *)sline->GetName());
			if (!toosmall)
			{ // show dlg. one time at max
				toosmall = TRUE;
				if (pDoc->Error(buffer, (MSG_WARN | MSG_CANCEL)))
				{
					delete x;
					return FALSE;
				}
				else
				{
					delete x;
					emptyspt.RemoveAll();
					continue;
				}
			}
			else
			{
				pDoc->Log(buffer, LOG_DETAIL);
				delete x;
				emptyspt.RemoveAll();
				continue;
			}
		}
		// allocate memory for derivatives
		dx2 = new double[2 * ptsize];
		dy2 = dx2 + ptsize;
		// calculate different quantities to specify how often and with how many
		// pts resampling should be undertaken...
		// all of these are intervals not points - so be careful...
		splinenum = (ptsize - 1) / CSD_2DRESAMPLEINTERVALS;
		intsleft = (ptsize - 1) % CSD_2DRESAMPLEINTERVALS;
		xtraints = (intsleft + splinenum - 1) / splinenum;
		ptsresampled = 0;
		startpt = 0;
		for (j = 0; (j < splinenum) && (ptsresampled < emptyspts); j++)
		{
			// to which point will we resample?
			endpt = startpt + CSD_2DRESAMPLEINTERVALS;
			// Check if there are any left over points and distribute them
			// evenly...
			if (intsleft)
			{
				if (intsleft > xtraints)
				{
					intsleft -= xtraints;
					endpt += xtraints;
				}
				else
				{
					endpt += intsleft;
					intsleft = 0;
				}
			}
			// find approximate derivatives at the beginning and end
			// of the sample data...
			// !!sk126 d1 = (x[startpt+1] - x[startpt])/(t[startpt+1] - t[startpt]);
			// !!sk126 dn = (x[endpt] - x[endpt-1])/(t[endpt] - t[endpt-1]);
			// Call the spline routine to calculate array of 2nd derivatives
			// Be carefule spline is unit offset for all arrays!
			spline(t + startpt - 1, x + startpt - 1, endpt - startpt + 1, 1.0E30, 1.0E30, dx2 - 1);
			// find approximate derivatives at the beginning and end
			// of the sample data...
			// !!sk126 d1 = (y[startpt+1] - y[startpt])/(t[startpt+1] - t[startpt]);
			// !!sk126 dn = (y[endpt] - y[endpt-1])/(t[endpt] - t[endpt-1]);
			// Call the spline routine to calculate array of 2nd derivatives
			// Be carefule spline is unit offset for all arrays!
			spline(t + startpt - 1, y + startpt - 1, endpt - startpt + 1, 1.0E30, 1.0E30, dy2 - 1);
			// loop till all points are resampled...
			while (ptsresampled < emptyspts)
			{
				// The shotpoint to be resampled...
				spt = (CShotPt *)emptyspt[ptsresampled];
				// The resample time is the shotpointid!
				ct = (double)spt->GetID();
				// Check if the point at which we want to resample is in this resampling interval
				// If not, continue to the next interval...
				if ((ct > t[endpt]) && (j != (splinenum - 1))) break;
				// evaluate X at sptid...
				splint(t + startpt - 1, x + startpt - 1, dx2 - 1, endpt - startpt + 1, ct, &cd);
				// put X into internal data base...
				spt->PutX(cd);
				// evaluate Y at sptid...
				splint(t + startpt - 1, y + startpt - 1, dy2 - 1, endpt - startpt + 1, ct, &cd);
				// put Y into internal data base...
				spt->PutY(cd);
				// increment resampled points
				ptsresampled++;
			}
			// go to the next interval;
			startpt = endpt;
		} // end of loop over splinenums
		// clear the emptypoints array...
		emptyspt.RemoveAll();
		// release memory for the spline derivative array...
		delete dx2;
		//release memory for the x, y, t components...
		delete x;
	} // end of loop over shotlines
	return TRUE;
}

//***********************************************************************
// Subroutine : DoDixEquation()
// Arguments : timegrid: pointer to list of time-horizons
// horizons: number of time horizons
// Use : Does Dix equation down each shotpoint. Attaches
// resulting vAvg as a third list of numbers down each shotp.
// Also "thins out" database: If number of shotpoints exceeds
// INI-File treshold value, we thin a certain percentage out
// so the value is no longer exceeded. Thinning out=do not compute
// vAvg down some shotpoints, so we need not tps them to time
// slices later
// Returns : int : total number of shotpoints
// rmspts : array of all rmspts at rmshorizons
//***********************************************************************
BOOL CSeismicData::DoDixEquation(void)
{
	int ptsize, percent;
	int shotpts, cid;
	CShotLine *shotline;
	CShotPt *timept, *rmspt;
	char buffer[512];
	double darg, v0, v1, t0, t1, depth, avgvel;
	BOOL dontknow = FALSE, notboth = FALSE, sizewrong = FALSE, toosmall = FALSE;
	BOOL badorder = FALSE;
	CObArray originalpts;
	CObArray keptpts; // list of all points kept
	//int MaxDerivative=AfxGetApp()->GetProfileInt("RMS Import", "MaxDerivative", 5000);
	//double radius = (double) AfxGetApp()->GetProfileInt("ArithmeticParams", "RMSRadius", 150);
	//radius *= radius;//square the radius for calc speed

	int NumShotpoints = 0;
	int NumSkippedPoints = 0;
	int NumBadPoints = 0;
	int NumKeptPoints = 0;
	double SkipLikelihood = 0.0, randomvalue;
	BOOL HaveToSkip = FALSE;
	BOOL AllVelsGood;
	double *vDix;

	pDoc->Log("Creating Average velocities...", LOG_STD);

	// count total number of shotpoints
	pDoc->m_pProgress->ChangeText("Counting shotpoints");
	NumShotpoints = 0;
	for (int i = 0; i < GetSize(); i++)
	{
		shotline = (*this)[i];
		shotpts = shotline->GetSize();
		for (int j = 0; j < shotpts; j++)
		{
			timept = (CShotPt *)shotline->GetAt(j);
			// find the timepts and later the rmspts...
			if (timept->GetType() == CSD_TYPE_TIME && timept->GetSize() > 0)
				NumShotpoints++;
		}
	}

	// compute a likelihood 0..1
	// if a random number 0..1 is less than that likelihood, we skip that
	// shotpoint because we have too many of them
	/*if(NumShotpoints > MaxDerivative)
	{
	HaveToSkip=TRUE;
	SkipLikelihood=(NumShotpoints-MaxDerivative)/(double)NumShotpoints;
	if(AfxGetApp()->GetProfileInt("Debug", "ForMyEyesOnly", 0))
	{
	sprintf(buffer, "Shotpoints total: %d\n"
	"Max number allowed: %d\n"
	"Skipping likelihood: %f%%", NumShotpoints, MaxDerivative,
	100*SkipLikelihood);
	pDoc->Log(buffer, LOG_BRIEF);
	}
	}
	else
	HaveToSkip=FALSE;*/

	// do the dix equation on most points, except those thinned out
	pDoc->m_pProgress->ChangeText("Computing average velocities...");

	// loop through all shotlines
	for (int i = 0; i < GetSize(); i++)
	{
		// Update the progress monitor...
		percent = (100 * i) / GetSize();
		pDoc->m_pProgress->ChangePercentage(percent);

		// Get the current shotline and its size...
		shotline = (*this)[i];
		shotpts = shotline->GetSize();

		// now loop through all the shotpoints in each shotline. As we use AddNewPt() at
		// the end of the routine, this list of shotpoints is going to CHANGE as we go along
		// the line. This is why we make a copy of it here, and walk along the copy regardless
		// of what happens to the real line.
		for (int j = 0; j < shotpts; j++)
		{
			originalpts.Add((*shotline)[j]);
		}

		// Now walk along the copy
		for (int j = 0; j < shotpts; j++)
		{
			timept = (CShotPt *)originalpts.GetAt(j);
			// now we found the timept with cid
			cid = (int)*timept;
			// Now find RMS velocity point for this cid
			rmspt = shotline->FindSPoint(cid, CSD_TYPE_RMSVEL);

			// find the timepts and later the rmspts...
			if (timept->GetType() != CSD_TYPE_TIME || timept->GetSize() == 0)
				continue;

			//// should we skip this point?
			//if(HaveToSkip)
			//{
			//	randomvalue=((double)rand())/RAND_MAX;
			//	if(randomvalue < SkipLikelihood)
			//	{
			//		NumSkippedPoints++;
			//		continue;
			//	}
			//}

			// Check that both points exist
			if (!timept || !rmspt)
			{
				sprintf(buffer, "Could not find both velocity and time data for shotline %s, shotpoint ID %d",
					(LPCTSTR)shotline->GetName(), cid);

				if (AfxGetApp()->GetProfileInt("Debug", "ForMyEyesOnly", 0))
				{
					if (!notboth)
					{
						notboth = TRUE;
						if (pDoc->Error(buffer, (MSG_WARN | MSG_CANCEL)))
						{
							originalpts.RemoveAll();
							return FALSE;
						}
					}
					else
					{
						pDoc->Log(buffer, LOG_STD);
					}
				}

				continue;
			}

			// See if all the coordinates are known
			if (timept->GetX() == CSD_DONTKNOW || timept->GetY() == CSD_DONTKNOW ||
				rmspt->GetX() == CSD_DONTKNOW || rmspt->GetY() == CSD_DONTKNOW)
			{
				sprintf(buffer, "Coordinates of shotline %s, shotpoint ID %d not known",
					(LPCTSTR)shotline->GetName(), cid);
				if (!dontknow)
				{
					dontknow = TRUE;
					if (pDoc->Error(buffer, (MSG_WARN | MSG_CANCEL)))
					{
						originalpts.RemoveAll();
						return FALSE;
					}
					else
						continue;
				}
				else
				{
					pDoc->Log(buffer, LOG_STD);
					continue;
				}
			}

			// Check that there are the same number of pts in both time and veloc
			// These are the lists of time and velocity measurement points respectively
			if ((ptsize = rmspt->GetSize()) != (timept->GetSize()))
			{
				sprintf(buffer, "Different sizes for shotline %s, shotpoint ID %d",
					(LPCTSTR)shotline->GetName(), cid);
				if (!sizewrong)
				{
					sizewrong = TRUE;
					if (pDoc->Error(buffer, (MSG_WARN | MSG_CANCEL)))
					{
						originalpts.RemoveAll();
						return FALSE;
					}
					else
						continue;
				}
				else
				{
					pDoc->Log(buffer, LOG_STD);
					continue;
				}
			}

			// Check that the number of data points is not too small
			// in order to do a spline...
			if (ptsize < 4)
			{
				sprintf(buffer, "Less than 6 pts for shotline %s, shotpoint ID %d",
					(LPCTSTR)shotline->GetName(), cid);

				if (!toosmall)
				{
					toosmall = TRUE;
					if (pDoc->Error(buffer, (MSG_WARN | MSG_CANCEL)))
					{
						originalpts.RemoveAll();
						return FALSE;
					}
					else
						continue;
				}
				else
				{
					pDoc->Log(buffer, LOG_DETAIL);
					continue;
				}
			}

			// now check each shotpoint - times must always increase...
			for (int k = 1; k < ptsize; k++)
			{
				if ((*timept)[k] < (*timept)[k - 1])
				{
					sprintf(buffer, "Time order not correct in shotline %s, shotpoint ID %d",
						(LPCTSTR)shotline->GetName(), cid);

					if (!badorder)
					{
						badorder = TRUE;
						if (pDoc->Error(buffer, (MSG_WARN | MSG_CANCEL)))
						{
							originalpts.RemoveAll();
							return FALSE;
						}
						else
							continue;
					}
					else
					{
						pDoc->Log(buffer, LOG_DETAIL);
						continue;
					}
				}
			}

			// Now check if there are any timepoints which are too close
			// to the point in question
			CShotLine *cmline = shotline;
			CShotPt *cmpt = NULL;

			//BOOL keeppt = TRUE;
			//if(radius > 0)
			//{
			//	// compare against all points kept so far
			//	for(int cj = 0; cj < keptpts.GetSize() ; cj++)
			//	{
			//		cmpt=(CShotPt *) keptpts.GetAt(cj);

			//		if(cmpt->GetType() == CSD_TYPE_TIME && TooClose(timept, cmpt, radius))
			//		{
			//			keeppt = FALSE;
			//			break;
			//		}
			//	}
			//}
			//if(!keeppt) 
			//{
			//	// not using this point
			//	continue;
			//}		

			// keeping this point so add it to list
			keptpts.Add(timept);
			NumKeptPoints++;

			// Now we can do the dix equation down each shotpoint...
			avgvel = 0.0;
			depth = 0.0;

			// tp100 start
			// we insert an artificial point with time=0, vRMS=0, if the first time
			// is not 0 anyway. In this way, we do not have to fake a surface layer later
			CDataPt *dpoint;
			if ((*timept)[0] != 0.0)
			{
				dpoint = new CDataPt(0.0);
				timept->InsertAt(0, dpoint);
				dpoint = new CDataPt(0.0);
				rmspt->InsertAt(0, dpoint);
				ptsize++;
			}

			vDix = new double[ptsize - 1]; // one less dix interval velocity than input points
			AllVelsGood = TRUE;

			for (int k = 1; k < ptsize; k++)
			{ // start at 1, not 0, because we refer to xx[k-1]
				t1 = (*timept)[k];
				v1 = (*rmspt)[k];
				t0 = (*timept)[k - 1];
				v0 = (*rmspt)[k - 1];
				// check if everything is OK...
				if (v0 == CSD_ILLEGAL || v1 == CSD_ILLEGAL ||
					t0 == CSD_ILLEGAL || t1 == CSD_ILLEGAL)
					darg = CSD_ILLEGAL;
				else
					darg = (t1 == t0) ? v1 * v0 : (v1*v1*t1 - v0 * v0*t0) / (t1 - t0);

				// If not OK print error message and set all further points to
				// illegal.
				if (darg < 0.0 || darg == CSD_ILLEGAL)
				{
					sprintf(buffer, "Negative or illegal dix argument between "
						"time/velocity pair #%d and #%d \nShotline %s, shotpoint ID %d \n"
						"darg = %lf v0 = %lf, v1 = %lf, t0 = %lf, t1 = %lf", k, k + 1,
						(LPCTSTR)shotline->GetName(), cid, darg, v0, v1, t0, t1);
					pDoc->Log(buffer, LOG_DETAIL);
					AllVelsGood = FALSE;
					avgvel = CSD_ILLEGAL;
				}

				// if the interval velocity is ok - compute the average vel
				if (avgvel != CSD_ILLEGAL)
				{
					depth += sqrt(darg)*(t1 - t0) / 2000.0;
					avgvel = (t1 > FLT_MIN) ? 2000.0*depth / t1 : 0.0;
				}
				vDix[k - 1] = avgvel;
			}

			// filter out wrong ordered average velocities
			for (int k = 1; k < ptsize - 1; k++)
			{
				if (vDix[k] < vDix[k - 1])
				{
					sprintf(buffer, "Wrong order in average velocity between layer #%d(%lf) and #%d(%lf), SL %s, SID %d",
						k - 1, vDix[k - 1], k, vDix[k], (LPCTSTR)shotline->GetName(), cid);

					pDoc->Log(buffer, LOG_DETAIL);
					AllVelsGood = FALSE;
				}
			}

			// if all velocities are OK then store the average velocity point
			// making sure all velocities are OK
			if (AllVelsGood)
			{
				for (int k = 1; k < ptsize; k++)
				{
					// start at 1, not 0, because we refer to xx[k-1]
					// add new avgvel point to the internal data base
					AddNewPt(shotline->GetName(), (int)(*shotline), cid, CSD_TYPE_AVGVEL, timept->GetX(), timept->GetY(), vDix[k - 1]);
				}
			}
			else
				NumBadPoints++;

			delete vDix;
		} // end of loop over shotpoints

		// give back original points array
		originalpts.RemoveAll();
	} // end of loop over shotlines

	//clear list of all points kept
	keptpts.RemoveAll();

	// clean out all the old rms velocity points
	ClearContents(CSD_TYPE_RMSVEL);

	// Log if there are any skipped points
	if (AfxGetApp()->GetProfileInt("Debug", "ForMyEyesOnly", 0))
	{
		if (HaveToSkip)
		{
			sprintf(buffer, "Dix equation successful. %d points skipped because there were too many", NumSkippedPoints);
			pDoc->Log(buffer, LOG_STD);
		}

		sprintf(buffer, "%d points kept in Dix equation", NumKeptPoints);
		pDoc->Log(buffer, LOG_STD);
	}

	// Check if there were any bad shotpoints found
	if (NumBadPoints > 0)
	{
		sprintf(buffer, "Could not compute interval velocities for %d out  of %d shotpoints (%d%%).\n\nCheck logfile for details.",
			NumBadPoints, NumShotpoints, (100 * NumBadPoints) / NumShotpoints);

		if (pDoc->Error(buffer, (MSG_WARN | MSG_CANCEL)))
			return FALSE;
	}

	return TRUE;
}

//***********************************************************************
// Subroutine : ResampleShotpoints()
// Arguments : timegrid: pointer to list of time-horizons
// horizons: number of time horizons
// Use : Resamples the Shotline data vertically close to the
// time horizons...
// Returns : int : total number of shotpoints
// intvelpts : array of all intvelpts at horizons
//***********************************************************************
int CSeismicData::ResampleShotpoints(CZimsGrid **timegrid, int horizons, CObArray &intvelpts)
{
	int i, j, k;
	int ptsize, percent;
	int shotpts, cid, totalpts = 0;
	CShotLine *shotline;
	CShotPt *timept, *avgvelpt, *intvelpt;
	double *v, *t, *v2, ct, cv;
	double vi, vk, vkp, tk, tkp;
	CObArray walkline;

	// output logfile and progress dialogue
	pDoc->Log("Interpolating Shotpoints...", LOG_STD);
	pDoc->m_pProgress->ChangeText("Interpolating Shotpoints...");

	// loop through all shotlines
	for (i = 0; i < GetSize(); i++)
	{
		// Update the progress monitor...
		percent = (100 * i) / GetSize();
		pDoc->m_pProgress->ChangePercentage(percent);
		// Get the current shotline and its size...
		shotline = (*this)[i];
		shotpts = shotline->GetSize();

		// walk along fake shotline as points are being added
		for (j = 0; j < shotpts; j++)
			walkline.Add((*shotline)[j]);

		// now loop through all the shotpoints in each shotline...
		for (j = 0; j < shotpts; j++)
		{
			avgvelpt = (CShotPt *)walkline.GetAt(j);
			// find the avgvelpts and later the corresponding timepts...
			if (avgvelpt->GetType() != CSD_TYPE_AVGVEL || avgvelpt->GetSize() == 0) continue;

			// now we found the avvelpt with cid
			cid = (int)*avgvelpt;

			// Now find time point for this cid
			timept = shotline->FindSPoint(cid, CSD_TYPE_TIME);
			if (!timept || avgvelpt->GetSize() != timept->GetSize() - 1)
				continue;

			// Allocate arrays for the spline
			// These are the three arrays needed for splint...
			ptsize = avgvelpt->GetSize(); // number of data points (time/avgvel) at that shotpoint
			v = new double[3 * ptsize];
			t = v + ptsize;
			v2 = t + ptsize;

			// fill arrays for the spline with velocity and time
			// delete points which are now obsolete...
			k = 0;
			while (k < ptsize)
			{
				v[k] = (*avgvelpt)[k];
				t[k] = (*timept)[k + 1];
				// delete all old data points...
				delete (CDataPt *)avgvelpt->GetAt(k);
				delete (CDataPt *)timept->GetAt(k + 1);
				// Check if the data point is correctly specified...
				// if not - skip the point
				if ((v[k] != CSD_DONTKNOW) && (t[k] != CSD_DONTKNOW) &&
					(v[k] != CSD_ILLEGAL) && (t[k] != CSD_ILLEGAL))
					k++;
			}

			// Set the sizes of new arrays to zero (everything was erased except the first timept)
			delete (CDataPt *)timept->GetAt(0);
			timept->RemoveAll();
			avgvelpt->RemoveAll();
			// compute 2nd derivative matrix...
			spline(t - 1, v - 1, ptsize, 1.0E30, 1.0E30, v2 - 1);

			// evaluate the average velocities at the horizons
			for (k = 0; k < horizons; k++)
			{
				ct = timegrid[k]->Evaluate(timept->GetX(), timept->GetY());
				if (timegrid[k]->GetZnon(ct))
				{
					timept->Add(new CDataPt(CSD_ILLEGAL));
					avgvelpt->Add(new CDataPt(CSD_ILLEGAL));
				}
				else
				{
					timept->Add(new CDataPt(ct));
					// if time is valid then evaluate the velocity there...
					splint(t - 1, v - 1, v2 - 1, ptsize, ct, &cv);
					avgvelpt->Add(new CDataPt(cv));
				}

				// now convert back to interval velocities...
				if (k == 0)
				{ // this is the first horizon -- interval and average velocity are equal
					AddNewPt(shotline->GetName(), (int)(*shotline), cid, CSD_TYPE_INTVEL,
						avgvelpt->GetX(), avgvelpt->GetY(), (*avgvelpt)[k]);
				}
				else
				{
					vk = (*avgvelpt)[k];
					vkp = (*avgvelpt)[k - 1];
					tk = (*timept)[k];
					tkp = (*timept)[k - 1];

					// Check if all values are OK - then calculate interval velocity
					if (vk == CSD_ILLEGAL || vkp == CSD_ILLEGAL || tk == CSD_ILLEGAL ||
						tkp == CSD_ILLEGAL || fabs(tk - tkp) < FLT_MIN)
					{
						AddNewPt(shotline->GetName(), (int)(*shotline), cid, CSD_TYPE_INTVEL,
							avgvelpt->GetX(), avgvelpt->GetY(), CSD_ILLEGAL);
					}
					else
					{
						// This is now the interval velocity...
						vi = (vk*tk - vkp * tkp) / (tk - tkp);
						AddNewPt(shotline->GetName(), (int)(*shotline), cid, CSD_TYPE_INTVEL,
							avgvelpt->GetX(), avgvelpt->GetY(), vi);
					}
				}
			}

			// give back the memory for this spline
			delete v;

			// increment the total number of intvelpts found.
			// and remember the point for later...
			intvelpt = shotline->FindSPoint(cid, CSD_TYPE_INTVEL);
			intvelpts.Add(intvelpt);
			totalpts++;
		} // end of shotpoint loop
		walkline.RemoveAll();
	} // end if shotline loop

	return(totalpts);
}

//***********************************************************************
// Subroutine : ~CShotLine()
// Arguments : none
// Use : Return memory allocated by CShotLine
// Returns : none
//***********************************************************************
CShotLine::~CShotLine()
{
	int i = 0;

	while (i < GetSize())
	{
		delete (CShotPt *)GetAt(i++);
	}
	RemoveAll();
}

//***********************************************************************
// Subroutine : FindSPoint()
// Arguments : sptid : identification integer of the shotpt
// type : data type of the shot point
// Use : Find a shotpoint in the int. database
// Returns : pointer to CShotPoint or NULL
//***********************************************************************
CShotPt *CShotLine::FindSPoint(int sptid, UBYTE type)
{
	CShotPt *tmppt;
	int i = 0;
	if (type == CSD_TYPE_ANY)
	{
		while (i < GetSize())
		{
			tmppt = (CShotPt *)GetAt(i);
			if (sptid == (int)*tmppt) return tmppt;
			i++;
		}
	}
	else
	{
		while (i < GetSize())
		{
			tmppt = (CShotPt *)GetAt(i);
			if ((sptid == (int)*tmppt) &&
				(type == (UBYTE)*tmppt))
				return tmppt;
			i++;
		}
	}
	return NULL;
}

//***********************************************************************
// Subroutine : GetSPoint()
// Arguments : sptid : identification integer of the shotpt
// Use : Find the column corresponding to a shot point in the
// internal database
// Returns : integer signifying the column of the shot point
//***********************************************************************
int CShotLine::GetSPoint(int sptid)
{
	CShotPt *tmppt;
	int i = 0, j = -1, currsptid, oldsptid = -1;
	while (i < GetSize())
	{
		tmppt = (CShotPt *)GetAt(i);
		currsptid = *tmppt;
		if (currsptid != oldsptid)
		{
			j++;
			oldsptid = currsptid;
		}
		if (sptid == currsptid)
			return j;
		i++;
	}
	return -1;
}

//***********************************************************************
// Subroutine : ~CShotPt()
// Arguments : none
// Use : Return memory allocated by CShotPt
// Returns : none
//***********************************************************************
CShotPt::~CShotPt()
{
	int i = 0;
	while (i < GetSize())
	{
		delete (CDataPt *)GetAt(i++);
	}
	RemoveAll();
}
