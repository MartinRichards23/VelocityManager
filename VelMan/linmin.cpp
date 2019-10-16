#include <math.h>
#include "linmin.h"
#include "NRUTIL.h"

#define M_PI 3.14159265358979323846

#undef SIGN
#define SIGN(z) ( (z) > 0.0 ? 1 : -1 )
#define SQR(z) ((z)*(z))

extern int dfpmin( double* p, int n, double ftol, int* iter, double* fret,
				  double (*func)( double* ), void ( *dfunc)( double*, double* ));

// global variables
int Gm;
double *Gx, *Gy, *Gw;
double Gp;

int linmin(int m, double *x, double *y, double *w, double p, double *alpha)
		   // Input:
		   // x[1..m], y[1..m] - (x, y) coordinates
		   // w[1..m] - weights
		   // p - determines p-norm. MUST have p>1.
		   // Output:
		   // alpha[1..2]
		   //
{

	int iter;
	double fret, ftol = 1.0e-11;

	//
	// set globals
	Gm = m;
	Gx = x; Gy = y; Gw = w;
	Gp = p;

	return dfpmin(alpha, 2, ftol, &iter, &fret, fp_lin, dfp_lin);

}

/*****************************************************************/

double fp_lin(double *alpha)
// alpha[1..2]
{
	int k;
	double s = 0.0, ctheta = cos(alpha[1]), stheta = sin(alpha[1]);

	double t = alpha[1]/(2.0*M_PI);
	double treduced;

	if ( (int) t < 0 )
		treduced = t + 1.0 - (int) t;
	else
		treduced = t - (int) t;

	alpha[1]=2.0*M_PI*treduced;

	for(k=1; k <= Gm; k++)
		s += Gw[k]*pow( fabs(Gx[k]*ctheta + Gy[k]*stheta - alpha[2]), Gp );

	return s;
}

/*****************************************************************/

void dfp_lin(double *alpha, double *g)
// alpha[1..2], g[1..2]
{
	int k;
	double D1 = 0.0, D2 = 0.0, temp, temp1;
	double  ctheta = cos(alpha[1]), stheta = sin(alpha[1]);

	for(k=1; k <= Gm; k++)
	{
		temp = Gx[k]*ctheta + Gy[k]*stheta - alpha[2];
		temp1 = Gw[k]*pow( fabs(temp), Gp-1.0 )*SIGN(temp);
		D1 += temp1*( -Gx[k]*stheta + Gy[k]*ctheta );
		D2 += temp1;
	}

	g[1] =  Gp*D1;
	g[2] = -Gp*D2;
}