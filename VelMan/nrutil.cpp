#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <math.h>
#ifdef VELMAN_UNIX
#include <iostream.h>
#include <fstream.h>
#else
#include <iostream>
#include <fstream>
#endif
#include "nrutil.h"
#define NR_END 0
#define SHIFT_ONE 1
#define FREE_ARG char*

#include "stdafx.h"	// so we can display Windows error message boxes

/* Numerical Recipes standard error handler */
void nrerror(char *error_text)
{
	CString msg;

	msg="VelocityManager encountered a mathematical or allocation problem!\n"
		"\nNumerical Recipes run-time error:\n" + CString(error_text) +
		"\n\nThe application will terminate now.";
	AfxMessageBox(msg, MB_OK | MB_ICONSTOP);
	exit(10);
}

/* allocate a float vector with subscript range v[nl..nh] */
float *vector(long nl, long nh)	
{
	float *v;
	v = new float[nh-nl+1+NR_END];
	if (!v) nrerror("allocation failure in vector()");
	return v-nl+NR_END;
}

/* allocate an int vector with subscript range v[nl..nh] */
int *ivector(long nl, long nh)	
{
	int *v;
	v = new int[nh-nl+1+NR_END];
	if (!v) nrerror("allocation failure in ivector()");
	return v-nl+NR_END;
}

/* allocate an unsigned char vector with subscript range v[nl..nh] */
unsigned char *cvector(long nl, long nh)	
{
	unsigned char *v;
	v = new unsigned char[nh-nl+1+NR_END];
	if (!v) nrerror("allocation failure in cvector()");
	return v-nl+NR_END;
}

/* allocate an unsigned long vector with subscript range v[nl..nh] */
unsigned long *lvector(long nl, long nh)	
{
	unsigned long *v;
	v = new unsigned long[nh-nl+1+NR_END];
	if (!v) nrerror("allocation failure in lvector()");
	return v-nl+NR_END;
}

/* allocate a double vector with subscript range v[nl..nh] */
double *dvector(long nl, long nh)
{
	double *v;
	v = new double[nh-nl+1+NR_END];
	if (!v) nrerror("allocation failure in dvector()");
	return v-nl+NR_END;
}

/* allocate a float matrix with subscript range m[nrl..nrh][ncl..nch] */
float **matrix(long nrl, long nrh, long ncl, long nch)
{
	long i, nrow=nrh-nrl+1, ncol=nch-ncl+1;
	float **m;
	/* allocate pointers to rows */
	m = new float *[nrow+NR_END];
	if (!m) nrerror("allocation failure 1 in matrix()");
	m += NR_END;
	m -= nrl;
	/* allocate rows and set pointers to them */
	m[nrl] = new float[nrow*ncol+NR_END];
	if (!m[nrl]) nrerror("allocation failure 2 in matrix()");
	m[nrl] += NR_END;
	m[nrl] -= ncl;
	for(i=nrl+1;i<=nrh;i++) m[i]=m[i-1]+ncol;
	/* return pointer to array of pointers to rows */
	return m;
}

/* allocate a double matrix with subscript range m[nrl..nrh][ncl..nch] */
double **dmatrix(long nrl, long nrh, long ncl, long nch)
{
	long i, nrow=nrh-nrl+1, ncol=nch-ncl+1;
	double **m;

	/* allocate pointers to rows */
	m = new double *[nrow+NR_END];
	if (!m) nrerror("allocation failure 1 in matrix()");
	m += NR_END;
	m -= nrl;
	/* allocate rows and set pointers to them */
	m[nrl] = new double[nrow*ncol+NR_END];
	if (!m[nrl]) nrerror("allocation failure 2 in matrix()");
	m[nrl] += NR_END;
	m[nrl] -= ncl;
	for(i=nrl+1;i<=nrh;i++) m[i]=m[i-1]+ncol;
	/* return pointer to array of pointers to rows */
	return m;
}

