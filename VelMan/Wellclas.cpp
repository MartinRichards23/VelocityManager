#include "VelmanDoc.h"
#include "velman.h" // because next include needs the resource IDs
#include "ProgressDlg.h" // because we use that dialog
#include <ctype.h>

// following vars are needed in optimisation (see FitLayer());

// this list stores the indices of the points to be removed after the round,
// the double list contains their residuals
struct ResidualInfoType
{
	int listindex;
	double residual;
};

int ResidualInfoCompare_Resid( const void *arg1, const void *arg2 );
int ResidualInfoCompare_Index( const void *arg1, const void *arg2 );

//***********************************************************************
// Subroutine : ~CHorizonList()
// Arguments : none
// Use : Return memory allocated by CHorizonList
// Returns : none
//***********************************************************************
CHorizonList::~CHorizonList()
{
	Clear();
	delete inputbuffer;
}

//***********************************************************************
// Subroutine : Init()
// Arguments : document : pointer to current CDocument
// Use : Allocates memory and initializes CHorizonList
// Returns : none
//***********************************************************************
BOOL CHorizonList::Init(CVelmanDoc *document)
{
	pDoc = document;

	if(!inputbuffer)
	{
		if(!(inputbuffer = new char[HORL_MAXLEN]))
		{
			pDoc->Error("Could not allocate inputbuffer", MSG_ERR);
			return FALSE;
		}
	}
	// the class might re-initialised; in that case we have to delete all
	// previous contents
	Clear();
	return TRUE;
}

// Reads well x/y/time/depth data from the given file
BOOL CHorizonList::ReadNew(CString filename, int UsePseudoVelocities)
{
	BOOL DepthsInFeet = FALSE;
	char *p= new char [2000];
	CString temp;
	CString s;
	int NumHorizons = 0;
	CHorizon *horizon, *abovehorizon;
	CMeasurePt *measurept, *abovemeasurept;
	int WellIndex = 0;

	CString MissingMessage = "Well value appears to be missing, don't leave any empty cells, use -9999 for null values. Line:\r\n\r\n";
	CString OriginalLine;

	std::ifstream file;

	//check it is ascii
	if(!CheckASCIIFile(filename))
	{
		pDoc->Error("File was not ascii text: " + filename, MSG_ERR);
		return FALSE;
	}

	file.open((const char *) filename);

	// check file opened ok
	if(file.fail())
	{
		pDoc->Error("Could not open file " + filename, MSG_ERR);
		return FALSE;
	}

	file.seekg(0L, ios::beg);

	// read first nonempty line of file
	if(GetNewLine(file, FALSE)==HORL_EOF)
	{
		pDoc->Error("The file appears to be empty (apart maybe from blank lines or comment lines). Check the file format.", MSG_ERR);
		goto ErrorEnd;
	}

	// check for keyword
	OriginalLine=inputbuffer;
	if(strstr(inputbuffer, "UNIT")==NULL)
	{
		pDoc->Error("Could not find keyword \"UNITS\" on line: " + OriginalLine , MSG_ERR);
		goto ErrorEnd;
	}

	// set units
	if(strstr(inputbuffer, "FEET")!=NULL || strstr(inputbuffer, "FOOT")!=NULL)
	{
		DepthsInFeet = TRUE;

		// we also assume that the RMS velocities will be given in ft/sec, although
		// the user might change that later.
		// all of this only applies to the primary well database
		if(PrimaryDatabase)
			pDoc->Units|=(UNIT_FEET_WELLDEPTHS | UNIT_FEET_RMSVELOCITY);
		pDoc->Log("Depth values in well data file given in feet.", LOG_BRIEF);
	}

	// Read in the horizons
	// the first column should contain "Horizons", then a list of the horizons
	GetNewLine(file, FALSE);
	// make a copy for error message purposes
	OriginalLine=inputbuffer;
	// find first "token", which will be "horizons"
	p = strtok(inputbuffer, ",");

	// check first word is "horizons"
	if(strstr(p, "HORIZON")==NULL)
	{
		pDoc->Error("Could not find keyword \"HORIZONS\" on line: " + OriginalLine , MSG_ERR);
		goto ErrorEnd;
	}

	while(p!=NULL)
	{
		p = strtok(NULL, ",");
		if(p!=NULL)
		{
			// create and initialize a new horizon
			temp = p;
			temp.Trim();			

			if(!(horizon = new CHorizon))
			{
				pDoc->Error("Could not allocate new horizon", MSG_ERR);
				goto ErrorEnd;
			}

			horizon->WriteName(temp);
			Add(horizon);
			pDoc->Log("Found horizon " + temp, LOG_STD);
		}
	}

	// make sure we have enough horizons. Only applies to primary database
	NumHorizons=GetSize();

	//changed from <2 to <1 because we can use 1 horizon now
	if(PrimaryDatabase && NumHorizons<1)
	{
		s="Not enough horizons defined in time/depth file. "
			"\n\nAfter parsing it, the line containing the horizon names reads\n\n" +
			OriginalLine +
			"\n\nMake sure that you specified the right file, and that the file format "
			"adheres to the description (see example file and the manual).";

		pDoc->Error(s, MSG_ERR);
		goto ErrorEnd;
	}

	// now iterate through the wells, one line at a time
	while(GetNewLine(file, FALSE)!=HORL_EOF)
	{
		// remember the line we are reading
		OriginalLine=inputbuffer;

		CString name;
		double x = MPT_INVALID;
		double y = MPT_INVALID;
		double prevX = MPT_INVALID;
		double prevY = MPT_INVALID;
		double time = MPT_INVALID;
		double depth = MPT_INVALID;

		// first token is the well name
		p = strtok(inputbuffer, ",");

		// check it wasn't null
		if(p == NULL)
		{
			pDoc->Error(MissingMessage + OriginalLine, MSG_ERR);
			goto ErrorEnd;
		}

		name = p;
		name.Trim();

		// a * can be put on the end of a name to stop excel screwing it up, remove it here		
		name.TrimRight("*");

		// then we have x, y, time, depth for each horizon
		for(int i=0; i < NumHorizons; i++)
		{
			//reset values
			x = MPT_INVALID;
			y = MPT_INVALID;
			time = MPT_INVALID;
			depth = MPT_INVALID;

			UBYTE status = 0;
						
			horizon=(CHorizon *)GetAt(i);

			// read coord values
			p = strtok(NULL, ",");
			if(p == NULL)
			{
				pDoc->Error(MissingMessage + OriginalLine, MSG_ERR);
				goto ErrorEnd;
			}
			else if(!strcmp(p, "#"))
				x = prevX;
			else
				x = atof(p);

			p = strtok(NULL, ",");
			if(p == NULL)
			{
				pDoc->Error(MissingMessage + OriginalLine, MSG_ERR);
				goto ErrorEnd;
			}
			else if(!strcmp(p, "#"))
				y = prevY;
			else
				y = atof(p);

			// check coords are valid
			if(x == MPT_INVALID || y == MPT_INVALID)
			{
				pDoc->Error("Coordinates are not valid on line:\r\n\r\n" + OriginalLine, MSG_ERR);
				goto ErrorEnd;
			}

			//remember x/y values for next column
			prevX = x;
			prevY = y;

			// read time/depth values
			p = strtok(NULL, ",");
			if(p == NULL)
			{
				pDoc->Error(MissingMessage + OriginalLine, MSG_ERR);
				goto ErrorEnd;
			}	
			else if(!strcmp(p, "#"))
			{
				// this point is marked as apseudo
				status |= MPT_STATUS_CALC_TIME;
			}	
			else
			{
				if(UsePseudoVelocities == Pseudo_All)
				{
					//user has selected pseudo vels on all wells, recalc ALL times
					status |= MPT_STATUS_CALC_TIME;
				}

				time = atof(p);
				time = CheckTime(time); // convert to msecs if need to
			}

			p = strtok(NULL, ",");
			if(p == NULL)
			{
				pDoc->Error(MissingMessage + OriginalLine, MSG_ERR);
				goto ErrorEnd;
			}
			else if(!strcmp(p, "#"))
			{
				status |= MPT_STATUS_CALC_DEPTH;
			}
			else
			{
				depth = atof(p);
			}

			// if it is the first horizon set name etc.
			if(i == 0)
			{
				wellnames.Add(name);
				wellXsurf.Add(x);
				wellYsurf.Add(y);
				wellstatus.Add((DWORD)WELL_STATUS_DEVIATED); // set as deviated for now, will check further down
			}

			// change units if necessary
			if(depth!=MPT_INVALID && depth!=MPT_MUSTNOTREPAIR_ITEM)
			{
				if(pDoc->Units & UNIT_OUTPUTINFEET)
				{
					if(!DepthsInFeet)
						depth/=METERS_PER_FOOT;
				}
				else
				{
					if(DepthsInFeet)
						depth*=METERS_PER_FOOT;
				}
			}

			measurept = new CMeasurePt();

			measurept->wellindex = WellIndex;

			horizon->Add(measurept);
			// Write survey data
			if(!(measurept->WriteSurveyData(x, y, status)))
			{
				pDoc->Error("Invalid (x, y) coordinates in well '" +name+
					"' and horizon '" + horizon->name + "'.\n\n"
					"Please check to make sure the file is in the proper format. Error line:\r\n\r\n" + OriginalLine,
					MSG_WARN);
				goto ErrorEnd;
			}
			//write time/depth data
			if(!measurept->WriteWellData(time, depth))
			{
				pDoc->Error("Invalid (time/depth) data in well '" +name+
					"' and horizon '" + horizon->name + "'.\n\n"
					"Please check to make sure the file is in the proper format. Error line:\r\n\r\n" + OriginalLine,
					MSG_WARN);
				goto ErrorEnd;
			}
		}

		WellIndex++;
	}

	file.close();
	file.clear();

	// deviation allowed in wells to class a well as 'vertical'. Anything above
	// this and the well is considered deviated. Note this is absolute measurement.
	// would be better to have a percentage error and make it relative to the depth
	// of the layer.
	double MaxDeviationAllowed = AfxGetApp()->GetProfileInt("Well Import", "MaxDeviationAllowed", 150); // this was 5

	if(pDoc->VerticaliseWells)
		MaxDeviationAllowed = 750;

	// now go through all wells. So far, they are all marked "deviated", but they
	// might actually be vertical. Check all coordinates, and if they do not stray far,
	// assume that the well is vertical.
	for(int i=0;i<wellnames.GetSize();i++)
	{
		BOOL IsActuallyVertical=TRUE;
		for(int j=0;j<GetSize();j++)
		{
			horizon=(CHorizon *)GetAt(j);
			measurept=(CMeasurePt *)horizon->GetAt(i);
			if (!measurept)
			{
				pDoc->Error("Unset coordinates found at horizon: " + horizon->name +
					" well name: " + wellnames[i] +
					".\nThis error means velman can no longer process the well file.\n "
					"Please make sure you have not made a mistake in the format of the well file.", MSG_ERR );
				goto ErrorEnd;
			}

			if(j==0)
			{
				if(fabs(wellXsurf.GetAt(i)-measurept->ReadX())>MaxDeviationAllowed ||
					fabs(wellYsurf.GetAt(i)-measurept->ReadY())>MaxDeviationAllowed)
				{
					IsActuallyVertical=FALSE;
					break;
				}
			}
			else
			{
				abovehorizon=(CHorizon *)GetAt(j-1);
				abovemeasurept=(CMeasurePt *)abovehorizon->GetAt(i);
				if(fabs(abovemeasurept->ReadX()-measurept->ReadX())>MaxDeviationAllowed ||
					fabs(abovemeasurept->ReadY()-measurept->ReadY())>MaxDeviationAllowed)
				{
					IsActuallyVertical=FALSE;
					break;
				}
			}
		}

		if(IsActuallyVertical)
		{
			// its vertical
			wellstatus.SetAt(i, wellstatus[i] & ~WELL_STATUS_DEVIATED);
		}
		else
		{
			// its deviated

			// if we want to recalc only deviated then set that now
			if(UsePseudoVelocities == Pseudo_Deviated)
			{
				for(int j=0;j<GetSize();j++)
				{
					horizon=(CHorizon *)GetAt(j);
					measurept=(CMeasurePt *)horizon->GetAt(i);

					UBYTE status = measurept->ReadStatus();

					status |= MPT_STATUS_CALC_TIME;

					measurept->WriteStatus(status);
				}
			}

		}
	}

	// primary well data is now being processed, but secondary well data is only
	// checked for consistency
	if(CheckData())
	{
		if(PrimaryDatabase)
		{
			for(int i=0;i<GetSize();i++)
				ProcessHorizon(i);
		}
		pDoc->Log("Successfully imported well data", LOG_STD);
		char msg_buf[512];
		int j=0; // number of vertical wells
		int k=0; // number of deviated wells
		for(int i=0;i<wellstatus.GetSize();i++)
		{
			if( wellstatus[i] & WELL_STATUS_DEVIATED )
				k++;
			else
				j++;
		}

		CString units = "meters";
		if(DepthsInFeet)
			units = "feet";

		sprintf(msg_buf, "Successfully imported %d wells (%d deviated, %d vertical) in %d horizons.\n"
			"Units of imported data: %s\n"
			"(Refer to logfile for details.)", wellnames.GetSize(), k, j, GetSize(), units);
		AfxMessageBox(msg_buf, MB_ICONINFORMATION);
		return TRUE;
	}
	else
	{
		pDoc->Log("Successfully imported well data, but could not process it.", LOG_STD);
		return FALSE;
	}

ErrorEnd:
	//delete startsurveyfilecontent;
	file.close();
	file.clear();
	Clear();
	pDoc->Log("Aborted the import of well data", LOG_STD);
	return FALSE;
}

