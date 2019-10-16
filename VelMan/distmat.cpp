#include "stdafx.h"

#include "nrutil.h"
#include <math.h>

#ifndef SQR
#define SQR(a) ((a)*(a))
#endif

/* phi: univariate function defined for non-negative reals only
this is the slower old version */
double phi_slow(double r)
{
	double s;
	if ( r == 0.0 )
		s = 0.0;
	else
		s = r*r*log(r);
	return s;
}

/* This is the fast version of the basis functions
this is the newer faster vesion */
double phi_fast(double r)
{
	double s;
	if ( r == 0.0 )
		s = 1;
	else
		s = sqrt(r + 1.0);
	return s;
}

/******************************************************************/
/* dist(u, v) = Euclidean distance between u[1..2] and v[1..2] */

double dist(double *u, double *v, int fasttps)
{
	double length;

	if(fasttps)
		length = (SQR(u[1]-v[1]) + SQR(u[2]-v[2]));
	else
		length = sqrt(SQR(u[1]-v[1]) + SQR(u[2]-v[2]));

	return length;
}

/******************************************************************/
/* distance_matrix: usage */
/* input: x[1..n][1..2] ; points stored by rows */
/* a[1..n][1..n] ; distance matrix returned */

double **distance_matrix(double **x, int n, int fasttps)
{
	int j, k;
	double **a;
	double alpha;

	a = dmatrix(1, n, 1, n);

	for(j=1; j<=n; j++) a[j][j] = 0.0;

	for(j=1; j<=n; j++)
		for(k=j+1; k<=n; k++)
		{
			if(fasttps)
				alpha = phi_fast(dist(x[j], x[k], 1));
			else
				alpha = phi_slow(dist(x[j], x[k], 0));
			a[j][k] = alpha;
			a[k][j] = alpha;
		}

		return a;
}

/******************************************************************/
/* x[1..n][1..2] = points stored by rows; f[1..n] = fn values at pts */

double *interpolate_rbf(double **x, int n, double *f, int fasttps)
{
	double **a, d, *c;
	int *indx, k;

	a = distance_matrix(x, n, fasttps);
	indx = ivector(1, n);

	c = dvector(1, n);
	for(k=1; k<=n; k++) c[k] = f[k];

	ludcmp(a, n, indx, &d);
	lubksb(a, n, indx, c);

	return c;
}

/******************************************************************/
/******************************************************************/
/* tps_matrix: usage */
/* input: x[1..n][1..2] ; points stored by rows */
/* a[1..n+3][1..n+3] ; tps distance matrix returned */

double **tps_matrix(double **x, int n, int fasttps)
{
	int j, k;
	double **a;
	double alpha;

	a = dmatrix(1, n+3, 1, n+3);

	/* construct principal nxn part */

	for(j=1; j<=n; j++) a[j][j] = 0.0;

	for(j=1; j<=n; j++)
		for(k=j+1; k<=n; k++)
		{
			if(fasttps)
				alpha = phi_fast(dist(x[j], x[k], 1));
			else
				alpha = phi_slow(dist(x[j], x[k], 0));

			a[j][k] = alpha;
			a[k][j] = alpha;
		}

		/* construct polynomial part */
		for(j=1; j<=n; j++)
		{
			a[n+1][j] = a[j][n+1] = 1.0;
			a[n+2][j] = a[j][n+2] = x[j][1];
			a[n+3][j] = a[j][n+3] = x[j][2];
		}

		for(j=n+1; j<=n+3; j++)
			for(k=n+1; k<=n+3; k++)
				a[j][k] = 0.0;

		return a;
}

/******************************************************************/
/* x[1..n][1..2] = points stored by rows; f[1..n] = fn values at pts */

double *interpolate_tps(double **x, int n, double *f, int fasttps)
{
	double **a, *c;
	double d;
	int *indx, k;

	a = tps_matrix(x, n, fasttps);
	indx = ivector(1, n+3);

	c = dvector(1, n+3);

	for(k=1; k<=n; k++) c[k] = f[k];
	for(k=n+1; k<=n+3; k++) c[k] = 0.0;

	ludcmp(a, n+3, indx, &d);
	lubksb(a, n+3, indx, c);

	free_dmatrix(a, 1, n+3, 1, n+3);
	free_ivector(indx, 1, n+3);

	return c;
}

/******************************************************************/

/* x[1..n][1..2] = centres of rbf; c[1..n] = coeffs of rbf */
/* z[1..m][1..2] = evaluation points */

