// Function library. Contains various types of velocity functions
// and fitting routines

//#define WRITE_FITTED_VALUES_TO_FILE

#include "funclib.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include "stdafx.h"
#include "velman.h"
#include "numerics.h"
#include "linmin.h"
#include "nrutil.h"
#include "expfit.h"

#define MYABS(X) ( (X) > 0.0 ? (X) : -(X) )

BOOL LinearRegress(int weight, int n, double *z1, double *x, double *y, double *w, double *a, double *b,
									 double *chisq)
{
	double sxx=0.0, sy=0.0, sx=0.0, sxy=0.0, ss=0.0, sxoss;
	double wt, t, st2=0.0;
	double *dbl_w;
	double p;  // This specifies the p-norm; it should be greater than 1
	double sol_vec[2];
	int i, j, result;

	if(n==0)
		return FALSE;

	// case 1: Fitting in x AND y, arbitrary weighting
	if(((CVelmanApp *)AfxGetApp())->FitInXandY)
	{
		// check that at least two x positions are different. If they are not,
		// the line would be vertical, leading the fitting algorithm to fail
		// search maximum difference between x[0] and all other points into sxx
		sxx=-1.0;
		for(i=1;i<n;i++)
			sxx=max(sxx, fabs(x[i]-x[0]));
		// if it's too small give up
		if(x[0]==0 && sxx<1.0E-10 || sxx/x[0]<=1.0E-15)
			return FALSE;

		dbl_w=new double[n];
		if(w==NULL)
		{
			for(i=0;i<n;i++)
				dbl_w[i]=1.0;
		}
		else
		{
			for(i=0;i<n;i++)
				dbl_w[i]=w[i];
		}
		sol_vec[0]=0.0;
		sol_vec[1]=0.0;
		if(weight==0)
			p=1.1;	 // linear distance weighting
		else
			p=2.0;	 // usual quadratic distance weighting
#ifdef WRITE_FITTED_VALUES_TO_FILE
	FILE *ddd=fopen("c:\\fit.txt", "w");
	for(i=0;i<n;i++)
		{
			if(w==NULL)
				fprintf(ddd, "%f\t%f\t1.0\n", x[i], y[i]);
			else
				fprintf(ddd, "%15.8f\t%15.8f\n", x[i], y[i]);
		}
	fclose(ddd);
#endif
		result=linmin(n, x-1, y-1, dbl_w-1, p, sol_vec-1);

		// returns solution in a rather weird way. If the slope of the normal
		// vector is very small (sin.. ~ 0), we cannot do it because the fit is
		// almost vertical. Also if we ran out of iterations
		if(!result || fabs(sin(sol_vec[0]))<1.0E-15)
		{
			delete dbl_w;
			return FALSE;
		}

		*a=-(cos(sol_vec[0])/sin(sol_vec[0]));
		*b=sol_vec[1]/sin(sol_vec[0]);
		// compute avg percentage residual as usual
		*chisq=0.0;
		for(i=0, j=0;i<n;i++)
		{
			if(MYABS(y[i])>1.0E-8)
			{
				*chisq+=(fabs(y[i]-(*b)-(*a)*x[i])/fabs(y[i]))*sqrt(dbl_w[i]);
				j++;
			}
		}
		*chisq*=100.0;
		if(j>0)
			*chisq/=j;
		delete dbl_w;
		return TRUE;
	}

	// case 2: Fit in y only (standard)
	if(weight==0)
	{
		//------------ absolute distance ("l1") fitting
		double xtol = 1.0e-14; //This parameter measures the relative error in the parameters
		p = 1.1;  // This specifies the p-norm; it should be greater than 1
		double *dbl_x, *dbl_y;
		long int num=(long)n;
		long int dimension=2;

		dbl_x=new double[n];
		dbl_y=new double[n];
		dbl_w=new double[n];
		for(i=0;i<n;i++)
		{
			dbl_x[i]=x[i];
			dbl_y[i]=y[i];
		}
		if(w==NULL)
		{
			for(i=0;i<n;i++)
				dbl_w[i]=1.0;
		}
		else
		{
			for(i=0;i<n;i++)
				dbl_w[i]=w[i];
		}
		sol_vec[0]=0.0;
		sol_vec[1]=0.0;

#ifdef WRITE_FITTED_VALUES_TO_FILE
	FILE *ddd=fopen("c:\\fit.txt", "w");
	fprintf(ddd, "x\ty\twgt\n");
	for(i=0;i<num;i++)
		fprintf(ddd, "%f\t%f\t%f\n", x[i], y[i], dbl_w[i]);
	fclose(ddd);
#endif

		lm_min(fcn_l1, num, dimension, sol_vec, xtol, dbl_x, dbl_y, dbl_w, p);
		*a=sol_vec[0];
		*b=sol_vec[1];
		delete dbl_x;
		delete dbl_y;
		delete dbl_w;
		// compute goodness of fit=average of percental residuals (apr)
		// with apr(i)= 100*|y(i)-func(i)|/|y(i)|
		*chisq=0.0;
		if(w==NULL)
		{
			for(i=0;i<n;i++)
			{
				if(y[i]<1.0E-8)
					*chisq+=1.0;
				else
					*chisq+=fabs(y[i]-(*b)-(*a)*x[i])/fabs(y[i]);
			}
		}
		else
		{
			for(i=0;i<n;i++)
			{
				if(y[i]<1.0E-8)
					*chisq+=sqrt(w[i]);
				else
					*chisq+=(fabs(y[i]-(*b)-(*a)*x[i])/fabs(y[i]))*sqrt(w[i]);
			}
		}
		*chisq*=100.0;
		*chisq/=n;
		// if start values (0, 0) for a and b did not change at all, we have a fitting problem
		// and return FALSE.
		if(*a==0.0 && *b==0.0)
			return FALSE;
		else
			return TRUE;

	}
	else
	{
		//------------ least squares fitting
#ifdef WRITE_FITTED_VALUES_TO_FILE
	FILE *ddd=fopen("c:\\fit.txt", "w");
	fprintf(ddd, "x\ty\twgt\n");
	for(i=0;i<n;i++)
		{
			if(w==NULL)
				fprintf(ddd, "%f\t%f\t1.0\n", x[i], y[i]);
			else
				fprintf(ddd, "%f\t%f\t%f\n", x[i], y[i], sqrt(w[i]));
		}
	fclose(ddd);
#endif

		for(i=0;i<n;i++)
		{
			if(w==NULL)
				wt=1.0;
			else
				wt=sqrt((w[i]));
			ss+=wt;
			sx+=wt*x[i];
			sy+=wt*y[i];
		}
		sxoss=sx/ss;
		*a=0.0;
		for(i=0;i<n;i++)
		{
			if(w==NULL)
				wt=1.0;
			else
				wt=sqrt((w[i]));
			t=(x[i]-sxoss)*wt;
			st2+=t*t;
			*a+=t*y[i]*wt;
		}
		if(st2==0.0)
			return FALSE;
		else
			*a /= st2;
		*b=(sy-sx*(*a))/ss;

		// compute goodness of fit
		*chisq=0.0;
		j=0;
		if(w==NULL)
		{
			for(i=0;i<n;i++)
			{
				if(MYABS(y[i])>1.0E-8)
				{
					*chisq+=fabs(y[i]-(*b)-(*a)*x[i])/fabs(y[i]);
					j++;
				}
			}
		}
		else
		{
			for(i=0;i<n;i++)
			{
				if(MYABS(y[i])>1.0E-8)
				{
					*chisq+=(fabs(y[i]-(*b)-(*a)*x[i])/fabs(y[i]))*sqrt(w[i]);
					j++;
				}
			}
		}
		*chisq*=100.0;
		if(j>0)
			*chisq/=j;
		return TRUE;
	}
	return FALSE;	// if we get here, there was a mistake in the weight parameter
}

