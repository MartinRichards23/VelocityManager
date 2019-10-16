#include "VelmanDoc.h"
#include "velman.h"			// because next include needs the resource ids

#include "ContourSettingsDlg.h"
#include "colourmodel.h"
#include <math.h>

//  define static variables in CContourPlot class: used to preserve ContourOptionsDialogue
//  settings between class invocations. Otherwise user changes are lost.
int CContourPlot::RequestedNumLevels=2;	// 0=none, 1=few, 2=normal, 3=many
COLORREF CContourPlot::textcol = RGB(0, 0, 0);
int CContourPlot::ColourPalette = 0;     // 0 = rainbow, 1 = greyscale
BOOL CContourPlot::ColorShading = TRUE;
int CContourPlot::NumLabels = 1;
// ... there should be more static variables really. We'd like to preserve whether the
// user has turned wells on for instance.

CContourSettingsDlg setDlg;

CContourPlot::CContourPlot(CVelmanDoc *pDoc)
{
	HaveData=FALSE;
	HaveResampled=FALSE;
	HaveIndividualLevels=FALSE;
	HaveDownsampledCopy=FALSE;
	MayDeleteGrid=TRUE;
	StdLineThickness=1;
	NthLineThickness=2;
	EveryNthThick = 50; // which practically disables this feature, because we use color coding
	EveryNthLabel=2;
	LabelTypeSize=8;
	KeyTypeSize=100;	// specifies point size in 10ths of a point (ie. 110 is 11pt text)
	NumLevels=10;	// the number of levels actually plotted, usually overriden automatically
	ShadeLevels=0;
	dotsize=5;
	coarseness=10;
	labelling=CNT_USERLABEL;
	rawxy=NULL;
	rawz=NULL;
	grid=NULL;
	pDoc=pDoc;
	dokey=FALSE;
	linecol=RGB(0, 0, 0);
	upside=FALSE;
	normalizegradients=FALSE;
	normgradfac=50.0;
	AutoKeyTypeSize=FALSE;
	/*if ( ColorShading )
		labelling = 0;*/
	velpal = NULL;
	smallest_maxmin = 1.0e-6;   // smallest distance in metres we bother to contour.
	
	labelnum =0 ;
}

CContourPlot::~CContourPlot()
{
	ForceContSetToClose();
	CleanUp();
}

//***********************************************************************
// Subroutine : InitRawData
// Arguments  : n    number of (x, y, z) coordinates given
//						: x, y, z lists of coordinates (these are copied into a local buffer)
// Use				: Provide raw data for Resample_Data() call
// Returns		: TRUE=ok, FALSE=memory problem
//***********************************************************************
BOOL CContourPlot::InitRawData(int n, double *x, double *y, double *z)
{
	int i = 0;

	npts=n;
	if(n<1)
		return TRUE;

	if((rawxy=dmatrix(1, n, 1, 2)))
	{
		if((rawz=new double[n+1]))
		{
			for(i=0;i<n;i++)
			{
				rawxy[i+1][1]=x[i];
				rawxy[i+1][2]=y[i];
				rawz[i+1]=z[i];
			}
			HaveData=TRUE;
			MayDeleteGrid=TRUE;
			return TRUE;
		}
	}
	return FALSE;
}

//***********************************************************************
// Subroutine : InitResampledData
// Arguments  : zimsgrid	pointer to a CZimsGrid object
// Use				: Provide data for DrawContourPlot directly. Resampling is not
//							necessary because zims grids already are gridded.
//***********************************************************************
void CContourPlot::InitResampledData(CZimsGrid *zimsgrid)
{
	HaveResampled=FALSE;
	if(!zimsgrid)
		return;

	grid=zimsgrid;
	// get number of nodes and check for size
	if(zimsgrid->ShouldDownsample())
	{
		CZimsGrid *smallergrid=new CZimsGrid(pDoc);
		if(smallergrid)
		{
			smallergrid->CopyAndDownsample(*grid);
			HaveDownsampledCopy=TRUE;
			grid=smallergrid;
		}
	}

	NumRows=grid->GetRows();
	NumCols=grid->GetColumns();
	CanAdjustCorseness=FALSE;
	MayDeleteGrid=FALSE;

	double *limits;
	limits=zimsgrid->GetXlimits();
	x1=limits[0];
	x2=limits[1];
	limits=zimsgrid->GetYlimits();
	y1=limits[0];
	y2=limits[1];

	HaveResampled=TRUE;
}

//***********************************************************************
// Subroutine : CleanUp
// Use				: Deletes all locally allocated memory
//***********************************************************************
void CContourPlot::CleanUp()
{
	if(rawxy)
	{
		free_dmatrix(rawxy, 1, npts, 1, 2);
		rawxy=NULL;
	}
	delete rawz;
	rawz=NULL;
	if(MayDeleteGrid || HaveDownsampledCopy)
	{
		delete grid;
		grid=NULL;
	}
	delete velpal;
	velpal = NULL;
}

//***********************************************************************
// Subroutine	:			Resample Data
// Arguments	:			(rx1, ry1)..(rx2, ry2) = region to resample
//									nrow, ncol = dimensions of grid for resampling
// Use        :     grids raw (x, y, z) data onto a rectangular grid
// Remarks    :     sets HaveResampled to TRUE if sucessful. Only then
//									can one call DrawContourPlot
//***********************************************************************
BOOL CContourPlot::ResampleData(double rx1, double ry1, double rx2, double ry2)
{
	CWaitCursor wait;
	int icoarseness=coarseness;
	if(coarseness<4) 
		icoarseness=4;

	NumRows=icoarseness*12;
	NumCols=icoarseness*12;
	CanAdjustCorseness=TRUE;
	x1=rx1;
	y1=ry1;
	x2=rx2;
	y2=ry2;

	delete grid;
	grid=new CZimsGrid();
	grid->Init(NumRows, NumCols, pDoc);

	double xlimit[2], ylimit[2];
	xlimit[0]=rx1;
	xlimit[1]=rx2;
	ylimit[0]=ry1;
	ylimit[1]=ry2;
	grid->WriteLimits(xlimit, ylimit);

	int stepsize = 6;

	if(icoarseness<=10 && icoarseness>=9) stepsize=6;
	else if(icoarseness<=8 && icoarseness>=6) stepsize=5;
	else if(icoarseness<=5 && icoarseness>=3) stepsize=4;
	else stepsize=4;

	HaveResampled=grid->Resample(npts, stepsize, rawz, rawxy, normalizegradients, normgradfac);

	return HaveResampled;
}