/* allocate a int matrix with subscript range m[nrl..nrh][ncl..nch] */
int **imatrix(long nrl, long nrh, long ncl, long nch)
{
	long i, nrow=nrh-nrl+1, ncol=nch-ncl+1;
	int **m;
	/* allocate pointers to rows */
	m = new int *[nrow+NR_END];
	if (!m) nrerror("allocation failure 1 in matrix()");
	m += NR_END;
	m -= nrl;
	/* allocate rows and set pointers to them */
	m[nrl] = new int[nrow*ncol+NR_END];
	if (!m[nrl]) nrerror("allocation failure 2 in matrix()");
	m[nrl] += NR_END;
	m[nrl] -= ncl;
	for(i=nrl+1;i<=nrh;i++) m[i]=m[i-1]+ncol;
	/* return pointer to array of pointers to rows */
	return m;
}

/* allocate a float 3tensor with range t[nrl..nrh][ncl..nch][ndl..ndh] */
float ***f3tensor(long nrl, long nrh, long ncl, long nch, long ndl, long ndh)
{
	long i, j, nrow=nrh-nrl+1, ncol=nch-ncl+1, ndep=ndh-ndl+1;
	float ***t;
	/* allocate pointers to pointers to rows */
	t = new float **[nrow+NR_END];
	if (!t) nrerror("allocation failure 1 in f3tensor()");
	t += NR_END;
	t -= nrl;
	/* allocate pointers to rows and set pointers to them */
	t[nrl] = new float *[nrow*ncol+NR_END];
	if (!t[nrl]) nrerror("allocation failure 2 in f3tensor()");
	t[nrl] += NR_END;
	t[nrl] -= ncl;
	/* allocate rows and set pointers to them */
	t[nrl][ncl] = new float[nrow*ncol*ndep+NR_END];
	if (!t[nrl][ncl]) nrerror("allocation failure 3 in f3tensor()");
	t[nrl][ncl] += NR_END;
	t[nrl][ncl] -= ndl;
	for(j=ncl+1;j<=nch;j++) t[nrl][j]=t[nrl][j-1]+ndep;
	for(i=nrl+1;i<=nrh;i++)
	{
		t[i]=t[i-1]+ncol;
		t[i][ncl]=t[i-1][ncl]+ncol*ndep;
		for(j=ncl+1;j<=nch;j++) t[i][j]=t[i][j-1]+ndep;
	}
	/* return pointer to array of pointers to rows */
	return t;
}

/* free a float vector allocated with vector() */
void free_vector(float *v, long nl, long nh)
{
	if(v) delete(v+nl-NR_END);
}

/* free an int vector allocated with ivector() */
void free_ivector(int *v, long nl, long nh)
{
	if(v) delete(v+nl-NR_END);
}

/* free an unsigned char vector allocated with cvector() */
void free_cvector(unsigned char *v, long nl, long nh)
{
	if(v) delete(v+nl-NR_END);
}

/* free an unsigned long vector allocated with lvector() */
void free_lvector(unsigned long *v, long nl, long nh)
{
	if(v) delete(v+nl-NR_END);
}

/* free a double vector allocated with dvector() */
void free_dvector(double *v, long nl, long nh)
{
	if(v) delete (v+nl-NR_END);
}

/* free a float matrix allocated by matrix() */
void free_matrix(float **m, long nrl, long nrh, long ncl, long nch)
{
	if(m)
	{
		delete (m[nrl]+ncl-NR_END);
		delete (m+nrl-NR_END);
	}
}

/* free a double matrix allocated by dmatrix() */
void free_dmatrix(double **m, long nrl, long nrh, long ncl, long nch)
{
	if(m)
	{
		delete (m[nrl]+ncl-NR_END);
		delete (m+nrl-NR_END);
	}
}

/* free an int matrix allocated by imatrix() */
void free_imatrix(int **m, long nrl, long nrh, long ncl, long nch)
{
	if(m)
	{
		delete (m[nrl]+ncl-NR_END);
		delete (m+nrl-NR_END);
	}
}

