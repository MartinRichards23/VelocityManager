//	ColourModel class.
//	Includes routines to convert between RGB and HSV and RGB with HLS.
//	Reference: Foley & van Dam, "Fundamentals of interactive computer graphics".

class	CColourModel
{
public:
	CColourModel() {}		// default (NULL) constructor

	//	Prototypes of member functions
	static void CColourModel::HSVtoRGB(double h, double s, double v, int &r, int &g, int &b);
	static void CColourModel::RGBtoHSV(int r, int g, int b, double &h, double &s, double &v );
	static void CColourModel::HLStoRGB( double h, double l, double s, int &r, int &g, int &b );
	static void CColourModel::RGBtoHLS( int r, int g, int b, double &h, double &l, double &s );
	static BOOL CColourModel::TestHSV();
	static BOOL CColourModel::TestHLS();

private:
	static double CColourModel::ComputeValue(double n1, double n2, double hue);

};
