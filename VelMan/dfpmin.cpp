#include "stdafx.h"

#include <stdio.h>
#include <math.h>

#include "NRUTIL.h"

#include "dlinmin.h"

#define ITMAX 1000
#define EPS 1.0e-10

double vnorm(double *vv, int n)
{
	int k;
	double s = 0.0;

	for(k=1; k<=n; k++)
		s += vv[k] * vv[k];

	return sqrt(s);
}

BOOL dfpmin( double* p, int n, double ftol, int* iter, double* fret, double (*func)( double* ), void ( *dfunc)( double*, double* ))
{
	int j, i, its;
	double fp, fae, fad, fac;
	double *xi, *g, *dg, *hdg; //, *dvector();
	double **hessin; //, **dmatrix();
	BOOL result;

	double vnorm(double *, int);

	hessin=dmatrix(1, n, 1, n);
	xi=dvector(1, n);
	g=dvector(1, n);
	dg=dvector(1, n);
	hdg=dvector(1, n);
	fp=(*func)(p);
	(*dfunc)(p, g);
	for (i=1;i<=n;i++)
	{
		for (j=1;j<=n;j++) hessin[i][j]=0.0;
		hessin[i][i]=1.0;
		xi[i] = -g[i];
	}
	for (its=1;its<=ITMAX;its++)
	{
		*iter=its;
		result=dlinmin(p, xi, n, fret, func, dfunc);
		if (2.0*fabs(*fret-fp) <= ftol*(fabs(*fret)+fabs(fp)+EPS))
		{
			// if (vnorm(g, n) <= ftol) {
			free_dvector(hdg, 1, n);
			free_dvector(dg, 1, n);
			free_dvector(g, 1, n);
			free_dvector(xi, 1, n);
			free_dmatrix(hessin, 1, n, 1, n);
			return result;
		}
		fp=(*fret);
		for (i=1;i<=n;i++) dg[i]=g[i];
		*fret=(*func)(p);
		(*dfunc)(p, g);
		/****************** Debug writes **************/
		//          TRACE("Iteration = %d, x = [%g  %g], f = %g\n", its, p[1], p[2], fp );
		/****************** end ***********************/

		for (i=1;i<=n;i++) dg[i]=g[i]-dg[i];
		for (i=1;i<=n;i++)
		{
			hdg[i]=0.0;
			for (j=1;j<=n;j++) hdg[i] += hessin[i][j]*dg[j];
		}
		fac=fae=0.0;
		for (i=1;i<=n;i++)
		{
			fac += dg[i]*xi[i];
			fae += dg[i]*hdg[i];
		}
		fac=1.0/fac;
		fad=1.0/fae;
		for (i=1;i<=n;i++) dg[i]=fac*xi[i]-fad*hdg[i];
		for (i=1;i<=n;i++)
			for (j=1;j<=n;j++)
				hessin[i][j] += fac*xi[i]*xi[j]
			-fad*hdg[i]*hdg[j]+fae*dg[i]*dg[j];
			for (i=1;i<=n;i++)
			{
				xi[i]=0.0;
				for (j=1;j<=n;j++) xi[i] -= hessin[i][j]*g[j];
			}
	}
	return FALSE; // nrerror("Too many iterations in DFPMIN");
}

#undef ITMAX
#undef EPS