//***********************************************************************
// Arguments : hor = number of horizon to be processed
// bool DEPTH = FALSE if depth is to be rebuilt
// bool TIME = FALSE if time is to be rebuilt
// Use : rebuilds the horizon depth and time if necessary
// Returns : FALSE or FALSE depending if successful
//***********************************************************************
BOOL CHorizonList::RebuildTimeDepth(int hor, BOOL DoDepth, BOOL DoTime)
	// do time and depth in single function to keep it efficient.
{
	CHorizon *horizon = (CHorizon *) GetAt(hor);
	CZimsGrid *depthgrid = pDoc->zimscube.FindHorizon(hor, CSD_TYPE_DEPTH, 0);
	CZimsGrid *timegrid = pDoc->zimscube.FindHorizon(hor, CSD_TYPE_TIME, 0);
	int wellnumber = wellnames.GetSize();
	CMeasurePt *measurept = NULL;

	// Check if horizon and depthgrid actually exist
	if( !( depthgrid || timegrid ) || !horizon)
		return(FALSE);

	if ( !DoDepth && !DoTime )
		return(FALSE);

	// go through all measurement points for layer
	for(int i=0; i<wellnumber; i++)
	{
		// get measurement point and the one from the layer above (if exists)
		measurept = (CMeasurePt *) horizon->GetAt(i);

		// see if it exists and then change time/depth if necessary
		if(!measurept)
			continue;

		if ( DoTime )
		{
			double time = measurept->ReadTime();

			if( (time == MPT_ILLEGAL || (measurept->ReadStatus() & MPT_STATUS_CALC_TIME)) 
				&& !(measurept->ReadStatus() & MPT_STATUS_MUSTNOTREPAIR_TIME))
			{
				if (timegrid)
				{
					double gridVal = timegrid->Evaluate(measurept->ReadX(), measurept->ReadY());

					//only set time to new value if new value is not null (we may overwrite a good value with a null otherwise)
					if (!(timegrid->GetZnon(gridVal)))
						time = gridVal;

					if (!(timegrid->GetZnon(time)))
					{
						measurept->WriteTime(time);
						measurept->WriteStatus(measurept->ReadStatus()|MPT_STATUS_CALC_TIME);
					}
					else
					{
						measurept->WriteTime(MPT_ILLEGAL);
						measurept->WriteStatus(measurept->ReadStatus()&~MPT_STATUS_CALC_TIME);
					}
				}
			}
		}

		if ( DoDepth )
		{
			double depth = measurept->ReadDepth();

			if ( (depth == MPT_ILLEGAL || (measurept->ReadStatus() & MPT_STATUS_CALC_DEPTH))
				&& !(measurept->ReadStatus() & MPT_STATUS_MUSTNOTREPAIR_DEPTH) )
			{
				// evaluate the depth from the depthgrid
				if ( depthgrid )
				{
					depth = depthgrid->Evaluate(measurept->ReadX(), measurept->ReadY());

					// Check if it is legal, then write
					if(!(depthgrid->GetZnon(depth)))
					{
						measurept->WriteDepth(depth);
						measurept->WriteStatus(measurept->ReadStatus()|MPT_STATUS_CALC_DEPTH);
					}
					else
					{
						measurept->WriteDepth(MPT_ILLEGAL);
						measurept->WriteStatus(measurept->ReadStatus()&~MPT_STATUS_CALC_DEPTH);
					}
				}
			}
		}		
	}

	return(TRUE);
}