//***********************************************************************
// Subroutine	:			SetContourLevels
// Arguments	:			n: Number of contouring levels
//									kp: Pointer to list of descriptions of these levels
// Use        :     Allows use of user-defined contour levels, each at any
//									z-value and with several settings (see Level type defintion)
// Remarks    :     Does NOT copy the list of levels, just stores the pointer!
//***********************************************************************
void CContourPlot::SetContourLevels(int num, Level *kp)
{
	numuserdeflevels=num;
	userdeflevels=kp;
	HaveIndividualLevels=TRUE;
}

//***********************************************************************
// Subroutine	:			SetContourLabels
// Arguments	:			*x, *y		ptr to list of (x, y) coordinates for labels
//									num			number of labels
//									lab			ptr to list of strings
//  2nd version:    opt     ptr to list of option flags.
//									(possible option: color this label in red)
// Use        :     sets individual label positions at texts
//									to revert to standard labels/no labels, call it with
//									lab=NULL and arbitrary num, x, y
// Note				:     No copy is made of the data; only the pointers are
//									stored!
//***********************************************************************
void CContourPlot::SetContourLabels(int num, double *x, double *y, char **lab)
{
	SetContourLabels(num, x, y, NULL, lab);
}

void CContourPlot::SetContourLabels(int num, double *x, double *y, char *opt, char **lab)
{
	if(!lab)
	{
		labelling&=(~CNT_USERLABEL);
		return;
	}
	labelling|=CNT_USERLABEL;
	labelx=x;
	labely=y;
	labeltext=lab;
	labelopt=opt;
	labelnum=num;
}

//***********************************************************************
// Subroutine	:			SetContourOption
// Arguments	:			option: number of option to set (see #defines in contour.h)
//									intvalue: new value of that option
// Use        :     sets one of the internal options
// Returns    :     TRUE=value was set, FALSE=error
//***********************************************************************