/* free a float f3tensor allocated by f3tensor() */
void free_f3tensor(float ***t, long nrl, long nrh, long ncl, long nch, long ndl, long ndh)
{
	if(t)
	{
		delete (t[nrl][ncl]+ndl-NR_END);
		delete (t[nrl]+ncl-NR_END);
		delete (t+nrl-NR_END);
	}
}

// New routines introduced with new evaluation routine
/* allocate a point vector with subscript range v[nl..nh] */
Point *pvector(long nl, long nh)
{
	Point *v;
	v=(Point *)malloc((size_t) ((nh-nl+1+SHIFT_ONE)*sizeof(Point)));
	if (!v)
	{
		perror("memory allocation failure in pvector()");
		exit(-3);
	}
	return v-nl+SHIFT_ONE;
}

/* allocate a point matrix with subscript range m[nrl..nrh][ncl..nch] */
Point **pmatrix(long nrl, long nrh, long ncl, long nch)
{
	long i, nrow=nrh-nrl+1, ncol=nch-ncl+1;
	Point **m;
	/* allocate pointers to rows */
	m=(Point **) malloc((size_t)((nrow+SHIFT_ONE)*sizeof(Point*)));
	if (!m) perror("primary level allocation failure in pmatrix()");
	m += SHIFT_ONE;
	m -= nrl;
	/* allocate rows and set pointers to them */
	m[nrl]=(Point *) malloc((size_t)((nrow*ncol+SHIFT_ONE)*sizeof(Point)));
	if (!m[nrl]) perror("second level allocation failure in pmatrix()");
	m[nrl] += SHIFT_ONE;
	m[nrl] -= ncl;
	for(i=nrl+1;i<=nrh;i++) m[i]=m[i-1]+ncol;
	/* return pointer to array of pointers to rows */
	return m;
}

/* free a point vector allocated with pvector() */
void free_pvector(Point *v, long nl, long nh)
{
	free((char*) (v+nl-SHIFT_ONE));
}

/* free a point matrix allocated by pmatrix() */
void free_pmatrix(Point **m, long nrl, long nrh, long ncl, long nch)
{
	free((char*) (m[nrl]+ncl-SHIFT_ONE));
	free((char*) (m+nrl-SHIFT_ONE));
}

/* allocate a double 3tensor with range t[nrl..nrh][ncl..nch][ndl..ndh] */
double ***d3tensor(long nrl, long nrh, long ncl, long nch, long ndl, long ndh)
{
	long i, j, nrow=nrh-nrl+1, ncol=nch-ncl+1, ndep=ndh-ndl+1;
	double ***t;
	/* allocate pointers to pointers to rows */
	t=(double ***) malloc((size_t)((nrow+SHIFT_ONE)*sizeof(double**)));
	if (!t)
		nrerror("primary level allocation failure in d3tensor()");
	t += SHIFT_ONE;
	t -= nrl;
	/* allocate pointers to rows and set pointers to them */
	t[nrl]=(double **) malloc((size_t)((nrow*ncol+SHIFT_ONE)*sizeof(double*)));
	if (!t[nrl])
		nrerror("seconde level allocation failure in d3tensor()");
	t[nrl] += SHIFT_ONE;
	t[nrl] -= ncl;
	/* allocate rows and set pointers to them */
	t[nrl][ncl]=(double *) malloc((size_t)((nrow*ncol*ndep+SHIFT_ONE)*sizeof(double)));
	if (!t[nrl][ncl])
		nrerror("third level allocation failure in d3tensor()");
	t[nrl][ncl] += SHIFT_ONE;
	t[nrl][ncl] -= ndl;
	for(j=ncl+1;j<=nch;j++) t[nrl][j]=t[nrl][j-1]+ndep;
	for(i=nrl+1;i<=nrh;i++)
	{
		t[i]=t[i-1]+ncol;
		t[i][ncl]=t[i-1][ncl]+ncol*ndep;
		for(j=ncl+1;j<=nch;j++)
			t[i][j]=t[i][j-1]+ndep;
	}
	/* return pointer to array of pointers to rows */
	return t;
}