//***********************************************************************
// Subroutine : ProcessHorizon()
// Arguments : hor = number of horizon to be processed
// Use : Computes some values from time and depth, see below
// Returns : none
//***********************************************************************
void CHorizonList::ProcessHorizon(int hor)
{
	CHorizon *horizon = (CHorizon *) GetAt(hor);
	CHorizon *abovehorizon = (hor == 0 ? NULL : (CHorizon *)GetAt(hor-1));
	CZimsGrid *aboveZgrid, *aboveTgrid;
	CZimsGrid *thisZgrid, *thisTgrid;

	int j, wellnumber = wellnames.GetSize();
	CMeasurePt *measurept, *abovemeasurept;

	double depth, time, vInt;
	double abovedepth, abovetime;
	BOOL timesOk, depthsOk;

	static BOOL HaveReadThresholds=FALSE;
	static BOOL WarnRebuiltT=FALSE;
	static BOOL WarnRebuiltD=FALSE;
	static double tIntMin, tIntMax, vIntMin, vIntMax, dIntMin, dIntMax;
	char buf[512];

	// interval time, depth, velocity must not exceed certain thresholds. Read
	// these from the INI file, but only once to save time
	if(!HaveReadThresholds)
	{
		tIntMin=(double)(AfxGetApp()->GetProfileInt("Well Import", "MinIsochronochore", 10));
		tIntMax=(double)(AfxGetApp()->GetProfileInt("Well Import", "MaxIsochronochore", 35000));
		dIntMin=(double)(AfxGetApp()->GetProfileInt("Well Import", "MinIsochore", 10));
		dIntMax=(double)(AfxGetApp()->GetProfileInt("Well Import", "MaxIsochore", 35000));
		vIntMin=(double)(AfxGetApp()->GetProfileInt("Well Import", "MinIntvel", 10));
		vIntMax=(double)(AfxGetApp()->GetProfileInt("Well Import", "MaxIntvel", 35000));
		HaveReadThresholds=TRUE;
	}

	thisZgrid=pDoc->zimscube.FindHorizon(hor, CSD_TYPE_DEPTH, 0);
	thisTgrid=pDoc->zimscube.FindHorizon(hor, CSD_TYPE_TIME, 0);
	aboveZgrid=pDoc->zimscube.FindHorizon(hor-1, CSD_TYPE_DEPTH, 0);
	aboveTgrid=pDoc->zimscube.FindHorizon(hor-1, CSD_TYPE_TIME, 0);

	// Rebuild base time values
	// note that we must not rebuild base depths otherwise the user cannot
	// use a different model and get repeatable results.

	RebuildTimeDepth( hor, FALSE, TRUE );

	horizon->NumPts=0;
	for(j=0;j<wellnumber;j++)
	{
		// get measurement point and the one from the layer above (if exists)
		measurept = (CMeasurePt *) horizon->GetAt(j);

		// see if it exists and then perform computations
		if(measurept)
		{
			// if the position of the well is not properly defined, nothing of the
			// stuff below will work.
			if(measurept->ReadX()==MPT_INVALID || measurept->ReadY()==MPT_INVALID)
				continue;

			time = measurept->ReadTime();
			depth = measurept->ReadDepth();

			// we can tentatively clear the "invalid" status flag
			measurept->WriteStatus(measurept->ReadStatus() & ~MPT_STATUS_INVALID);

			horizon->NumPts++;

			if(hor>0)
			{
				// time and depth on layer above are always evaluated, if grids are there
				// time comes first:

				abovemeasurept = (CMeasurePt *) abovehorizon->GetAt(j);
				if(aboveTgrid)
				{
					if ( thisTgrid->QueryLimits(measurept->ReadX(), measurept->ReadY()) )// inside grid
					{
						abovetime=aboveTgrid->Evaluate(measurept->ReadX(), measurept->ReadY());

						// if we get a NULL value and it's a deviated well, we can do no more
						// however, if it's a vertical well we can simply copy the well data.
						// In principle we could test for vertical/deviated wells and only bother to do
						// Evaluate() if it's deviated but we prefer to always evaluate.
						if( aboveTgrid->GetZnon(abovetime))
							if ( (wellstatus[j] & WELL_STATUS_DEVIATED) )
								abovetime=MPT_ILLEGAL;
							else
								if ( abovemeasurept )
									abovetime = abovemeasurept->ReadTime(); // use the well data instead
					}
					else// outside grid; vertical wells considered only
					{
						if ( !(wellstatus[j] & WELL_STATUS_DEVIATED) && abovemeasurept )
							abovetime = abovemeasurept->ReadTime();
						else
							abovetime=MPT_ILLEGAL;
					}
				}
				else
					abovetime=MPT_ILLEGAL;

				// Problem arises when the input data is so bad that
				// the surface fitted across this data results in bad values for points
				// evaluated above. In an actual case, the evaluated abovetime was greater
				// than the base time. We test and correct it here. A similar test is
				// done in CheckData. Note this only applies to deviated wells, since
				// for vertical wells we use the points from above.

				if ( abovetime!=MPT_INVALID && time!=MPT_INVALID && abovetime > time )
				{
					abovetime = MPT_ILLEGAL;
					pDoc->Log("In Horizon "+ horizon->GetName()+ " and well " +
						wellnames[j] + ", the evaluation of time tTop is not viable "
						"and has been skipped.", LOG_DETAIL );
				}

				// now the depth:
				if(aboveZgrid)
				{
					if ( aboveZgrid->QueryLimits(measurept->ReadX(), measurept->ReadY()) )
					{
						abovedepth=aboveZgrid->Evaluate(measurept->ReadX(), measurept->ReadY());

						if(aboveZgrid->GetZnon(abovedepth))
							if ( (wellstatus[j] & WELL_STATUS_DEVIATED) )
								abovedepth=MPT_ILLEGAL;
							else
								if ( abovemeasurept )
									abovedepth = abovemeasurept->ReadDepth();
					}
					else// outside grid; vertical wells only
					{
						if ( !(wellstatus[j] & WELL_STATUS_DEVIATED) && abovemeasurept )
							abovedepth = abovemeasurept->ReadDepth();
						else
							abovedepth=MPT_ILLEGAL;
					}
				}
				else
					abovedepth=MPT_ILLEGAL;

				// Check depth is doing something sensible.
				if ( abovedepth!=MPT_INVALID && depth!=MPT_INVALID && abovedepth > depth )
				{
					if ( !WarnRebuiltD )
					{
						WarnRebuiltD = TRUE;
						pDoc->Error("One or more rebuilt depth values are not viable."
							"\nSee log file for more details. Further errors of this type"
							"\nwill be logged but not displayed.", MSG_WARN );
					}
					abovedepth = MPT_ILLEGAL;
					pDoc->Log("In Horizon "+ horizon->GetName()+ " and well " +
						wellnames[j] + ", the evaluation of depth dTop is not viable and "
						"has been skipped.", LOG_DETAIL );
				}
			}
			else// horz = 0
			{
				// the layer above the first one is the surface with standard values
				abovedepth=0.0;
				abovetime=0.0;
			}

			// compute interval and mid point time
			// changed this code so that if any of
			// time, depth of interval velocity fell outside the limits set above, then
			// ALL of the values should be set null.
			BOOL badinterval = FALSE;
			if(time!=MPT_ILLEGAL && abovetime!=MPT_ILLEGAL)
			{
				timesOk=TRUE;
				measurept->Writemptime((abovetime+time)/2.0);
				if(time-abovetime>=tIntMin && time-abovetime<=tIntMax)
					measurept->WritetInt((time-abovetime));
				else
				{
					sprintf(buf,
						"Well %s in horizon %s: Isochronochore=%7.2f msec, value discarded",
						(const char *)wellnames.GetAt(j), (const char *)horizon->GetName(),
						time-abovetime);
					pDoc->Log(buf, LOG_DETAIL);
					measurept->WritetInt(MPT_ILLEGAL);
					badinterval = TRUE; // make sure all 3 of time, depth and int. vel are NULL
				}
			}
			else
			{
				timesOk=FALSE;
				measurept->Writemptime(MPT_ILLEGAL);
			}

			// compute interval and mid point depth
			if(depth!=MPT_ILLEGAL && abovedepth!=MPT_ILLEGAL)
			{
				depthsOk=TRUE;
				measurept->Writempdepth((abovedepth+depth)/2.0);
				if(depth-abovedepth>=dIntMin && depth-abovedepth<=dIntMax && !badinterval )
					measurept->WritedInt((depth-abovedepth));
				else
				{
					sprintf(buf,
						"Well %s in horizon %s: Isochore=%7.2f, value discarded",
						(const char *)wellnames.GetAt(j), (const char *)horizon->GetName(),
						depth-abovedepth);
					pDoc->Log(buf, LOG_DETAIL);
					measurept->WritedInt(MPT_ILLEGAL);
					badinterval = TRUE;
				}
			}
			else
			{
				depthsOk=FALSE;
				measurept->Writempdepth(MPT_ILLEGAL);
			}

			// Top time and depth are just taken from above layer
			measurept->WritetTop(abovetime);
			measurept->WritedTop(abovedepth);

			// Compute interval velocity = velocity from the layer above to this one
			// for i-th layer: vint = 2000 * (depth(i)-depth(i-1))/(time(i)-time(i-1))
			if(!timesOk || !depthsOk || time==abovetime)
				measurept->WritevInt(MPT_ILLEGAL);
			else
			{
				vInt=2000.0*(abovedepth-depth)/(abovetime-time);
				if ( vInt>=vIntMin && vInt<=vIntMax && !badinterval )
					measurept->WritevInt(vInt);
				else
				{
					sprintf(buf,
						"Well %s in horizon %s: Interval velocity=%7.2f, value discarded",
						(const char *)wellnames.GetAt(j), (const char *)horizon->GetName(),
						vInt);
					pDoc->Log(buf, LOG_DETAIL);
					measurept->WritevInt(MPT_ILLEGAL);
				}
			}
		}
	}
}

