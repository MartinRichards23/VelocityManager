//	ColourModel Class: Supports conversion from RGB to various colour models (HSV/HLS) and back.

#include <math.h>
#include "stdafx.h"     // for 'ASSERT', BOOL type
#include "colourmodel.h"

#define EPS 10e-6
#define zero_val(x) (-EPS < (x) && (x) < EPS)

//	Inputs HSV and returns corresponding RGB values.
//	h is a hue value in the range 0-360.0.
//	v is 'value' and has a range 0-1.0.
//  s is the saturation and has a range 0-1.0.
//	r, g, b will be returned in the range 0-255.0
//
//	See Foley and van Dam, "Fundamentals of Interactive Computer Graphics".
void CColourModel::HSVtoRGB(double h, double s, double v, int &r, int &g, int &b)
{
	double	red = 0, green = 0, blue = 0;
	double	f = 0, p = 0, q = 0, t = 0;
	int     i = 0;

	//	check range of input variables.
	ASSERT( h < 360.0 );
	ASSERT( 0.0 <= s && s <= 1.0 );
	ASSERT( s <= v && v <= 1.0 );
	h = fmod( h, 360.0 );
	if ( h < 0 )
		h = h + 360.0;
	if ( s < 0.0 )
		s = 0.0;
	if ( s > 1.0 )
		s = 1.0;
	if ( v < s )
		v = s;
	if ( v > 1.0 )
		v = 1.0;

	// Achromatic case. RGB value is a grey equal to v so h is not defined & we ignore it.
	if (zero_val(s) )
	{
		red   = v;
		green = v;
		blue  = v;
	}
	else
	{
		// Chromatic case
		if ( zero_val(h-360.0) )
			h = 0.0;
		h /= 60.;
		i = (int) floor(h);
		f = h - i;
		p = v*(1 - s);
		q = v*(1 - s*f);
		t = v*(1 - s*(1 - f));
		switch (i)
		{
		case 0:
			red   = v;
			green = t;
			blue  = p;
			break;

		case 1:
			red   = q;
			green = v;
			blue  = p;
			break;

		case 2:
			red   = p;
			green = v;
			blue  = t;
			break;

		case 3:
			red   = p;
			green = q;
			blue  = v;
			break;

		case 4:
			red   = t;
			green = p;
			blue  = v;
			break;

		case 5:
			red   = v;
			green = p;
			blue  = q;
			break;
		}
	}

	// Convert to r, g, b values for Windows
	// The 0.001 provides just a little "boost" for those values that
	// are just about the next whole number so that they get truncated
	// to int's properly (e.g. 1.9995 normally becomes 1, but the correct
	// answer in this implementation is 2, so 1.9995 + 0.001 = 2.0005, or 2
	// when represented as an int). If a number is something like 5.0032 adding
	// 0.001 wont have any effect.
	r = (int) (red*255.   + 0.001);
	g = (int) (green*255. + 0.001);
	b = (int) (blue*255.  + 0.001);
}

void CColourModel::RGBtoHSV(int r, int g, int b, double &h, double &s, double &v )
//
//	Inputs RGB and returns corresponding HSV.
//	r, g, b are each in the range 0-255.
//	h will be returned in the rang 0-360.0
//	s and v will be returned in the range 0-1, except if s = 0, then h will be returned < 0.0
//
//	See Foley and van Dam, "Fundamentals of Interactive Computer Graphics" for algorithm.
//
{
	double	red, green, blue;
	double	maxColor, minColor;
	double	delta;

	//	Check range of input variables
	ASSERT( 0 <= r && r <= 255 );
	ASSERT( 0 <= b && b <= 255 );
	ASSERT( 0 <= g && g <= 255 );
	if ( r < 0 )
		r = 0;
	if ( r > 255 )
		r = 255;
	if ( g < 0 )
		g = 0;
	if ( g > 255 )
		g = 255;
	if ( b < 0 )
		b = 0;
	if ( b > 255 )
		b = 255;

	// Convert from Windows color numbers to normalized values [0, 1]
	red   = r / 255.0;
	green = g / 255.0;
	blue  = b / 255.0;

	//	Compute value
	maxColor = max(max(red, green), blue);
	minColor = min(min(red, green), blue);

	v = maxColor; // This is the Value (Brightness)

	//	Calculate saturation
	if ( zero_val(maxColor) )
		s = 0.0;
	else
		s = (maxColor - minColor)/maxColor;

	//	Compute hue
	if (zero_val(s) )
	{
		// Achromatic case
		h = -1; 	// UNDEFINED
	}
	else
	{
		// Chromatic case
		delta = maxColor - minColor;
		if ( zero_val(red - maxColor ) )
			h = (green - blue)/delta;		// Resulting color is between yellow and magenta
		else if ( zero_val(green - maxColor) )
			h = 2 + (blue - red)/delta;		// Resulting color is between cyan and yellow
		else if( zero_val(blue - maxColor) )
			h = 4 + (red - green)/delta;	// Resulting color is between magenta and cyan

		h = h*60.;	// convert to degrees

		// make sure hue is nonnegative
		if (h < 0)
			h = h+360;
	}
}

BOOL CColourModel::TestHSV()
{
	BOOL	success = TRUE; // assume successful
	int		i, j, k;
	int		rIn, gIn, bIn;
	int		rOut, gOut, bOut;
	double	h, l, s;

	for (i = 0; i < 255; i++)
	{
		for (j = 0; j < 255; j++)
		{
			for (k = 0; k < 255; k++)
			{
				rIn = i;
				gIn = j;
				bIn = k;

				RGBtoHSV(rIn, gIn, bIn, h, l, s);
				HSVtoRGB(h, l, s, rOut, gOut, bOut);

				if (rIn != rOut || gIn != gOut || bIn != bOut)
				{
					success = FALSE;
					break;
				}
			}
			if (!success)
				break;
		}
		if (!success)
			break;
	}
	return success;
}

