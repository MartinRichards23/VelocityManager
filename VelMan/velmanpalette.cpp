// VelmanPalette.cpp: implementation of the CVelmanPalette class.
//
//  NOTE! This class follows the style of the windows palette class
//  on which it is based. The default constructor does NOT create
//  a palette, only setting space and default variable values.
//  Use the CreateVelPalette() functions to actually create the custom
//  palettes.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "velman.h"
#include "velmanpalette.h"
#include "colourmodel.h"
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// Default contructor for velman custom colour palette
// Default is for variation in hue according to default values.
CVelmanPalette::CVelmanPalette()
{
	start_hue  = DEF_START_HUE; // initialise class variables
	end_hue    = DEF_END_HUE;
	saturation = DEF_SATURATION;
	start_value = DEF_START_VALUE;
	end_value   = DEF_END_VALUE;
}

// Creates the default Velman palette; rainbow effect
void CVelmanPalette::CreateVelPalette()
{

	LOGPALETTE *lp = (LOGPALETTE *) calloc(1, sizeof(LOGPALETTE)+(MAX_FILL_COLOURS*sizeof(PALETTEENTRY)));

	MakeHuePalette(lp);
	CreatePalette(lp);

	free(lp);
}

//  Constructor for greyscale velman palette object.
//  We only set the start and end 'value', hue is ignored and saturation MUST be zero.
void CVelmanPalette::CreateVelPalette( double start_val, double end_val )
{
	LOGPALETTE *lp = (LOGPALETTE *) calloc(1, sizeof(LOGPALETTE)+(MAX_FILL_COLOURS*sizeof(PALETTEENTRY)));

	start_hue = 0.0;
	end_hue = 0.0;      // hue is irrelevent for greyscale colour palette
	saturation = 0.0;   // saturation must be zero
	start_value = start_val;
	end_value = end_val;

	MakeGreyscalePalette(lp);
	CreatePalette(lp);

	free(lp);
}

void CVelmanPalette::CreateVelPalette(double starthue, double endhue, double sat, double val )
{
	LOGPALETTE *lp = (LOGPALETTE *) calloc(1, sizeof(LOGPALETTE)+(MAX_FILL_COLOURS*sizeof(PALETTEENTRY)));

	//  Copy colour wheel values into class
	start_hue = starthue;
	end_hue = endhue;
	saturation = sat;
	start_value = val;
	end_value = val;

	MakeHuePalette(lp);
	CreatePalette(lp);

	free(lp);
}

CVelmanPalette::~CVelmanPalette()
{

}

void CVelmanPalette::MakeHuePalette(LOGPALETTE *lp)
{
	double hue, hue_incr;
	int i, red, green, blue;
	CColourModel    cmod;

	//  Construct custom colour palette for contour fill plots using HSV colour model
	//  (see Swanke p144 for code on which some of this is based)

	lp->palVersion = 0x300;     // means its windows v3 and above
	lp->palNumEntries = MAX_FILL_COLOURS;

	hue_incr = (fmod(end_hue-start_hue+719, 360) + 1 ) / (MAX_FILL_COLOURS-1.0);     // comes from Uplot
	hue = start_hue;

	for ( i = 0; i < MAX_FILL_COLOURS; i++ )
	{
		//  Convert HSV to RGB and enter into custom table
		cmod.HSVtoRGB(hue, saturation, start_value, red, green, blue);
		//TRACE( "InterpolateHSV: RGB %d %d %d\n", red, green, blue );

		lp->palPalEntry[i].peRed = red;
		lp->palPalEntry[i].peGreen = green;
		lp->palPalEntry[i].peBlue = blue;
		lp->palPalEntry[i].peFlags = NULL;

		hue = hue + hue_incr;
		if ( hue > 360.0 )
			hue = fmod(hue, 360.0);
	}
}

void CVelmanPalette::MakeGreyscalePalette(LOGPALETTE *lp)
{
	double val_incr, value;
	int i, red, green, blue;
	CColourModel    cmod;

	lp->palVersion = 0x300;
	lp->palNumEntries = MAX_FILL_COLOURS;

	val_incr = (end_value-start_value) / MAX_FILL_COLOURS;
	value = start_value;

	for ( i = 0; i < MAX_FILL_COLOURS; i++ )
	{
		cmod.HSVtoRGB(0.0, saturation, value, red, green, blue );
		lp->palPalEntry[i].peRed = red;
		lp->palPalEntry[i].peGreen = green;
		lp->palPalEntry[i].peBlue = blue;
		lp->palPalEntry[i].peFlags = NULL;

		value = value + val_incr;
		if ( value > 1.0 )
			value = 1.0;
		if ( value < 0.0 )  // val_incr may be negative!
			value = 0.0;
	}
}

//  Returns colour in current colour palette based on the index
//  into the range of colours.
//  NB. It would be easier in this function to use PALETTEENTRY(index)
//  instead of going to the trouble of pulling out the RGB values using
//  GetPaletteEntries. Unfortunately, I've found that some windows
//  objects don't work with the COLORREF index returned by PALETTEINDEX()
//  and only work with an RGB COLORREF. (e.g. CBrush doesn't work).
COLORREF CVelmanPalette::GetColour(int index)
{
	BYTE    red, green, blue;
	PALETTEENTRY *pe = (PALETTEENTRY *) calloc(1, sizeof(PALETTEENTRY));

	if ( index < 0 )
		index = 0;
	if ( index > MAX_FILL_COLOURS-1 )
		index = MAX_FILL_COLOURS - 1;
	GetPaletteEntries(index, 1, pe);
	red = pe->peRed;
	green = pe->peGreen;
	blue = pe->peBlue;
	return(PALETTERGB(red, green, blue));
}