/* free a double d3tensor allocated by d3tensor() */
void free_d3tensor(double ***t, long nrl, long nrh, long ncl, long nch, long ndl, long ndh)
{
	free((char*) (t[nrl][ncl]+ndl-SHIFT_ONE));
	free((char*) (t[nrl]+ncl-SHIFT_ONE));
	free((char*) (t+nrl-SHIFT_ONE));
}

//***********************************************************************
// Subroutine	:			spline()
// Arguments	:			x[], y[]  :  tabulated values yi(xi)
//                  n        :  number of values
//                  yp1, ypn  :  derivative at endpoints
// Use        :     Calculates array of 2nd derivatives for interpolation
// Returns    :     y2[1..n] :  array of 2nd derivatives
// (C) Copr. 1986-92 Numerical Recipes Software Q{,+;1.35, wa.
//***********************************************************************
void spline(double x[], double y[], int n, double yp1, double ypn, double y2[])
{
	int i, k;
	double p, qn, sig, un, *u;
	u=dvector(1, n-1);
	// clever splining
	double yofa, yofb, yofx0, amnx0, bmnx0;
	if(yp1>0.99e30F)
	{
		// find out gradient at starting point using O(h^2) method
		yofx0=y[1];
		yofa=y[2];
		yofb=y[3];
		amnx0=x[2]-x[1];
		bmnx0=x[3]-x[1];
		yp1=(bmnx0/amnx0*yofa-amnx0/bmnx0*yofb-bmnx0/amnx0*yofx0+amnx0/bmnx0*yofx0)/(x[3]-x[2]);
	}
	if(ypn>0.99e30F)
	{
		// find out gradient at starting point using O(h^2) method
		yofx0=y[n];
		yofa=y[n-1];
		yofb=y[n-2];
		amnx0=x[n-1]-x[n];
		bmnx0=x[n-2]-x[n];
		ypn=(bmnx0/amnx0*yofa-amnx0/bmnx0*yofb-bmnx0/amnx0*yofx0+amnx0/bmnx0*yofx0)/(x[n-2]-x[n-1]);
	}
	if (yp1 < -0.99e30F) // natural spline
		y2[1]=u[1]=0.0F;
	else
	{
		y2[1] = -0.5F;
		u[1]=(3.0F/(x[2]-x[1]))*((y[2]-y[1])/(x[2]-x[1])-yp1);
	}
	for (i=2;i<=n-1;i++)
	{
		sig=(x[i]-x[i-1])/(x[i+1]-x[i-1]);
		p=sig*y2[i-1]+2.0F;
		y2[i]=(sig-1.0F)/p;
		u[i]=(y[i+1]-y[i])/(x[i+1]-x[i]) - (y[i]-y[i-1])/(x[i]-x[i-1]);
		u[i]=(6.0F*u[i]/(x[i+1]-x[i-1])-sig*u[i-1])/p;
	}
	if (ypn < -0.99e30F) // natural spline
		qn=un=0.0F;
	else
	{
		qn=0.5F;
		un=(3.0F/(x[n]-x[n-1]))*(ypn-(y[n]-y[n-1])/(x[n]-x[n-1]));
	}
	y2[n]=(un-qn*u[n-1])/(qn*y2[n-1]+1.0F);
	for (k=n-1;k>=1;k--)
		y2[k]=y2[k]*y2[k+1]+u[k];
	free_dvector(u, 1, n-1);
}

//***********************************************************************
// Subroutine	:			splint()
// Arguments	:			xa[], ya[]  :  tabulated values yi(xi)
//                  y2a[]      :  array of 2nd derivatives
//                  x          :  value of x at which y is to be found
// Use        :     Interpolates ya(xa) at x
// Returns    :     *y         :  pointer to value of y(x)
// (C) Copr. 1986-92 Numerical Recipes Software Q{,+;1.35, wa.
//***********************************************************************
void splint(double xa[], double ya[], double y2a[], int n, double x, double *y)
{
	void nrerror(char error_text[]);
	int klo, khi, k;
	double h, b, a;
	klo=1;
	khi=n;
	while (khi-klo > 1)
	{
		k=(khi+klo) >> 1;
		if (xa[k] > x) khi=k;
		else klo=k;
	}
	h=xa[khi]-xa[klo];
	if (h == 0.0) nrerror("Bad xa input to routine splint");
	a=(xa[khi]-x)/h;
	b=(x-xa[klo])/h;
	*y=a*ya[klo]+b*ya[khi]+((a*a*a-a)*y2a[klo]+(b*b*b-b)*y2a[khi])*(h*h)/6.0F;
}