double*evaluate_rbf(double **x, double *c, int n, double **z, int m)
{
	int j, k;
	double*s;

	s = dvector(1, m);

	for(j=1; j<=m; j++)
	{
		s[j] = 0.0;
		for(k=1; k<=n; k++)
			s[j] += c[k] * phi_slow(dist(z[j], x[k], 0));
	}

	return s;
}

/******************************************************************/
/* x[1..n][1..2] = centres of rbf; c[1..n+3] = coeffs of rbf */
/* z[1..m][1..2] = evaluation points */

double *evaluate_tps(double **x, double *c, int n, double ** z, int  m)
{
	int j, k;
	double *s;

	s = dvector(1, m);

	for(j=1; j<=m; j++)
	{
		s[j] = 0.0;
		for(k=1; k<=n; k++)
			s[j] += c[k] * phi_slow(dist(z[j], x[k], 0));
		s[j] += c[n+1] + c[n+2]*z[j][1] + c[n+3]*z[j][2];
	}

	return s;
}

/******************************************************************/
/* x[1..n][1..2] = centres of rbf; c[1..n] = coeffs of rbf */
/* z[1..m][1..2] = evaluation points */

double evaluate_point_tps(double **x, double *c, int n, double *z)
{
	int k;
	double s = 0.0;

	for(k=1; k<=n; k++)
		s += c[k] * phi_slow(dist(z, x[k], 0));

	s += c[n+1] + c[n+2]*z[1] + c[n+3]*z[2];

	return s;
}

/******************************************************************/

/* x[1..n][1..2] = centres of rbf; c[1..n] = coeffs of rbf */
/* z[1..m][1..2] = evaluation points */

double **evaluate_grid_tps(double **x, double *c, int n, int rs, int cs,
						   double dy, double dx, double y0, double x0)
{
	int i, j, k;
	double **s;
	double xn[2];

	s = dmatrix(1, rs, 1, cs);

	for(i=1;i<=rs;i++) {
		xn[1] = y0 + (i-1) * dy;
		for(j=1; j<=cs; j++) {
			xn[0] = x0 + (j-1) * dx;

			s[i][j] = 0.0;
			for(k=1; k<=n; k++)
				s[i][j] += c[k] * phi_slow(dist(xn-1, x[k], 0));
			s[i][j] += c[n+1] + c[n+2]*xn[0] + c[n+3]*xn[1];
		}
	}

	return s;
}

/******************************************************************/

// FindAverageValue()
// given a list of control points f[], finds their "average" so that if
// we grid the values and want some grid normalisation points to be added
// outside the actual grid, they'll get this value.
// so far, we just use the average value - but that might change...

double FindAverageValue(int n, double *f)
{
	double avg=0.0;

	if(n>0)
	{
		for(int i=0;i<n;i++)
			avg+=f[i];

		avg/=n;
	}
	return avg;
}

/******************************************************************/

/* Gets a list of points (xin, yin)  (numin entries in total) and interpolates
or extrapolates to return the y value to x.
Method is 1=cubic spline or 0=linear interpolate or 2=just take vInt=const,
(note this routine is used to interpolate (time, vAvg) pairs to obtain the
average velocity for a given time)
If extrapolation is required (x larger than largest xin), then method 1 is
always used. The last gradient
(between the last but one and the last point) is changed, according to
extrapolate_percentage (where 100 means keep gradient, 200 means double it etc.)

Returns the "errval" value if things go wrong.

WARNING: This assumes that the xin[] array is in ascending order. Equal
values do not present problems, but if they decrease the output of this
routine is undefined.

x must be positive, otherwise we return errval.
*/