//***********************************************************************
// Subroutine : FitLayer()
// Arguments : layer = number of layer to be fitted
// pDoc = pointer to document (so that we can access the function library)
// Use : Fits horizon, using knowledge about included/excluded points
// Returns : nothing, but sets flags in CHorizon class (FitA, FitB, HaveFit)
// Note: Slightly GUI-dependent as it turns hourglass mouse cursor on and off
//***********************************************************************
void CHorizonList::FitLayer(int layer, CVelmanDoc *pDoc)
{
	// Performs fitting exactly, taking excluded points into account as well
	// we need a pointer to the document to access the list of model functions

	// there is one "goto"-Label in this function, because returning from the
	// function also requires freeing memory. The label is "DeleteReturn", and is
	// situated directly before the end of the function.

	AfxGetApp()->DoWaitCursor(1);
	CHorizon *horizon=(CHorizon *)GetAt(layer);

	if(horizon->UseCustomFit)
	{
		horizon->FitA=horizon->CustomFitA;
		horizon->FitB=horizon->CustomFitB;
		horizon->HaveFit=TRUE;
		return;
	}

	int wellnum=wellnames.GetSize();
	int j, status;

	// create temporary copy of the list of points to be correlated
	double *x, *y, *z1, *w;
	double thisres;
	CMeasurePt **datapoint;
	int i, l, numpts;
	double a, b, chisq;
	BOOL result;
	CWellModelFunction *modfunc;
	int maxresi_ind;
	double dummyx, dummyy;
	// NumberToExclude=the actual number of points to exclude during the next round. Might be less
	// than NumToExclude, if it is the last round!
	int iround, NumberOfRounds, NumPointsPerRound, NumberToExclude;
	BOOL LessInLastRound;
	struct ResidualInfoType *residinfo;

	x=new double[wellnum];
	y=new double[wellnum];
	z1=new double[wellnum];
	w=new double[wellnum];
	datapoint=new CMeasurePt *[wellnum];

	l=0;
	int numexcl=0; // count number of wells excluded so far (ie manually)
	int moretoexclude; // will contain number of wells additionally to be excluded

	for(i=0;i<wellnum;i++)
	{
		// get measurement point
		CMeasurePt *measurept = (CMeasurePt *) horizon->GetAt(i);
		if(measurept && !( (measurept->ReadStatus()) & MPT_STATUS_INVALID) )
		{
			measurept->ReadValuePair(horizon->ModelNumber, &dummyx, &dummyy);
			if(dummyx!=MPT_ILLEGAL && dummyy!=MPT_ILLEGAL && measurept->ReadZ1()!=MPT_ILLEGAL)
			{
				// always reset auto-exclude bit (might be a leftover from last calculation)
				// do this for every point, even the ones that might not make it into the
				// local lists we create here
				status=measurept->ReadStatus() & (~MPT_STATUS_SCRL_EXCLUDE);
				measurept->WriteStatus(status);

				if(!(status & MPT_STATUS_USER_EXCLUDE))
				{
					x[l]=dummyx;
					y[l]=dummyy;
					w[l]=measurept->ReadWeight();
					z1[l]=measurept->ReadZ1();

					datapoint[l]=measurept;
					l++;
				}
				else
					numexcl++;
			}
		}
	}
	numpts=l; // now we know the number of interesting points

	// reset flag
	horizon->HaveFit=FALSE;

	// find the function (that's what we need the document for!)
	modfunc=pDoc->well_model_lib[horizon->ModelNumber];

	if(!(modfunc->fit)) // cannot do much without a fitting function
		goto DeleteReturn;
	if(numpts<2)
		goto DeleteReturn;// cannot do much without enough points

	// we already excluded <numexcl> wells through the wells that were manually
	// deselected. Now find out how many more to exclude by iteratively fitting,
	// finding the furtherst point, excluding it, and repeating the process

	if(horizon->ExcludePercent==0)
		moretoexclude=0;
	else
		// uncomment following line to count excluded points differently: If numexcl is subtracted,
		// then if x% of the points have to be excluded, the manually excluded points already
		// count towards this value.
		// moretoexclude=(numpts*(horizon->ExcludePercent))/100-numexcl;
		moretoexclude=((numpts+numexcl)*(horizon->ExcludePercent))/100;

	// we exclude the points in chunks, always xx% of all points at once (the xx%
	// worst points), where "all points" really means "all pts in the dataset", not
	// just the ones currently used. xx is read from the INI-File. If it's small,
	// then small datasets will have their points excluded one by one.

	NumPointsPerRound=AfxGetApp()->GetProfileInt("Well models", "PercentExcludePerChunk", 6);

	// make a real number from that percentage value
	NumPointsPerRound=max(1, (numpts*NumPointsPerRound)/100);

	// how many rounds do we need? we have to round upwards
	LessInLastRound=(moretoexclude % NumPointsPerRound == 0 ? 0 : 1);
	NumberOfRounds=moretoexclude/NumPointsPerRound+LessInLastRound;

	residinfo=new struct ResidualInfoType[numpts];

	for(iround=0;iround<NumberOfRounds;iround++)
	{
		if(LessInLastRound && iround==NumberOfRounds-1)
			NumberToExclude=moretoexclude % NumPointsPerRound;
		else
			NumberToExclude=NumPointsPerRound;

		// make one fit. We'll find the <NumberToExclude> worst residuals of this one
		result=modfunc->fit(horizon->DistWeight, numpts, z1, x, y, w, &a, &b, &chisq);
		if(!result)
			goto DeleteReturn; // give up if something goes wrong

		for(i=0;i<numpts;i++)
		{
			// compute residual of point
			if(horizon->ModelNumber==0)
				thisres=exp(x[i]/a)-( (b*y[i]+1.0)/(b*z1[i]+1.0) );
			else
				thisres=y[i]-modfunc->func(x[i], a, b);

			// stow it away
			residinfo[i].listindex=i;
			residinfo[i].residual=fabs(thisres);
		}

		// now sort the residual list by decreasing residuals
		qsort(residinfo, numpts, sizeof(struct ResidualInfoType),
			ResidualInfoCompare_Resid);

		// now we want to delete the first <NumbertoExclude> entries. As well shift
		// memory to and fro, we want to start with the LAST point in the last, hence
		// sort the first few entries again, this time with decreasing indices!
		qsort(residinfo, NumberToExclude, sizeof(struct ResidualInfoType),
			ResidualInfoCompare_Index);

		// the first NumberToExclude points now must go away!
		for(j=0;j<NumberToExclude;j++)
		{
			maxresi_ind=residinfo[j].listindex;
			status=datapoint[maxresi_ind]->ReadStatus();
			datapoint[maxresi_ind]->WriteStatus(status | MPT_STATUS_SCRL_EXCLUDE);
			memmove(&x[maxresi_ind], &x[maxresi_ind+1], sizeof(double)*(numpts-maxresi_ind-1));
			memmove(&y[maxresi_ind], &y[maxresi_ind+1], sizeof(double)*(numpts-maxresi_ind-1));
			memmove(&w[maxresi_ind], &w[maxresi_ind+1], sizeof(double)*(numpts-maxresi_ind-1));
			memmove(&z1[maxresi_ind], &z1[maxresi_ind+1], sizeof(double)*(numpts-maxresi_ind-1));
			memmove(&datapoint[maxresi_ind], &datapoint[maxresi_ind+1], sizeof(CMeasurePt *)*(numpts-maxresi_ind-1));
			numpts--;
		}
	}
	delete residinfo;

	/*
	for(i=0;i<moretoexclude;i++)
	{
	result=modfunc->fit(horizon->DistWeight, numpts, z1, x, y, w, &a, &b, &chisq);
	// find out point with largest residual
	if(!result)
	goto DeleteReturn;
	maxresi=-1.0;
	maxresi_ind=0; // will ALWAYS be changed, because maxresi will rise to 0.0 at least
	for(j=0;j<numpts;j++)
	{
	if(horizon->ModelNumber==0)
	thisres=exp(x[j]/a)-log( (b*y[j]+1.0)/(b*z1[j]+1.0) );
	else
	thisres=y[j]-modfunc->func(x[j], a, b);
	if(fabs(thisres)>maxresi)
	{
	maxresi_ind=j;
	maxresi=fabs(thisres);
	}
	}
	// exclude that point from list by moving the rest of the list downwards and
	// adjusting the list size numpts. Before that, set the "SCRL_EXCLUDE" bit
	status=datapoint[maxresi_ind]->ReadStatus();
	datapoint[maxresi_ind]->WriteStatus(status | MPT_STATUS_SCRL_EXCLUDE);
	memmove(&x[maxresi_ind], &x[maxresi_ind+1], sizeof(double)*(numpts-maxresi_ind-1));
	memmove(&y[maxresi_ind], &y[maxresi_ind+1], sizeof(double)*(numpts-maxresi_ind-1));
	memmove(&w[maxresi_ind], &w[maxresi_ind+1], sizeof(double)*(numpts-maxresi_ind-1));
	memmove(&z1[maxresi_ind], &z1[maxresi_ind+1], sizeof(double)*(numpts-maxresi_ind-1));
	memmove(&datapoint[maxresi_ind], &datapoint[maxresi_ind+1], sizeof(CMeasurePt *)*(numpts-maxresi_ind-1));
	numpts--;
	}

	*/

	// now compute and store the latest model
	horizon->HaveFit=modfunc->fit(horizon->DistWeight, numpts, z1, x, y, w, &a, &b, &chisq);
	horizon->FitA=a;
	horizon->FitB=b;
	horizon->FitGoodness=chisq;

DeleteReturn:
	delete x;
	delete y;
	delete z1;
	delete w;
	delete datapoint;
	AfxGetApp()->DoWaitCursor(-1);
	return;
}

//***********************************************************************
// Subroutine : Write()
// Arguments : none
// Use : Write raw well data file into internal data base
// Returns : none
//***********************************************************************
BOOL CHorizonList::Write(void)
{
	CString filename;
	CHorizon *horizon;
	CMeasurePt *measurept;
	double tempdouble;
	UBYTE tempbyte;
	BOOL tempbool;
	int tempint;
	DWORD tempdword;

	if(pDoc == NULL)
		AfxMessageBox("hh");

	// the file name depends on the database to be stored
	if(PrimaryDatabase)
		filename = pDoc->TargetDir+pDoc->BaseName+".wll";
	else
		filename = pDoc->TargetDir+pDoc->BaseName+".wl2";

#ifdef VELMAN_UNIX
	ofstream outfile((const char *) filename);
#else
	ofstream outfile((const char *) filename, ios::binary);
#endif

	int hornumber = GetSize();
	int wellnumber = wellnames.GetSize();
	int length, status, j, i;
	const char* outputbuffer;

	// Could not create file
	if(outfile.fail())
	{
		pDoc->Error("Could not open file well data file " +
			filename, MSG_ERR);
		return FALSE;
	}

	// write number of horizons and wells
	outfile.write((char *) &hornumber, sizeof(int));
	outfile.write((char *) &wellnumber, sizeof(int));

	// write well name lengths and names
	for(j=0;j<wellnumber;j++)
	{
		outputbuffer = (const char *) wellnames[j];
		length = strlen(outputbuffer)+1;
		outfile.write((char *) &length, sizeof(int));
		outfile.write(outputbuffer, strlen(outputbuffer)+1);
		tempdouble=wellXsurf[j];
		outfile.write((char *)&tempdouble, sizeof(double));
		tempdouble=wellYsurf[j];
		outfile.write((char *)&tempdouble, sizeof(double));
		tempdword=wellstatus[j];
		outfile.write((char *)&tempdword, sizeof(DWORD));
	}

	// step through all horizons
	for(i=0;i<hornumber;i++)
	{
		if(pDoc->m_pProgress)
			pDoc->m_pProgress->ChangePercentage(80+(i*20)/hornumber);
		// write horizon name length and name
		horizon = (CHorizon *) GetAt(i);
		outputbuffer = (const char *) horizon->name;
		length = strlen(outputbuffer)+1;
		outfile.write((char *) &length, sizeof(int));
		outfile.write(outputbuffer, length);
		// write information on model
		tempbool=horizon->HaveModel;
		outfile.write((char *)&tempbool, sizeof(BOOL));
		tempint=horizon->ModelNumber;
		outfile.write((char *)&tempint, sizeof(int));
		tempint=horizon->DistWeight;
		outfile.write((char *)&tempint, sizeof(int));
		tempint=horizon->ExcludePercent;
		outfile.write((char *)&tempint, sizeof(int));
		tempint=horizon->NumPts;
		outfile.write((char *)&tempint, sizeof(int));
		tempbool=horizon->HaveFit;
		outfile.write((char *)&tempbool, sizeof(BOOL));
		tempdouble=horizon->FitA;
		outfile.write((char *) &tempdouble, sizeof(double));
		tempdouble=horizon->FitB;
		outfile.write((char *) &tempdouble, sizeof(double));
		tempdouble=horizon->FitGoodness;
		outfile.write((char *) &tempdouble, sizeof(double));
		// write information on weighting options chosen
		tempdouble=horizon->WeightCenterX;
		outfile.write((char *) &tempdouble, sizeof(double));
		tempdouble=horizon->WeightCenterY;
		outfile.write((char *) &tempdouble, sizeof(double));
		tempdouble=horizon->WeightCenterRad;
		outfile.write((char *) &tempdouble, sizeof(double));
		tempint=horizon->WeightDecreaseFunc;
		outfile.write((char *)&tempint, sizeof(int));
		// gradient normalisation
		tempbool=horizon->NormalizeGradients;
		outfile.write((char *)&tempbool, sizeof(BOOL));
		tempdouble=horizon->NormGradFactor;
		outfile.write((char *) &tempdouble, sizeof(double));
		tempbool=horizon->HowToHandleWellResiduals;
		outfile.write((char *)&tempbool, sizeof(BOOL));
		// custom fits
		tempbool=horizon->UseCustomFit;
		outfile.write((char *)&tempbool, sizeof(BOOL));
		tempdouble=horizon->CustomFitA;
		outfile.write((char *) &tempdouble, sizeof(double));
		tempdouble=horizon->CustomFitB;
		outfile.write((char *) &tempdouble, sizeof(double));

		// step through all wells
		for(j=0;j<wellnumber;j++)
		{
			// get measurement point
			measurept = (CMeasurePt *) horizon->GetAt(j);
			// see if it exists and then write status byte and measurement point
			if(measurept)
			{
				status = 1;

				outfile.write((char *) &status, sizeof(int));
				tempdouble = measurept->ReadX();
				outfile.write((char *) &tempdouble, sizeof(double));
				tempdouble = measurept->ReadY();
				outfile.write((char *) &tempdouble, sizeof(double));
				tempdouble = measurept->ReadDepth();
				outfile.write((char *) &tempdouble, sizeof(double));
				tempdouble = measurept->ReadTime();
				outfile.write((char *) &tempdouble, sizeof(double));
				tempdouble = measurept->ReadvInt();
				outfile.write((char *) &tempdouble, sizeof(double));
				tempdouble = measurept->Readmptime();
				outfile.write((char *) &tempdouble, sizeof(double));
				tempdouble = measurept->Readmpdepth();
				outfile.write((char *) &tempdouble, sizeof(double));
				tempdouble = measurept->ReadtTop();
				outfile.write((char *) &tempdouble, sizeof(double));
				tempdouble = measurept->ReaddInt();
				outfile.write((char *) &tempdouble, sizeof(double));
				tempdouble = measurept->ReadtInt();
				outfile.write((char *) &tempdouble, sizeof(double));
				tempdouble = measurept->ReadWeight();
				outfile.write((char *) &tempdouble, sizeof(double));
				tempdouble = measurept->ReadZ1();
				outfile.write((char *) &tempdouble, sizeof(double));
				tempdouble = measurept->ReadTieTdiff();
				outfile.write((char *) &tempdouble, sizeof(double));
				tempdouble = measurept->ReadTieVdiff();
				outfile.write((char *) &tempdouble, sizeof(double));
				tempdouble = measurept->ReadTieDdiff();
				outfile.write((char *) &tempdouble, sizeof(double));
				tempbyte = measurept->ReadStatus();
				outfile.write((char *) &tempbyte, sizeof(UBYTE));
				tempbyte = measurept->ReadRegion();
				outfile.write((char *) &tempbyte, sizeof(UBYTE));
			}
			else
			{
				status = 0;
				outfile.write((char *) &status, sizeof(int));
			}
		}
	}
	return(TRUE);
}