//***********************************************************************
// Subroutine	:			splie2()
// Arguments	:			xa2[], ya[] :  tabulated values yi(xa1i, xa2i)
//                  m, n        :  size of y[m][n]
// Use        :                   finds sets of 2nd derivatives for a
//                                set of points of size [m][n]
// Returns    :     y2a[]      : 2nd derivatives to be computed
// (C) Copr. 1986-92 Numerical Recipes Software Q{,+;1.35, wa.
//***********************************************************************
void splie2(double x2a[], double **ya, int m, int n, double **y2a)
{
	int j;
	for (j=1;j<=m;j++)
		spline(x2a, ya[j], n, 1.0e30F, 1.0e30F, y2a[j]);
}

//***********************************************************************
// Subroutine	:			splin2()
// Arguments	:			x1a[], x2a[] :  tabulated values x1i and x2i
//                  ya[][]      :  tabulated values of yai(x1ai, x2ai)
//                  y2a[][]     :  2D array of 2nd derivatives
//                  m, n         :  size of ya[m][n]
//                  x1, x2       :  value of x1, x2 at which
//                                 y is to be found
// Use        :     Interpolates y2a(x1a, x2a) at x1, x2
// Returns    :     *y          :  pointer to value of y(x)
// (C) Copr. 1986-92 Numerical Recipes Software Q{,+;1.35, wa.
//***********************************************************************
void splin2(double x1a[], double x2a[], double **ya, double **y2a, int m, int n,
	double x1, double x2, double *y)
{
	int j;
	double *ytmp, *yytmp;
	ytmp=dvector(1, m);
	yytmp=dvector(1, m);
	for (j=1;j<=m;j++)
		splint(x2a, ya[j], y2a[j], n, x2, &yytmp[j]);
	spline(x1a, yytmp, m, 1.0e30F, 1.0e30F, ytmp);
	splint(x1a, yytmp, ytmp, m, x1, y);
	free_dvector(yytmp, 1, m);
	free_dvector(ytmp, 1, m);
}

void lubksb(double **a, int n, int *indx, double*b)
{
	int i, ii=0, ip, j;
	double sum;
	for (i=1;i<=n;i++)
	{
		ip=indx[i];
		sum=b[ip];
		b[ip]=b[i];
		if (ii)
			for (j=ii;j<=i-1;j++) sum -= a[i][j]*b[j];
		else if (sum) ii=i;
		b[i]=sum;
	}
	for (i=n;i>=1;i--)
	{
		sum=b[i];
		for (j=i+1;j<=n;j++) sum -= a[i][j]*b[j];
		b[i]=sum/a[i][i];
	}
}

void piksr2(int n, double arr[], double brr[])
{
	int i, j;
	double a, b;
	for (j=2;j<=n;j++)
	{
		a=arr[j];
		b=brr[j];
		i=j-1;
		while (i > 0 && arr[i] > a)
		{
			arr[i+1]=arr[i];
			brr[i+1]=brr[i];
			i--;
		}
		arr[i+1]=a;
		brr[i+1]=b;
	}
}

