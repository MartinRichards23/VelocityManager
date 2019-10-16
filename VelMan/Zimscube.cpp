#include "VelmanDoc.h"
#include "resource.h"	// needed for importpr.h to function
#include "ProgressDlg.h"	// needed to update "progress" dialog box
#include "raytracepropsDlg.h"
#include <direct.h>
#include <dos.h>
#include <io.h>
#include <string.h>

//***********************************************************************
// Subroutine	:			ClearContents()
// Arguments	:			none
// Use        :     Return memory allocated by CZimsCube
// Returns    :     none
//***********************************************************************
void CZimsCube::ClearContents(void)
{
	struct _finddata_t  c_file;
	long hfile;
	char buf[513];
	CString s = pDoc->TargetDir + "*.raw";
	int i=0;
	// remove all zimsgrids...
	while(i < GetSize())
	{
		delete (CZimsGrid *) GetAt(i++);
	}
	RemoveAll();
	// remove all old swap files...
	strcpy(buf, (const char *)s);
	if((hfile=_findfirst(buf, &c_file ))!=-1L)
		_unlink((const char *) c_file.name);

	// Find the rest of the files
	if ( hfile != -1L )
		while( _findnext( hfile, &c_file ) == 0 )
			_unlink((const char *) c_file.name);
}

//***********************************************************************
// Subroutine	:			Read()
// Arguments	:			filename : zims grid filename (.ZMG)
// Use        :     Read binary file from CZimsCube
// Returns    :     sucess
//***********************************************************************
BOOL CZimsCube::Read(void)
{
	CString filename = pDoc->TargetDir+pDoc->BaseName+".zmg";
#ifdef VELMAN_UNIX
	std::ifstream infile((const char *) filename, ios::in);
#else
	std::ifstream infile((const char *) filename, ios::in|ios::binary);
#endif
	int i, size, gridid;
	CZimsGrid *zgrid;
	// ClearContents before reading new data base...
	ClearContents();
	if(infile.fail())
	{
		pDoc->Error("Could not read zimscube data file " + filename, MSG_ERR);
		return FALSE;
	}
	infile.read((char *) &size, sizeof(int));
	i=0;
	while(i < size)
	{
		if(pDoc->m_pProgress)
			pDoc->m_pProgress->ChangePercentage(20+(i*40)/size);
		zgrid = new CZimsGrid;
		if(!(zgrid->Read(infile, pDoc)))
			return(FALSE);
		if(zgrid->GetID() == 0)
		{
			DesignateGrid(zgrid);
		}
		else
		{
			gridid = FindNewID(zgrid->GetHorizon(), zgrid->GetType());
			zgrid->SetID(gridid);
		}
		CObArray::Add(zgrid);
		i++;
	}
	return(TRUE);
}

//***********************************************************************
// Subroutine	:			ReadASCIIZimsGrid()
// Arguments	:			suppname : full name of file to be loaded
// Use        :     Read Seismic Data into internal data base from a
//                  Zims datafile
// Returns    :     void
//***********************************************************************
BOOL CZimsCube::ReadASCIIZimsGrid(CString suppname)
{
	int i, length;
	int rows=0, columns=1, c=0, r=0;
	int atype, history;
	double xlim[2], ylim[2];
	double nullval, tempf;
	CZimsGrid *zimsgrid;
	CString name;
	char *buffer, **endb=NULL;
	char inputbuffer[CSD_MAXLEN];
#ifdef VELMAN_UNIX
	ifstream suppfile;
#else
	std::ifstream suppfile;
#endif
	suppname = pDoc->ZimsDir + suppname;
	suppfile.open(suppname);
	if(suppfile.fail())
	{
		pDoc->Error("Could not open seismic data file " + suppname, MSG_ERR);
		return FALSE;
	}
	pDoc->Log("Opened zims grid data file " + suppname, LOG_STD);
	// Read entire file
	while(!suppfile.eof())
	{
		suppfile.getline(inputbuffer, CSD_MAXLEN-1);
		// Look for comment lines
		if(inputbuffer[0] == '!')
		{
			// if comment line contains filename, read filename
			if(strstr((const char*) inputbuffer, "ZIMS FILE"))
			{
				// New Grid: check if there was enough data read into grid
				if(c != (columns-1) || r != rows)
				{
					pDoc->Error("Error in reading Zims file, not enough data", MSG_ERR);
					suppfile.close();
					return FALSE;
				}
				// as new file name, allocate new grid, reset grid position
				zimsgrid = new CZimsGrid(pDoc);
				c=0; r=0;
				zimsgrid->Stamp();
				// Extract name from file name line and add new grid to array
				length = strlen(inputbuffer);
				for(i=0;i<length;i++)
					if(inputbuffer[i] == ':')
					{
						sscanf(inputbuffer+i+1, "%s", name);
						Add(zimsgrid, name,-1); //||?? Cannot add with horizon -1!
						break ;
					}
					// ignore further comment lines
					do
					{
						suppfile.getline(inputbuffer, CSD_MAXLEN-1);
					}	while(inputbuffer[0] == '!');
					// this is the first definition data line, contains type and history
					sscanf(inputbuffer, "%d%*s%d", &atype, &history);
					// Check if type of zims file is correct
					if(atype != 1)
					{
						pDoc->Error(suppname + " is not a valid zims grid file", MSG_ERR);
						return FALSE;
					}
					// this is the second definition data line, contains rows and columns
					suppfile.getline(inputbuffer, CSD_MAXLEN-1);
					sscanf(inputbuffer, "%d%*s%d", &rows, &columns);
					// Initialize new zimsgrid and allocate memory
					if(!zimsgrid->Init(rows, columns, pDoc))
					{
						pDoc->Error("Error allocating new Zims grid " + zimsgrid->GetName(), MSG_ERR);
						suppfile.close();
						return FALSE;
					}
					// this is the third definition data line, contains x and y limits
					suppfile.getline(inputbuffer, CSD_MAXLEN-1);
					sscanf(inputbuffer, "%f%*s%f%*s%f%*s%f", xlim, ylim, xlim+1, ylim+1);
					zimsgrid->WriteLimits(xlim, ylim);
					// this is the fourth definition data line, contains null value
					suppfile.getline(inputbuffer, CSD_MAXLEN-1);
					sscanf(inputbuffer, "%f", &nullval);
					continue;
			}
			else continue;
		}
		// rows and colums defined properly ?
		if(rows == 0 || columns == 0)
		{
			pDoc->Error("Error in reading Zims file, no proper header", MSG_ERR);
			suppfile.close();
			return FALSE;
		}
		// loop over all column data :
		buffer = inputbuffer;
		tempf = (double) strtod(buffer, endb);
		while(buffer != *endb)
		{
			// see if nullval was read
			if(tempf == nullval) tempf = CSD_ILLEGAL;
			// new column?
			if(r == rows)
			{ r = 0; c++; }
			// too much data in file
			if(c == columns)
			{
				pDoc->Error("Error in reading Zims file, too many columns",	MSG_ERR);
				suppfile.close();
				return FALSE;
			}
			// put data into grid
			(zimsgrid->GetGrid())[r++][c] = tempf;
			// read new grid value
			buffer = *endb;
			tempf = (double) strtod(buffer, endb);
		}
	}
	return TRUE;
}

//***********************************************************************
// Subroutine	:			Write()
// Arguments	:			filename : zims grid filename (.MFD)
// Use        :     Write binary file of CZimsCube
// Returns    :     sucess
//***********************************************************************
BOOL CZimsCube::Write(void)
{
	CString filename = pDoc->TargetDir+pDoc->BaseName;
#ifdef VELMAN_UNIX
	ofstream outfile((const char *) (filename+".zm1"));
#else
	std::ofstream outfile((const char *) (filename+".zm1"), ios::binary);
#endif
	int i, size=GetSize();
	if(outfile.fail())
	{
		pDoc->Error("Could not write grid data file " + filename, MSG_ERR);
		return FALSE;
	}
	outfile.write((char *) &size, sizeof(int));
	i=0;
	while(i < size)
	{
		if(pDoc->m_pProgress)
			pDoc->m_pProgress->ChangePercentage(20+(i*40)/size);
		if(!(((*this)[i])->Write(outfile)))
			return FALSE;
		i++;
	}
	// close the output file and copy to proper location
	outfile.close();
	_unlink((const char *) (filename+".zmg"));
	rename((const char *) (filename+".zm1"), (const char *) (filename+".zmg"));
	return(TRUE);
}

//***********************************************************************
// Subroutine	:			WriteASCIIInternal()
// Arguments	:			none
// Use        :     Write ASCII file of CZimsCube
// Returns    :     sucess
//***********************************************************************
BOOL CZimsCube::WriteASCIIInternal(void)
{
	CString filename = pDoc->TargetDir+pDoc->BaseName+".exp";

	int i=0;
	while(i < GetSize())
	{
		// change progress monitor if there is any. Changes in the range 20%..70%
		if(pDoc->m_pProgress)
			pDoc->m_pProgress->ChangePercentage(30+(i*70)/GetSize());
		if(!(((CZimsGrid *) GetAt(i++))->WriteASCII(filename))) return FALSE;
	}
	return(TRUE);
}

//***********************************************************************
// Subroutine	:			WriteZimsFile()
// Arguments	:			zimsdata   : opened file for zimsdata
//                  zimsheader : opened file for zimsheader data
// Use        :     Write Zims Cube to disk
// Returns    :     void
//***********************************************************************
BOOL CZimsCube::WriteZimsFiles(void)
{
	CZimsGrid *zimsgrid;
	for(int i=0;i<GetSize();i++)
	{
		zimsgrid = (CZimsGrid *) GetAt(i);
		if(!zimsgrid->WriteZimsFile(pDoc->GridFileType, pDoc->TargetDir))
			return FALSE;
	}
	return TRUE;
}