BOOL CContourPlot::SetContourOption(int option, int intvalue)
{
	switch(option)
	{
	case CNTOPT_STD_THICKNESS:
		StdLineThickness=intvalue;break;
	case CNTOPT_NTH_THICKNESS:
		NthLineThickness=intvalue;break;
	case CNTOPT_EVERY_NTH_THICK:
		EveryNthThick=intvalue;break;
	case CNTOPT_EVERY_NTH_LABEL:
		EveryNthLabel=intvalue;break;
	case CNTOPT_NUMLEVELS:
		NumLevels=intvalue;break;
	case CNTOPT_LABELTYPESIZE:
		LabelTypeSize=intvalue;break;
	case CNTOPT_SHADELEVELS:
		ColorShading=intvalue;break;
	case CNTOPT_COARSENESS:
		coarseness=intvalue;break;
	case CNTOPT_LABELLING:
		labelling=intvalue;break;
	case CNTOPT_LABELDOTSIZE:
		dotsize=intvalue;break;
	case CNTOPT_UPSIDEDOWN:
		upside=(BOOL)intvalue;break;
	case CNTOPT_KEYTYPESIZE:
		KeyTypeSize=intvalue;
		AutoKeyTypeSize=(KeyTypeSize==-1);
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

BOOL CContourPlot::SetContourOption(int option, long longvalue)
{
	switch(option)
	{
	case CNTOPT_TEXTCOL:
		textcol=longvalue;break;
	case CNTOPT_LINECOL:
		linecol=longvalue;break;
	case CNTOPT_CREATEKEY:
		keyx=longvalue & 0xFFFF;
		keyy=longvalue >> 16;
		if(keyx!=0 && keyy!=0)
			dokey=TRUE;
		else
			dokey=FALSE;
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

BOOL CContourPlot::SetContourOption(int option, BOOL flag, double floatvalue)
{
	switch(option)
	{
	case CNTOPT_NORMALIZEGRADIENTS:
		normalizegradients=flag;
		normgradfac=floatvalue;
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

//***********************************************************************
// Subroutine	    ContourOptionsDialog
// Use        :     Evokes dialog that allows user to change some of the
//									plot's settings

// Arguments  :     showstuff: bit field structure, see its definition in contour.h
// Returns    :      Bit 1 set if plot has to be redrawn (OK clicked)
//					Bit 2 set if plot needs resampling before drawing (coarseness changed)
//***********************************************************************
int CContourPlot::ContourOptionsDialog(int options, struct showspecialstuff_type *showstuff)
{
	int result=0;

	setDlg.m_numlines=RequestedNumLevels;
	setDlg.m_thicknum=EveryNthThick-1;
	setDlg.m_textcol=textcol;
	setDlg.options=options;
	setDlg.furtheroptions=CanAdjustCorseness ? 1 :0;
	setDlg.minval=zmin;
	setDlg.maxval=zmax;
	setDlg.mincol = velpal->GetColour(0);
	setDlg.maxcol=velpal->GetColour(velpal->GetMaxFillColours());
	setDlg.m_showlabels=(labelling & CNT_AUTOLABEL);
	setDlg.m_numlabels=NumLabels;      // choose between few, normal and high nos. of labels
	setDlg.m_labelnum=EveryNthLabel;
	setDlg.m_labelsize=LabelTypeSize;
	setDlg.m_colorshading=ColorShading;
	setDlg.showstuff=showstuff;
	setDlg.m_palette = ColourPalette;      // default colour palette is rainbow.

	if (((CVelmanApp*) AfxGetApp())->OpendContSettings==TRUE)
		// there is already a settings box open
		return 1;

	((CVelmanApp*) AfxGetApp())->OpendContSettings=TRUE;

	if(setDlg.DoModal()==IDOK)
	{
		EveryNthThick=setDlg.m_thicknum+1;
		textcol=setDlg.m_textcol;

		RequestedNumLevels=setDlg.m_numlines;
		NumLabels = setDlg.m_numlabels;
		result|=CNTDLG_NEEDREDRAW;
		labelling &= ~CNT_AUTOLABEL;
		if(setDlg.m_showlabels)
			labelling |= CNT_AUTOLABEL;
		EveryNthLabel=setDlg.m_labelnum;
		LabelTypeSize=setDlg.m_labelsize;
		ColorShading=setDlg.m_colorshading;
		ColourPalette=setDlg.m_palette;
	}
	((CVelmanApp*) AfxGetApp())->OpendContSettings=FALSE;
	return result;
}

void CContourPlot::AllocTerrain (short nx, short ny, Terrain *tp)
{
	short j;			/* count */

	tp->vert = (BOOL **) calloc (ny, sizeof (BOOL *));
	tp->vx = (double **) calloc (ny, sizeof (double *));

	for (j = 0; j < ny; j++)
	{
		tp->vert[j] = (BOOL *) calloc (nx + 1, sizeof (BOOL));
		tp->vx[j] = (double *) calloc (nx + 1, sizeof (double));
	}

	tp->hori = (BOOL **) calloc (ny + 1, sizeof (BOOL *));
	tp->hy = (double **) calloc (ny + 1, sizeof (double *));

	for (j = 0; j <= ny; j++)
	{
		tp->hori[j] = (BOOL *) calloc (nx, sizeof (BOOL));
		tp->hy[j] = (double *) calloc (nx, sizeof (double));
	}

	return;
}

// FreeTerrain () frees the Terrain structure
void CContourPlot::FreeTerrain (short nx, short ny, Terrain *tp)
{
	short j;			/* count */

	for (j = 0; j < ny; j++)
	{
		free (tp->vert[j]);
		free (tp->vx[j]);
	}

	free (tp->vert);
	free (tp->vx);

	for (j = 0; j <= ny; j++)
	{
		free (tp->hori[j]);
		free (tp->hy[j]);
	}

	free (tp->hori);
	free (tp->hy);

	return;
}

//MapTerrain () fills the Terrain structure
void CContourPlot::MapTerrain (short k, Plot *pp, Data *dp, Terrain *tp)
{
	double **cp = dp->cells;       /* cells array */
	double h = pp->levels[k].height; /* how high is this level? */
	double xu = pp->xu;            /* half real width of cell */
	double yu = pp->yu;            /* half real height of cell */
	short i, j;			/* count columns and rows respectively */
	short nx = dp->nx;            /* number of columns of data */
	short ny = dp->ny;            /* number of rows of data */

	/* map edges first */
	for (j = 0; j < ny; j++)
	{
		if (cp[j][0] >= h)
		{
			tp->vert[j][0] = RIGHT;
			tp->vx[j][0] = xu;
		}
		else
			tp->vert[j][0] = FALSE;

		if (cp[j][nx-1] >= h)
		{
			tp->vert[j][nx] = LEFT;
			tp->vx[j][nx] = -xu;
		}
		else
			tp->vert[j][nx] = FALSE;
	}

	for (i = 0; i < nx; i++)
	{
		if (cp[0][i] >= h)
		{
			tp->hori[0][i] = UP;
			tp->hy[0][i] = yu;
		}
		else
			tp->hori[0][i] = FALSE;

		if (cp[ny-1][i] >= h)
		{
			tp->hori[ny][i] = DOWN;
			tp->hy[ny][i] = -yu;
		}
		else
			tp->hori[ny][i] = FALSE;
	}
	/* now map interior */
	for (j = 0; j < ny; j++)
		for (i = 1; i < nx; i++)
		{
			if (cp[j][i] <= h)
				if (i > 0 && cp[j][i-1] >= h && cp[j][i-1] != cp[j][i])
				{
					tp->vert[j][i] = LEFT;
					tp->vx[j][i] =
						(xu * (cp[j][i-1] + cp[j][i] - 2 * h) / (cp[j][i-1] -
						cp[j][i]));
				}
				else
					tp->vert[j][i] = FALSE;

			if (cp[j][i] >= h)
				if (i > 0 && cp[j][i-1] <= h && cp[j][i-1] != cp[j][i])
				{
					tp->vert[j][i] = RIGHT;
					tp->vx[j][i] =
						(xu * (cp[j][i-1] + cp[j][i] - 2 * h) / (cp[j][i-1] -
						cp[j][i]));
				}
				else
					tp->vert[j][i] = FALSE;
		}

		for (j = 1; j < ny; j++)
			for (i = 0; i < nx; i++)
			{
				if (cp[j][i] <= h)
					if (j > 0 && cp[j-1][i] >= h && cp[j-1][i] != cp[j][i])
					{
						tp->hori[j][i] = DOWN;
						tp->hy[j][i] =
							(yu * (cp[j-1][i] + cp[j][i] - 2 * h) / (cp[j-1][i] -
							cp[j][i]));
					}
					else
						tp->hori[j][i] = FALSE;

				if (cp[j][i] >= h)
					if (j > 0 && cp[j-1][i] <= h && cp[j-1][i] != cp[j][i])
					{
						tp->hori[j][i] = UP;
						tp->hy[j][i] =
							(yu * (cp[j-1][i] + cp[j][i] - 2 * h) / (cp[j-1][i] -
							cp[j][i]));
					}
					else
						tp->hori[j][i] = FALSE;
			}
}

// finds the next direction for the contour path.
BOOL CContourPlot::GetNext (short nx, short ny, Terrain *tp, State *sp)
{
	Direction next;               /* proposed next value for sp->direction */
	int turn;                     /* directional offset from sp->direction */
	int dummy;	// addition by TP

	for (turn = 2; turn >= -2; turn--)
	{
		dummy=(int)sp->direction+turn;
		dummy=dummy % 8;
		if(dummy<0) dummy+=8;
		next=(Direction)dummy;

		switch (next)
		{
		case North:
			if (sp->attitude)     /* Vertical */
				if (sp->jj < ny - 1 && tp->vert[sp->jj+1][sp->ii] == LEFT)
				{
					sp->direction = North;
					sp->jj++;
					return TRUE;
				}
				else
					break;
			else                  /* Horizontal */
				break;

		case NorthEast:
			if (sp->attitude)     /* Vertical */
				if (sp->ii < nx && tp->hori[sp->jj+1][sp->ii] == UP)
				{
					sp->direction = NorthEast;
					sp->attitude = Horizontal;
					sp->jj++;
					return TRUE;
				}
				else
					break;
			else                  /* Horizontal */
				if (sp->jj < ny && tp->vert[sp->jj][sp->ii+1] == LEFT)
				{
					sp->direction = NorthEast;
					sp->attitude = Vertical;
					sp->ii++;
					return TRUE;
				}
				else
					break;

		case East:
			if (sp->attitude)     /* Vertical */
				break;
			else                  /* Horizontal */
				if (sp->ii < nx - 1 && tp->hori[sp->jj][sp->ii+1] == UP)
				{
					sp->direction = East;
					sp->ii++;
					return TRUE;
				}
				else
					break;

		case SouthEast:
			if (sp->attitude)     /* Vertical */
				if (sp->ii < nx && tp->hori[sp->jj][sp->ii] == UP)
				{
					sp->direction = SouthEast;
					sp->attitude = Horizontal;
					return TRUE;
				}
				else
					break;
			else                  /* Horizontal */
				if (sp->jj > 0 && tp->vert[sp->jj-1][sp->ii+1] == RIGHT)
				{
					sp->direction = SouthEast;
					sp->attitude = Vertical;
					sp->jj--;
					sp->ii++;
					return TRUE;
				}
				else
					break;

		case South:
			if (sp->attitude)     /* Vertical */
				if (sp->jj > 0 && tp->vert[sp->jj-1][sp->ii] == RIGHT)
				{
					sp->direction = South;
					sp->jj--;
					return TRUE;
				}
				else
					break;
			else                  /* Horizontal */
				break;

		case SouthWest:
			if (sp->attitude)     /* Vertical */
				if (sp->ii > 0 && tp->hori[sp->jj][sp->ii-1] == DOWN)
				{
					sp->direction = SouthWest;
					sp->attitude = Horizontal;
					sp->ii--;
					return TRUE;
				}
				else
					break;
			else                  /* Horizontal */
				if (sp->jj > 0 && tp->vert[sp->jj-1][sp->ii] == RIGHT)
				{
					sp->direction = SouthWest;
					sp->attitude = Vertical;
					sp->jj--;
					return TRUE;
				}
				else
					break;

		case West:
			if (sp->attitude)     /* Vertical */
				break;
			else			/* Horizontal */
				if (sp->ii > 0 && tp->hori[sp->jj][sp->ii-1] == DOWN)
				{
					sp->direction = West;
					sp->ii--;
					return TRUE;
				}
				else
					break;

		case NorthWest:
			if (sp->attitude)     /* Vertical */
				if (sp->ii > 0 && tp->hori[sp->jj+1][sp->ii-1] == DOWN)
				{
					sp->direction = NorthWest;
					sp->attitude = Horizontal;
					sp->jj++;
					sp->ii--;
					return TRUE;
				}
				else
					break;
			else                  /* Horizontal */
				if (sp->jj < ny && tp->vert[sp->jj][sp->ii] == LEFT)
				{
					sp->direction = NorthWest;
					sp->attitude = Vertical;
					return TRUE;
				}
				else
					break;

		default:
			AfxMessageBox("Contouring severe warning.\n\nInternal error in GetNext()");
			return FALSE;
		}
	}
	return FALSE;                 /* couldn't find good next direction */
}

//***********************************************************************
// Subroutine : DrawContourPlot
// Arguments  : dc		device context into which to draw the contour
//  note that for some mapping modes (ANISOTROPIC) the UPSIDEDOWN flag must be set!!!
//							left, top, right, bottom		coords of rectangle around plot area
// beforehand one might want to set the number of levels using CNTOPT_NUMLEVELS in
// SetContourOption
//						levels  number of levels. Either >0, then uses that many levels
//								and determines their z-values automatically (eq. spacing)
//								OR levels <=0, then the levels are taken from a previous
//								call to SetContourLevels(). There must have been such a
//								previous call
// Use				: Draws a contour
// Returns		: TRUE=ok, FALSE=memory problem or levels not known or no data
//***********************************************************************
BOOL CContourPlot::DrawContourPlot(CDC *dc, int left, int top, int right, int bottom)
{
	CWaitCursor wait; // display an hourglass cursor while we go along

	int width = right-left;
	int height = bottom-top;

	if ( velpal )
		delete velpal;      // delete old one before we create a new one
	velpal = new CVelmanPalette;
	// set up colour palette for this plot
	switch( ColourPalette)      // set in contouroptions dialogue
	{
	case 0:     // rainbow effect
		velpal->CreateVelPalette();
		break;
	case 1:     // greyscale
		velpal->CreateVelPalette(0.9, 0.1);   // start_value (white), end_value (black)
		break;
	default:
		break;
	}
	dc->SelectPalette(velpal->FromHandle(*velpal), FALSE);
	if ( dc->RealizePalette() )
		dc->UpdateColors();

	if(!HaveResampled)		// Resample_Data() not called?
		return FALSE;

	dc->SetBkMode(TRANSPARENT);

	int levels=NumLevels;
	if(levels<=0 && !HaveIndividualLevels)
		return FALSE;
	if(x1>=x2 || y1>=y2)
		return FALSE;

	Plot pp;
	Data dp;
	Level *kp=NULL;
	int i, j;
	// fill these structures with data
	// just a data-reformatting operation to communicate between the data passed to
	// the class and the data that the original contouring code requires
	pp.x0=left;
	pp.y0=top;
	pp.x1=right;
	pp.y1=bottom;
	ysize=-top+bottom;		// used for flipping the graph (org. postscript has (0, 0)=btm left)

	zmin=grid->CalcMinVal();
	zmax=grid->CalcMaxVal();
	if(levels<=0)
	{
		pp.nz=numuserdeflevels;
		pp.levels=userdeflevels;
	}
	else
	{
		grid->Stamp();

		int numlev;
		int powten;
		double curdist;
		numlev=12;

		curdist=(zmax-zmin)/numlev;

		powten=(int)(log10(curdist+FLT_MIN));
		curdist/=pow(10.0, (double)powten);		// will be 1..10

		if(curdist<2.0)
		{
			switch(RequestedNumLevels)
			{
			case 0:         // no levels
				curdist=0.0;
				break;
			case 1:         // few levels
				curdist=2.0;
				break;
			case 2:         // normal number of levels
				curdist=1.0;
				break;
			case 3:         // high number of levels
				curdist=0.5;
				break;
			}
		}
		else if(curdist<5.0)
		{
			switch(RequestedNumLevels)
			{
			case 0:
				curdist=0.0;
				break;
			case 1:
				curdist=5.0;
				break;
			case 2:
				curdist=2.0;
				break;
			case 3:
				curdist=1.0;
				break;
			}
		}
		else
		{
			switch(RequestedNumLevels)
			{
			case 0:
				curdist=0.0;
				break;
			case 1:
				curdist=10.0;
				break;
			case 2:
				curdist=5.0;
				break;
			case 3:
				curdist=2.0;
				break;
			}
		}

		if ( curdist == 0.0 )
		{
			levels = 0;
		}
		else
		{
			curdist*=pow(10.0, (double)powten);
			zmax=ceil(zmax/curdist)*curdist;
			zmin=floor(zmin/curdist)*curdist;

			// 27/5/01. levels gets set to 2 even if zmax-zmin is less
			// than 1cm (ie. 0.01 m).
			// What should happen is that we don't attempt any contours at all.
			if ( zmax-zmin > smallest_maxmin )
			{
				levels=(int)((zmax-zmin)/curdist)+1;
			}
			else
			{
				levels = 0;
				curdist = 0.0;
			}

			if((kp=new Level[levels])==NULL)
				return FALSE;
		}

		for(i=0;i<levels;i++)
		{
			kp[i].dash=0;
			kp[i].label=( (i % EveryNthLabel)==0 ? 1 : 0);
			kp[i].shade=1;
			kp[i].linewidth=( (i % EveryNthThick)==EveryNthThick-1 ? 3 : 1);
			kp[i].height=zmin+curdist*i;
		}
		pp.nz=levels;
		pp.levels=kp;
	}

	pp.lowest_shade = 0.0;
	pp.highest_shade = 1.0;
	pp.type_size = LabelTypeSize;

	dp.nx=NumCols;
	dp.ny=NumRows;
	dp.cells=grid->GetGrid();

	grid->Stamp();

	pDC=dc;

	pDC->SelectStockObject(WHITE_BRUSH);
	pDC->Rectangle(left, top, right, bottom);

	pDC->OffsetWindowOrg(-left,-top);

	// draw coloured shading?
	if(ColorShading)
	{
		double **data=grid->GetGrid();

		COLORREF nullcol=((CVelmanApp *)AfxGetApp())->GetProfileColor("Attributes", "ContourPlotNULL", RGB(0, 255, 255));
		double boxX=(((double)width)+1.0)/NumCols;	// # pixels per gridpoint horizontally
		double boxY=(((double)height)+1.0)/NumRows;

		grid->Stamp();
		for(i=0;i<NumRows;i++)
		{
			for(j=0;j<NumCols;j++)
			{
				double value = data[i][j];

				//draw nulls as blue
				if(grid->GetZnon(value))
				{
					dc->FillSolidRect((int)(j*boxX), (int)(i*boxY), (int)boxX+1, (int)boxY+1, nullcol);
					continue;
				}

				// this was just for debugging
				/*if(value >= -0.5 && value <= 0.5)
				{
					dc->FillSolidRect((int)(j*boxX), (int)(i*boxY), (int)boxX+1, (int)boxY+1, RGB(0,0,0));
					continue;
				}*/				

				dc->FillSolidRect((int)(j*boxX), (int)(i*boxY), (int)boxX+1, (int)boxY+1, InterpolateHSV(value, zmin, zmax));

			}
		}

	}

	if ( levels != 0 ) // only draw contours if we have some levels to do
		DrawPlot(&pp, &dp);

	pDC->OffsetWindowOrg(left, top);

	// Used to draw well labels here 18/09/09

	if(dokey)
	{
		if(AutoKeyTypeSize)		// now we're doing a proper colour bar, better to turn this off.
			KeyTypeSize=max(60, 10*int(width/80));
		DrawContourKey(&pp);
	}
	delete kp;

	// black frame around the whole lot, so we overwrite ugly coloured borders
	pDC->SelectStockObject(BLACK_PEN);
	pDC->MoveTo(left, top);
	pDC->LineTo(right, top);
	pDC->LineTo(right, bottom);
	pDC->LineTo(left, bottom);
	pDC->LineTo(left, top);

	return TRUE;
}

// Draw box around grid
void CContourPlot::DrawBox(CDC *dc, int left, int top, int right, int bottom, CZimsGrid *bgrid)
{
	int width = right-left;
	int height = bottom-top;

	// get min/max coords of grid to draw box of
	double xmin = bgrid->GetXlimits()[0];
	double xmax = bgrid->GetXlimits()[1];

	double ymin = bgrid->GetYlimits()[0];
	double ymax = bgrid->GetYlimits()[1];

	int boxX, boxY, boxX2, boxY2;

	//get the screen coords to draw
	if(xmin>=x1 && xmax<=x2 && ymin>=y1 && ymax<=y2)
	{
		boxX=left  +(int)(((xmin-x1)/(x2-x1)) * width);
		boxY=bottom - (int)(((ymin-y1)/(y2-y1)) * height);

		boxX2=left + (int)(((xmax-x1)/(x2-x1)) * width);
		boxY2=bottom - (int)(((ymax-y1)/(y2-y1))  *height);

		//draw the box
		pDC->SelectStockObject(BLACK_PEN);
		pDC->MoveTo(boxX, boxY);
		pDC->LineTo(boxX2, boxY);
		pDC->LineTo(boxX2, boxY2);
		pDC->LineTo(boxX, boxY2);
		pDC->LineTo(boxX, boxY);
	}
}

// Draws the axis labels and values
void CContourPlot::DrawAxisLabels(CDC *dc, int left, int top, int right, int bottom)
{
	int width = right-left;
	int height = bottom-top;

	char buf[256];
	int hgt, x, y, numlabel;

	double *xlimits, *ylimits;
	xlimits=grid->GetXlimits();
	double cx=xlimits[1]-xlimits[0];
	ylimits=grid->GetYlimits();
	double cy=ylimits[1]-ylimits[0];

	numlabel=width > height ? 5 : 3;
	// so we don't have the hassle with "how many points BETWEEN" etc later
	width--;
	height--;

	// make font for axis
	CFont keyfont;
	int fontsize = width/8;
	if(fontsize < 80)
		fontsize = 80;
	if(fontsize > 120)
		fontsize = 120;
	keyfont.CreatePointFont(fontsize, "Arial");

	TEXTMETRIC tm;
	pDC->GetTextMetrics(&tm);
	hgt=tm.tmHeight/2;
	CFont *pOldFont=pDC->SelectObject(&keyfont);

	pDC->SetTextAlign(TA_LEFT | TA_BOTTOM);
	for(int i=0;i<numlabel;i++)
	{
		x=left+(i*width)/(numlabel-1);
		pDC->MoveTo(x, top);
		pDC->LineTo(x, top-10);
		sprintf(buf, "%7.f", xlimits[0]+i*cx/(numlabel-1));
		pDC->TextOut(x, top-10, buf);

		y=top+(i*height)/(numlabel-1);
		pDC->MoveTo(right, y);
		pDC->LineTo(right  +10, y);
		sprintf(buf, "%7.f", ylimits[1]-i*cy/(numlabel-1));
		pDC->TextOut(right + 15, y+hgt, buf);
	}

	pDC->SelectObject(pOldFont);
	keyfont.DeleteObject();
}

// Draws the wells
void CContourPlot::DrawWellLabels(CDC *dc, int left, int top, int right, int bottom)
{
	CFont userlabelfont;

	int fontsize = LabelTypeSize*9;
	if(fontsize > 120)
		fontsize = 120; // limit max fontsize

	userlabelfont.CreatePointFont(fontsize, "Arial");
	CFont *pOldFont=pDC->SelectObject(&userlabelfont);
	COLORREF oldtextcol=pDC->SetTextColor(textcol);

	// Draw well labels
	if(labelling & CNT_USERLABEL)
	{
		int lx = 0;
		int ly = 0;
		int is = 0;

		CBrush redbrush(RGB(200, 0, 0));
		//pDC->SetBkColor(RGB(255, 255, 255));	// set background text colour
		pDC->SetBkMode(TRANSPARENT);

		for(int i=0;i<labelnum;i++)
		{
			if(labelx[i]>=x1 && labelx[i]<=x2 && labely[i]>=y1 && labely[i]<=y2)
			{
				lx=left+(int)(((labelx[i]-x1)/(x2-x1))*(right-left));
				ly=top+(int)(((labely[i]-y1)/(y2-y1))*(bottom-top));
				if(upside)
					ly=ysize-ly+2*top;
				if(labeltext[i]!=NULL)
				{
					for ( is = 0; *(labeltext[i]+is) == ' '; is++ )	// skip white space
						;
					if ( ! _strnicmp(labeltext[i]+is, "XX", 2)	)	// artifical wells always start name with 'XX'
					{											// ..spaces from tgcontv::OnInitialUpdate
						pDC->SelectObject(&redbrush);		// artifical wells should be coloured red.
						pDC->SetTextColor(RGB(200, 0, 0));
					}
					else
					{
						pDC->SelectStockObject(BLACK_BRUSH);
						pDC->SetTextColor(RGB(0, 0, 0));
					}

					//draw well marker and label
					pDC->TextOut(lx, ly, labeltext[i], strlen(labeltext[i]));
					pDC->Ellipse(lx-dotsize/2, ly-dotsize/2, lx+dotsize/2, ly+dotsize/2);
				}
			}
		}
	}

	pDC->SelectObject(pOldFont);	
	pDC->SetTextColor(oldtextcol);
	userlabelfont.DeleteObject();
}

void CContourPlot::DrawPlot (Plot *pp, Data *dp)
{
	//TRACE("\n\na new contour plot");

	pp->xu = (pp->x1 - pp->x0) / ( 2 * dp->nx-2);
	pp->yu = (pp->y1 - pp->y0) / ( 2 * dp->ny-2);

	MainLoop (pp, dp);	/* all the interesting stuff */
	return;
}

/* MainLoop () draws the plot--for each level, it draws all contours by
calling DrawContour when appropriate. */
void CContourPlot::MainLoop (Plot *pp, Data *dp)
{
	Label l;			/* list of labels to be printed */
	Terrain t;			/* shape of data to be contoured */
	short i;                      /* counts columns */
	short j;                      /* counts rows */
	short k;                      /* counts levels */

	AllocTerrain (dp->nx, dp->ny, &t);

	CPen *thislevelpen;

	l.nlab = 0;
	Level *kp = pp->levels;	/* list of levels */

	pDC->SelectStockObject(BLACK_PEN);
	for (k = 0; k < pp->nz; k++)
	{
		if(!ColorShading)
		{
			// no colored pens if color shading; just use black pen then
			thislevelpen=new CPen(PS_SOLID, kp[k].linewidth, InterpolateHSV(kp[k].height, zmin, zmax));
			pDC->SelectObject(thislevelpen);
		}
		MapTerrain (k, pp, dp, &t);

		for (j = 0; j < dp->ny; j++) /* scan entire data */
		{
			for (i = 0; i < dp->nx; i++)
			{
				if (t.vert[j][i] == LEFT)
					DrawContour (Vertical, North, i, j, k, &l, pp, dp, &t);
				if (t.hori[j][i] == UP)
					DrawContour (Horizontal, East, i, j, k, &l, pp, dp, &t);
			}
		}

		if(!ColorShading)
		{
			pDC->SelectStockObject(BLACK_PEN);
			thislevelpen->DeleteObject();
			delete thislevelpen;
		}
	}

	FreeTerrain (dp->nx, dp->ny, &t);

	if(labelling & CNT_AUTOLABEL )
		DrawLabels (pp->type_size, &l, pp->levels);

	return;
}

// DrawLabels () writes all the automatic labels (so far) in the plot.
void CContourPlot::DrawLabels (double type_size, Label *lp, Level *kp)
{
	short n;			/* number of label */
	short k;			/* number of level */
	char buf[256];
	int lx, ly;
	int dx, dy;
	double alpha, beta, l;
	CFont labelfont;

	pDC->SelectStockObject(BLACK_BRUSH);
	pDC->SetBkColor(RGB(255, 255, 255));

	// createpointfont takes the size in 10ths of a point size
	// Set up the font that we are going to use for labelling. We set it up properly
	// below, this is just to get the correct brush height. We delete it below and create
	// the correctly orientated font.
	labelfont.CreatePointFont((int)type_size*10, "Arial");
	CFont *pOldFont=pDC->SelectObject(&labelfont);
	UINT oldAlign=pDC->SetTextAlign(TA_LEFT | TA_BASELINE);
#ifdef VELMAN_UNIX
	// Under unix, horizontal labels are still drawn the correct way with opaque background
	// ie. a white square box. We have to turn this off since all the other labels will
	// have our custom label box.
	UINT oldMode=pDC->SetBkMode(TRANSPARENT);
#else
	//UINT oldMode=pDC->SetBkMode(OPAQUE);
	UINT oldMode=pDC->SetBkMode(TRANSPARENT);
#endif

	CSize textsize;
	textsize=pDC->GetTextExtent("1234567890", 10);
#ifdef VELMAN_UNIX
	//  Opaque text under unix does not work when the labels are at an angle. It's ok for horizontal
	//  ones. Therefore, instead, put a white box behind every label to work around the problem
	//  (which is essentially a mainwin problem).

	//  Originally the code used the short form of the CPen contructor. However, by default this
	//  gives round ends which extend beyond the text of the label. This bug was never seen
	//  because of the incorrect size given to create a brush. To remove the rounded ends we
	//  need to use the long form of the constructor. Note that in this mode, all the lines need
	//  to be drawn as paths as this is the only way the FLAT endcaps are supported under win98.
	//  See Petzold, p1084-1086 for more details.
	//LOGBRUSH    labsbrush;
	//labsbrush.lbColor = RGB(255, 255, 255);
	//labsbrush.lbStyle = BS_SOLID;
	//labsbrush.lbHatch = 0;
	//CPen thickpen(PS_SOLID|PS_GEOMETRIC|PS_ENDCAP_FLAT|PS_JOIN_MITER, textsize.cy, &labsbrush, 0, NULL);
	//
	//  6/6/00. Unfortunately due to bugs in MainWin the above code with the code below
	//  to draw the label box as a path, doesn't work. All we get is a single pixel wide line.
	//  Although the code works fine on the PC. Since we can't use 'paths' we have to use the
	//  simple form of the CPen constructor and put up with the rounded ends. What a pain!!
	CPen    thickpen(PS_SOLID, textsize.cy, RGB(255, 255, 255) );

	pDC->SelectObject(&thickpen);
#endif
	//  CreatePen takes a size in logical units and not point size. The
	//  following converts between the two. This was not in the original code so the type size
	//  was actually incorrect. Note the - infront of MulDiv is important!!
	int labelsize = -MulDiv((int)type_size, pDC->GetDeviceCaps(LOGPIXELSY), 72);

	//  These adjustment variables are used to shrink the label box under Unix to compensate
	//  for having the extra rounded ends on the box. See comments above for why we need to do this.
#ifdef VELMAN_UNIX
	int adjustx, adjusty;
	double  adjustment;
#endif
	for (n = 0; n < lp->nlab; n++)
	{
		// label string to output

		k = lp->klab[n];

		sprintf(buf, " %.1f ", kp[k].height);

		// the correctly rotated font

		labelfont.DeleteObject();
		labelfont.CreateFont((int)labelsize, 0, lp->escapement[n], 0, FW_REGULAR, FALSE, FALSE, 0, DEFAULT_CHARSET,
			OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
			DEFAULT_PITCH | FF_SWISS, "Arial");
		pDC->SelectObject(&labelfont);
		alpha=3.1415926/180.0*0.1*lp->escapement[n];
		textsize=pDC->GetTextExtent(buf, strlen(buf));

#ifdef VELMAN_UNIX
		// strange font problem under Unix: it also alignes text differently
		lx=(int)(lp->xlab[n]);
		ly=(int)(lp->ylab[n]);
		dx=(int)(textsize.cx*cos(alpha));
		dy=(int)(textsize.cx*sin(alpha));
		// Had to comment out the path commands since drawing the box as a path doesn't
		// work on Unix because of MainWin bugs, although it works fine on the PC. See
		// comments above. If we ever get a more recent MainWin then this code could be
		// reinstated as it looks better than the rounded end label boxes.
		//pDC->BeginPath();       // Must define label box as a path to get right appearance
		adjustment = 0.3;   // used to shrink label box slightly. See comments above.
		adjustx = (int) (adjustment*textsize.cx*cos(alpha));
		adjusty = (int) (adjustment*textsize.cx*sin(alpha));
		pDC->MoveTo(lx+adjustx, ly-adjusty);
		pDC->LineTo(lx+(dx-adjustx), ly-(dy-adjusty));
		//pDC->EndPath();
		//pDC->StrokePath();
#else
		// starting position. Modify x and y so that label is across contour line
		beta=atan2((double)(textsize.cy), (double)(textsize.cx));
		l=0.5*sqrt((double)(textsize.cx*textsize.cx+textsize.cy*textsize.cy));
		dx=(int)(cos(alpha+beta)*l);
		dy=(int)(sin(alpha+beta)*l);
		lx=(int)(lp->xlab[n])-dx;
		ly=(int)(lp->ylab[n])+dy;
#endif
		pDC->TextOut(lx, ly, buf, strlen(buf));
		pDC->SelectStockObject(SYSTEM_FONT); // so we can delete the font next time round
	}
	lp->nlab = 0;
	pDC->SelectObject(pOldFont);
	pDC->SetTextAlign(oldAlign);
	pDC->SetBkMode(oldMode);
	pDC->SelectStockObject(BLACK_PEN);
	return;
}

// DrawContour () draws a single contour curve.
void CContourPlot::DrawContour (Attitude start, Direction direction, short i, short j, short
								k, Label *lp, Plot *pp, Data *dp, Terrain *tp)
{
	BOOL gradient;		/* saves gradient of vert or hori (i, j) */
	BOOL is_next;                 /* GetNext () says contour continues */
	BOOL v = start == Vertical;	/* for shorter notation */
	Level *kp = pp->levels;	/* list of levels */
	State s;                      /* state of path search */
	double xu = pp->xu;            /* half real width of cell */
	double xw = 2 * xu;            /* real width of cell */
	double yu = pp->yu;            /* half real height of cell */
	double yw = 2 * yu;            /* real height of cell */
	short npts = 0;               /* number of points on current path */
	short nx = dp->nx;            /* number of columns of data */
	short ny = dp->ny;            /* number of rows of data */
	double Sx, Sy;										// start position of this contour
	double currposX, currposY, nextposX, nextposY; // current and next position of this stroke
	double prevposX, prevposY;

	double lastlabelX = 0, lastlabelY = 0;
	BOOL HaveLabel; // TRUE if there already was a label, then (lastlabel x, y) stores pos
	double esc, dist;
	int currentStroke;

	s.to_label = kp[k].label;
	gradient = v ? tp->vert[j][i] : tp->hori[j][i];

	s.attitude = start;		/* preparations */
	s.direction = direction;
	s.ii = i;
	s.jj = j;

	// starting position
	Sx=((i-0.5) * xw + (v ? tp->vx[j][i] : xu));
	Sy=((j-0.5) * yw +(v ? yu : tp->hy[j][i]));
	currposX=Sx;
	currposY=Sy;
	prevposX=Sx;

	prevposY=Sy;

	pDC->MoveTo((int)Sx, (int)Sy);

	if (v)
		tp->vert[j][i] = FALSE;
	else
		tp->hori[j][i] = FALSE;

	is_next = GetNext (nx, ny, tp, &s);
	currentStroke=0;  // count number of strokes
	HaveLabel=FALSE; // no label yet

	//TRACE("\nheight %f, label:%d", kp[k].height, s.to_label);

	while (is_next)
	{
		if (s.attitude)		/* Vertical */
		{
			nextposX=((s.ii-0.5) * xw + tp->vx[s.jj][s.ii]);
			nextposY=((s.jj-0.5) *yw + yu);
		}
		else
		{
			nextposX=((s.ii-0.5) * xw + xu);
			nextposY=((s.jj-0.5) * yw + tp->hy[s.jj][s.ii]);
		}
		pDC->LineTo((int)nextposX, (int)nextposY);
		if (s.attitude)		/* Vertical */
			tp->vert[s.jj][s.ii] = FALSE;
		else
			tp->hori[s.jj][s.ii] = FALSE;
		npts++;
		if(HaveLabel)
		{
			// if there already was a label, maybe we are far enough away to draw a new one
			dist=(nextposX-lastlabelX)*(nextposX-lastlabelX)+(nextposY-lastlabelY)*(nextposY-lastlabelY);
			if(dist>10000*EveryNthLabel)
				s.to_label=TRUE; // time for another label
		}
		// draw a label if
		//  - the flag is set
		//  - we are not too close to any of the borders
		//  - the current gradient is not local, i.e. we are on a rather long stroke, not in
		//    a sharp bend

		if (s.to_label &&
			s.ii * xw > 20 && (nx - s.ii) * xw > 20 &&
			s.jj * yw > 10 && (ny - s.jj) * yw > 10 &&
			(nextposX-currposX)*(nextposX-currposX)+
			(nextposY-currposY)*(nextposY-currposY) > 0.0)
		{
			lp->klab[lp->nlab] = k;
			lp->xlab[lp->nlab] = nextposX;
			lp->ylab[lp->nlab] = nextposY;
			lastlabelX=nextposX;
			lastlabelY=nextposY;
			// angle at which to draw contour label
			if(nextposX==currposX)
			{
				// vertical slope or maybe ill-defined slope
				if(nextposY==currposY)
					esc=0.0;  // ill-defined; use horizontal text
				else
					esc=90.0; // truly vertical slope
			}
			else
			{
				// find slope
				esc=-(nextposY-currposY)/(nextposX-currposX);
				// find angle in degrees
				esc=180.0/3.1415926*atan((esc));
			}
			lp->escapement[lp->nlab]=(int)(10*esc);
			if(labelling & CNT_AUTOLABEL) lp->nlab++; // just increment if we are labelling
			HaveLabel=TRUE;
			s.to_label = ! s.to_label;

			if (lp->nlab >= MAX_LAB && (labelling & CNT_AUTOLABEL) )
				DrawLabels (pp->type_size, lp, kp);
		}
		// if we run outside the plot (by hitting any of the borders) re-enable
		// labelling because to the user the continued line will look like a new
		// contour
		if ((s.ii == nx && s.attitude == Vertical   && s.direction == North) ||
			(s.jj == ny && s.attitude == Horizontal && s.direction == West)  ||
			(s.ii == 0  && s.attitude == Vertical   && s.direction == South) ||
			(s.jj == 0  && s.attitude == Horizontal && s.direction == East))
			s.to_label = kp[k].label;

		prevposX=currposX;
		prevposY=currposY;
		currposX=nextposX;
		currposY=nextposY;
		is_next = GetNext (nx, ny, tp, &s);
		currentStroke++;
	} // end while()

	pDC->LineTo((int)Sx, (int)Sy);			 // close each contour by drawing back to first pt
	if (v)
		tp->vert[j][i] = FALSE;
	else
		tp->hori[j][i] = FALSE;

	return;
}

// Returns a colour based on the allowed number of colours from the velman custom
// colour palette (see CVelmanPalette).
COLORREF CContourPlot::InterpolateHSV(double val, double min, double max)
{
	int             index;
	double          max_ncolours = velpal->GetMaxFillColours();

	if ( min >= max || val<min || val>max )
		return(PALETTEINDEX(0));
	if ( max-min < smallest_maxmin )        // if range is too small, don't bother. round off will
		return(PALETTEINDEX(0));    // mess up the interpolation below.

	index = (int) floor( ((val-min)/(max-min))*(max_ncolours-1) );  // ranges from 0 to max_ncolours-1
	if ( index > max_ncolours-1 )
		index = (int)max_ncolours - 1;
	return(PALETTEINDEX(index));
}

// Draws legend and colour key on contour plot.
void CContourPlot::DrawContourKey(Plot *pp)
{
	// Modified to improve the colour key, now use blocks of colour for min & max values.
	// Modified to allow for when zmax = or almost = zmin. In which case we do no
	// colour fill at all.

	char buf[256];
	int xblocksize = KeyTypeSize/15;	// keytypesize is 10*textpointsize.
	int yblocksize = 2*xblocksize;
	int maxcolour, n, ssize;
	CPoint curpos;
	CFont keyfont, smallfont, *poldfont;

	UINT oldalign=pDC->SetTextAlign(TA_UPDATECP);
	int oldbk=pDC->SetBkMode(TRANSPARENT);

	keyfont.CreatePointFont(KeyTypeSize, "Arial");
	ssize=80;		// small font size for colourbar labels (specified in 10ths of a point size)
	smallfont.CreatePointFont(ssize, "Arial");

	//	If contour range is too small don't plot a colour key at all
	if ( zmax-zmin < smallest_maxmin )
		return;

	//	draw colour bar
	pDC->MoveTo(keyx, keyy);
	maxcolour = velpal->GetMaxFillColours();
	curpos = pDC->GetCurrentPosition();
	for ( n = 0; n < maxcolour; n++ )
	{
		pDC->FillSolidRect(curpos.x, curpos.y, xblocksize, yblocksize, velpal->GetColour(n));
		pDC->MoveTo(curpos.x+xblocksize, curpos.y);
		curpos = pDC->GetCurrentPosition();
	}

	// draw labels underneath; interpolate across the colour bar and position a black
	// line and text in the correct place for each contour level
	int barlen = xblocksize*maxcolour;
	double lx;
	int   g10, startx, endx;
	poldfont = pDC->SelectObject(&smallfont);
	for ( n = 0; n < pp->nz; n++ )
	{
		lx = (pp->levels[n].height - zmin) / (zmax-zmin);
		pDC->FillSolidRect(keyx+int(lx*barlen), keyy, 1, yblocksize, RGB(0, 0, 0));
	}

	endx = 0; startx = 0;
	for ( n = 0; n < pp->nz; n++ )
	{
		lx = (pp->levels[n].height - zmin) / (zmax-zmin);

		// draw labels
		double h = pp->levels[n].height;
		//make positive
		h = sqrt(h*h);
		h = log10(h);
		g10 = int(h);
		if ( g10 > 0 )
			sprintf(buf, "%.*f", 0, pp->levels[n].height );
		else
			sprintf(buf, "%.*f", abs(g10)+1, pp->levels[n].height );

		startx = keyx + int(lx*barlen)-((strlen(buf)-1)/2)*(ssize/10);
		pDC->MoveTo(startx, int(keyy+2.5*(ssize/10)));
		// only write out label if we have room and previous label isn't going to overlap
		if ( startx > endx )
		{
			pDC->TextOut(0, 0, buf, strlen(buf));
			endx = startx + strlen(buf)*ssize/10;
		}
	}

	// black frame around the colour bar
	pDC->SelectStockObject(BLACK_PEN);
	pDC->MoveTo(keyx, keyy);
	pDC->LineTo(keyx+barlen, keyy);
	pDC->LineTo(keyx+barlen, keyy+yblocksize);
	pDC->LineTo(keyx, keyy+yblocksize);
	pDC->LineTo(keyx, keyy);

	//  plot some legend information
	pDC->SelectObject(&keyfont);
	if ( pp->nz > 1 )
	{
		pDC->MoveTo(keyx, keyy+40);
		sprintf(buf, "Min. value is %5.3f, max. value is %5.3f, contour interval is %5.3f",
			zmin, zmax, (zmax-zmin)/(pp->nz-1) );
		pDC->TextOut(0, 0, buf, strlen(buf));
	}

	pDC->SetTextAlign(oldalign);
	pDC->SetBkMode(oldbk);

	pDC->SelectObject(poldfont);
	keyfont.DeleteObject();
	smallfont.DeleteObject();
}

void CContourPlot::ForceContSetToClose(void)
{
	if (((CVelmanApp*) AfxGetApp())->OpendContSettings==TRUE)
	{
		((CVelmanApp*) AfxGetApp())->OpendContSettings=FALSE;
		setDlg.sQuit();
	}
}