//***********************************************************************
// Subroutine : WriteASCII()
// Arguments : Folder to put file in
// Use : Write data file in ASCII format (for user readable output)
// Returns : Bool; FALSE for failure, FALSE for success.
//***********************************************************************
BOOL CHorizonList::WriteASCII(CString FolderPath)
{
	CString filename;
	CHorizon *horizon;
	CMeasurePt *measurept;
	double value;

	// the file name depends on the database to be exported
	if(PrimaryDatabase)
		filename = FolderPath + pDoc->BaseName + "_well_data.txt";
	else
		filename = FolderPath + pDoc->BaseName + "_well_data.tx2";

	ofstream outfile((const char *) filename);
	int hornumber = GetSize();
	int wellnumber = wellnames.GetSize();
	int length, status, j, i, oldp, newp;
	const char* outputbuffer;

	// Could not create file
	if(outfile.fail())
	{
		pDoc->Error("Could not open file " + filename, MSG_ERR);
		return FALSE;
	}

	// write number of horizons and wells
	outfile << "Number of horizons:\t" << hornumber << endl;
	outfile << "Number of wells:\t" << wellnumber << endl;

	// write well name lengths and names
	for(j=0;j<wellnumber;j++)
	{
		outputbuffer = (const char *) wellnames[j];
		length = strlen(outputbuffer)+1;
		outfile << "Well #" << j<< " : '"
			<< outputbuffer << "' at (" << wellXsurf.GetAt(j) << ", " << wellYsurf.GetAt(j) << ")"
			<< (wellstatus[j] & WELL_STATUS_DEVIATED ? "\tdeviated" : "\tvertical") << endl;
	}

	outfile << endl;

	// step through all horizons
	for(i=0;i<hornumber;i++)
	{
		// write horizon name length and name
		horizon = (CHorizon *) GetAt(i);
		outputbuffer = (const char *) horizon->name;
		length = strlen(outputbuffer)+1;
		outfile << "Horizon #" << i << " : '"
			<< outputbuffer << "'\n===============================\n";
		if(horizon->HaveModel)
		{
			outfile << "Have model for this layer, function #" << horizon->ModelNumber <<
				" distance weighting:" << (horizon->DistWeight == 0 ? "linear" : "quadratic")
				<< "\n";
		}

		// Changed default precision for parameters to 14 dec. places (double)
		newp = 14;
		oldp = (long)outfile.precision( newp );
		if(horizon->HaveFit)
		{
			outfile << "Have fit for this layer, parameters =("<< horizon->FitA << ", " <<
				horizon->FitB << ")\n";
		}
		outfile.precision( oldp );

		outfile << "\nGradient normalisation: " << (horizon->NormalizeGradients ? "on" : "off");
		if(horizon->NormalizeGradients)
			outfile << ", factor " << horizon->NormGradFactor << "%.";
		outfile << endl;

		// step through all wells
		for(j=0;j<wellnumber;j++)
		{
			// get measurement point
			// Rewrote this to impose more control over the precision to which
			// float values were written out. The code is complicated because we want to
			// be precise over the no. of decimal places used. However, some values may be
			// 1e30 so we need to test for this for each value.
			// Any value < 1e10 (arbitrary limit) will use fixed point format. Others use the
			// default.
			double flimit = 1.0e10;
			int decpl = 2; // how many decimal places for fixed pt output of t, d & v's.
			measurept = (CMeasurePt *) horizon->GetAt(j);
			// see if it exists and then write status byte and measurement point
			if(measurept)
			{
				status = 1;
				outfile << "MPoint #" << j << ", " << i << " valid:" <<
					((measurept->ReadStatus() & MPT_STATUS_INVALID) ? "no" : "yes") << endl;

				if ( (value = measurept->ReadX()) < flimit )
				{
					outfile.setf( ios::fixed, ios::floatfield );
					outfile << setprecision(0) << "\t X: " << value; // X & Y should have 0 dec.pls.
					outfile.setf( 0, ios::floatfield);
				}
				else
				{
					outfile << "\t X: " << value;
				}

				if ( (value = measurept->ReadY()) < flimit )
				{
					outfile.setf( ios::fixed, ios::floatfield );
					outfile << setprecision(0) << ", Y: " << value << endl;
					outfile.setf( 0, ios::floatfield );
				}
				else
				{
					outfile << ", Y: " << value << endl;
				}

				if ( (value = measurept->ReadTime()) < flimit )
				{
					outfile.setf( ios::fixed, ios::floatfield );
					outfile << setprecision(decpl) << "tBase: " << value;
					outfile.setf( 0, ios::floatfield );
				}
				else
				{
					outfile << "tBase: " << value;
				}

				if ( (value = measurept->ReadDepth()) < flimit )
				{
					outfile.setf( ios::fixed, ios::floatfield );
					outfile << setprecision(decpl) << ", dBase: " << value << endl;
					outfile.setf( 0, ios::floatfield );
				}
				else
				{
					outfile << ", dBase: " << value << endl;
				}

				if ( (value = measurept->ReadtTop()) < flimit )
				{
					outfile.setf( ios::fixed, ios::floatfield );
					outfile << setprecision(decpl) << "tTop: " << value;
					outfile.setf( 0, ios::floatfield );
				}
				else
				{
					outfile << "tTop: " << value;
				}

				if ( (value = measurept->ReadZ1()) < flimit )
				{
					outfile.setf( ios::fixed, ios::floatfield );
					outfile << setprecision(decpl) << ", dTop: " << value << endl;
					outfile.setf( 0, ios::floatfield );
				}
				else
				{
					outfile << ", dTop: " << value << endl;
				}

				if ( (value = measurept->ReadvInt()) < flimit )
				{
					outfile.setf( ios::fixed, ios::floatfield );
					outfile << setprecision(decpl) << "vInt: " << value << endl;
					outfile.setf( 0, ios::floatfield );
				}
				else
				{
					outfile << "vInt: " << value << endl;
				}

				if ( (value = measurept->ReadtInt()) < flimit )
				{
					outfile.setf( ios::fixed, ios::floatfield );
					outfile << setprecision(decpl) << "tInt: " << value;
					outfile.setf( 0, ios::floatfield );
				}
				else
				{
					outfile << "tInt: " << value;
				}

				if ( (value = measurept->ReaddInt()) < flimit )
				{
					outfile.setf( ios::fixed, ios::floatfield );
					outfile << setprecision(decpl) << " \tdInt: " << value << endl;
					outfile.setf( 0, ios::floatfield );
				}
				else
				{
					outfile << " \tdInt: " << value << endl;
				}

				if ( (value = measurept->Readmptime()) < flimit )
				{
					outfile.setf( ios::fixed, ios::floatfield );
					outfile << setprecision(decpl) << "mptime: " << value;
					outfile.setf( 0, ios::floatfield );
				}
				else
				{
					outfile << "mptime: " << value;
				}

				if ( (value = measurept->Readmpdepth()) < flimit )
				{
					outfile.setf(ios::fixed, ios::floatfield );
					outfile << setprecision(decpl) << " \tmpdepth: " << value << endl;
					outfile.setf( 0, ios::floatfield );
				}
				else
				{
					outfile << " \tmpdepth: " << value << endl;
				}

				outfile.precision( oldp ); // reset default precision.

				outfile << "Weight:" << measurept->ReadWeight() << " \t S: ";
				outfile.setf(ios::hex, ios::basefield);
				outfile << (unsigned int)(measurept->ReadStatus());
				outfile.setf(ios::dec, ios::basefield);
				outfile << "\n\n";
			}
			else
			{
				status = 0;
				outfile << "MPoint #" << j << ", " << i << " : exist=NO\n\n";
			}
		}
	}

	// explain whereabouts of files
	AfxMessageBox("Created the following well data files in the project sub directory:\n\n" +
		filename,
		MB_ICONINFORMATION);

	return(TRUE);
}