//***********************************************************************
// Subroutine	:			CreateGrids()
// Arguments  :     none
// Use        :     Does all the resampling (1D along the shotpoints
//                  and from the irregular to the regular grid...)
// Returns    :     void
//***********************************************************************
BOOL CZimsCube::CreateGrids(void)
{
	int i, j, k, percent;
	int rows, columns;
	int horizons, totalpts, stepsize;
	CObArray intvelpts;
	double t0, t1;
	double *x, *y, **p, *f;
	char buffer[128];
	BOOL NormaliseBeforeResampling;
	CZimsGrid **timegrid, **intvelgrid;

	// read .ini entries
	NormaliseBeforeResampling=
		(AfxGetApp()->GetProfileInt("Debug", "NormaliseBeforeRMSResampling", 1))==1;
	stepsize = AfxGetApp()->GetProfileInt("ArithmeticParams", "RMSResampleStep", 3);

	// Number of Horizons from document
	horizons = pDoc->horizonlist.GetSize();
	pDoc->Log("Finding time grids and checking for inconsistencies...", LOG_STD);
	pDoc->m_pProgress->ChangeText("Checking for grid inconsistencies...");

	// get memory for time and intvelgrid pointers
	timegrid = (CZimsGrid **) new CZimsGrid *[2*horizons];
	intvelgrid = timegrid + horizons;

	// Get all the time horizons
	for(i=0;i<horizons;i++)
	{
		if(!(timegrid[i] = FindHorizon(i, CSD_TYPE_TIME, 0)))
		{
			sprintf(buffer, "Could not find data for horizon %d", i+1);
			pDoc->Error(buffer, MSG_ERR);
			return(FALSE);
		}
	}

	// Get limits and size of time horizons
	rows = timegrid[0]->GetRows();
	columns = timegrid[0]->GetColumns();
	x = timegrid[0]->GetXlimits();
	y = timegrid[0]->GetYlimits();

	// Check if all the horizons have the same size and extent
	for(i=1;i<horizons;i++)
	{
		timegrid[i]->Stamp();
		timegrid[i-1]->Stamp();
		if(*timegrid[i] != *timegrid[0])
		{
			pDoc->Error(timegrid[i]->GetName()+CString(" is not size compatible"), MSG_ERR);
			return FALSE;
		}
	}

	// Check for inconsistencies in the timegrid values...
	// If there is an inconsistency always set to timevalue of the
	// higher grid
	for(j=0;j<rows;j++)
		for(k=0;k<columns;k++)
			for(i=1;i<horizons;i++)
			{
				t1 = timegrid[i]->GetGrid()[j][k];
				t0 = timegrid[i-1]->GetGrid()[j][k];
				if(timegrid[i]->GetZnon(t1) || timegrid[i-1]->GetZnon(t0))
					continue;
				if(t1 < t0)
				{
					timegrid[i]->GetGrid()[j][k] = t0;
					sprintf(buffer, "Point[%3d][%3d] overlaps in t-horizons %d and %d", j, k, i, i-1);
					pDoc->Log(buffer, LOG_STD);
				}
			}

			// now resample the internal data base at the horizons...
			// the rmspoints are the result.
			totalpts = pDoc->seismicdata.ResampleShotpoints(timegrid, horizons, intvelpts);

			// check if resampling is possible...
			if(totalpts == 0)
			{
				pDoc->Error("No valid shotpoint has been found", MSG_ERR);
				return FALSE;
			}
			// after this step the internal data base is filled with rms and time
			// data *close* to the horizons - we can now start to move to a regular
			// grid... Now we proceed to do the 2D resampling using the tps.
			// timepts and rmspts contain totalpts number of resampled shotpoints

			pDoc->m_pProgress->ChangeText("Keying interval velocities to time grids.");
			// Allocate arrays for the coord positions and function values

			p = dmatrix(1, totalpts, 1, 2);  // this is the shotpoint x-position array
			f = dvector(1, totalpts);      // this is the array for the rmstimes and velocities
			// First make an array of all shotpoints coordinates for the thin plate spline

			for(i=1;i<=totalpts;i++)
			{
				// fill values for positions into matrix - be careful unit offset!
				p[i][1] = (double) ((CShotPt *) intvelpts[i-1])->GetX();
				p[i][2] = (double) ((CShotPt *) intvelpts[i-1])->GetY();
			}

			// Allocate new rmshorizons
			for(i=0;i<horizons;i++)
			{
				// Set up new avgvelgrid...
				intvelgrid[i] = new CZimsGrid;
				if(!intvelgrid[i]->Init(rows, columns, pDoc, CSD_TYPE_INTVEL))
				{
					pDoc->Error("Could not allocate all IntVelGrids", MSG_ERR);
					delete intvelgrid[i];
					delete timegrid;
					free_dvector(f, 1, totalpts);
					free_dmatrix(p, 1, totalpts, 1, 2);
					return FALSE;
				}
				// initialize avgvelgrid and add it to the cube at horizon i
				intvelgrid[i]->WriteLimits(x, y);

				// Write name of new intvelgrid
				if(i == 0)
				{
					Add(intvelgrid[i], "Surface_to_" + pDoc->horizonlist[i]->GetName(), i);
				}
				else
				{
					Add(intvelgrid[i], pDoc->horizonlist[i-1]->GetName() + "_to_" +
						pDoc->horizonlist[i]->GetName(), i);
				}

				// Log that this grid was created
				pDoc->Log("Created grid " + intvelgrid[i]->GetName(), LOG_DETAIL);

				// This is for the velocities in the rms horizons
				// fill the time function values into the f array
				for(j=1;j<=totalpts;j++)
					f[j] = (*((CShotPt *) intvelpts[j-1]))[i];

				// update the progress dialogue
				percent = 100*i/horizons;
				pDoc->m_pProgress->ChangePercentage(percent);

				// Do the resampling
				if(NormaliseBeforeResampling)
					intvelgrid[i]->Resample(totalpts,-stepsize, f, p, TRUE, 90.0);
				else
					intvelgrid[i]->Resample(totalpts,-stepsize, f, p);
				intvelgrid[i]->ClearResample();
				intvelgrid[i]->CalcAverage();
			}

			delete timegrid;
			free_dvector(f, 1, totalpts);
			free_dmatrix(p, 1, totalpts, 1, 2);

			return(TRUE);
}