double LinearFunc(double x, double a, double b)
{
	return (a*x+b);
}

double InverseLinearFunc(double y, double a, double b)
{
	return ((y-b)/a);
}

double ExpFunc(double x, double a, double b)
{
	return ( (exp(x/a)-1.0)/b );
}

double FakeFunc(double x, double a, double b)
{
	return ( 0.0 );
}

BOOL ExpRegress(int weight, int n, double *z1, double *x, double *y, double *w, double *a, double *b, double *chisq)
{
	double t;
	int i, j;

	double xtol = 1.0e-14; //This parameter measures the relative error in the parameters
	// This specifies the p-norm; it should be greater than 1
	double p = (weight == 0 ? 1.1 : 2.0);
	double *dbl_w;
	long int num=(long)n;
	long int dimension=2;
	double sol_vec[2];
	BOOL result;

	dbl_w=new double[n];

	if(w==NULL)
	{
		for(i=0;i<n;i++)
			dbl_w[i]=1.0;
	}
	else
	{
		for(i=0;i<n;i++)
			dbl_w[i]=w[i];
	}

	sol_vec[0]=1000.0;	// init values of fitting params // 9/1/07. expfit sets initial values
	sol_vec[1]=1000.0;

#ifdef WRITE_FITTED_VALUES_TO_FILE
	FILE *ddd=fopen("c:\\fit.txt", "w");
	fprintf(ddd, "x\ty\twgt\n");
	for(i=0;i<num;i++)
		fprintf(ddd, "%f\t%f\t%f\n", x[i], y[i], dbl_w[i]);
	fclose(ddd);
#endif

	result=expfit(num, x-1, z1-1, y-1, dbl_w-1, p, sol_vec-1);
	delete dbl_w;
	if(!result)
		return FALSE;

	*a=MYABS(sol_vec[0]);
	*b=1.0/MYABS(sol_vec[1]);

	// compute goodness of fit
	*chisq=0.0;
	j=0;
	if(w==NULL)
	{
		for(i=0;i<n;i++)
		{
			if(MYABS(y[i])>1.0E-8)
			{
				t=(*a)*(log( (MYABS(*b)*y[i]+1.0) / (MYABS(*b)*z1[i] + 1.0) ));
				*chisq+=fabs(t-x[i])/fabs(x[i]);
				j++;
			}
		}
	}
	else
	{
		for(i=0;i<n;i++)
		{
			if(MYABS(y[i])>1.0E-8)
			{
				t=(*a)*(log( (MYABS(*b)*y[i]+1.0) / (MYABS(*b)*z1[i] + 1.0) ));
				*chisq+=(fabs(t-x[i])/fabs(x[i]))*sqrt(w[i]);
				j++;
			}
		}
	}
	*chisq*=100.0;
	if(j>0)
		*chisq/=j;
	// if start values for a and b did not change at all, we have a fitting problem
	// and return FALSE.
	if(*a==1.0 && *b==1.0)
		return FALSE;
	else
		return TRUE;
}