double InterpolateExtrapolate(double x, double *xin, double *yin, int numin,
							  int method, int extrapolate_percentage, double errval)
{
	int i, j;
	double y, grad, *second_der;
	double dTop, dBtm, d;
	double minDt;   // now read from ini file.

	CString buf = AfxGetApp()->GetProfileString("ArithmeticParams", "InterpMinTime", "0.010" );
	minDt = atof((const char *)buf);
	//buf = AfxGetApp()->GetProfileString("ArithmeticParams", "InterpMaxVel", "30000" );
	//maxvel = atof((const char *)buf);
	//buf = AfxGetApp()->GetProfileString("ArithmeticParams", "InterpMaxDeltaV", "25" );
	//maxdeltav = atof((const char *)buf) / 100.0;    // ini setting is a percentage

	if(x<=0.0)
		return errval;

	// make sure that all the x values really increase
	for(i=1;i<numin;i++)
	{
		if(xin[i]<xin[i-1])
			return errval;
	}

	for(i=1;i<numin;i++)
	{
		// if any two consecutive x values have the same value, only use one of them
		// and use the average of the y values that go with them.
		if(xin[i]-xin[i-1] < minDt)
		{
			yin[i-1]=(yin[i-1]+yin[i])/2.0; // y-values = average value
			for(j=i;j<numin-1;j++)					// shift all other points down one step
			{
				xin[j]=xin[j+1];
				yin[j]=yin[j+1];
			}
			numin--;
			i--;	// test the current value again -- if 3 or more equal consecutive numbers
		}
		// N.B. this is not the best place for these checks. They should really be done
		//  outside this routine, since this routine is generic. By putting these checks
		//  in it's making it specific to interpolating velocities. But.. I need to do
		//  this quick..
		//  if velocity on a layer exceeds the max. permissible, then we ignore that layer
		/* if ( yin[i] > maxvel )
		{
		for (j=i;j<numin-1;j++)     // shift input down one place
		{
		xin[j] = xin[j+1];
		yin[j] = xin[j+1];
		}
		numin--;
		} */
		//  check change in velocity from one layer to the next is not > 25%
		/*if ( fabs(yin[i]-yin[i-1]) > maxdeltav*yin[i-1] )
		{
		for (j=i;j<numin-1;j++)     // shift input down one place
		{
		xin[j] = xin[j+1];
		yin[j] = xin[j+1];
		}
		numin--;
		} */
		//  last check is that the average interval velocity (in yin) is positive.
		if ( yin[i] < 0.0 )
		{
			for (j=i;j<numin-1;j++)     // shift input down one place
			{
				xin[j] = xin[j+1];
				yin[j] = xin[j+1];
			}
			numin--;
		}
	}

	// if this reduced the number of points too far, give up
	if(numin<2)
		return errval;

	// select linear rather than spline interpolation if the number has been
	// reduced too far
	if(method==1 && numin<4)
		method=0;

	// shallower extrapolation always done by "constant vInt" model, though in
	// a very simple form because dTop=tTop=0 makes the equations simpler
	if(x<xin[0])
		// return vAvg=vInt=the first vInt
		return yin[0];

	if(method==0 || (method==1 && x>=xin[numin-1]) )
	{
		// linear interpolation, or generic extrapolation
		if(numin<2)
			return errval;
		for(i=1;i<numin;i++)
		{
			if(x<=xin[i])
			{
				// we found the correct interval; now linearly interpolate it and quit
				// interpolate between last pair and this pair. Note that we never will
				// have xin[i-1]>=xin[i], because then the if(x<xin[i]) would have
				// triggered before
				return (yin[i-1]+( (yin[i]-yin[i-1])/(xin[i]-xin[i-1]) )*(x-xin[i-1]) );
			}
		}
		// the loop quit; hence we have to extrapolate the last vAvg
		grad=(yin[numin-1]-yin[numin-2])/(xin[numin-1]-xin[numin-2]);
		return (yin[numin-1]+(x-xin[numin-1])*(extrapolate_percentage/100.0)*grad);
	}

	if(method==1)
	{
		// cubic spline (never extrapolated). We are by now sure we have >=4 points
		// we are also sure that x values of first and last pair of points are different

		// set up spline. gradients at endpoints are set rather cleverly...
		second_der=new double[numin];
		spline(xin-1, yin-1, numin, 1.0E30, 1.0E30, second_der-1);
		splint(xin-1, yin-1, second_der-1, numin, x, &y);
		delete second_der;
		return y;
	}

	if(method==2)
	{
		// interpolation using constant interval velocity
		if(numin<2)
			return errval;
		for(i=1;i<numin;i++)  // note we start at 1 not 0: x<xin[0] is done linearly
		{
			if(x<=xin[i])
			{
				// we are in the right interval
				// convert the two vAvg values back to depths
				dTop=yin[i-1]*xin[i-1];
				dBtm=yin[i]*xin[i];
				// d=dTop + (t-tTop)*vInt
				d=dTop+(x-xin[i-1])*(dBtm-dTop)/(xin[i]-xin[i-1]);
				// return vAvg
				return (d/x);
			}
		}
		// the loop quit; we have to extrapolate the last vInt
		grad=(xin[numin-1]*yin[numin-1]-xin[numin-2]*yin[numin-2])/(xin[numin-1]-xin[numin-2]);
		// corresponding depth
		d=xin[numin-1]*yin[numin-1]+(x-xin[numin-1])*(extrapolate_percentage/100.0)*grad;
		// return vAvg
		return (d/x);
	}
	return errval; // wrong method requested or points not in ascending order or so
}