// Converts hls to rgb
// h in [0, 360) (or less than zero = UNDEFINED)
// l and s in [0, 1]
// r, g, b each in [0, 255]
void CColourModel::HLStoRGB( double h, double l, double s, int &r, int &g, int &b )
{
	double	red = 0, green = 0, blue = 0;
	double	m1 = 0, m2 = 0;

	//	Check range of input variables
	ASSERT( h < 360.0 );
	ASSERT( 0.0 <= l && l <= 1.0 );
	ASSERT( 0.0 <= s && s <= 1.0 );
	h = fmod( h, 360.0 );
	if ( h < 0 )
		h = h + 360.0;
	if ( l < 0.0 )
		l = 0.0;
	if ( l > 1.0 )
		l = 1.0;
	if ( s < 0.0 )
		s = 0.0;
	if ( s > 1.0 )
		s = 1.0;

	//  Compute r g b values.
	if (l <= 0.5)
		m2 = l*(1 + s);
	else
		m2 = l + s - l*s;

	m1 = 2*l - m2;

	if (zero_val(s))
	{
		// Achromatic : there is no hue
		if (h < 0) // UNDEFINED
		{
			red   = l;
			green = l;
			blue  = l;
		}	 // else => an error but we ignore it here.
	}
	else
	{
		// Chromatic case, so there is a hue
		red   = ComputeValue(m1, m2, h + 120.);
		green = ComputeValue(m1, m2, h);
		blue  = ComputeValue(m1, m2, h - 120.);
	}

	// Convert to r, g, b values for Windows
	// The 0.001 provides just a little "boost" for those values that
	// are just about the next whole number so that they get truncated
	// to int's properly (e.g. 1.9995 normally becomes 1, but the correct
	// answer in this implementation is 2, so 1.9995 + 0.001 = 2.0005, or 2
	// when represented as an int). If a number is something like 5.0032 adding
	// 0.001 wont have any effect.
	r = (int) (red*255. + 0.001);
	g = (int) (green*255. + 0.001);
	b = (int) (blue*255. + 0.001);
}

void CColourModel::RGBtoHLS( int r, int g, int b, double &h, double &l, double &s )
//
// Converts rgb to hls.
// r, g, b each in [0, 255]
// h in [0, 360)
// l and s in [0, 1], except if s = 0, then h = (less than zero - UNDEFINED)
{
	double	maxColor, minColor;
	double	delta;
	double	red, green, blue;

	//	Check input range
	ASSERT( 0 <= r && r <= 255 );
	ASSERT( 0 <= b && b <= 255 );
	ASSERT( 0 <= g && g <= 255 );
	if ( r < 0 )
		r = 0;
	if ( r > 255 )
		r = 255;
	if ( g < 0 )
		g = 0;
	if ( g > 255 )
		g = 255;
	if ( b < 0 )
		b = 0;
	if ( b > 255 )
		b = 255;

	// convert r, g, b to [0, 1]
	red   = r/255.;
	green = g/255.;
	blue  = b/255.;

	maxColor = max(max(red, green), blue);
	minColor = min(min(red, green), blue);

	// Compute lightness
	l = (maxColor + minColor)/2;

	// Compute saturation
	if (zero_val(maxColor - minColor))
	{
		// Achromatic case, because r=b=g
		s = 0.0;
		h = -1.0; // UNDEFINED
	}
	else
	{
		// Chromatic case
		// First calculate saturation
		if (l <= 0.5)
			s = (maxColor - minColor)/(maxColor + minColor);
		else
			s = (maxColor - minColor)/(2 - maxColor - minColor);

		// Now, calculate hue
		delta = maxColor - minColor;
		if (zero_val(red - maxColor))
			h = (green - blue)/delta; // resulting color is between yellow and magenta
		else if (zero_val(green - maxColor))
			h = 2 + (blue - red)/delta; // resulting color is between cyan and yellow
		else if (zero_val(blue - maxColor))
			h = 4 + (red - green)/delta; // resulting color is between magenta and cyan

		// convert to degrees
		h *= 60.;

		// Make degrees be nonnegative
		if (h < 0)
			h += 360.;
	}
}

double CColourModel::ComputeValue(double n1, double n2, double hue)
// Used by rgbhls
{
	double value;

	if (hue > 360.)
		hue -= 360.;
	else if (hue < 0.)
		hue += 360.;

	if (hue < 60.)
		value = n1 + (n2 - n1)*hue/60.;
	else if (hue < 180.)
		value = n2;
	else if (hue < 240.)
		value = n1 + (n2 - n1)*(240. - hue)/60.;
	else
		value = n1;

	return value;
}

BOOL CColourModel::TestHLS()
{
	BOOL success = TRUE; // assume successful
	int i, j, k;
	int rIn, gIn, bIn;
	int rOut, gOut, bOut;
	double h, l, s;

	for (i = 0; i < 255; i++)
	{
		for (j = 0; j < 255; j++)
		{
			for (k = 0; k < 255; k++)
			{
				rIn = i;
				gIn = j;
				bIn = k;

				RGBtoHLS(rIn, gIn, bIn, h, l, s);
				HLStoRGB(h, l, s, rOut, gOut, bOut);

				if (rIn != rOut || gIn != gOut || bIn != bOut)
				{
					success = FALSE;
					break;
				}
			}
			if (!success)
				break;
		}
		if (!success)
			break;
	}

	return success;
}