double InstVelPropDepth(double z1, double t1, double t2, double a, double b, double deltav)
{
	// depth conversion "Instantanous velocity ~ depth"
	double dt=(t2-t1), zz;
	zz = exp(dt/a)/b-1.0/b + z1*exp(dt/a) + deltav;
	if (zz < z1)
		zz = z1;
	return zz;
}

double IsoChorPropIsoChron(double z1, double t1, double t2, double a, double b, double deltav)
{
	// depth conversion "Isochore ~ Isochronochore"
	// note that "deltav" here is just a depth difference to be added
	double dt=(t2-t1), zz;
	zz =  z1 + a*dt + b + deltav;
	if (zz < z1)
		zz = z1;
	return zz;
}

double IntVelPropBDepth(double z1, double t1, double t2, double a, double b, double deltav)
{
	// depth conversion "Interval velocity ~ bottom depth"
	double tInt=(t2-t1)/2000.0;	// one-way interval time in sec
	if((1.0-a*tInt)<=1.0E-10)
		return(-1.0E30);
	else
		return( (z1+b*tInt+deltav*tInt)/(1.0-a*tInt) );
}

double IntVelPropIsochor(double z1, double t1, double t2, double a, double b, double deltav)
{
	// depth conversion "Interval velocity ~ isochore"
	double tInt=(t2-t1)/2000.0;	// one-way interval time in sec
	if((1.0-a*tInt)<=1.0E-10)
		return(-1.0E30);
	else
		return( (z1*(1-a*tInt)+b*tInt+deltav*tInt)/(1.0-a*tInt) );
}

double IntVelPropMDepth(double z1, double t1, double t2, double a, double b, double deltav)
{
	// depth conversion "Interval velocity ~ mid depth"
	double tInt=(t2-t1)/2000.0;	// one-way interval time in sec
	if((1.0-a*tInt/2.0)<=1.0E-10)
		return(-1.0E30);
	else
		return( (z1*(1+a*tInt/2.0)+b*tInt+deltav*tInt)/(1.0-a*tInt/2.0) );
}

double IntVelPropMTime(double z1, double t1, double t2, double a, double b, double deltav)
{
	// depth conversion "Interval velocity ~ mid time"
	double vInt=(b+a*(t2+t1)/2.0);
	double tInt=(t2-t1)/2000.0;	// one-way interval time in sec
	return( z1 + tInt*vInt + deltav*tInt);
}

double IntVelPropTTime(double z1, double t1, double t2, double a, double b, double deltav)
{
	// depth conversion "Interval velocity ~ top time"
	double vInt=(b+a*t1);
	double tInt=(t2-t1)/2000.0;	// one-way interval time in sec
	return( z1 + tInt*vInt + deltav*tInt);
}

double IntVelPropBTime(double z1, double t1, double t2, double a, double b, double deltav)
{
	// depth conversion "Interval velocity ~ bottom time"
	double vInt=(b+a*t2);
	double tInt=(t2-t1)/2000.0;	// one-way interval time in sec
	return( z1 + tInt*vInt + deltav*tInt);
}

double IntVelPropIsochron(double z1, double t1, double t2, double a, double b, double deltav)
{
	// depth conversion "Interval velocity ~ isochronochore"
	double vInt=(b+a*(t2-t1));
	double tInt=(t2-t1)/2000.0;	// one-way interval time in sec
	return( z1 + tInt*vInt + deltav*tInt);
}

double IntVelPropTDepth(double z1, double t1, double t2, double a, double b, double deltav)
{
	// depth conversion "Interval velocity ~ top depth"
	double tInt=(t2-t1)/2000.0;	// one-way interval time in sec
	return(z1*(1+a*tInt)+b*tInt+deltav*tInt);
}

double IntVelContoured(double z1, double t1, double t2, double a, double b, double deltav)
{
	// depth conversion "interval velocity contoured"
	double tInt=(t2-t1)/2000.0;	// one-way interval time in sec
	return(z1 + tInt*deltav);	 // note a, b will be 0 anyway
}

BOOL FakeRegress(int weight, int n, double *z1, double *x, double *y, double *w, double *a, double *b,
									 double *chisq)
{
	*a=0.0;
	*b=0.0;
	*chisq=-1.0;
	return TRUE;
}