#define TINY 1.0e-20;
void ludcmp(double **a, int n, int *indx, double*d)
{
	int i, imax, j, k;
	double big, dum, sum, temp;
	double *vv;
	vv=dvector(1, n);
	*d=1.0;
	for (i=1;i<=n;i++)
	{
		big=0.0;
		for (j=1;j<=n;j++)
			if ((temp=fabs(a[i][j])) > big) big=temp;
		if (big == 0.0) nrerror("Singular matrix in routine LUDCMP");
		vv[i]=1.0/big;
	}
	for (j=1;j<=n;j++)
	{
		for (i=1;i<j;i++)
		{
			sum=a[i][j];
			for (k=1;k<i;k++) sum -= a[i][k]*a[k][j];
			a[i][j]=sum;
		}
		big=0.0;
		for (i=j;i<=n;i++)
		{
			sum=a[i][j];
			for (k=1;k<j;k++)
				sum -= a[i][k]*a[k][j];
			a[i][j]=sum;
			if ( (dum=vv[i]*fabs(sum)) >= big)
			{
				big=dum;
				imax=i;
			}
		}
		if (j != imax)
		{
			for (k=1;k<=n;k++)
			{
				dum=a[imax][k];
				a[imax][k]=a[j][k];
				a[j][k]=dum;
			}
			*d = -(*d);
			vv[imax]=vv[j];
		}
		indx[j]=imax;
		if (a[j][j] == 0.0) a[j][j]=TINY;
		if (j != n)
		{
			dum=1.0/(a[j][j]);
			for (i=j+1;i<=n;i++) a[i][j] *= dum;
		}
	}
	free_dvector(vv, 1, n);
}

#undef TINY
#define SWAP(a, b) {temp=(a);(a)=(b);(b)=temp;}

int gaussj(double **a, int n, double **b, int m)
{
	int *indxc, *indxr, *ipiv;
	int i = 0, icol = 0, irow = 0, j = 0, k = 0, l = 0, ll = 0;
	double big = 0, dum = 0, pivinv = 0, temp = 0;
	indxc=ivector(1, n);
	indxr=ivector(1, n);
	ipiv=ivector(1, n);
	for (j=1;j<=n;j++) ipiv[j]=0;
	for (i=1;i<=n;i++)
	{
		big=0.0;
		for (j=1;j<=n;j++)
			if (ipiv[j] != 1)
				for (k=1;k<=n;k++)
				{
					if (ipiv[k] == 0)
					{
						if (fabs(a[j][k]) >= big)
						{
							big=fabs(a[j][k]);
							irow=j;
							icol=k;
						}
					}
					else if (ipiv[k] > 1)
					{
						free_ivector(ipiv, 1, n);
						free_ivector(indxr, 1, n);
						free_ivector(indxc, 1, n);
						return 0;
					}
				}
				++(ipiv[icol]);
				if (irow != icol)
				{
					for (l=1;l<=n;l++) SWAP(a[irow][l], a[icol][l])
						for (l=1;l<=m;l++) SWAP(b[irow][l], b[icol][l])
				}
				indxr[i]=irow;
				indxc[i]=icol;
				if (a[icol][icol] == 0.0)
				{
					free_ivector(ipiv, 1, n);
					free_ivector(indxr, 1, n);
					free_ivector(indxc, 1, n);
					return 0;
				}
				pivinv=1.0/a[icol][icol];
				a[icol][icol]=1.0;
				for (l=1;l<=n;l++) a[icol][l] *= pivinv;
				for (l=1;l<=m;l++) b[icol][l] *= pivinv;
				for (ll=1;ll<=n;ll++)
					if (ll != icol)
					{
						dum=a[ll][icol];
						a[ll][icol]=0.0;
						for (l=1;l<=n;l++) a[ll][l] -= a[icol][l]*dum;
						for (l=1;l<=m;l++) b[ll][l] -= b[icol][l]*dum;
					}
	}
	for (l=n;l>=1;l--)
	{
		if (indxr[l] != indxc[l])
			for (k=1;k<=n;k++)
				SWAP(a[k][indxr[l]], a[k][indxc[l]]);
	}
	free_ivector(ipiv, 1, n);
	free_ivector(indxr, 1, n);
	free_ivector(indxc, 1, n);
	return 1;
}
#undef SWAP

