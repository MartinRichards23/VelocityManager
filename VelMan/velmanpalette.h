// VelmanPalette.h: interface for the CVelmanPalette class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VELMANPALETTE_H__6711B6E0_2B71_11D4_A4D4_002018723DE7__INCLUDED_)
#define AFX_VELMANPALETTE_H__6711B6E0_2B71_11D4_A4D4_002018723DE7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MAX_FILL_COLOURS    100
#define DEF_START_HUE       355.0
#define DEF_END_HUE         255.0
#define DEF_SATURATION      0.9
#define DEF_START_VALUE     1.0
#define DEF_END_VALUE       0.0

class CVelmanPalette : public CPalette
{
public:
	CVelmanPalette();
	void CreateVelPalette();
	void CreateVelPalette( double start_val, double end_val );
	void CreateVelPalette( double starthue, double endhue, double sat, double val );
	COLORREF GetColour(int index);
	virtual ~CVelmanPalette();

	int GetMaxFillColours() { return MAX_FILL_COLOURS; }

private:

	//  member functions
	void MakeHuePalette(LOGPALETTE *lp);
	void MakeGreyscalePalette(LOGPALETTE *lp);

	double start_hue, end_hue;  // start and end hues for colour palette
	double saturation;          // saturation never changes
	double start_value, end_value;  // start and end value for greyscale (start_value used for hue colour wheel)

};

#endif // !defined(AFX_VELMANPALETTE_H__6711B6E0_2B71_11D4_A4D4_002018723DE7__INCLUDED_)