/***********************************/
// same as above, but this one is used to compute the time for a given depth
// point using interpolated average velocities, rather than the above which is
// using given times to compute a depth point. So "x" is a DEPTH value
double InterpolateExtrapolateDepthToTime(double x, double *xin, double *yin, int numin,
										 int method, int extrapolate_percentage, double errval)
{
	int i, j;
	double y, grad, *second_der;
	double dTop, dBtm, t, p;

	if(x<=0.0)
		return errval;

	// make sure that all the x values really increase
	for(i=1;i<numin;i++)
	{
		// make sure all the x values increase or that they do so at least to a few digits
		// of precision
		if(xin[i-1]-xin[i]>2.0)
			return errval;

		if(xin[i]-xin[i-1]<0.005)
		{
			yin[i-1]=(yin[i-1]+yin[i])/2.0; // y-values = average value
			for(j=i;j<numin-1;j++)
			{
				xin[j]=xin[j+1];
				yin[j]=yin[j+1];
			}
			numin--;
			i--;	// test the current value again -- if 3 or more equal consecutive numbers
		}
	}
	// if this reduced the number of points too far, give up
	if(numin<2)
		return errval;

	// select linear rather than spline interpolation if the number has been
	// reduced too far
	if(method==1 && numin<4)
		method=0;

	// shallower extrapolation always done by "constant vInt" model, though in
	// a very simple form because dTop=tTop=0 makes the equations simpler
	if(x<yin[0]*xin[0])
		// vInt=vAvg if dTop=tTop=0. This is stored in y[], so just return it
		return yin[0];

	if(method==0 || (method==1 && x>=yin[numin-1]*xin[numin-1]) )
	{
		// linear interpolation, or generic extrapolation
		if(numin<2)
			return errval;

		for(i=0;i<numin;i++)
		{
			if(x<=xin[i]*yin[i])
			{
				// we found the correct interval; now lineary interpolate it and quit.
				// interpolate between last pair and this pair. Note that we never will
				// have xin[i-1]>=xin[i], because then the if(x<xin[i]) would have
				// triggered before

				// the equation is t=t1 + (vAvg-v1)*(t1-t2)/(v1-v2)
				// but vAvg=d/t, hence it really is a quadratic equation that we solve for t
				grad=(xin[i]-xin[i-1])/(yin[i]-yin[i-1]);
				p=(yin[i]*grad-xin[i]);
				t=-p/2+sqrt(p*p/4+x*grad);
				return (x/t);
			}
		}
		// the loop quit; hence we have to extrapolate
		// grad=last known interval velocity
		grad=(xin[numin-1]-xin[numin-2])/(yin[numin-1]-yin[numin-2]);
		grad/=(extrapolate_percentage/100.0); // decrease/increase that last gradient
		p=(yin[numin-1]*grad-xin[numin-1]);
		t=-p/2+sqrt(p*p/4+x*grad);
		return (x/t);
	}

	if(method==1)
	{
		// cubic spline
		// cubic spline (never extrapolated). We are by now sure we have >=4 points
		// we are also sure that x values of first and last pair of points are different

		// set up spline. note we call spline() with times as Y-argument, not X-arg
		// and we convert all y[] things back to depths, not avererage vels.
		// then "x" in the parameter list of this function can be used as an "x"-
		// argument in splint(), where we time-convert a depth
		for(i=0;i<numin;i++)
			yin[i]*=xin[i];  // vAvg->depth
		second_der=new double[numin];
		spline(yin-1, xin-1, numin, 1.0E30, 1.0E30, second_der-1);
		splint(yin-1, xin-1, second_der-1, numin, x, &y);
		delete second_der;
		return x/y; // time->vAvg again
	}

	if(method==2)
	{
		// interpolation using constant interval velocity
		if(numin<2)
			return errval;
		for(i=0;i<numin;i++)
			yin[i]*=xin[i];  // vAvg->depth
		for(i=1;i<numin;i++)  // note we start at 1 not 0: x<xin[0] is done linearly
		{
			if(x<=yin[i])  // input depth < depth of i-th interval?
			{
				// we are in the right interval
				// convert the two vAvg values back to dephts
				dTop=yin[i-1];
				dBtm=yin[i];
				// t=tTop + (d-dTop)/vInt
				t=xin[i-1]+(x-dTop)*(xin[i]-xin[i-1])/(dBtm-dTop);
				// return vAvg
				return (x/t);
			}
		}
		// the loop quit; hence we have to extrapolate
		grad=(xin[numin-1]-xin[numin-2])/(yin[numin-1]-yin[numin-2]);
		grad/=(extrapolate_percentage/100.0); // decrease/increase that last gradient
		t=xin[numin-1]+grad*(x-yin[numin-1]);
		return (x/t);
	}
	return errval; // wrong method requested or points not in ascending order or so
}
