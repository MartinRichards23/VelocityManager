
#include "NRUTIL.h"

double dbrent(double, double, double, double (*)(double), double (*)(double), double, double *, int *);
void mnbrak(double *, double *, double *, double *, double *, double *, double (*)(double));
double f1dim(double);
double df1dim(double);

#define TOL 2.0e-4

int ncom=0;	/* defining declarations */
double *pcom=0, *xicom=0, (*nrfunc)( double* );
void (*nrdfun)( double*, double* );

int dlinmin( double* p, double* xi, int n, double* fret, double (*func)( double* ), void (*dfunc)( double*, double* ))
{
	int j;
	double xx, xmin, fx, fb, fa, bx, ax;
	int result;

	ncom=n;
	pcom=dvector(1, n);
	xicom=dvector(1, n);
	nrfunc=func;
	nrdfun=dfunc;
	for (j=1;j<=n;j++)
	{
		pcom[j]=p[j];
		xicom[j]=xi[j];
	}
	ax=0.0;
	xx=1.0;
	bx=2.0;
	mnbrak(&ax, &xx, &bx, &fa, &fx, &fb, f1dim);
	*fret=dbrent(ax, xx, bx, f1dim, df1dim, TOL, &xmin, &result);
	for (j=1;j<=n;j++)
	{
		xi[j] *= xmin;
		p[j] += xi[j];
	}
	free_dvector(xicom, 1, n);
	free_dvector(pcom, 1, n);
	return result;
}

#undef TOL