//***********************************************************************
// Subroutine	:			SplineRmsHorizons()
// Arguments  :     timegrid : array of pointers to all grids containing
//                     timehorizons
//                  avgvelgrid : initialized but empty array to be
//                     filled with resampled rms-times
//                  rmshorizon : array of pointers to resampled slices
//                     at constant time
//                  horizons : number of physical horizons
//                  rmshorizons : number of time slices
//                  rmsinterval : time between time slices
// Use        :     Splines all the time slices to the real physical
//                  horizons
// Returns    :     **&avgvelgrid : pointer to an array of pointers to all
//                    grids containing the velocity horizons .These are
//                    the horizons resampled from the internal data base...
//***********************************************************************
BOOL CZimsCube::SplineAvgHorizons(CZimsGrid **&timegrid, CZimsGrid **&avgvelgrid,
								  CZimsGrid **&rmshorizon, int horizons, int rmshorizons, double rmsinterval)
{
	int i, j, k, rows, columns, area;
	int startpt, endpt, intsleft, xtraints;
	int splinenum, ptsresampled;
	int percent, oldpercent=-5;
	double ct, cv, *v, *t, *v2, min;
	// change progress monitor header...
	pDoc->m_pProgress->ChangeText("Splining RMS data to time horizons");
	pDoc->Log("Splining RMS data to time horizons", LOG_STD);
	// get rows and columns from array...
	rows = timegrid[0]->GetRows();
	columns = timegrid[0]->GetColumns();
	area = rows*columns;
	// allocate memory for spline
	v = dvector(0, rmshorizons-1);
	t = dvector(0, rmshorizons-1);
	v2 = dvector(0, rmshorizons-1);
	// find minimum time in horizons
	min	= floor(timegrid[0]->CalcMinVal()/rmsinterval)*rmsinterval;
	// fill vector for avghorizon times...
	for(i=0;i<rmshorizons;i++)
	{
		t[i] = min+rmsinterval*i;
		rmshorizon[i]->Stamp();
	}
	// calculate different quantities to specify how often and with how many
	// pts resampling should be undertaken...
	// all of these are intervals not points - so be careful...
	splinenum = (rmshorizons-1)/ZIMS_RMSRESAMPLEINTERVALS;
	// loops over all points in data base...
	for(i=0;i<rows;i++)
	{
		for(j=0;j<columns;j++)
		{
			// Update percentage in monitor box...
			percent = 100*(i*columns+j)/area;
			if(percent >= (oldpercent+5))
			{
				pDoc->m_pProgress->ChangePercentage(percent);
				oldpercent = percent;
			}
			// fill v vector for this point...
			for(k=0;k<rmshorizons;k++)
				v[k] = ((double **)(*rmshorizon[k]))[i][j];
			// define values for resampling
			intsleft = (rmshorizons-1)%ZIMS_RMSRESAMPLEINTERVALS;
			xtraints = (intsleft+splinenum-1)/splinenum;
			ptsresampled = 0;
			startpt = 0;
			// do splines...
			for(k=0;(k<splinenum) && (ptsresampled<horizons);k++)
			{
				// to which point will we resample?
				endpt = startpt+ZIMS_RMSRESAMPLEINTERVALS;
				// Check if there are any left over points and distribute them
				// evenly...
				if(intsleft)
				{
					if(intsleft > xtraints)
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
				// !!sk126 vd1 = (v[startpt+1] - v[startpt])/(t[startpt+1] - t[startpt]);
				// !!sk126 vdn = (v[endpt] - v[endpt-1])/(t[endpt] - t[endpt-1]);
				// Call the spline routine to calculate array of 2nd derivatives
				// Be carefule spline is unit offset for all arrays!
				spline(t+startpt-1, v+startpt-1, endpt-startpt+1, 1.0E30, 1.0E30, v2-1);
				// loop till all points are resampled...
				while(ptsresampled < horizons)
				{
					// resample internal data base at time of the horizons...
					ct = ((double **)(*timegrid[ptsresampled]))[i][j];
					// check if there is a znon value in the timegrid at that location
					// if there is then put a znon into the corresponding velocity grid...
					if(timegrid[ptsresampled]->GetZnon(ct))
					{
						((double **)(*avgvelgrid[ptsresampled]))[i][j] =
							avgvelgrid[ptsresampled]->GetZnon();
						ptsresampled++;
						continue;
					}
					// Check if the point at which we want to resample is in this resampling interval
					// If not, continue to the next interval...
					if((ct > t[endpt]) && (k != (splinenum-1))) break;
					// evaluate at that point close to the horizon...
					splint(t+startpt-1, v+startpt-1, v2-1, endpt-startpt+1, ct, &cv);
					// put resampled value back into the avgvelgrid...
					((double **) (*avgvelgrid[ptsresampled]))[i][j] = cv;
					ptsresampled++;
				}
				// go to the next interval;
				startpt=endpt;
			} // end of loop over splinenums
		} // end of loop over columns
	} // end of loop over rows
	// Fill in average and znons of avgvelgrids...
	for(i=0;i<horizons;i++)
		avgvelgrid[i]->CalcAverage();
	// release memory
	free_dvector(v, 0, rmshorizons-1);
	free_dvector(t, 0, rmshorizons-1);
	free_dvector(v2, 0, rmshorizons-1);
	return(TRUE);
}

//***********************************************************************
// Subroutine	:			DoDixEquation()
// Arguments  :     none
// Use        :     Calculates the interval velocity in of all layers
//                  and stores the result in the ZimsCube
// Returns    :     T/F depending on outcome
//***********************************************************************
BOOL CZimsCube::DoDixEquation(void)
{
	// First Do the Dix-Equation on the internal data base...
	// This will actually first create interval velocities, then
	// it will create average velocities, which can then be splined
	// change Progress monitor header...
	pDoc->m_pProgress->ChangeText("Calculating interval velocities");
	if(!pDoc->seismicdata.DoDixEquation())
		return FALSE;

	// Fill these grids with the relevant data...
	if(!CreateGrids())
		return FALSE;

	return(TRUE);
}

//***********************************************************************
// Subroutine	:			DepthConvert()
// Arguments  :     Scope   : requested project scope
//                  layer   : hornum to be converted (we know all input data is avail.)
// Returns    :     TRUE if sucessful
//***********************************************************************
BOOL CZimsCube::DepthConvert(int ProjectScope, int layer)
{
	CZimsGrid *timegrid0=NULL, *timegrid1=NULL, *depthgrid=NULL, *gridabove=NULL;
	CZimsGrid *velgrid=NULL, *depthgrid0=NULL;
	CHorizon *horizon0=NULL, *horizon1=NULL;
	CMeasurePt *mpt0=NULL, *mpt1=NULL;
	char buf[512];
	int i, j;
	double tInt, dInt, z1, tTop;

	// find the velocity and timegrid for this layer
	timegrid1 = FindHorizon(layer, CSD_TYPE_TIME, 0);
	velgrid = FindHorizon(layer, CSD_TYPE_INTVEL, 0);

	if(layer != 0)
	{
		timegrid0 = FindHorizon(layer-1, CSD_TYPE_TIME, 0);
		depthgrid0 = FindHorizon(layer-1, CSD_TYPE_DEPTH, 0);
	}
	if(!timegrid1 || !velgrid || (layer != 0 && !timegrid0 && !depthgrid0)) 
	{
		pDoc->Error("Could not find all grids for this operation (although I was sure I could)\n\n"
			"This is an internal error, please contact Cambridge Petroleum Software.", MSG_ERR);
		return FALSE;	// some really weird error
	}

	// stamp all grids
	if(velgrid) velgrid->Stamp();
	if(timegrid0) timegrid0->Stamp();
	if(timegrid1) timegrid1->Stamp();
	if(depthgrid0) depthgrid0->Stamp();

	// make sure no negative velocities anywhere
	for(i=0;i<velgrid->GetRows();i++)
	{
		for(j=0;j<velgrid->GetColumns();j++)
		{
			if(velgrid->GetGrid()[i][j]<0.0)
				velgrid->GetGrid()[i][j]=velgrid->GetZnon();
		}
	}

	// Now if we have well data we must tie the velgrids to the wells
	if(pDoc->ProjectScope==SCOPE_TIMEWELLRMS)
	{
		if(layer != 0)
		{
			// Get horizon data of this layer and the layer above...
			horizon0 = (CHorizon *) pDoc->horizonlist[layer-1];
			horizon1 = (CHorizon *) pDoc->horizonlist[layer];
			// go through all the measurement points
			for(i=0;i<horizon1->GetSize();i++)
			{
				// Get measurement points out of the data base
				mpt1 = (*horizon1)[i];
				if(i < horizon0->GetSize())
					mpt0 = (*horizon0)[i];
				else
					mpt0 = NULL;

				// check if the well is deviated...
				if(!mpt0 || mpt0->ReadX() != mpt1->ReadX() || mpt0->ReadY() != mpt1->ReadY())
				{
					// store the depth and time of the layer above
					z1=depthgrid0->Evaluate(mpt1->ReadX(), mpt1->ReadY());
					if(depthgrid0->GetZnon(z1))
						mpt1->WritedTop(MPT_ILLEGAL);
					else
						mpt1->WritedTop(z1);

					tTop=timegrid0->Evaluate(mpt1->ReadX(), mpt1->ReadY());
					if(timegrid0->GetZnon(tTop))
						mpt1->WritetTop(MPT_ILLEGAL);
					else
						mpt1->WritetTop(tTop);
					// compute the interval time
					if(mpt1->ReadTime()!=MPT_ILLEGAL && tTop!=MPT_ILLEGAL)
						mpt1->WritetInt((tInt=max(0.0, mpt1->ReadTime()-tTop)));
					else
						mpt1->WritetInt((tInt=MPT_ILLEGAL));
					// compute the interval depth
					if(mpt1->ReadDepth()!=MPT_ILLEGAL && z1!=MPT_ILLEGAL)
						mpt1->WritedInt((dInt=max(0.0, mpt1->ReadDepth()-z1)));
					else
						mpt1->WritedInt((dInt=MPT_ILLEGAL));
					// compute the interval velocity and store it in the data base...
					if(tInt!=MPT_ILLEGAL && dInt!=MPT_ILLEGAL)
						mpt1->WritevInt(2000.0*dInt/tInt);
					else
						mpt1->WritevInt(MPT_INVALID);
				}
			}
		}

		// now use the data base values to tie the velocity grid
		velgrid->TieGrid(TRUE, 90.0);
	}

	// update dialogue box
	sprintf(buf, "Layer %s being converted",
		(const char *)(((CHorizon *)pDoc->horizonlist.GetAt(layer))->GetName()));
	pDoc->m_pProgress->ChangeText(buf);
	pDoc->m_pProgress->ChangePercentage(0);

	depthgrid = new CZimsGrid(pDoc);
	*depthgrid = *timegrid1; // assign all grid data and copy time1

	if(layer != 0)
		*depthgrid -= *timegrid0;

	*depthgrid *= (1.0/2000.0);	  			// times are two-way times in msec
	*depthgrid *= *velgrid;
	pDoc->m_pProgress->ChangePercentage(50);

	depthgrid->SetType(CSD_TYPE_DEPTH);
	depthgrid->SetUnit();
	depthgrid->Stamp();
	Add(depthgrid, CString("D_")+timegrid1->GetName(), layer);

	pDoc->Log(CString("Depth converted layer ")+timegrid1->GetName(), LOG_STD);

	if(DesignateGrid(depthgrid) != 0)
	{             // make into master depthgrid for layer
		if(pDoc->Error("There already seems to be a depth grid for horizon " +
			pDoc->horizonlist[layer]->GetName(), MSG_WARN|MSG_CANCEL)) return FALSE;
	}

	if(layer != 0)
	{
		gridabove = FindHorizon(layer-1, CSD_TYPE_DEPTH, 0);
		if(!gridabove)
		{
			pDoc->Error("Could not find depthgrid above "+depthgrid->GetName()+
				" to calculate absolute depth", MSG_ERR);
			return FALSE;
		}
		gridabove->Stamp();
		*depthgrid += *gridabove;
	}
	pDoc->m_pProgress->ChangePercentage(100);

	return TRUE;
}

//***********************************************************************
// Subroutine	: FindHorizon()
// Arguments    : horizon : horizon number to be found
//                type    : type of horizon to be found
//                gridid  : gridid of the grid with that type and horizon
//                          if gridid == -1 the last grid will be found
// Use        :     Finds the horizon with the correct number, type
//                  and ID in the ZimsCube
// Returns    :     A pointer to the correct CZimsGrid or NULL
//***********************************************************************
CZimsGrid *CZimsCube::FindHorizon(int horizon, UBYTE type, int gridid)
{
	int i = 0;
	CZimsGrid *currentgrid, *maxgrid=NULL;
	if(type == CSD_TYPE_UNKNOWN)
		return(NULL);
	if(gridid == -1)
	{
		for(i=0;i<GetSize();i++)
		{
			currentgrid = (*this)[i];

			if(currentgrid->GetType() == type && currentgrid->GetHorizon() == horizon
				&& currentgrid->GetID() > gridid)
			{
				gridid = currentgrid->GetID();
				maxgrid = currentgrid;
			}
		}
		return(maxgrid);
	}
	else
	{
		for(i=0;i<GetSize();i++)
		{
			currentgrid = (*this)[i];

			if(currentgrid->GetType() == type && currentgrid->GetHorizon() == horizon
				&& currentgrid->GetID() == gridid)
			{
				return(currentgrid);
			}
		}
	}
	return(NULL);
}

//***********************************************************************
// Subroutine	:			FindHorizon()
// Arguments  :     type    : type of horizon to be found (gridid always 0)
// Use        :     Finds the horizon with any number, type
//                  and ID 0 in the ZimsCube
// Returns    :     TRUE if any horizon found
//***********************************************************************
BOOL CZimsCube::FindHorizon(UBYTE type)
{
	CZimsGrid *currentgrid;

	for(int i=0;i<GetSize();i++)
	{
		currentgrid = (*this)[i];
		if((currentgrid->GetType() == type) && (currentgrid->GetID() == 0))
			return(TRUE);
	}

	return(FALSE);
}

//***********************************************************************
// Subroutine	:			FindHorizon()
// Arguments  :     horizon : horizon number to be found
//                  type   : type of horizon to be found
// Use        :     Finds all horizons with the correct number and type
//                  in the ZimsCube
// Returns    :     A pointer to a CObArray which contains the grids
//***********************************************************************
CObArray *CZimsCube::FindHorizon(int horizon, UBYTE type)
{
	CZimsGrid *currentgrid;
	CObArray *grids = new CObArray;

	if(type == CSD_TYPE_UNKNOWN)
		return(NULL);

	for(int i=0;i<GetSize();i++)
	{
		currentgrid = (*this)[i];
		if((currentgrid->GetType() == type) && (currentgrid->GetHorizon() == horizon))
			grids->Add(currentgrid);
	}
	return(grids);
}

int CZimsCube::FindHorizon(CZimsGrid *grid)
{
	for(int i=0;i<GetSize();i++)
	{
		if(((CZimsGrid *) GetAt(i)) == grid)
			return(i);
	}

	return(-1);
}

//***********************************************************************
// Subroutine	:		  DesignateGrid()
// Arguments  :     pointer to grid
// Use        :     Makes the specified grid the new default grid for
//                  the grid's horizon and type...
// Returns    :     new gridid of the grid which was expelled from that
//                  place...
//***********************************************************************
int CZimsCube::DesignateGrid(CZimsGrid *newgrid)
{
	CZimsGrid *oldgrid;
	int id = -1;

	if((int) (*newgrid) == -1)
	{
		if(newgrid->GetID() == -1)
		{
			id = FindNewID((int)(*newgrid), newgrid->GetType());
			newgrid->SetID(id);
		}
	}
	else
	{
		oldgrid = FindHorizon((int) (*newgrid), newgrid->GetType(), 0);
		if(oldgrid && oldgrid != newgrid)
		{
			id = FindNewID((int) (*oldgrid), oldgrid->GetType());
			oldgrid->SetID(id);
		}
		else
		{
			id = 0;
		}
		newgrid->SetID(0);
	}
	return(id);
}

//***********************************************************************
// Subroutine	:		  RemoveAt()
// Arguments  :     horizon : CZimsGrid * to horizon to be removed
// Use        :     Takes the horizon specified out of the zimscube
// Returns    :     1 if successful,
//                  0 if not
//***********************************************************************
BOOL CZimsCube::RemoveAt(CZimsGrid *horizon)
{
	if(!horizon)
		return(TRUE);

	for(int i=0;i<GetSize();i++)
	{
		if((*this)[i] ==  horizon)
		{
			delete horizon;
			CObArray::RemoveAt(i);
			return(TRUE);
		}
	}
	return(FALSE);
}

//***********************************************************************
// Subroutine	:		  FindNewID()
// Arguments  :     type    : type of grid for which the new id is required
//                  horizon : horizon under which the grid should be added
// Use        :     Searches through zimscube for next free space of
//									the correct type
// Returns    :     new grid id of the grid in that horizon with that type
//									that is still empty...
//***********************************************************************
int CZimsCube::FindNewID(int horizon, UBYTE type)
{
	int i, gridid = (horizon == -1) ? 0 : -1;
	CZimsGrid *currentgrid;
	for(i=0;i<GetSize();i++)
	{
		currentgrid = (*this)[i];
		if((currentgrid->GetHorizon() == horizon) && (currentgrid->GetType() == type)
			&& (currentgrid->GetID() > gridid))
			gridid = currentgrid->GetID();
	}
	return(gridid + 1);
}

//***********************************************************************
// Subroutine	:		  Add()
// Arguments  :     grid : CZimsGrid * to grid to be added
//                  name : name of grid to be added
//                  horizon : horizon under which the grid should be added
//                  if horizon is -1 - add under the horizon in the grid.
// Use        :     Adds new grid into zimscube...
// Returns    :     gridid of the grid in that horizon with that type
//***********************************************************************
int CZimsCube::Add(CZimsGrid *grid, CString name, int horizon)
{
	int gridid;
	if(horizon != -1)
		grid->SetHorizon(horizon);
	if(!name.IsEmpty())
		grid->SetName(name);
	// find next free ID in that grid type and horizon...
	gridid = FindNewID(grid->GetHorizon(), grid->GetType());
	// fill grid with new data...
	grid->SetID(gridid);
	grid->CalcAverage();

	// Add the grid to the cube...
	CObArray::Add(grid);
	return (gridid);
}

//***********************************************************************
// Subroutine	:		  FillListboxWithGridnames()
// Arguments  :     pLB : pointer to list box
//                  entries : references to entries in the box
//									timegridsonly: excludes all other grids
// Use        :     Puts all grid names into the List box
// Returns    :     number of gridnames...
//***********************************************************************
int CZimsCube::FillListboxWithGridnames(CListBox *pLB, CWordArray &entries, BOOL Timegridsonly)
{
	int numentries=0, newentry = 0;
	int i = 0;
	BOOL found;
	CZimsGrid *grid;
	CString s;
	// add depth grids to list
	if(!Timegridsonly)
	{
		found=FALSE;
		for(i=0;i<GetSize();i++)
		{
			grid=(CZimsGrid *)GetAt(i);
			if(grid->GetType()==CSD_TYPE_DEPTH)
			{
				if(!found)
				{
					found=TRUE;
					pLB->AddString("Depth grids:");
					numentries++;
				}
				s="\t"+grid->GetName();
				if(grid->GetID()==0 && grid->GetHorizon()>=0)
					s="[P]"+s;
				pLB->AddString(s);
				numentries++;
				newentry=(i<<8) + (numentries-1);
				entries.Add(newentry);
			}
		}
	}
	// add time grids to list
	found=FALSE;
	for(i=0;i<GetSize();i++)
	{
		grid=(CZimsGrid *)GetAt(i);
		if(grid->GetType()==CSD_TYPE_TIME)
		{
			if(!found)
			{
				found=TRUE;
				if(!Timegridsonly)
				{
					pLB->AddString("Time grids:");
					numentries++;
				}
			}
			s="\t"+grid->GetName();
			if(grid->GetID()==0 && grid->GetHorizon()>=0)
				s="[P]"+s;
			pLB->AddString(s);
			numentries++;
			newentry=(i<<8) + (numentries-1);
			entries.Add(newentry);
		}
	}
	// add interval velocity grids to list
	if(!Timegridsonly)
	{
		found=FALSE;
		for(i=0;i<GetSize();i++)
		{
			grid=(CZimsGrid *)GetAt(i);
			if(grid->GetType()==CSD_TYPE_INTVEL)
			{
				if(!found)
				{
					found=TRUE;
					pLB->AddString("Interval velocity grids:");
					numentries++;
				}
				s="\t"+grid->GetName();
				if(grid->GetID()==0 && grid->GetHorizon()>=0)
					s="[P]"+s;
				pLB->AddString(s);
				numentries++;
				newentry=(i<<8) + (numentries-1);
				entries.Add(newentry);
			}
		}
		// add average velocity grids to list
		found=FALSE;
		for(i=0;i<GetSize();i++)
		{
			grid=(CZimsGrid *)GetAt(i);
			if(grid->GetType()==CSD_TYPE_AVGVEL)
			{
				if(!found)
				{
					found=TRUE;
					pLB->AddString("Average velocity grids:");
					numentries++;
				}
				s="\t"+grid->GetName();
				if(grid->GetID()==0 && grid->GetHorizon()>=0)
					s="[P]"+s;
				pLB->AddString(s);
				numentries++;
				newentry=(i<<8) + (numentries-1);
				entries.Add(newentry);
			}
		}
		// add residual grids to list
		found=FALSE;
		for(i=0;i<GetSize();i++)
		{
			grid=(CZimsGrid *)GetAt(i);
			if(grid->GetType()==CSD_TYPE_RESIDUAL)
			{
				if(!found)
				{
					found=TRUE;
					pLB->AddString("Residual grids:");
					numentries++;
				}
				s="\t"+grid->GetName(); //residual grids are never primary

				pLB->AddString(s);
				numentries++;
				newentry=(i<<8) + (numentries-1);
				entries.Add(newentry);
			}
		}
		// add "other" grids to list
		found=FALSE;
		for(i=0;i<GetSize();i++)
		{
			grid=(CZimsGrid *)GetAt(i);
			if(grid->GetType()==CSD_TYPE_OTHER)
			{
				if(!found)
				{
					found=TRUE;
					pLB->AddString("Other grids of varying type:");
					numentries++;
				}
				s="\t"+grid->GetName();
				pLB->AddString(s);
				numentries++;
				newentry=(i<<8) + (numentries-1);
				entries.Add(newentry);
			}
		}
	}
	return numentries;
}

//***********************************************************************
// Subroutine	:			Can DepthConvertLayer()
// Arguments  :     layer = layernum
// Use        :     can we depth convert the layer?
// Returns    :     0 if yes, errorcode<0 otherwise (expl. see below)
//***********************************************************************
int CZimsCube::CanDepthConvertLayer(int layer)
{
	CZimsGrid *tBot=NULL, *tTop=NULL;
	int retval=0;
	tBot=FindHorizon(layer, CSD_TYPE_TIME, 0);
	if(layer>0)
		tTop=FindHorizon(layer-1, CSD_TYPE_TIME, 0);
	// do we have time data for the layer?
	if(tBot==NULL)
		retval|=1;
	if(layer>0 && tTop==NULL)
		retval|=8;	// no bottom times of above layer = our top times
	// while we're at it, autocorrect the two time layers
	if(tBot && tTop)
		tBot->AutoCorrect(ZIMS_AUTOCORRECT_SMART, tTop);
	// do we have other data for the layer (depends on project scope)
	switch(pDoc->ProjectScope)
	{
	case SCOPE_TIMEWELL:	// need a horizon with a defined model
		// need timegrids of the 2 layers above to obtain z1 of next layer
		if(!(pDoc->ProjState & PRJ_HAVEWELLS))
			retval|=2;
		if(!(((CHorizon *)pDoc->horizonlist.GetAt(layer))->HaveModel))
			retval|=4;
		if(layer>0 && FindHorizon(layer-1, CSD_TYPE_DEPTH, 0)==NULL)
			retval|=16;	// no bottom depths of above layer = our top depths
		break;
	case SCOPE_TIMERMS:	// need a horizonlist (just names) and RMS data
		if(!(pDoc->ProjState & PRJ_HAVEHORIZONS))
			retval|=2;
		if(FindHorizon(layer, CSD_TYPE_INTVEL, 0)==NULL)
			retval|=32;
		break;
	case SCOPE_TIMEWELLRMS:	// need well and RMS data
		if(!(pDoc->ProjState & PRJ_HAVEWELLS))
			retval|=2;
		if(FindHorizon(layer, CSD_TYPE_INTVEL, 0)==NULL)
			retval|=32;
		break;
	}
	return retval;
}

//***********************************************************************
// Subroutine	:			CanDepthConvertSecondaryTimegrid()
// Arguments  :     grid=grid to be converted (we check it's a time grid at all!)
//									numlayers=number of layers in total=number of grids to check
// Use        :     can we depth convert a secondary time grid?
// Returns    :     0 if yes, errorcode<0 otherwise (expl. see below)
//***********************************************************************
int CZimsCube::CanDepthConvertSecondaryTimegrid(CZimsGrid *grid, int numlayers)
{
	CZimsGrid *anyothergrid;
	int retval=CanDepthConvertSecondaryTimeslice(numlayers);
	if(!grid)
		return retval;
	if(grid->GetUnitTimePower()!=1 || grid->GetUnitDistPower()!=0 ||
		grid->GetType()!=CSD_TYPE_TIME)
		retval|=4;		// is not a time grid
	if(grid->GetType()==CSD_TYPE_TIME && grid->GetID()==0)
		retval|=8;		// is a primary time grid (used for depth conversion already!)
	if((anyothergrid=FindHorizon(0, CSD_TYPE_TIME, 0)))
	{
		if ( *grid != *anyothergrid)
			retval|=16;	// grids are not compatible
	}
	return retval;
}

//***********************************************************************
// Subroutine	:			CanDepthConvertSecondaryTimeslice()
// Arguments  :     none
// Use        :     can we depth convert a secondary time slice?
// Returns    :     0 if yes, errorcode<0 otherwise (expl. see below)
//***********************************************************************
int CZimsCube::CanDepthConvertSecondaryTimeslice(int numlayers)
{
	int retval=0;
	for(int i=0;i<numlayers;i++)
	{
		if(!FindHorizon(i, CSD_TYPE_TIME, 0))
			retval|=1;	// time grid missing
		if(!FindHorizon(i, CSD_TYPE_DEPTH, 0))
			retval|=2;	// depth grid missing
	}
	return retval;
}

//***********************************************************************
// Subroutine	:	DepthConvertSecondaryTimegrids()
// Arguments  :     numgrids=number of grids to be converted
//					gridindex=integer array of their indices in zimscube
//					interp_method: 1=cubic spline, 0=linear, 2=with const vAvg
//					..percent: if method=1, this is percent change of last gradient
//					 to yield gradient used for extrapolation
//					rebuild: if TRUE, avgVel grid is rebuilt prior to depth conv.
// Use        :     ONLY use if CanDepthConvertSecondaryTimegrid() said "ok"!
//					splines down primary time/depth grids at each node to get
//					average velocities (linear or cubic spline), then interpolates
//					or extrapolates to depth convert this secondary time grid
//					as well. May rebuild the average vel grid to remove ZNONS
//					There must be a progress monitor dialog open
// Returns    :     TRUE if all ok
//***********************************************************************
BOOL CZimsCube::DepthConvertSecondaryTimegrids(int numgrids, int *gridindex,
											   int interp_method, int extrap_gradient_percent, BOOL rebuild_avgvels, int Layers)
{
	int i, j, layer, numlayers, rows, cols;
	int grid;
	double *t, *d;
	CZimsGrid **velgrid, *outputgrid, **timegrid, **depthgrid;
	CZimsGrid *prototypegrid=(CZimsGrid *)GetAt(gridindex[0]);
	BOOL FoundZnon;
	double ourznon;

	// find out number of horizons
	//numlayers = pDoc->horizonlist.GetSize();
	numlayers = Layers;

	// allocate memory for the lists containing data to be splined
	t=new double[numlayers];
	d=new double[numlayers];
	velgrid=new CZimsGrid *[numgrids];

	// find pointers to all grids involved (so we do not have to search them
	// every time)
	timegrid=new CZimsGrid *[numlayers];
	depthgrid=new CZimsGrid *[numlayers];
	for(layer=0;layer<numlayers;layer++)
	{
		timegrid[layer]=FindHorizon(layer, CSD_TYPE_TIME, 0);
		depthgrid[layer]=FindHorizon(layer, CSD_TYPE_DEPTH, 0);
		timegrid[layer]->Stamp();
		depthgrid[layer]->Stamp();
	}

	ourznon=timegrid[0]->GetZnon();
	// step 1: create a temporary grid with downsplined average velocities
	// this occupies 80% in the progress monitor
	pDoc->m_pProgress->ChangeText("Computing velocity model");
	for(grid=0;grid<numgrids;grid++)
	{
		velgrid[grid] = new CZimsGrid(pDoc);
		*velgrid[grid] << *prototypegrid; // set rows, cols etc.
	}

	rows=prototypegrid->GetRows();
	cols=prototypegrid->GetColumns();

	// Time stamp all the grids used...
	for(grid=0;grid<numgrids;grid++)
	{
		((CZimsGrid *)GetAt(gridindex[grid]))->Stamp();
	}

	// outer loop over all grid nodes
	for(i=0;i<rows;i++)
	{
		if((i%(rows/20))==0)
			pDoc->m_pProgress->ChangePercentage((80*i)/rows);
		for(j=0;j<cols;j++)
		{

			// inner loop over all layers to spline down
			// a) collect all (time, depth) value pairs
			for(layer=0;layer<numlayers;layer++)
			{
				t[layer]=((double **)(*timegrid[layer]))[i][j];
				d[layer]=((double **)(*depthgrid[layer]))[i][j];
			}

			// b) convert them to (time, avgvel) pairs. If we encounter any ZNON value
			//    at all, we just give up altogether
			FoundZnon=FALSE;
			for(layer=0;layer<numlayers;layer++)
			{
				if(timegrid[layer]->GetZnon(t[layer]) ||
					depthgrid[layer]->GetZnon(d[layer]) ||
					t[layer]==0.0)
				{
					FoundZnon=TRUE;
					break;
				}
				else
					d[layer]=d[layer]/t[layer];
			}

			// c) spline/linearly interpolate them to the given time of the secondary grid
			for(grid=0;grid<numgrids;grid++)
			{
				if(FoundZnon)
					((double **)(*velgrid[grid]))[i][j]=ourznon;
				else
				{
					((double **)(*velgrid[grid]))[i][j]=InterpolateExtrapolate(
						((CZimsGrid *)GetAt(gridindex[grid]))->GetGrid()[i][j], t, d, layer,
						interp_method, extrap_gradient_percent, ourznon);
				}
			}
		}
	}

	delete t;
	delete d;
	delete timegrid;
	delete depthgrid;

	// step 2: rebuild the average velocity grids; it is smooth enough so that
	// splining across the ZNONs will not matter that much
	pDoc->m_pProgress->ChangeText("Rebuilding average velocity grid");
	for(grid=0;grid<numgrids;grid++)
	{
		velgrid[grid]->CalcAverage();
		if(rebuild_avgvels)
			velgrid[grid]->IndexGrid();
	}
	pDoc->m_pProgress->ChangePercentage(90);

	// step 3: vAvg*time will yield the depth grid
	pDoc->m_pProgress->ChangeText("Computing depth grid");
	for(grid=0;grid<numgrids;grid++)
	{
		prototypegrid=(CZimsGrid *)GetAt(gridindex[grid]);
		outputgrid=new CZimsGrid(pDoc);
		*outputgrid = *prototypegrid; // assign all grid data and copy time grid
		*outputgrid *= *velgrid[grid];
		delete velgrid[grid];
		outputgrid->SetType(CSD_TYPE_DEPTH);
		outputgrid->SetUnit();
		Add(outputgrid, CString("D_")+prototypegrid->GetName(),-1);
	}

	delete velgrid;

	return TRUE;
}

//***********************************************************************
// Subroutine	:			DepthConvertSecondaryTimeslice()
// Arguments  :     value=value of time slice to be converted
//			    	interp_method: 1=cubic spline, 0=linear, 2=with const vAvg
//					..percent: if method=1, this is percent change of last gradient
//					to yield gradient used for extrapolation
//					rebuild: if TRUE, avgVel grid is rebuilt prior to depth conv.
// Use        :     ONLY use if CanDepthConvertSecondaryTimegrid() said "ok"!
//					splines down primary time/depth grids at each node to get
//					average velocities (linear or cubic spline), then interpolates
//					or extrapolates to depth convert this secondary time grid
//					as well. May rebuild the average vel grid to remove ZNONS
//					There must be a progress monitor dialog open
// Returns    :     TRUE if all ok
//***********************************************************************
BOOL CZimsCube::DepthConvertSecondaryTimeslice(double value,
											   int interp_method, int extrap_gradient_percent, BOOL rebuild_avgvels)
{
	int i, j, layer, numlayers, rows, cols;
	double *t, *d;
	CZimsGrid *velgrid, *outputgrid, **timegrid, **depthgrid;
	CZimsGrid *prototypegrid;
	BOOL FoundZnon;
	double ourznon;
	char buf[256];

	// find out number of horizons
	numlayers=pDoc->horizonlist.GetSize();

	// allocate memory for the lists containing data to be splined
	t=new double[numlayers];
	d=new double[numlayers];
	velgrid=new CZimsGrid(pDoc);

	// find pointers to all grids involved (so we do not have to search them
	// every time)
	timegrid=new CZimsGrid *[numlayers];
	depthgrid=new CZimsGrid *[numlayers];

	for(layer=0;layer<numlayers;layer++)
	{
		timegrid[layer]=FindHorizon(layer, CSD_TYPE_TIME, 0);
		depthgrid[layer]=FindHorizon(layer, CSD_TYPE_DEPTH, 0);
		timegrid[layer]->Stamp();
		depthgrid[layer]->Stamp();
	}

	ourznon=timegrid[0]->GetZnon();
	prototypegrid=timegrid[0];

	// step 1: create a temporary grid with downsplined average velocities
	// this occupies 80% in the progress monitor
	pDoc->m_pProgress->ChangeText("Computing velocity model");
	rows=prototypegrid->GetRows();
	cols=prototypegrid->GetColumns();
	*velgrid << *prototypegrid; // set rows, cols etc.
	velgrid->Stamp();

	// outer loop over all grid nodes
	for(i=0;i<rows;i++)
	{
		if((i%(rows/20))==0)
			pDoc->m_pProgress->ChangePercentage((80*i)/rows);
		for(j=0;j<cols;j++)
		{
			// inner loop over all layers to spline down
			// a) collect all (time, depth) value pairs
			for(layer=0;layer<numlayers;layer++)
			{
				t[layer]=((double **)(*timegrid[layer]))[i][j];
				d[layer]=((double **)(*depthgrid[layer]))[i][j];
			}
			// b) convert them to (time, avgvel) pairs. If we encounter any ZNON value
			// at all, we just give up altogether
			FoundZnon=FALSE;
			for(layer=0;layer<numlayers;layer++)
			{
				if(timegrid[layer]->GetZnon(t[layer]) ||
					depthgrid[layer]->GetZnon(d[layer]) ||
					t[layer]==0.0)
				{
					FoundZnon=TRUE;
					break;
				}
				else
					d[layer]=d[layer]/t[layer];
			}
			// c) spline/linearly interpolate them to the given time of the secondary grid
			if(FoundZnon)
				velgrid->GetGrid()[i][j]=ourznon;
			else
			{
				velgrid->GetGrid()[i][j]=InterpolateExtrapolate(
					value, t, d, layer,
					interp_method, extrap_gradient_percent, ourznon);
			}
		}
	}

	delete t;
	delete d;
	delete timegrid;
	delete depthgrid;

	// step 2: rebuild the average velocity grids; it is smooth enough so that
	// splining across the ZNONs will not matter that much
	pDoc->m_pProgress->ChangeText("Rebuilding average velocity grid");
	velgrid->CalcAverage();
	if(rebuild_avgvels)
		velgrid->IndexGrid();
	pDoc->m_pProgress->ChangePercentage(90);

	// step 3: vAvg*time will yield the depth grid
	pDoc->m_pProgress->ChangeText("Computing depth grid");
	outputgrid=new CZimsGrid(pDoc);
	*outputgrid = *velgrid; // assign all grid data and copy time grid
	*outputgrid *= value;
	outputgrid->SetType(CSD_TYPE_DEPTH);
	outputgrid->SetUnit();
	sprintf(buf, "D_slice_%4.f", value);
	Add(outputgrid, CString(buf),-1);
	delete velgrid;
	return TRUE;
}

//***********************************************************************
// Subroutine	:			VelocityConvertSecondaryTimeslice()
// Arguments  :     value=value of time slice to be converted
//									interp_method: 1=cubic spline, 0=linear, 2=with const vAvg
//									..percent: if method=1, this is percent change of last gradient
//									 to yield gradient used for extrapolation
// Use        :     ONLY use if CanDepthConvertSecondaryTimegrid() said "ok"!
//									splines down primary time/depth grids at each node to get
//									average velocities (linear or cubic spline), then interpolates
//									or extrapolates to velocity convert this secondary time grid,
//									i.e. to obtain an average velocity grid for this time slice.
//									May rebuild the average vel grid to remove ZNONS
//									There must be a progress monitor dialog open
// Returns    :     TRUE if all ok
//***********************************************************************
BOOL CZimsCube::VelocityConvertSecondaryTimeslice(double value,
												  int interp_method, int extrap_gradient_percent)
{
	int i, j, layer, numlayers, rows, cols;
	double *t, *d;
	CZimsGrid *velgrid, **timegrid, **depthgrid;
	CZimsGrid *prototypegrid;
	BOOL FoundZnon;
	double ourznon;
	char buf[256];

	// find out number of horizons
	numlayers=pDoc->horizonlist.GetSize();

	// allocate memory for the lists containing data to be splined
	t=new double[numlayers];
	d=new double[numlayers];
	velgrid=new CZimsGrid(pDoc);

	// find pointers to all grids involved (so we do not have to search them
	// every time)
	timegrid=new CZimsGrid *[numlayers];
	depthgrid=new CZimsGrid *[numlayers];
	for(layer=0;layer<numlayers;layer++)
	{
		timegrid[layer]=FindHorizon(layer, CSD_TYPE_TIME, 0);
		depthgrid[layer]=FindHorizon(layer, CSD_TYPE_DEPTH, 0);
		timegrid[layer]->Stamp();
		depthgrid[layer]->Stamp();
	}

	ourznon=timegrid[0]->GetZnon();
	prototypegrid=timegrid[0];

	// step 1: create a temporary grid with downsplined average velocities
	// this occupies 80% in the progress monitor
	pDoc->m_pProgress->ChangeText("Computing velocity model");
	rows=prototypegrid->GetRows();
	cols=prototypegrid->GetColumns();
	*velgrid << *prototypegrid; // set rows, cols etc...
	velgrid->Stamp();

	// outer loop over all grid nodes
	for(i=0;i<rows;i++)
	{
		if((i%(rows/20))==0)
			pDoc->m_pProgress->ChangePercentage((80*i)/rows);
		for(j=0;j<cols;j++)
		{
			// inner loop over all layers to spline down
			// a) collect all (time, depth) value pairs
			for(layer=0;layer<numlayers;layer++)
			{
				t[layer]=((double **)(*timegrid[layer]))[i][j];
				d[layer]=((double **)(*depthgrid[layer]))[i][j];
			}

			// b) convert them to (time, avgvel) pairs. If we encounter any ZNON value
			//    at all, we just give up altogether
			FoundZnon=FALSE;
			for(layer=0;layer<numlayers;layer++)
			{
				if(timegrid[layer]->GetZnon(t[layer]) ||
					depthgrid[layer]->GetZnon(d[layer]) ||
					t[layer]==0.0 ||
					(layer>0 && t[layer]<t[layer-1]))
				{
					FoundZnon=TRUE;
					break;
				}
				else
					d[layer]=d[layer]/t[layer];
			}

			// c) spline/linearly interpolate them to the given time of the secondary grid
			if(FoundZnon)
				velgrid->GetGrid()[i][j]=ourznon;
			else
			{
				velgrid->GetGrid()[i][j]=InterpolateExtrapolate(
					value, t, d, layer,
					interp_method, extrap_gradient_percent, ourznon);
			}
		}
	}

	delete t;
	delete d;
	delete timegrid;
	delete depthgrid;

	// step 2: Finalise the grid, take it times 2000 to account for two-way
	// times and msec/sec conversion
	pDoc->m_pProgress->ChangeText("Finalising average velocity grid");
	velgrid->CalcAverage();
	(*velgrid)*=2000.0;
	pDoc->m_pProgress->ChangePercentage(90);
	velgrid->SetType(CSD_TYPE_AVGVEL);
	velgrid->SetUnit();
	sprintf(buf, "vAvg_slice_%4.f", value);
	Add(velgrid, CString(buf),-1);
	return TRUE;
}

//***********************************************************************
// Subroutine	:			TimeConvertDepthPoint()
// Arguments  :     depth=value to be time converted
//									x, y = its position
//									interp_method: 1=cubic spline, 0=linear, 2=with const vAvg
//									..percent: if method=1, this is percent change of last gradient
//									 to yield gradient used for extrapolation
// Use        :     ONLY use if CanDepthConvertSecondaryTimegrid() said "ok"!
//									splines down primary time/depth grids at each node to get
//									average velocities (linear or cubic spline), then interpolates
//									or extrapolates to velocity convert this depth value,
//									i.e. to obtain an average velocity for this depth.
// Returns    :     the time value
//***********************************************************************
double CZimsCube::TimeConvertDepthPoint(double x, double y, double depth,
										int interp_method, int extrap_gradient_percent)
{
	int layer, numlayers;
	double *t, *d;
	CZimsGrid **timegrid, **depthgrid;
	BOOL FoundZnon;
	double time, vAvg;

	// find out number of horizons
	numlayers=pDoc->horizonlist.GetSize();

	// allocate memory for the lists containing data to be splined
	t=new double[numlayers];
	d=new double[numlayers];

	// find pointers to all grids involved (so we do not have to search them
	// every time)
	timegrid=new CZimsGrid *[numlayers];
	depthgrid=new CZimsGrid *[numlayers];
	for(layer=0;layer<numlayers;layer++)
	{
		timegrid[layer]=FindHorizon(layer, CSD_TYPE_TIME, 0);
		depthgrid[layer]=FindHorizon(layer, CSD_TYPE_DEPTH, 0);
	}
	// a) collect all times and dephts
	for(layer=0;layer<numlayers;layer++)
	{
		t[layer]=timegrid[layer]->Evaluate(x, y);
		d[layer]=depthgrid[layer]->Evaluate(x, y);
	}
	// b) convert them to (time, avgvel) pairs. If we encounter any ZNON value
	//    at all, we just give up altogether
	// then d contains all depths, and t all average velocities
	FoundZnon=FALSE;
	for(layer=0;layer<numlayers;layer++)
	{
		if(timegrid[layer]->GetZnon(t[layer]) ||
			depthgrid[layer]->GetZnon(d[layer]) ||
			t[layer]==0.0)
		{
			FoundZnon=TRUE;
			break;
		}
		else
			d[layer]=d[layer]/t[layer];
	}
	// c) spline/linearly interpolate them to the given depth of the secondary grid
	if(!FoundZnon)
	{
		vAvg=InterpolateExtrapolateDepthToTime(
			depth, t, d, layer,
			interp_method, extrap_gradient_percent, 1.0E30);
		time=(vAvg==1.0E30 ? 1.0E30 : depth/vAvg);
	}
	else
		time=1.0E30;
	delete t;
	delete d;
	delete timegrid;
	delete depthgrid;
	return time;
}

//***********************************************************************
//  Function:       RayDepth()
//  Purpose         To raytrace through a set of horizons.
//
//  Based on algorithm published in appendix to Larner et al. "Depth
//  migration of imaged time sections", Geophysics, vol 46, 734-750, 1981.
//
//  Notes
//  -----
//  Note that two sets of time grids are passed through. The first is the
//  original timegrid which may or may not have null regions. The second
//  timegrid (re_timegrids) must be the rebuilt grids with the nulls removed.
//  The rebuilt values are used during the calculations below so that a ray
//  can always be computed leaving a horizon. But the real timegrids are used
//  to check whether such a ray hits a null region on the latter horizon. In
//  other words we use the rebuilt arrays for calculations and the original
//  timegrids for null region checks only. Note also that the internal velocity
//  grids passed into this routine must also be rebuilt grids with all nulls
//  removed.
//
//  IMPORTANT!!!!
//  -------------
//  This algorithm expects that units are ALWAYS metres or m/sec. You will
//  need to ensure passed velocity fields are in m/sec regardless of what
//  the user sets. Also this function will convert back to the set units
//  before evaluating position etc.
//
//************************************************************************
BOOL CZimsCube::RayDepth( CZimsGrid** raydepthgrids, CZimsGrid** raypregrids,
						 CZimsGrid** timegrids, CZimsGrid** re_timegrids,
						 CZimsGrid** intvelgrids, CZimsGrid** xoffsetgrids,
						 CZimsGrid** yoffsetgrids, CZimsGrid** anglegrids,
						 double anglelimit, double regridbinsize )
{
	double  r[3], r1[3];                // ray for current and previous horizons
	double  rx[3], rx1[3];              // x-direction offset ray for current and previous horizs.
	double  ry[3], ry1[3];              // y-direction offset ray for current and previous horizs.
	double  dTdx, dTdx1;                // time grid horiz. gradients in x dirn for current and prev. horiz
	double  dTdy, dTdy1;                // time grid horiz. gradients in y dirn for current and prev. horiz
	double  p[3];                       // unit vector in direction of ray for current horiz.
	double  a[3], b[3];                 // vectors for differences r - rx and r - ry.
	double  n[3];                       // orientation of interface vector.
	double  s;                          // slant distance down ray between current and prev. horiz.
	double  sx, sy;                     // slant distances for x & y dirn. offset vectors.
	double  bigA, bigB;                 // coeffs used to orientation of interface.
	double  v, vp1;                     // velocities for int. vel. grid for current and NEXT. horiz.
	double  t, t1, tnew;                // times for current and previous horiz.
	double  **xoffset, **yoffset;       // pointers to x and y offset grids.
	double  **angle;                    // pointer to anglegrid;

	double  ***np;                      // arrays used to hold x/y coordinates of rays for TPS.
	double  **zvals;                    // arrays used to hold ray depth values for TPS.
	double  **cp;                       // holds the coefficients for the radial basis functions.
	Point   *gridpts = NULL;            // holds the node x/y coords at which we want to evaluate depth.
	int     *posn;                      // holds no. of valid points at each horizon

	double  grfrac, z, zv, dx, dy, delx, dely;     // delta x & y used as offsets for extra rays.
	double  t2sec = 0.001;              // constant to convert all times to secs from millisecs
	double  xlims[2], ylims[2], xwidth, ywidth, xp1, yp1, xm1, ym1, tp1, tm1;
	double  epsx, epsy;     // v. small number used below to avoid rounding problems.
	double  margin = 0.001;  // distance in metres. Used to move rays back into region if they go outside by this much.
	int     ir, i, j, k, k1, rows, columns, nhorizons;
	char    buf[256];
	CProgressMonit *pProgress;

	nhorizons = pDoc->horizonlist.GetSize();
	rows      = timegrids[0]->GetRows();
	columns   = timegrids[0]->GetColumns();
	xlims[0]  = timegrids[0]->X(0);     // must do it this way to get correct order...
	xlims[1]  = timegrids[0]->X(columns-1); // don't use ZimsGrid xlims & ylims arrays.
	ylims[0]  = timegrids[0]->Y(0);     // as they give max & min values rather than values at
	ylims[1]  = timegrids[0]->Y(rows-1);    // start and end of axes.

	xwidth    = xlims[1] - xlims[0];
	ywidth    = ylims[1] - ylims[0];

	//  allocate arrays needs for the TPS to regrid rays
	np    = new double** [nhorizons];
	zvals = new double* [nhorizons];
	cp    = new double* [nhorizons];
	posn  = new int [nhorizons];
	gridpts = pvector(1, rows*columns);
	for ( i = 0; i < nhorizons; i++ )
		posn[i] = 0;
	for ( i = 0; i < nhorizons; i++ )
	{
		np[i]    = dmatrix(1, rows*columns, 1, 2);
		zvals[i] = dvector(1, rows*columns);
	}

	//  find the fraction of grid spacing we are going to use to start
	//  off the extra rays AND compute the gradients over. Ideally these
	//  two uses should be separate. Half the grid spacing seems a sensible
	//  choice.
	grfrac = AfxGetApp()->GetProfileInt("RayTracing", "FracGridForGradient", 2 );
	dx = timegrids[0]->Getdx();
	dy = timegrids[0]->Getdy();
	delx = dx / grfrac;    // delx and dely can be set to half the grid spacing
	dely = dy / grfrac;   // used in calc. of the gradients.
	epsx = dx * 1.0e-8;
	epsy = dy * 1.0e-8;

	//  Set up progress monitor
	// See CVelmanDoc:OnProjectDump for example.
	AfxGetApp()->DoWaitCursor(1);                   // turns on 'busy' cursor
	pProgress = new CProgressMonit(pDoc);
	if ( pProgress->GetSafeHwnd() == 0 )
	{
		pProgress->Create();
		pProgress->SetWindowText("Ray tracing...");
		pProgress->ChangeGeneralDescription("Please wait while ray tracing is completed.");
		pProgress->InitializePercentage();
		pProgress->ChangeText("");
		pProgress->ChangeText2("" );
	}

	//  Main loops over points
	//  NOTE!!!  For the i loop, we MUST start at i=1 and not i=0. This is because the raytracing
	//  algorithm requires an extra ray which the equations assume is always further north.
	//  Since i=0 is the northernmost row, we cannot compute this row. Likewise for the j loop,
	//  we cannot compute the last column so we only loop to columns-2.

	for ( i = 1; i < rows; i++ )
		//for ( i = 32; i < 36; i++ ) // testing only!!
	{
		pProgress->ChangePercentage( ((i+1)*100)/rows );
		sprintf( buf, "Raytracing in 3-D row %d of %d..", i+1, rows );
		pProgress->ChangeText( buf );

		for ( j = 0; j < columns-1; j++ )
			//for ( j = 14; j < 17; j++ )
			//j = columns/2; // for testing only !!!
		{
			// .. initialise vector r (starting posn at surface)
			//  It doesn't matter what grids we use for this.
			//  NOTE!! The x axis maps to the columns on the array.
			r1[0] = timegrids[0]->X(j);
			r1[1] = timegrids[0]->Y(i);
			r1[2] = 0.0;        // zero because we're at the surface.

			//  check starting position is not in a null region
			t = timegrids[0]->Evaluate(timegrids[0]->X(j), timegrids[0]->Y(i));
			if ( timegrids[0]->GetZnon(t) )
				continue;

			// .. then initialise vectors rx and ry (bearing in mind the edges)
			//.. and that some grids might have descending X values with ascending array index
			rx1[0] = r1[0] + delx;
			if ( xlims[0] > xlims[1] )
			{
				if ( rx1[0] > xlims[0] )
					rx1[0] = r1[0] - delx;
			}
			else
			{
				if ( rx1[0] > xlims[1] )
					rx1[0] = r1[0] - delx;
			}

			rx1[1] = r1[1];
			rx1[2] = r1[2];
			ry1[0] = r1[0];
			ry1[1] = r1[1] + dely;
			if ( ylims[0] > ylims[1] )
			{
				if ( ry1[1] > ylims[0] )
					ry1[1] = r1[1] - dely;
			}
			else
			{
				if ( ry1[1] > ylims[1] )
					ry1[1] = r1[1] - dely;
			}
			ry1[2] = r1[2];

			// .. initialise unit vector p (in direction of ray). This is straight down at first horiz.
			p[0] = 0.0;
			p[1] = 0.0;
			p[2] = 1.0;

			// .. initialise travel time gradients for layer 0 (since this would be saved in the loop)
			dTdx  = 0.0;
			dTdx1 = 0.0;    // zero because we assume the surface is flat.
			dTdy  = 0.0;
			dTdy1 = 0.0;

			//  Clear raydepth values
			for ( k1 = 0; k1 < nhorizons; k1++ )
			{
				((double **)(*raydepthgrids[k1]))[i][j] = timegrids[0]->GetZnon();
				((double **)(*raypregrids[k1]))[i][j]   = timegrids[0]->GetZnon();
			}

			for ( k = 0; k < nhorizons; k++ )
			{
				xoffset = xoffsetgrids[k]->GetGrid();
				yoffset = yoffsetgrids[k]->GetGrid();
				angle   = anglegrids[k]->GetGrid();

				// for ray r, first compute slant distance s (this must be saved)
				// note that times are assumed to be in milliseconds here (i.e. 0.001*..)

				//  First compute the times and velocities we need for the current layer.
				//  The travel time is the difference between the times at the two horizons.
				//  The time at the next horizon is the actual travel time of the ray which
				//  although located vertically underneath the ray's departure point in reality
				//  may be at a different location. This is the migration we need to compute.
				//  Note that if the time at the next horizon is a null, then the ray has do
				//  stop. We do NOT check for null values where we compute the ray ends because
				//  the data at that location in 'ray-traced' space should be elsewhere and is
				//  not relevant to the ray under consideration.

				v = intvelgrids[k]->Evaluate(r1[0], r1[1]);
				t = timegrids[k]->Evaluate(r1[0], r1[1]);    // don't use rebuilt timegrid here.
				if ( timegrids[k]->GetZnon(t) )             // need to know if ray saw a null region
					break;

				if ( k < nhorizons-1 )
					vp1 = intvelgrids[k+1]->Evaluate(r1[0], r1[1]);
				else
					vp1 = NULL;
				if ( k > 0 )
					t1 = timegrids[k-1]->Evaluate(r1[0], r1[1]);
				else
					t1 = 0.0;
				if ( intvelgrids[k]->GetZnon( v ) )     // check we haven't gone out of region
					break;                              // NOTE! test for vp1 is done below.

				s = t2sec*0.5*v*(t-t1);

				//          .. then compute r(j+1) (eqn B-1).
				for ( ir = 0; ir < 3; ir++ )
					r[ir] = r1[ir] + s*p[ir];

				//TRACE( "r[0] = %f, r[1] = %f, r[2] = %f\n", r[0], r[1], r[2] );
				//TRACE( "r1[0] = %f, r1[1] = %f, r1[2] = %f\n", r1[0], r1[1], r1[2] );
				//TRACE( "p[0] = %f, p[1] = %f, p[2] = %f\n", p[0], p[1], p[2] );
				//TRACE( "s = %f, v = %f, t = %f, t1 = %f at %d %d %d\n", s, v, t, t1, i, j, k );

				//  Check to see if ray has gone *slightly* outside the grid. Have found
				//  problems where very small rounding errors can move the ray out of
				//  the grid by just a single bit which causes QueryLimits() in Evaluate()
				//  to fail. So here we make sure the ray is on the edge if it's a fraction
				//  away from the limit.

				if ( r[0] < xlims[0] && xlims[0]-r[0] < margin )
					r[0] = xlims[0];
				else if ( r[0] > xlims[1] && r[0]-xlims[1] < margin )
					r[0] = xlims[1];
				if ( r[1] > ylims[0] && r[1]-ylims[0] < margin )
					r[1] = ylims[0];
				else if ( r[1] < ylims[1] && ylims[1]-r[1] < margin )
					r[1] = ylims[1];

				//  Check the ray has not gone outside the domain

				if ( ! timegrids[k]->QueryLimits(r[0], r[1]) )
					break;

				//  Check the time at the point where the ray hits the new horizon. Make
				//  sure time is increasing. If not, set the distance to zero (ie make the
				//  horizons coincident). Since we have checked for nulls, can use the proper
				//  timegrids here.

				tnew = re_timegrids[k]->Evaluate(r[0], r[1]);
				if ( tnew < t1 )
				{
					for ( ir = 0; ir < 3; ir++ )
						r[ir] = r1[ir];
				}

				//  Compute the angles with the vertical.

				angle[i][j] = acos(fabs(p[2]))*180.0/3.1415926;

				//  Check the angle to make sure it's not gone beyond the limit that the
				//  user can set.

				if ( fabs(angle[i][j]) > anglelimit )
					break;

				//  Add ray traced points to the arrays ready for the regridding to be done
				//  below.

				posn[k]++;
				np[k][posn[k]][1] = r[0];   // note posn indexing starts at 1.
				np[k][posn[k]][2] = r[1];
				zvals[k][posn[k]] = r[2];

				//  now compute the x and y offset (or displacement) grids
				//  subtract (x, y) coords on first grid from current position
				//  If xoffset is positive, ray has moved further west (positive x).
				//  If yoffset is positive, ray has moved further north (negative y).
				xoffset[i][j] = r[0] - timegrids[0]->X(j);
				yoffset[i][j] = r[1] - timegrids[0]->Y(i);

				//  For testing only, save raytraced fields before regridding
				((double **)(*raypregrids[k]))[i][j] = r[2];

				// for extra rays, rx and ry ...
				// 1. first compute the horizontal gradients in travel time, dT/dx and dT/dy for new layer
				//    (the travel time gradients for previous layers are saved)
				//  We just use a centred difference here. This could be improved if the TPS method
				//  evaluates gradients. Note that velman doesn't seem to store the array in ascending
				//  order. ie. as j increases so the value on the y axis decreases.

				xp1 = r[0]+delx;
				xm1 = r[0]-delx;
				if ( xlims[0] > xlims[1] )  // check x coords are within bounds.
				{                           // takes care of boundaries.
					if ( xp1 > xlims[0] )
						xp1 = xlims[0];
					if ( xm1 < xlims[1] )
						xm1 = xlims[1];
				}
				else
				{
					if ( xp1 > xlims[1] )
						xp1 = xlims[1];
					if ( xm1 < xlims[0] )
						xm1 = xlims[0];
				}
				tp1 = re_timegrids[k]->Evaluate(xp1, r[1]);  // always use rebuilt (no NULLs) grid here.. we want
				tm1 = re_timegrids[k]->Evaluate(xm1, r[1]);  // the ray to stop only if the real ray saw a null
				if ( timegrids[k]->GetZnon( tp1 ) )
					break;
				if ( timegrids[k]->GetZnon( tm1 ) )
					break;
				dTdx = t2sec*( tp1 - tm1 ) / ( xp1 - xm1 );

				yp1 = r[1]+dely;
				ym1 = r[1]-dely;
				if ( ylims[0] > ylims[1] )
				{
					if ( yp1 > ylims[0] )
						yp1 = ylims[0];
					if ( ym1 < ylims[1] )
						ym1 = ylims[1];
				}
				else
				{
					if ( yp1 > ylims[1] )
						yp1 = ylims[1];
					if ( ym1 < ylims[0] )
						ym1 = ylims[0];
				}
				tp1 = re_timegrids[k]->Evaluate(r[0], yp1);
				tm1 = re_timegrids[k]->Evaluate(r[0], ym1);
				if ( timegrids[k]->GetZnon( tp1 ) )
					break;
				if ( timegrids[k]->GetZnon( tm1 ) )
					break;
				dTdy = t2sec*( tp1 - tm1 ) / ( yp1 - ym1 );
				//TRACE( "dTdy is %f, tp1 = %f, tm1 = %f, yp1 = %f, ym1 = %f at point %d %d %d (i, j, k)\n", dTdy, tp1, tm1, yp1, ym1, i, j, k );

				// 2. next compute slant distances for extra rays from eqns B-6.

				if ( k > 0 )
				{
					sx = 0.5*v*( (t2sec*t+dTdx*delx) - (t2sec*t1+dTdx1*delx) );
					sy = 0.5*v*( (t2sec*t+dTdy*dely) - (t2sec*t1+dTdy1*dely) );
				}
				else
				{
					sx = 0.5*v*( (t2sec*t+dTdx*delx) - (dTdx1*delx) );
					sy = 0.5*v*( (t2sec*t+dTdy*dely) - (dTdy1*dely) );
				}

				//  and update the horizontal time gradients

				dTdx1 = dTdx;
				dTdy1 = dTdy;

				// 3. finally update rx and ry using eqn B-5.

				for ( ir = 0; ir < 3; ir++ )
				{
					rx[ir] = rx1[ir] + sx*p[ir];
					ry[ir] = ry1[ir] + sy*p[ir];
				}

				//  Like the main ray, check to see if these extra rays have gone *slightly*
				//  outside the region. If they have we move them back onto the boundary.
				//  (see comments above).

				if ( rx[0] < xlims[0] && xlims[0]-rx[0] < margin )
					rx[0] = xlims[0];
				else if ( rx[0] > xlims[1] && rx[0]-xlims[1] < margin )
					rx[0] = xlims[1];
				if ( ry[0] < xlims[0] && xlims[0]-ry[0] < margin )
					ry[0] = xlims[0];
				else if ( ry[0] > xlims[1] && ry[0]-xlims[1] < margin )
					ry[0] = xlims[1];
				if ( rx[1] > ylims[0] && rx[1]-ylims[0] < margin )
					rx[1] = ylims[0];
				else if ( rx[1] < ylims[1] && ylims[1]-rx[1] < margin )
					rx[1] = ylims[1];
				if ( ry[1] > ylims[0] && ry[1]-ylims[0] < margin )
					ry[1] = ylims[0];
				else if ( ry[1] < ylims[1] && ylims[1]-ry[1] < margin )
					ry[1] = ylims[1];

				//  update old rays ( rj -> rj-1  etc)

				for ( ir = 0; ir < 3; ir++ )
				{
					r1[ir] = r[ir];
					rx1[ir] = rx[ir];
					ry1[ir] = ry[ir];
				}

				//  Compute the orientation of the new interface (n)
				//  1. first compute the vector differences, r-rx and r-ry to give a's and b's.

				for ( ir = 0; ir < 3; ir++ )
				{
					a[ir] = r[ir] - rx[ir];
					b[ir] = r[ir] - ry[ir];
				}
				//TRACE("r[0] = %f, r[1] = %f, r[2] = %f\n", r[0], r[1], r[2] );
				//TRACE("rx[0] = %f, rx[1] = %f, rx[2] = %f\n", rx[0], rx[1], rx[2] );
				//TRACE("ry[0] = %f, ry[1] = %f, ry[2] = %f\n", ry[0], ry[1], ry[2] );
				//TRACE("a[0] = %f, a[1] = %f, a[2] = %f\n", a[0], a[1], a[2] );
				//TRACE("b[0] = %f, b[1] = %f, b[2] = %f\n", b[0], b[1], b[2] );

				//  2. solve for A and B using eqn B-11.

				bigA = (-a[2]*b[1] + a[1]*b[2] ) / ( a[0]*b[1] - a[1]*b[0] );
				bigB = (-a[0]*b[2] + a[2]*b[0] ) / ( a[0]*b[1] - a[1]*b[0] );

				//  3. compute vector n using A & B in eqn B-12.

				z = 1.0/sqrt(bigA*bigA + bigB*bigB + 1.0);
				n[0] = bigA*z;
				n[1] = bigB*z;
				n[2] = z;
				//TRACE("n[1] = %f, bigA = %f, bigB = %f, z = %f\n", n[1], bigA, bigB, z );

				//  Use Snell's law to compute unit vector, p, for the refracted ray.
				//  (nb. we don't need to do this step for the very last horizon)
				//  .. use eqn B-3.
				//  note. small rounding errors may give a -ve value for the sqrt function
				//  in which case set the operand to zero.

				//  first test to make sure vp1 is not a NULL value. If it is, we can't
				//  continue.
				if ( k+1 < nhorizons )
				{
					if ( intvelgrids[k+1]->GetZnon( vp1 ) )
						break;
				}
				if ( k < nhorizons-1 )
				{
					z = p[0]*n[0]+p[1]*n[1]+p[2]*n[2];
					zv = 1.0 - ( v*v ) / ( vp1*vp1 );
					zv = z*z - zv;
					if ( zv < 0.0 )
						zv = 0.0;
					z = z - sqrt(zv);
					for ( ir = 0; ir < 3; ir++ )
					{
						p[ir] = ( vp1/v ) * ( p[ir] - n[ir]*z );
					}
					//TRACE("p[1] = %f, vp1 = %f, v = %f, n[1] = %f, z=%f\n", p[1], vp1, v, n[1], z );
				}
			}
		}
	}

	//  Having got the positions of the rays for all the horizons, we now need to
	//  interpolate the x/y positions of the rays at each horizon back to a regular
	//  grid.

	//  In the method below we simply locate each ray to the cell that it lies in
	//  and then assign the average depth value to that cell, where the average is
	//  the mean of the depths of all the rays that lie in that cell. In most cases
	//  there will be only one ray per cell. This method is not as accurate as using
	//  the TPS method but it is the fastest. Note that all the points on the depth
	//  grids are set to Znon so that only cells with actual rays passing through
	//  them are dealt with.

	//  Reset progress window
	pProgress->ChangePercentage( 0 );
	pProgress->ChangeText("Regridding rays.");
	pProgress->ChangeText2(" ");

	//  Loop over all horizons and all rays on each horizon finding the cell they
	//  belong to.

	int ci, cj, ni, nj, mini, maxi, minj, maxj;

	double rayx, rayy, binsize, minx, maxx, miny, maxy, zx, zy;
	double **count;              // used to count how many rays in a cell

	binsize = regridbinsize / 100.0;
	if ( binsize < 1.0 )
		binsize = 1.0;      // bin size must be at least equal to cell size
	if ( binsize > 5.0 )
		binsize = 5.0;      // bin size of more than 5 x cell size is probably meaningless

	count = dmatrix(0, rows-1, 0, columns-1);

	for ( k = 0; k < nhorizons; k++ )       // loop starts from 1
	{
		sprintf( buf, "Regridding horizon %d of %d..", k+1, nhorizons );
		pProgress->ChangeText( buf );
		for ( j = 0; j < columns; j++ )     // clear the count array
		{
			for ( i = 0; i < rows; i++ )
			{
				count[i][j] = 0.0;
			}
		}

		//  First find which grid cell the ray falls in. Then compute the
		//  loop indicies of the regridding 'bins' by using the binsize percentage
		//  to estimate what overlapping bins the ray may fall into.

		for ( i = 1; i < posn[k]; i++ )
		{
			pProgress->ChangePercentage( (i*100)/posn[k] );
			rayx = np[k][i][1];
			rayy = np[k][i][2];

			//  Test for edges; allow a small amount for round-off errors.
			if ( rayx < xlims[0] && xlims[0]-rayx < margin )
				nj = 0;
			else if ( rayx > xlims[1] && rayx-xlims[1] < margin )
				nj = columns - 1;
			else
			{
				nj = (int)( ( (rayx+dx) - xlims[0] ) / dx );
				zx = timegrids[k]->X(nj-1);
				if ( rayx > zx+(dx/2) )     // have to allow for rounding which can put ray in previous cell
					nj = nj + 1;
				nj = nj - 1;
			}
			if ( rayy > ylims[0] && rayy-ylims[0] < margin ) // assumes ylim[0] > ylim[1]
				ni = 0;
			else if ( rayy < ylims[1] && ylims[1]-rayy < margin )
				ni = rows-1;
			else
			{
				ni = (int)( ( (rayy-dy) - ylims[0] ) / -dy );
				zy = timegrids[k]->Y(ni-1);
				if ( rayy < zy-(dy/2) )
					ni = ni + 1;
				ni = ni - 1;
			}

			//  Note!!  The ylims as used in this function is not the same as defined
			//  by the rest of velman. In this function I have ylims[0] and ylims[1]
			//  the reverse of how it is normally setup. If I change them I will need
			//  to replace ni code by the reverse of what Y() does.
			//nj = abs((int)( ( np[k][i][1]+epsx - xlims[0] ) / dx ) ) - 1;
			//ni = abs((int)( ( np[k][i][2]+epsy - ylims[0] ) / dy ) ) - 1;

			if ( ni < 0 )
				ni = 0;
			if ( ni >= rows )
				ni = rows - 1;
			if ( nj < 0 )
				nj = 0;
			if ( nj >= columns )
				nj = columns;

			// The binsize defines the patch of grid 'bins' (possibly overlapping) which
			//  the ray may fall into. Set the limits for the loops.

			mini = ni - (int)binsize;
			maxi = ni + (int)binsize;
			minj = nj - (int)binsize;
			maxj = nj + (int)binsize;
			if ( mini < 0 )
				mini = 0;
			if ( maxi >= rows )
				maxi = rows - 1;
			if ( minj < 0 )
				minj = 0;
			if ( maxj >= columns )
				maxj = columns - 1;

			//  For each grid cell, we construct the limits that each 'bin' is going to use
			//  then go through all the rays testing to see if it falls within those limits

			for ( cj = minj; cj <= maxj; cj++ )
			{
				minx = timegrids[0]->X(cj) - binsize*(dx/2);
				maxx = timegrids[0]->X(cj) + binsize*(dx/2);
				if ( minx < xlims[0] )
					minx = xlims[0];
				if ( maxx < xlims[0] )
					maxx = xlims[0];
				if ( minx > xlims[1] )
					minx = xlims[1];
				if ( maxx > xlims[1] )
					maxx = xlims[1];

				for ( ci = mini; ci <= maxi; ci++ )
				{
					miny = timegrids[0]->Y(ci) - binsize*(dy/2);
					maxy = timegrids[0]->Y(ci) + binsize*(dy/2);
					if ( miny < ylims[1] )      // NOTE!! Not [0] here because y decreases with increasing ci
						miny = ylims[1];        // since we are in the NH. WOuld need changing for SH oil fields.
					if ( maxy < ylims[1] )
						maxy = ylims[1];
					if ( miny > ylims[0] )
						miny = ylims[0];
					if ( maxy > ylims[0] )
						maxy = ylims[0];

					//  test to see if it falls within bin limits

					if ( rayx >= minx && rayx <= maxx &&
						rayy >= miny && rayy <= maxy )
					{
						//  Found a ray that hits this bin

						if ( count[ci][cj] > 0.99 )
						{
							((double **)(*raydepthgrids[k]))[ci][cj] += zvals[k][i];
						}
						else
						{
							((double **)(*raydepthgrids[k]))[ci][cj] = zvals[k][i];
						}
						count[ci][cj] += 1.0;
					}
				}
			}
		}

		//  Now have all rays binned to their grid cell. Go through all cells
		//  and compute mean in case more than one ray is in each cell.
		for ( i = 0; i < rows; i++ )
		{
			for ( j = 0; j < columns; j++ )
			{
				if ( count[i][j] > 1.99 )    // most cells will only have one ray
				{
					((double **)(*raydepthgrids[k]))[i][j] /= count[i][j];
				}
			}
		}
	}

	free_dmatrix( count, 0, rows-1, 0, columns-1 );

	//  Here we use the thin plate spline method which the rest of
	//  the program uses. Note that we only use the points that were found to be
	//  valid during the ray tracing process. Alot of this code is based around what
	//  ReSample does except for the way in which the arrays are handled.
	/*************
	int fasttps = AfxGetApp()->GetProfileInt("ArithmeticParams", "FastTPS", 0);
	double dist;
	double **fg;

	//  Reset progress window
	pProgress->ChangePercentage( 0 );
	pProgress->ChangeText("Regridding rays.");
	pProgress->ChangeText2("Initialising...");

	//  First normalise coordinates so they lie in a unit square. 'safe' is used to
	//  avoid rounding errors at the 0.0 and 1.0 boundaries.

	dist = min(xwidth, ywidth)*0.01;

	for ( k = 1; k < nhorizons; k++ )
	{
	for ( i = 1; i < posn[k]; i++ )
	{
	np[k][i][1] = (np[k][i][1]-xlims[0]+dist)/(xwidth+2*dist);
	np[k][i][2] = (np[k][i][2]-ylims[0]+dist)/(ywidth+2*dist);
	}
	}
	for ( k = 1; k < nhorizons; k++ )
	{
	pProgress->ChangePercentage( ((k+1)*100)/nhorizons);
	sprintf( buf, "Initialising horizon %d out of %d..", k+1, nhorizons );
	pProgress->ChangeText( buf );
	cp[k] = interpolate_tps( np[k], posn[k], zvals[k], fasttps );
	}

	//  Construct normalised grid coordinates where we want to evaluate the depth values
	//  This is just the grid node x/y values.

	for ( i = 0, k = 1; i < rows; i++ )
	{
	for ( j = 0; j < columns; j++, k++ )
	{
	gridpts[k].x = (dist+j*dx)/(xwidth+2*dist);
	gridpts[k].y = (ywidth+dist-i*dy)/(ywidth+2*dist);
	}
	}

	pProgress->ChangeText2( " ");
	for ( k = 0; k < nhorizons; k++ )
	{
	pProgress->ChangePercentage( ((k+1)*100)/nhorizons );
	sprintf( buf, "Regridding horizon %d out of %d..", k+1, nhorizons );
	pProgress->ChangeText( buf );

	fg = GEvaluateGridTPS(np[k], cp[k], posn[k], gridpts, rows, columns, fasttps);

	for ( i = 0; i < rows; i++ )
	{
	for ( j = 0; j < columns; j++ )
	{
	((double **)(*raydepthgrids[k]))[i][j] = fg[i+1][j+1];
	}
	}
	free_dmatrix( fg, 1, rows, 1, columns); // free matrix allocated from evaluate_tps
	}
	**********/

	//  Tidy up

	//  Free memory
	free_pvector(gridpts, 1, rows*columns);
	for ( k = 0; k < nhorizons; k++ )
	{
		free_dvector(zvals[k], 1, rows*columns);
		free_dmatrix(np[k], 1, rows*columns, 1, 2);
	}
	delete[] np;
	delete[] zvals;
	delete[] cp;
	delete[] posn;

	//   Kill progress monitor
	if ( pProgress )
	{
		pProgress->DestroyWindow();
		delete pProgress;
		pProgress = NULL;
	}
	AfxGetApp()->DoWaitCursor(-1);

	return TRUE;
}