//***********************************************************************
// Subroutine : WriteASCII2()
// Arguments : Folder to put file in
// Use : Write time/depth data file in ASCII format, can be read back in by velman
// Returns : Bool; FALSE for failure, FALSE for success.
//***********************************************************************
BOOL CHorizonList::WriteASCII2(CString FolderPath)
{
	int i, j;
	double t, d, x, y;
	CString ts, ds, xs, ys;
	int layernum = GetSize();
	int wellnum = wellnames.GetSize();
	CMeasurePt *measurept;
	CHorizon *horizon;
	CStdioFile spread;
	CString file;

	file = pDoc->GetExportDir() + pDoc->BaseName + "_well_td.csv";

	if(!spread.Open( (LPCTSTR)file, CFile::modeCreate | CFile::modeReadWrite | CFile::typeText ))
	{
		AfxMessageBox("Could not open spreadsheet file.\n\nProject creation aborted.",
			MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}

	// write units
	if(pDoc->Units & UNIT_OUTPUTINFEET)
		spread.WriteString("Units:, feet\n");
	else
		spread.WriteString("Units:, metres\n");

	//write horizons
	spread.WriteString("Horizons, ");
	for(i=0;i<layernum;i++)
	{
		horizon=(CHorizon *)pDoc->horizonlist.GetAt(i);

		spread.WriteString(horizon->name + ",,,, ");
	}
	spread.WriteString("\n");

	// write comment labelling time depth etc.
	spread.WriteString(";");
	for(i=0;i<layernum;i++)
	{
		spread.WriteString(", x, y, time, depth");
	}
	spread.WriteString("\n");

	// all wells one by one
	for(i=0;i<wellnum;i++)
	{
		CString a = pDoc->horizonlist.wellnames[i];

		spread.WriteString(a + ", ");

		for(j=0;j<layernum;j++)
		{
			horizon=(CHorizon *)pDoc->horizonlist.GetAt(j);
			measurept = (CMeasurePt *) horizon->GetAt(i);

			x = MPT_INVALID;
			y = MPT_INVALID;
			t = MPT_INVALID;
			d = MPT_INVALID;

			xs = "#";
			ys = "#";
			ts = "-9999";
			ds = "-9999";

			if(measurept!=NULL && !(measurept->ReadStatus() & MPT_STATUS_INVALID))
			{
				x=measurept->ReadX();
				y=measurept->ReadY();

				t=measurept->ReadTime();
				d=measurept->ReadDepth();

				xs.Format("%6.0f", x);
				ys.Format("%6.0f", y);

				if(t==MPT_INVALID)
					t = -9999;

				if(d==MPT_INVALID)
					d = -9999;

				ts.Format("%6.0f", t);
				ds.Format("%6.0f", d);
			}

			spread.WriteString(xs + ", ");
			spread.WriteString(ys + ", ");
			spread.WriteString(ts + ", ");
			spread.WriteString(ds + ", ");
		}

		spread.WriteString("\n");
	}

	spread.Close();
	AfxMessageBox("Well time/depth file has been written to project export sub directory.\n\nThis file can be read back into VelocityManager if necessary.\n\n" +
		file, MB_OK );

	return TRUE;
}

//***********************************************************************
// Subroutine : Read()
// Arguments : none
// Use : Read raw data file into internal data base
// Returns : none
//***********************************************************************
BOOL CHorizonList::Read(void)
{
	CString filename;

	// the file name depends on the database to be read
	if(PrimaryDatabase)
		filename = pDoc->TargetDir+pDoc->BaseName+".wll";
	else
		filename = pDoc->TargetDir+pDoc->BaseName+".wl2";

#ifdef VELMAN_UNIX
	ifstream infile((const char *) filename);
#else
	std::ifstream infile((const char *) filename, ios::binary);
#endif
	CHorizon *horizon;
	CMeasurePt *measurept;

	int hornumber, wellnumber, status, length, i, j;
	char readbuffer[HORL_MAXCHUNK];
	double tempdouble;
	UBYTE tempbyte;
	BOOL tempbool;
	int tempint;
	DWORD tempdword;

	// see if file exists
	if(infile.fail())
	{
		pDoc->Error("Could not open file " + filename, MSG_ERR);
		return FALSE;
	}

	// read number of horizons and wells
	infile.read((char *) &hornumber, sizeof(int));
	infile.read((char *) &wellnumber, sizeof(int));

	// read wellnames, well positions and status flags
	for(j=0;j<wellnumber;j++)
	{
		infile.read((char *) &length, sizeof(int));
		infile.read(readbuffer, length);
		wellnames.Add(CString(readbuffer));
		infile.read((char *) &tempdouble, sizeof(double));
		wellXsurf.Add(tempdouble);
		infile.read((char *) &tempdouble, sizeof(double));
		wellYsurf.Add(tempdouble);
		infile.read((char *) &tempdword, sizeof(DWORD));
		wellstatus.Add(tempdword);
	}

	// read

	// step through all horizons
	for(i=0;i<hornumber;i++)
	{
		if(pDoc->m_pProgress)
			pDoc->m_pProgress->ChangePercentage(80+(i*20)/hornumber);
		// read horizon name
		infile.read((char *) &length, sizeof(int));
		infile.read(readbuffer, length);
		horizon = (CHorizon *) GetAt(Add(new CHorizon(CString(readbuffer))));
		// read "have model" flag and model description
		infile.read((char *) &tempbool, sizeof(BOOL));
		horizon->HaveModel=tempbool;
		infile.read((char *) &tempint, sizeof(int));
		horizon->ModelNumber=tempint;
		infile.read((char *) &tempint, sizeof(int));
		horizon->DistWeight=tempint;
		infile.read((char *) &tempint, sizeof(int));
		horizon->ExcludePercent=tempint;
		infile.read((char *) &tempint, sizeof(int));
		horizon->NumPts=tempint;
		infile.read((char *) &tempbool, sizeof(BOOL));
		horizon->HaveFit=tempbool;
		infile.read((char *) &tempdouble, sizeof(double));
		horizon->FitA=tempdouble;
		infile.read((char *) &tempdouble, sizeof(double));
		horizon->FitB=tempdouble;
		infile.read((char *) &tempdouble, sizeof(double));
		horizon->FitGoodness=tempdouble;
		// read information on well weighting
		infile.read((char *) &tempdouble, sizeof(double));
		horizon->WeightCenterX=tempdouble;
		infile.read((char *) &tempdouble, sizeof(double));
		horizon->WeightCenterY=tempdouble;
		infile.read((char *) &tempdouble, sizeof(double));
		horizon->WeightCenterRad=tempdouble;
		infile.read((char *) &tempint, sizeof(int));
		horizon->WeightDecreaseFunc=tempint;
		// gradient normalisation
		infile.read((char *) &tempbool, sizeof(BOOL));
		horizon->NormalizeGradients=tempbool;
		infile.read((char *) &tempdouble, sizeof(double));
		horizon->NormGradFactor=tempdouble;
		infile.read((char *) &tempbool, sizeof(BOOL));
		horizon->HowToHandleWellResiduals=tempbool;
		// custom fits
		infile.read((char *) &tempbool, sizeof(BOOL));
		horizon->UseCustomFit=tempbool;
		infile.read((char *) &tempdouble, sizeof(double));
		horizon->CustomFitA=tempdouble;
		infile.read((char *) &tempdouble, sizeof(double));
		horizon->CustomFitB=tempdouble;

		// step through all wells
		for(j=0;j<wellnumber;j++)
		{
			// read measurement point if necessary
			infile.read((char *) &status, sizeof(int));
			if(status)
			{
				// create new measurement point as status == 1
				measurept = new CMeasurePt;
				measurept->wellindex=j;
				infile.read((char *) &tempdouble, sizeof(double));
				measurept->WriteX(tempdouble);
				infile.read((char *) &tempdouble, sizeof(double));
				measurept->WriteY(tempdouble);
				infile.read((char *) &tempdouble, sizeof(double));
				measurept->WriteDepth(tempdouble);
				infile.read((char *) &tempdouble, sizeof(double));
				measurept->WriteTime(tempdouble);
				infile.read((char *) &tempdouble, sizeof(double));
				measurept->WritevInt(tempdouble);
				infile.read((char *) &tempdouble, sizeof(double));
				measurept->Writemptime(tempdouble);
				infile.read((char *) &tempdouble, sizeof(double));
				measurept->Writempdepth(tempdouble);
				infile.read((char *) &tempdouble, sizeof(double));
				measurept->WritetTop(tempdouble);
				infile.read((char *) &tempdouble, sizeof(double));
				measurept->WritedInt(tempdouble);
				infile.read((char *) &tempdouble, sizeof(double));
				measurept->WritetInt(tempdouble);
				infile.read((char *) &tempdouble, sizeof(double));
				measurept->WriteWeight(tempdouble);
				infile.read((char *) &tempdouble, sizeof(double));
				measurept->WritedTop(tempdouble);
				infile.read((char *) &tempdouble, sizeof(double));
				measurept->WriteTieTdiff(tempdouble);
				infile.read((char *) &tempdouble, sizeof(double));
				measurept->WriteTieVdiff(tempdouble);
				infile.read((char *) &tempdouble, sizeof(double));
				measurept->WriteTieDdiff(tempdouble);
				infile.read((char *) &tempbyte, sizeof(UBYTE));
				measurept->WriteStatus(tempbyte);
				infile.read((char *) &tempbyte, sizeof(UBYTE));
				measurept->WriteRegion(tempbyte);

				horizon->InsertAt(j, measurept);
			}
		}
	}
	return(TRUE);
}

//***********************************************************************
// Subroutine : GetNewLine()
// Arguments : file : ifstream reference to file
// Use : Reads line from file and puts it into inputbuffer
// Returns : UBYTE containing the status byte of the line
// (see wellclas.h for a list of status byte bitflags)
// The line is modified during reading:
// - everything is converted into uppercase
// - if the line starts with a double quote, also its second character
// is checked to establish whether or not it is a comment line
// - double quotes around the line are removed
// - extra whitespace within the line is compressed into single TAB characters
// - whitespace at the beginning and end of the line is removed
// - if it is a comment line, the next line is read until end of file or
// a non-comment line has been found. The same is FALSE for empty lines.
//***********************************************************************
#ifdef VELMAN_UNIX
UBYTE CHorizonList::GetNewLine(ifstream &file, BOOL compressSpaces)
#else
UBYTE CHorizonList::GetNewLine(std::ifstream &file, BOOL compressSpaces)
#endif
{
	int len;
	CString line, newline;
	BOOL InWhitespace, IsComment;

	while(1)// this loop quits if return() statements are encountered
	{
		// end of file?
		if(file.eof())
			return(HORL_EOF);

		file.getline(inputbuffer, HORL_MAXLEN-1);

		// convert to uppercase
		line = inputbuffer;
		line.MakeUpper();

		// remove trailing and initial whitespace
		line.TrimLeft();
		line.TrimRight();
		// remove trailing commas
		line.TrimRight(", ");

		// remove quotes and either end
		len=line.GetLength();
		if(len>2 && line[0]=='"' && line.Right(1)=="\"")
			line=line.Mid(1, len-2);

		// remove trailing and initial whitespace
		line.TrimLeft();
		line.TrimRight();

		// compress all internal whitespace, make one TAB out of each group of
		// whitespace characters
		newline="";

		if(compressSpaces)
		{
			len=line.GetLength();
			InWhitespace=FALSE;
			for(int i=0;i<len;i++)
			{
				if(isspace(line[i]))
				{
					if(!InWhitespace)
					{
						newline+='\t'; // add a TAB for every block of whitespace characters
						InWhitespace=TRUE;
					}
				}
				else
				{
					newline+=line[i];
					InWhitespace=FALSE;
				}
			}
		}
		else
		{
			newline = line;
		}

		// newline now contains a "compressed" version of the line just read
		if(newline.GetLength()==0)
			continue; // next line, this one is empty

		// check for comment lines
		IsComment=FALSE;
		for(int i=0;i<HORL_EXCLUDE;i++)
		{
			if(newline.Find(excludestring[i]) == 0)
				IsComment=TRUE;
		}

		if(IsComment)
			continue; // next line, this one is a comment line

		// otherwise this is a nice, correctly formatted line!
		strcpy(inputbuffer, (const char *)newline);
		return(HORL_VALIDLINE);
	}
}

UBYTE CHorizonList::GetMemNewLine(char **memfile)
{
	int len, i;
	CString line, newline;
	char *p;
	char *src, *dest;
	BOOL InWhitespace, IsComment;

	while(1)
	{
		p=*memfile;
		src=p;
		dest=inputbuffer;

		if(*p==-1)
			return(HORL_EOF);

		i=0;
		// 7/1/00. Test for -1. memfile terminates with -1. If there is
		// no terminating \n, -1 will signify end of the file & line.
		while( *src != '\n' && *src != -1 &&
			i<HORL_MAXLEN && numberOfBytesRead<SurveyFileLength)
		{
			*dest++=*src++;
			i++;
			numberOfBytesRead++;
		}
		*dest=0;
		*memfile=src+1;

		// convert to uppercase
		line = inputbuffer;
		line.MakeUpper();

		// remove quotes and either end
		len=line.GetLength();
		if(len>2 && line[0]=='"' && line.Right(1)=="\"")
			line=line.Mid(1, len-2);

		// remove trailing and initial whitespace
		line.TrimLeft();
		line.TrimRight();

		// compress all internal whitespace, make one TAB out of each group of
		// whitespace characters
		newline="";
		len=line.GetLength();
		InWhitespace=FALSE;
		for(i=0;i<len;i++)
		{
			if(isspace(line[i]))
			{
				if(!InWhitespace)
				{
					newline+='\t'; // add a TAB for every block of whitespace characters
					InWhitespace=TRUE;
				}
			}
			else
			{
				newline+=line[i];
				InWhitespace=FALSE;
			}
		}

		// newline now contains a "compressed" version of the line just read
		if(newline.GetLength()==0)
			continue; // next line, this one is empty

		// check for comment lines
		IsComment=FALSE;
		for(i=0;i<HORL_EXCLUDE;i++)
			if(newline.Find(excludestring[i]) == 0)
				IsComment=TRUE;
		if(IsComment)
			continue; // next line, this one is a comment line

		// otherwise this is a nice, correctly formatted line!
		strcpy(inputbuffer, (const char *)newline);
		return(HORL_VALIDLINE);
	}
}

//***********************************************************************
// Subroutine : CheckData()
// Arguments : none
// Use : Checks input data for plausibility
// Returns : none
//***********************************************************************
BOOL CHorizonList::CheckData(void)
{
	int i, j, k;
	CMeasurePt *measurept, *m1, *m2;
	CHorizon *currhorizon;
	double oldtime, olddepth, mindist;
	double depth, time, x, y;
	double maxtime=-1.0;
	CString thisname;
	int invalid;
	BOOL HaveNotYetDisplayedTDincrease=TRUE;
	DWORD status;
	UBYTE ptstatus;

	//sk126
	// prevent pairs of points which are too close to one another
	mindist=(double)(AfxGetApp()->GetProfileInt("ArithmeticParams", "WellProximityThreshold", 50));
	mindist*=mindist;
	for(i=0;i<GetSize();i++)
	{ // go through all horizons
		currhorizon = (CHorizon *) GetAt(i);
		// Check that any point is not closer than a certain distance to any other point
		for(j=0;j<currhorizon->GetSize();j++)
		{
			// check all previous points whether we are getting uncomfortably close
			if(!(m1 = (*currhorizon)[j])) 

				continue;
			if(m1->ReadX() == MPT_INVALID || m1->ReadY() == MPT_INVALID) 
				continue;

			for(k=0;k<j;k++)
			{
				if(!(m2 = (*currhorizon)[k])) continue;
				if(m2->ReadX() != MPT_INVALID && m2->ReadY() != MPT_INVALID)
				{
					x=m1->ReadX()-m2->ReadX();
					y=m1->ReadY()-m2->ReadY();

					if(x*x+y*y<=mindist)
					{
						//m2->WriteX(MPT_INVALID);
						//m2->WriteY(MPT_INVALID);
						m2->WriteDepth(MPT_INVALID);
						m2->WriteTime(MPT_INVALID);

						CString message = "In Horizon " + currhorizon->GetName() + " well " +
							wellnames[j] + " and " + wellnames[k] +
							" are too close, discarding the latter...";

						pDoc->Log(message, LOG_DETAIL);

					}
				}
			}
		}
	}
	//sk126

	for(j=0;j<wellnames.GetSize();j++)
	{
		oldtime = -2*MPT_INVALID;
		olddepth = -2*MPT_INVALID;
		thisname= wellnames[j];
		status=wellstatus.GetAt(j);
		for(i=0;i<GetSize();i++)
		{
			measurept = (CMeasurePt *) ((CHorizon *) GetAt(i))->GetAt(j);

			if(!measurept) continue;

			depth = measurept->ReadDepth();
			time = measurept->ReadTime();
			x=measurept->ReadX();
			y=measurept->ReadY();

			// either three values are invalid, or none of them. Count number of
			// invalid entries. Note that we do not check time: That value might be
			// missing
			invalid=0;
			if(depth==MPT_INVALID) invalid++;
			if(x==MPT_INVALID) invalid++;
			if(y==MPT_INVALID) invalid++;

			// find out largest time in dataset
			if(time!=MPT_INVALID)
				maxtime=max(time, maxtime);

			ptstatus=measurept->ReadStatus();
			if(invalid>0 || time==MPT_INVALID)
				measurept->WriteStatus(ptstatus | MPT_STATUS_INVALID);

			if(invalid!=0 && invalid!=3 &&
				!(invalid==1 && depth==MPT_INVALID))
			{
				pDoc->Log("Discrepancy between time/depth and survey file: Well "+thisname+
					", horizon " + ((CHorizon *) GetAt(i))->name, LOG_BRIEF);
				continue;
			}

			if(time!=MPT_INVALID && time < oldtime)
			{
				if(HaveNotYetDisplayedTDincrease)
				{
					if(pDoc->Error("Time values not ascending in well " +
						thisname + " starting at horizon " + ((CHorizon *) GetAt(i))->name +
						"\n(Further similar errors will only be logged, but not displayed)",
						MSG_WARN | MSG_CANCEL))
						return FALSE;
					HaveNotYetDisplayedTDincrease=FALSE;
				}
				else
					pDoc->Log("Time values not ascending in well " +
					thisname + " starting at horizon " + ((CHorizon *) GetAt(i))->name, LOG_BRIEF);
			}
			else
			{
				if(time!=MPT_INVALID)
					oldtime = time;
			}

			if(depth!=MPT_INVALID && depth < olddepth)
			{
				if(HaveNotYetDisplayedTDincrease)
				{
					if(pDoc->Error("Depth values not ascending in well " +
						thisname + " starting at horizon " +((CHorizon *) GetAt(i))->name +
						"\n(Further similar errors will only be logged, but not displayed)",
						MSG_WARN | MSG_CANCEL))
						return FALSE;
					HaveNotYetDisplayedTDincrease=FALSE;
				}
				else
				{
					pDoc->Log("Depth values not ascending in well " +
						thisname + " starting at horizon " +((CHorizon *) GetAt(i))->name, LOG_BRIEF);
				}
			}
			else
			{
				if(depth!=MPT_INVALID)
					olddepth = depth;
			}
		}
	}

	return(TRUE);
}

// checks incoming file to decide whether it's in msecs or in secs.
// if in secs, converts it to millisecs.
double CHorizonList::CheckTime(double time)
{
	int MinSecs=AfxGetApp()->GetProfileInt("Well Import", "MSecThreshold", (int)MPT_MINSECS);

	// -999 is a special marker so we must leave it alone,
	// hence the use of abs().

	if ( fabs(time) < (double) MinSecs )
		return(time*1000.0);
	else
		return(time);
}

//***********************************************************************
// Subroutine : CheckASCIIFile()
// Arguments : filename : filename of possible ascii file
// Use : Checks if file is actually an ascii file
// Returns : TRUE if ascii - else FALSE
//***********************************************************************
BOOL CHorizonList::CheckASCIIFile(CString &filename)
{
#ifdef VELMAN_UNIX
	ifstream asciifile(filename);
#else
	std::ifstream asciifile(filename);
#endif
	char buffer[1024];
	int size = 0, i = 0;

	// check if one can even open the file
	if(asciifile.fail()) return(TRUE);

	do
	{
		asciifile.read(buffer, 1024);
		size = (long)asciifile.gcount();
		for(i=0;i<size;i++)
			if(!isascii(buffer[i]))
			{
				pDoc->Error("File " + filename + " is not an valid input file", MSG_ERR);
				return(FALSE);
			}
	}while(!asciifile.eof());

	return(TRUE);
}

//Removes a well at a given index
void CHorizonList::RemoveWell(int index)
{
	CHorizon *horizon;
	CString s = pDoc->horizonlist.wellnames.GetAt(index);

	//remove well
	wellnames.RemoveAt(index);
	wellXsurf.RemoveAt(index);
	wellYsurf.RemoveAt(index);
	wellstatus.RemoveAt(index);

	//remove well points
	for(int i =0; i< GetSize(); i++)
	{
		horizon = (CHorizon *)GetAt(i);

		horizon->RemoveAt(index);
	}

	pDoc->Log("Removed well: "+s, LOG_STD);
}

//***********************************************************************
// Subroutine : Clear()
// Use : De-Allocates all memory
// Returns : none
//***********************************************************************
void CHorizonList::Clear()
{
	int i=0;

	while(i < GetSize())
	{
		delete (CHorizon *) GetAt(i++);
	}
	RemoveAll();

	wellnames.RemoveAll();
	wellXsurf.RemoveAll();
	wellYsurf.RemoveAll();
	wellstatus.RemoveAll();
}

//***********************************************************************
// Subroutine : SetHorizonWeights()
// Arguments : type=WEIGHTMODEL_DEFAULTS (set all weights to 1.0)
// or =WEIGHTMODEL_DECREASE (decrease all weights, use other args)
// (x, y) = center of region of interest, rad=radius of region
// model=type of decrease (0=linearly, 1=quadratically)
// Use : set weights of all wells in one horizon, either to 1.0 or to
// a value corresponding to the well position (xi, yi):
// distance (x, y), (xi, yi) < rad : weight = 1.0
// distance >=rad:
// model=linear: linearly decrease, so that farthest well has
// weight "downto"
// model=quadr.: decrease by w=a*distance^2+b*distance+c
// Returns : none
//***********************************************************************

#include <stdio.h>

void CHorizon::SetHorizonWeights(int type, int model, double x, double y, double rad,
	double downto, CVelmanDoc *pDoc)
{
	if(rad<1.0)
		rad=1.0;

	double d = 0, dx = 0, dy = 0, maxrad = 0;
	double a = 0, b = 0, c = 0, R = 0;
	int i = 0;
	CMeasurePt *measurept;
	char buf[256];
	// find out maximum distance from center before actual calculation IF a
	// calculation is necessary

	/*
	LINEAR:
	We also have to determine the coefficients for the formula. We have
	f(rad)=1 (start with weight 1 at border of r.o.i.)
	f(maxrad)=downto (end with certain weight out there)
	this yields
	a=(downto-1)/(maxrad-rad) b=1-a*rad

	QUADRATIC
	f(r)=a*r^2+b*r+c
	z=radius of region of interest
	y=distance of farthest well (y>z)

	and three boundary conditions:
	f(rad)=1 (start with weight 1 at border of r.o.i.)
	f'(rad)=0 (start smoothly there)
	f(maxrad)=downto (end with certain weight out there)

	This is solved to
	a=(downto-1)/(maxrad-rad)^2 b=-2a*rad c=1+a*rad^2
	*/

	if(type==WEIGHTMODEL_DECREASE)
	{
		maxrad=0.0;
		for(i=0;i<GetSize();i++)
		{
			measurept=(CMeasurePt *)GetAt(i);
			if(measurept!=NULL && !( (measurept->ReadStatus()) & MPT_STATUS_INVALID) )
			{
				dx=(measurept->ReadX()-x);
				dy=(measurept->ReadY()-y);
				d=dx*dx+dy*dy;
				if(d>maxrad)
					maxrad=d;
			}
		}
	}

	// if the radius of the region of interest (where everything has weight 1) is
	// larger than the radius of the entire field, we just set defaults and that's it
	maxrad=sqrt(maxrad);
	if(maxrad<=rad)
		type=WEIGHTMODEL_DEFAULTS;

	if(type==WEIGHTMODEL_DECREASE)
	{
		R=maxrad-rad;
		if(model==0)
		{
			a=(downto-1.0)/R;
			b=1.0-a*rad;
		}
		else
		{
			a=(downto-1.0)/(R*R);
			b=-2*a*rad;
			c=1.0+a*rad*rad;
		}
	}
	if(pDoc)
	{
		sprintf(buf, "%-32sweight", "well name");
		pDoc->Log(buf, LOG_DETAIL);
	}
	for(i=0;i<GetSize();i++)
	{
		measurept=(CMeasurePt *)GetAt(i);
		if(measurept!=NULL && !( (measurept->ReadStatus()) & MPT_STATUS_INVALID) )
		{
			if(type==WEIGHTMODEL_DEFAULTS)
				measurept->WriteWeight(1.0);
			else
			{
				dx=(measurept->ReadX()-x);
				dy=(measurept->ReadY()-y);
				d=sqrt((dx*dx+dy*dy));
				if(d<=rad)
					measurept->WriteWeight(1.0);
				else
				{
					if(model==0)
						measurept->WriteWeight(a*d+b);
					else
						measurept->WriteWeight(a*d*d+b*d+c);
					if(pDoc)
					{
						sprintf(buf, "%-32s%f",
							(const char *) (pDoc->horizonlist.wellnames[i]),
							measurept->ReadWeight());
						pDoc->Log(buf, LOG_DETAIL);
					}
				}
			}
		}
	}
}

//***********************************************************************
// Subroutine : ~CHorizon()
// Arguments : none
// Use : Return memory allocated in class CHorizon
// Returns : none
//***********************************************************************

CHorizon::~CHorizon()
{
	int i=0;

	while(i < GetSize())
	{
		delete (CMeasurePt *) GetAt(i++);
	}
	RemoveAll();
}

//***********************************************************************
// Subroutine : WriteSurveyData()
// Arguments : nx, ny : new x and y values for measurement point
// Use : Puts position values into internal database
// Returns : none
//***********************************************************************

BOOL CMeasurePt::WriteSurveyData(double nx, double ny, UBYTE newstatus)
{
	// Put statusflags into internal status byte
	status |= newstatus;

	if((nx > MPT_PMAX) || (nx < MPT_PMIN))
		return(FALSE);

	if((ny > MPT_PMAX) || (ny < MPT_PMIN))
		return(FALSE);

	// read data array into object
	x = nx;
	y = ny;

	return(TRUE);
}

//***********************************************************************
// Subroutine : WriteWellData()
// Arguments : ntime : new time values for measurement point
// ndepth : new depth values for measurement point
// Use : Puts time/depth values into internal database
// Returns : none
//***********************************************************************

BOOL CMeasurePt::WriteWellData(double ntime, double ndepth)
{
	// Decide if depth and time are within acceptable limits
	if(ndepth!=MPT_INVALID && ndepth!=MPT_MUSTNOTREPAIR_ITEM
		&& (ndepth > MPT_DMAX || ndepth < MPT_DMIN))
	{
		return(FALSE);
	}

	if(ntime!=MPT_INVALID && ntime!=MPT_MUSTNOTREPAIR_ITEM
		&& (ntime > MPT_TMAX || ntime < MPT_TMIN))
	{
		return(FALSE);
	}

	// if they are, store them. If they read "-9999", store them as illegal
	// and mark them "not to be rebuilt at any later time"
	if(ndepth==MPT_MUSTNOTREPAIR_ITEM)
	{
		depth=MPT_INVALID;
		status|=MPT_STATUS_MUSTNOTREPAIR_DEPTH;
	}
	else
		depth = ndepth;

	if(ntime==MPT_MUSTNOTREPAIR_ITEM)
	{
		time=MPT_INVALID;
		status|=MPT_STATUS_MUSTNOTREPAIR_TIME;
	}
	else
		time = ntime;

	return(TRUE);
}

void CMeasurePt::ReadValuePair(int model, double *px, double *py)
{
	// returns a pair of values corresponding to a certain model. Each model
	// correlates different pairs of values, and this function knows about the
	// correct combination for each model. Therefore the order of models must not
	// be changed!!!

	switch(model)
	{
	case 0:*py=depth;*px=tInt;break;
	case 1:*py=dInt;*px=tInt;break;
	case 2:*py=vInt;*px=mpdepth;break;
	case 3:*py=vInt;*px=mptime;break;
	case 4:*py=vInt;*px=tTop;break;
	case 5:*py=vInt;*px=time;break;
	case 6:*py=vInt;*px=dTop;break;
	case 7:*py=vInt;*px=depth;break;
	case 8:*py=vInt;*px=dInt;break;
	case 9:*py=vInt;*px=tInt;break;
	case 10:*py=vInt;*px=vInt;break;// x value should never be used anyway
		// do not want to set it to a constant, so that xmax-xmin != 0
	}
}

int ResidualInfoCompare_Resid( const void *arg1, const void *arg2 )
{
	double r1=((struct ResidualInfoType *)arg1)->residual;
	double r2=((struct ResidualInfoType *)arg2)->residual;
	return (r1>r2 ? -1 :(r1==r2 ? 0 : 1));
}

int ResidualInfoCompare_Index( const void *arg1, const void *arg2 )
{
	int i1=((struct ResidualInfoType *)arg1)->listindex;
	int i2=((struct ResidualInfoType *)arg2)->listindex;
	return (i1>i2 ? -1 :1);
}

// The following four functions convert the a, b fitting parameters to what we call
// V0 (V-nought) and K, which is what we should show to the user in the dialogue
// boxes.
// Decided probably best not to make these part of a class.

double ab2V( double a, double b )
	// converts a, b to V using formula
{
	return( 2000.0/(a*b) );
}

double ab2K( double a, double b )
	// convert a, b to K using formula
{
	return( 2000.0/a );
}

double VK2a( double V, double K )
	// converts K to a using formula
{
	return( 2000.0/K );
}

double VK2b( double V, double K )
	// convert V, K to b using formula
{
	return( K/V );
}