/* multi quadratic evaluation code
given n=1..xxx points (x, y) with function values f
it computes the value of that surface at the point (ptx, pty)
using radial basis functions
*/
#define c 1.0		// basis funcs are sqrt( (x-xr)**2 + (y-yr)**2 + c**2 )
#define	SQR(x) ((x)*(x))
double multiquadric_eval(int n, double ptx, double pty, double *x, double *y, double *f)
{
	int j, k, np1=n+1;
	double c2=c*c;
	double **a, **b;
	a=dmatrix(1, np1, 1, np1);
	b=dmatrix(1, np1, 1, 1);
	// set up right hand side b
	for(k=1;k<=n;k++)
		b[k][1]=f[k-1];
	b[np1][1]=0.0;
	// set up matrix a
	// last row+column
	for(k=1;k<=n;k++)
	{
		a[np1][k]=1.0;
		a[k][np1]=1.0;
		a[k][k]=c;
	}
	a[np1][np1]=0.0;
	// upper and lower triangle
	for(j=2;j<=n;j++)
	{
		for(k=1;k<j;k++)
		{
			a[j][k]=sqrt(SQR(x[j-1]-x[k-1])+SQR(y[j-1]-y[k-1])+c2);
			a[k][j]=a[j][k];
		}
	}
	// solve system
	gaussj(a, np1, b, 1);
	// evaluate function value
	double s=b[np1][1];
	for(k=1;k<=n;k++)
		s+=b[k][1]*sqrt(SQR(ptx-x[k-1])+SQR(pty-y[k-1])+c2);
	free_dmatrix(a, 1, np1, 1, np1);
	free_dmatrix(b, 1, np1, 1, 1);
	return s;
}
#undef c

/* This is called from the new fetps routine */

#define EPS 3.0e-14
#define MAXIT 10

void gaulag(double x[], double w[], int n, double alf)
{
	double gammln(double xx);
	void nrerror(char error_text[]);
	int i = 0, its = 0, j = 0;
	double ai = 0;
	double p1 = 0, p2 = 0, p3 = 0, pp = 0, z = 0, z1 = 0;

	for (i=1;i<=n;i++)
	{
		if (i == 1)
		{
			z=(1.0+alf)*(3.0+0.92*alf)/(1.0+2.4*n+1.8*alf);
		}
		else if (i == 2)
		{
			z += (15.0+6.25*alf)/(1.0+0.9*alf+2.5*n);
		}
		else
		{
			ai=i-2;
			z += ((1.0+2.55*ai)/(1.9*ai)+1.26*ai*alf/
				(1.0+3.5*ai))*(z-x[i-2])/(1.0+0.3*alf);
		}
		for (its=1;its<=MAXIT;its++)
		{
			p1=1.0;
			p2=0.0;
			for (j=1;j<=n;j++)
			{
				p3=p2;
				p2=p1;
				p1=((2*j-1+alf-z)*p2-(j-1+alf)*p3)/j;
			}
			pp=(n*p1-(n+alf)*p2)/z;
			z1=z;
			z=z1-p1/pp;
			if (fabs(z-z1) <= EPS) break;
		}
		if (its > MAXIT) nrerror("too many iterations in gaulag");
		x[i]=z;
		w[i] = -exp(gammln(alf+n)-gammln((double)n))/(pp*n*p2);
	}
}
#undef EPS
#undef MAXIT

/* This is called from the new fetps routine */

double gammln(double xx)
{
	double x, y, tmp, ser;
	static double cof[6]={76.18009172947146,-86.50532032941677,
		24.01409824083091,-1.231739572450155,
		0.1208650973866179e-2,-0.5395239384953e-5};
	int j;

	y=x=xx;
	tmp=x+5.5;
	tmp -= (x+0.5)*log(tmp);
	ser=1.000000000190015;
	for (j=0;j<=5;j++) ser += cof[j]/++y;
	return -tmp+log(2.5066282746310005*ser/x);
}

/* Routine to evaluate the derivative of a
function at a certain point. This is used in the ray-tracing
part of the program code. Thus function was extended by sk126
to allow derivatives of a 3D functions into any direction. */

