/*
Numerical routines for velman.
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "numerics.h"

#define min(a, b) ((a) <= (b) ? (a) : (b))
#define max(a, b) ((a) >= (b) ? (a) : (b))
#define MYABS(X) ( X > 0.0 ? (X) : -(X) )

// Global variables
double *u, *v; // t, z values
double *w; // weights
double p; // p-norm

static long int c__1 = 1;	/* Table of constant values */
static long int c__2 = 2;

double signum(double x)
{
	double s = 0.0;
	if (x > 0.0) s =  1.0;
	if (x < 0.0) s = -1.0;
	return s;
}

long *ivectorl(long nl, long nh)
{
	long *v;

	v=(long *)malloc((unsigned) (nh-nl+1)*sizeof(long));
	if (!v) return NULL;
	return v-nl;
}

double *dvectorl(long nl, long nh)
{
	double *v;

	v=(double *)malloc((unsigned) (nh-nl+1)*sizeof(double));
	if (!v) return NULL;
	return v-nl;
}

void free_ivectorl(long int *v, long int nl, long int nh)
{
	free((char*) (v+nl));
}

void free_dvectorl(double *v, long int nl, long int nh)
{
	free((char*) (v+nl));
}

double enorm_(long int *n, double *x)
{
	/* Initialized data */

	static double one = 1.;
	static double zero = 0.;
	static double rdwarf = 3.834e-20;
	static double rgiant = 1.304e19;

	/* System generated locals */
	long int i__1;
	double ret_val, d__1;

	/* Local variables */
	static double xabs, x1max, x3max;
	static long int i;
	static double s1, s2, s3, agiant, floatn;

	/* function enorm

	given an n-vector x, this function calculates the
	euclidean norm of x.

	the euclidean norm is computed by accumulating the sum of
	squares in three different sums. the sums of squares for the
	small and large components are scaled so that no overflows
	occur. non-destructive underflows are permitted. underflows
	and overflows do not occur in the computation of the unscaled
	sum of squares for the intermediate components.
	the definitions of small, intermediate and large components
	depend on two constants, rdwarf and rgiant. the main
	restrictions on these constants are that rdwarf**2 not
	underflow and rgiant**2 not overflow. the constants
	given here are suitable for every known computer.

	the function statement is

	function enorm(n, x)

	where

	n is a positive long int input variable.

	x is an input array of length n.

	argonne national laboratory. minpack project.

	*/

	/* Parameter adjustments */

	--x;

	/* Function Body */
	s1 = zero;
	s2 = zero;
	s3 = zero;
	x1max = zero;
	x3max = zero;
	floatn = (double) (*n);
	agiant = rgiant / floatn;
	i__1 = *n;
	for (i = 1; i <= i__1; ++i)
	{
		xabs = (d__1 = x[i], fabs(d__1));
		if (xabs > rdwarf && xabs < agiant)
		{
			goto L70;
		}
		if (xabs <= rdwarf)
		{
			goto L30;
		}

		/* sum for large components. */

		if (xabs <= x1max)
		{
			goto L10;
		}

		/* Computing 2nd power */
		d__1 = x1max / xabs;
		s1 = one + s1 * (d__1 * d__1);
		x1max = xabs;
		goto L20;
L10:
		/* Computing 2nd power */
		d__1 = xabs / x1max;
		s1 += d__1 * d__1;
L20:
		goto L60;
L30:

		/* sum for small components. */

		if (xabs <= x3max)
		{
			goto L40;
		}

		/* Computing 2nd power */
		d__1 = x3max / xabs;
		s3 = one + s3 * (d__1 * d__1);
		x3max = xabs;
		goto L50;
L40:
		if (xabs != zero)
		{

			/* Computing 2nd power */
			d__1 = xabs / x3max;
			s3 += d__1 * d__1;
		}
L50:
L60:
		goto L80;
L70:

		/* sum for intermediate components. */

		/* Computing 2nd power */
		d__1 = xabs;
		s2 += d__1 * d__1;
L80:

		/* L90: */
		;
	}

	/* calculation of norm. */
	if (s1 == zero)
	{
		goto L100;
	}
	ret_val = x1max * sqrt(s1 + s2 / x1max / x1max);
	goto L130;
L100:
	if (s2 == zero)
	{
		goto L110;
	}
	if (s2 >= x3max)
	{
		ret_val = sqrt(s2 * (one + x3max / s2 * (x3max * s3)));
	}
	if (s2 < x3max)
	{
		ret_val = sqrt(x3max * (s2 / x3max + x3max * s3));
	}
	goto L120;
L110:
	ret_val = x3max * sqrt(s3);
L120:
L130:
	return ret_val;

} /* enorm_ */

double dpmpar_(long int *i)
{
	/* Initialized data */

	static struct 
	{
		long int e_1[6];
		double fill_2[1];
		double e_3;
	} equiv_2 = { 9472, 0, 128, 0, -32769, -1, {0}, 0. };

	/* System generated locals */
	double ret_val;

	/* Local variables */
#define dmach ((double *)&equiv_2)
#define minmag ((long int *)&equiv_2 + 2)
#define maxmag ((long int *)&equiv_2 + 4)
#define mcheps ((long int *)&equiv_2)

	/* function dpmpar

	This function provides double precision machine parameters
	when the appropriate set of data statements is activated (by
	removing the c from column 1) and all other data statements are
	rendered inactive. Most of the parameter values were obtained
	from the corresponding Bell Laboratories Port Library function.

	The function statement is

	function dpmpar(i)

	where

	i is an long int input variable set to 1, 2, or 3 which
	selects the desired machine parameter. If the machine has
	t base b digits and its smallest and largest exponents are
	emin and emax, respectively, then these parameters are

	dpmpar(1) = b**(1 - t), the machine precision,

	dpmpar(2) = b**(emin - 1), the smallest magnitude,

	dpmpar(3) = b**emax*(1 - b**(-t)), the largest magnitude.

	Argonne National Laboratory. MINPACK Project.

	*/

	ret_val = dmach[*i - 1];
	return ret_val;

} /* dpmpar_ */

#undef mcheps
#undef maxmag
#undef minmag

long int qrsolv_(long int *n, double *r, long int *ldr, long int *ipvt,
	double *diag, double *qtb, double *x, double *sdiag, double *wa)
{
	/* Initialized data */

	static double p5 = .5;
	static double p25 = .25;
	static double zero = 0.;

	/* System generated locals */
	long int r_dim1, r_offset, i__1, i__2, i__3;
	double d__1, d__2;

	/* Local variables */
	static double temp;
	static long int i, j, k, l;
	static double cotan;
	static long int nsing;
	static double qtbpj;
	static long int jp1, kp1;
	static double tan_, cos_, sin_, sum;

	/*   qrsolv

	given an m by n matrix a, an n by n diagonal matrix d,
	and an m-vector b, the problem is to determine an x which
	solves the system

	a*x = b ,     d*x = 0 ,

	in the least squares sense.

	this completes the solution of the problem
	if it is provided with the necessary information from the
	qr factorization, with column pivoting, of a. that is, if
	a*p = q*r, where p is a permutation matrix, q has orthogonal
	columns, and r is an upper triangular matrix with diagonal
	elements of nonincreasing magnitude, then qrsolv expects
	the full upper triangle of r, the permutation matrix p,
	and the first n components of (q transpose)*b. the system
	a*x = b, d*x = 0, is then equivalent to

	t       t
	r*z = q *b ,  p *d*p*z = 0 ,

	where x = p*z. if this system does not have full rank,
	then a least squares solution is obtained. on output qrsolv
	also provides an upper triangular matrix s such that

	t   t               t
	p *(a *a + d*d)*p = s *s .

	s is computed within qrsolv and may be of separate interest.

	the statement is

	qrsolv(n, r, ldr, ipvt, diag, qtb, x, sdiag, wa)

	where

	n is a positive long int input variable set to the order of r.

	r is an n by n array. on input the full upper triangle
	must contain the full upper triangle of the matrix r.
	on output the full upper triangle is unaltered, and the
	strict lower triangle contains the strict upper triangle
	(transposed) of the upper triangular matrix s.

	ldr is a positive long int input variable not less than n
	which specifies the leading dimension of the array r.

	ipvt is an long int input array of length n which defines the
	permutation matrix p such that a*p = q*r. column j of p
	is column ipvt(j) of the identity matrix.

	diag is an input array of length n which must contain the
	diagonal elements of the matrix d.

	qtb is an input array of length n which must contain the first
	n elements of the vector (q transpose)*b.

	x is an output array of length n which contains the least
	squares solution of the system a*x = b, d*x = 0.

	sdiag is an output array of length n which contains the
	diagonal elements of the upper triangular matrix s.

	wa is a work array of length n.

	subprograms called

	... dabs, dsqrt

	argonne national laboratory. minpack project.

	*/

	/* Parameter adjustments */
	--wa;
	--sdiag;
	--x;
	--qtb;
	--diag;
	--ipvt;
	r_dim1 = *ldr;
	r_offset = r_dim1 + 1;
	r -= r_offset;

	/* Function Body */

	/* copy r and (q transpose)*b to preserve input and initialize s.
	in particular, save the diagonal elements of r in x.
	*/

	i__1 = *n;
	for (j = 1; j <= i__1; ++j)
	{
		i__2 = *n;
		for (i = j; i <= i__2; ++i)
		{
			r[i + j * r_dim1] = r[j + i * r_dim1];
		}
		x[j] = r[j + j * r_dim1];
		wa[j] = qtb[j];
	}

	/* eliminate the diagonal matrix d using a givens rotation. */

	i__1 = *n;
	for (j = 1; j <= i__1; ++j)
	{

		/* prepare the row of d to be eliminated, locating the
		diagonal element using p from the qr factorization.
		*/

		l = ipvt[j];
		if (diag[l] == zero)
		{
			goto L90;
		}
		i__2 = *n;
		for (k = j; k <= i__2; ++k)
		{
			sdiag[k] = zero;
		}
		sdiag[j] = diag[l];

		/* the transformations to eliminate the row of d
		modify only a single element of (q transpose)*b
		beyond the first n, which is initially zero.
		*/

		qtbpj = zero;
		i__2 = *n;
		for (k = j; k <= i__2; ++k)
		{

			/* determine a givens rotation which eliminates the
			appropriate element in the current row of d.
			*/

			if (sdiag[k] == zero)
			{
				goto L70;
			}
			if ((d__1 = r[k + k * r_dim1], fabs(d__1)) >= (d__2 = sdiag[k], fabs(d__2)))
			{
				goto L40;
			}
			cotan = r[k + k * r_dim1] / sdiag[k];
			/* Computing 2nd power */
			d__1 = cotan;
			sin_ = p5 / sqrt(p25 + p25 * (d__1 * d__1));
			cos_ = sin_ * cotan;
			goto L50;
L40:
			tan_ = sdiag[k] / r[k + k * r_dim1];
			/* Computing 2nd power */
			d__1 = tan_;
			cos_ = p5 / sqrt(p25 + p25 * (d__1 * d__1));
			sin_ = cos_ * tan_;
L50:

			/* compute the modified diagonal element of r and
			the modified element of ((q transpose)*b, 0).
			*/

			r[k + k * r_dim1] = cos_ * r[k + k * r_dim1] + sin_ * sdiag[k];
			temp = cos_ * wa[k] + sin_ * qtbpj;
			qtbpj = -sin_ * wa[k] + cos_ * qtbpj;
			wa[k] = temp;

			/* accumulate the tranformation in the row of s. */

			kp1 = k + 1;
			if (*n < kp1)
			{
				goto L70;
			}
			i__3 = *n;
			for (i = kp1; i <= i__3; ++i)
			{
				temp = cos_ * r[i + k * r_dim1] + sin_ * sdiag[i];
				sdiag[i] = -sin_ * r[i + k * r_dim1] + cos_ * sdiag[i];
				r[i + k * r_dim1] = temp;
			}
L70:
			;
		}
L90:

		/* store the diagonal element of s and restore
		the corresponding diagonal element of r.
		*/

		sdiag[j] = r[j + j * r_dim1];
		r[j + j * r_dim1] = x[j];
	}

	/* solve the triangular system for z. if the system is
	singular, then obtain a least squares solution.
	*/

	nsing = *n;
	i__1 = *n;
	for (j = 1; j <= i__1; ++j)
	{
		if (sdiag[j] == zero && nsing == *n)
		{
			nsing = j - 1;
		}
		if (nsing < *n)
		{
			wa[j] = zero;
		}
	}
	if (nsing < 1)
	{
		goto L150;
	}
	i__1 = nsing;
	for (k = 1; k <= i__1; ++k)
	{
		j = nsing - k + 1;
		sum = zero;
		jp1 = j + 1;
		if (nsing < jp1)
		{
			goto L130;
		}
		i__2 = nsing;
		for (i = jp1; i <= i__2; ++i)
		{
			sum += r[i + j * r_dim1] * wa[i];
		}
L130:
		wa[j] = (wa[j] - sum) / sdiag[j];
	}
L150:

	/* permute the components of z back to components of x. */

	i__1 = *n;
	for (j = 1; j <= i__1; ++j)
	{
		l = ipvt[j];
		x[l] = wa[j];
	}
	return 0;

} /* qrsolv_ */

#undef dmach

long int qrfac_(long int *m, long int *n, double *a, long int *lda, long *pivot,
	long int *ipvt, long int *lipvt, double *rdiag, double *acnorm, double *wa)
{
	/* Initialized data */

	static double one = 1.;
	static double p05 = .05;
	static double zero = 0.;

	/* System generated locals */
	long int a_dim1, a_offset, i__1, i__2, i__3;
	double d__1, d__2, d__3;

	/* Local variables */
	static long int kmax;
	static double temp;
	static long int i, j, k, minmn;
	static double epsmch;
	static double ajnorm;
	static long int jp1;
	static double sum;

	/*   qrfac

	this uses householder transformations with column
	pivoting (optional) to compute a qr factorization of the
	m by n matrix a. that is, qrfac determines an orthogonal
	matrix q, a permutation matrix p, and an upper trapezoidal
	matrix r with diagonal elements of nonincreasing magnitude,
	such that a*p = q*r. the householder transformation for
	column k, k = 1, 2,..., min(m, n), is of the form

	t
	i - (1/u(k))*u*u

	where u has zeros in the first k-1 positions. the form of
	this transformation and the method of pivoting first
	appeared in the corresponding linpack function.

	the statement is

	qrfac(m, n, a, lda, pivot, ipvt, lipvt, rdiag, acnorm, wa)

	where

	m is a positive long int input variable set to the number
	of rows of a.

	n is a positive long int input variable set to the number
	of columns of a.

	a is an m by n array. on input a contains the matrix for
	which the qr factorization is to be computed. on output
	the strict upper trapezoidal part of a contains the strict
	upper trapezoidal part of r, and the lower trapezoidal
	part of a contains a factored form of q (the non-trivial
	elements of the u vectors described above).

	lda is a positive long int input variable not less than m
	which specifies the leading dimension of the array a.

	pivot is a logical input variable. if pivot is set FALSE,
	then column pivoting is enforced. if pivot is set FALSE,
	then no column pivoting is done.

	ipvt is an long int output array of length lipvt. ipvt
	defines the permutation matrix p such that a*p = q*r.
	column j of p is column ipvt(j) of the identity matrix.
	if pivot is FALSE, ipvt is not referenced.

	lipvt is a positive long int input variable. if pivot is FALSE,
	then lipvt may be as small as 1. if pivot is FALSE, then
	lipvt must be at least n.

	rdiag is an output array of length n which contains the
	diagonal elements of r.

	acnorm is an output array of length n which contains the
	norms of the corresponding columns of the input matrix a.
	if this information is not needed, then acnorm can coincide
	with rdiag.

	wa is a work array of length n. if pivot is FALSE, then wa
	can coincide with rdiag.

	subprograms called

	minpack-supplied ... dpmpar, enorm

	system-supplied ... dmax1, dsqrt, min0

	argonne national laboratory. minpack project.

	*/

	/* Parameter adjustments */
	--wa;
	--acnorm;
	--rdiag;
	a_dim1 = *lda;
	a_offset = a_dim1 + 1;
	a -= a_offset;
	--ipvt;

	/* Function Body */

	/* epsmch is the machine precision. */
	epsmch = dpmpar_(&c__1);
	/* compute the initial column norms and initialize several arrays. */

	i__1 = *n;
	for (j = 1; j <= i__1; ++j)
	{
		acnorm[j] = enorm_(m, &a[j * a_dim1 + 1]);
		rdiag[j] = acnorm[j];
		wa[j] = rdiag[j];
		if (*pivot)
		{
			ipvt[j] = j;
		}
	}

	/* reduce a to r with householder transformations. */

	minmn = min(*m, *n);
	i__1 = minmn;
	for (j = 1; j <= i__1; ++j)
	{
		if (! (*pivot))
		{
			goto L40;
		}

		/* bring the column of largest norm into the pivot position. */

		kmax = j;
		i__2 = *n;
		for (k = j; k <= i__2; ++k)
		{
			if (rdiag[k] > rdiag[kmax])
			{
				kmax = k;
			}
		}
		if (kmax == j)
		{
			goto L40;
		}
		i__2 = *m;
		for (i = 1; i <= i__2; ++i)
		{
			temp = a[i + j * a_dim1];
			a[i + j * a_dim1] = a[i + kmax * a_dim1];
			a[i + kmax * a_dim1] = temp;
		}
		rdiag[kmax] = rdiag[j];
		wa[kmax] = wa[j];
		k = ipvt[j];
		ipvt[j] = ipvt[kmax];
		ipvt[kmax] = k;
L40:

		/* compute the householder transformation to reduce the
		j-th column of a to a multiple of the j-th unit vector.
		*/

		i__2 = *m - j + 1;
		ajnorm = enorm_(&i__2, &a[j + j * a_dim1]);

		if (ajnorm == zero)
		{
			goto L100;
		}
		if (a[j + j * a_dim1] < zero)
		{
			ajnorm = -ajnorm;
		}
		i__2 = *m;
		for (i = j; i <= i__2; ++i)
		{
			a[i + j * a_dim1] /= ajnorm;
		}
		a[j + j * a_dim1] += one;

		/* apply the transformation to the remaining columns
		and update the norms.
		*/

		jp1 = j + 1;
		if (*n < jp1)
		{
			goto L100;
		}
		i__2 = *n;
		for (k = jp1; k <= i__2; ++k)
		{
			sum = zero;
			i__3 = *m;
			for (i = j; i <= i__3; ++i)
			{
				sum += a[i + j * a_dim1] * a[i + k * a_dim1];
			}
			temp = sum / a[j + j * a_dim1];
			i__3 = *m;
			for (i = j; i <= i__3; ++i)
			{
				a[i + k * a_dim1] -= temp * a[i + j * a_dim1];
			}
			if (! (*pivot) || rdiag[k] == zero)
			{
				goto L80;
			}
			temp = a[j + k * a_dim1] / rdiag[k];

			/* Computing MAX */

			/* Computing 2nd power */
			d__3 = temp;
			d__1 = zero, d__2 = one - d__3 * d__3;
			rdiag[k] *= sqrt((max(d__1, d__2)));
			/* Computing 2nd power */
			d__1 = rdiag[k] / wa[k];
			if (p05 * (d__1 * d__1) > epsmch)
			{
				goto L80;
			}
			i__3 = *m - j;
			rdiag[k] = enorm_(&i__3, &a[jp1 + k * a_dim1]);
			wa[k] = rdiag[k];
L80:
			;
		}
L100:
		rdiag[j] = -ajnorm;
	}
	return 0;

} /* qrfac_ */

long int lmpar_(long int *n, double *r, long int *ldr, long int *ipvt,
	double *diag, double *qtb, double *delta, double *par,
	double *x, double *sdiag, double *wa1, double *wa2)
{
	/* Initialized data */

	static double p1 = .1;
	static double p001 = .001;
	static double zero = 0.;

	/* System generated locals */
	int r_dim1, r_offset, i__1, i__2;
	double d__1, d__2;

	/* Local variables */
	static double parc, parl;
	static int iter;
	static double temp, paru;
	static int i, j, k, l;
	static double dwarf;
	static long int nsing;
	static double gnorm, fp;
	static double dxnorm;
	static long int jm1, jp1;
	static double sum;

	/* lmpar

	given an m by n matrix a, an n by n nonsingular diagonal
	matrix d, an m-vector b, and a positive number delta,
	the problem is to determine a value for the parameter
	par such that if x solves the system

	a*x = b ,     sqrt(par)*d*x = 0 ,

	in the least squares sense, and dxnorm is the euclidean
	norm of d*x, then either par is zero and

	(dxnorm-delta) .le. 0.1*delta ,

	or par is positive and

	abs(dxnorm-delta) .le. 0.1*delta .

	this completes the solution of the problem
	if it is provided with the necessary information from the
	qr factorization, with column pivoting, of a. that is, if
	a*p = q*r, where p is a permutation matrix, q has orthogonal
	columns, and r is an upper triangular matrix with diagonal
	elements of nonincreasing magnitude, then lmpar expects
	the full upper triangle of r, the permutation matrix p,
	and the first n components of (q transpose)*b. on output
	lmpar also provides an upper triangular matrix s such that

	t   t                   t
	p *(a *a + par*d*d)*p = s *s .

	s is employed within lmpar and may be of separate interest.

	only a few iterations are generally needed for convergence
	of the algorithm. if, however, the limit of 10 iterations
	is reached, then the output par will contain the best
	value obtained so far.

	the statement is

	lmpar(n, r, ldr, ipvt, diag, qtb, delta, par, x, sdiag,
	wa1, wa2)

	where

	n is a positive long int input variable set to the order of r.

	r is an n by n array. on input the full upper triangle
	must contain the full upper triangle of the matrix r.
	on output the full upper triangle is unaltered, and the
	strict lower triangle contains the strict upper triangle
	(transposed) of the upper triangular matrix s.

	ldr is a positive long int input variable not less than n
	which specifies the leading dimension of the array r.

	ipvt is an long int input array of length n which defines the
	permutation matrix p such that a*p = q*r. column j of p
	is column ipvt(j) of the identity matrix.

	diag is an input array of length n which must contain the
	diagonal elements of the matrix d.

	qtb is an input array of length n which must contain the first
	n elements of the vector (q transpose)*b.

	delta is a positive input variable which specifies an upper
	bound on the euclidean norm of d*x.

	par is a nonnegative variable. on input par contains an
	initial estimate of the levenberg-marquardt parameter.
	on output par contains the final estimate.

	x is an output array of length n which contains the least
	squares solution of the system a*x = b, sqrt(par)*d*x = 0,
	for the output par.

	sdiag is an output array of length n which contains the
	diagonal elements of the upper triangular matrix s.

	wa1 and wa2 are work arrays of length n.

	subprograms called

	minpack-supplied ... dpmpar, enorm, qrsolv

	system-supplied ... dabs, dmax1, dmin1, dsqrt

	argonne national laboratory. minpack project.

	*/

	/* Parameter adjustments */
	--wa2;
	--wa1;
	--sdiag;
	--x;
	--qtb;
	--diag;
	--ipvt;
	r_dim1 = *ldr;
	r_offset = r_dim1 + 1;
	r -= r_offset;

	/* Function Body */

	/* dwarf is the smallest positive magnitude. */

	dwarf = dpmpar_(&c__2);

	/* compute and store in x the gauss-newton direction. if the
	jacobian is rank-deficient, obtain a least squares solution.
	*/

	nsing = *n;
	i__1 = *n;
	for (j = 1; j <= i__1; ++j)
	{
		wa1[j] = qtb[j];
		if (r[j + j * r_dim1] == zero && nsing == *n)
		{
			nsing = j - 1;
		}
		if (nsing < *n)
		{
			wa1[j] = zero;
		}
	}
	if (nsing < 1)
	{
		goto L50;
	}
	i__1 = nsing;
	for (k = 1; k <= i__1; ++k)
	{
		j = nsing - k + 1;
		wa1[j] /= r[j + j * r_dim1];
		temp = wa1[j];
		jm1 = j - 1;
		if (jm1 < 1)
		{
			goto L30;
		}
		i__2 = jm1;
		for (i = 1; i <= i__2; ++i)
		{
			wa1[i] -= r[i + j * r_dim1] * temp;
		}
L30:
		;
	}
L50:
	i__1 = *n;
	for (j = 1L; j <= i__1; ++j)
	{
		l = ipvt[j];
		x[l] = wa1[j];
	}

	/* initialize the iteration counter.
	evaluate the function at the origin, and test
	for acceptance of the gauss-newton direction.
	*/

	iter = 0;
	i__1 = *n;
	for (j = 1L; j <= i__1; ++j)
	{
		wa2[j] = diag[j] * x[j];
	}
	dxnorm = enorm_(n, &wa2[1]);
	fp = dxnorm - *delta;
	if (fp <= p1 * *delta)
	{
		goto L220;
	}

	/* if the jacobian is not rank deficient, the newton
	step provides a lower bound, parl, for the zero of
	the function. otherwise set this bound to zero.
	*/

	parl = zero;
	if (nsing < *n)
	{
		goto L120;
	}
	i__1 = *n;
	for (j = 1; j <= i__1; ++j)
	{
		l = ipvt[j];
		wa1[j] = diag[l] * (wa2[l] / dxnorm);
	}
	i__1 = *n;
	for (j = 1; j <= i__1; ++j)
	{
		sum = zero;
		jm1 = j - 1;
		if (jm1 < 1)
		{
			goto L100;
		}
		i__2 = jm1;
		for (i = 1; i <= i__2; ++i)
		{
			sum += r[i + j * r_dim1] * wa1[i];
		}
L100:
		wa1[j] = (wa1[j] - sum) / r[j + j * r_dim1];
	}
	temp = enorm_(n, &wa1[1]);
	parl = fp / *delta / temp / temp;
L120:

	/* calculate an upper bound, paru, for the zero of the function. */

	i__1 = *n;
	for (j = 1; j <= i__1; ++j)
	{
		sum = zero;
		i__2 = j;
		for (i = 1; i <= i__2; ++i)
		{
			sum += r[i + j * r_dim1] * qtb[i];
		}
		l = ipvt[j];
		wa1[j] = sum / diag[l];
	}
	gnorm = enorm_(n, &wa1[1]);
	paru = gnorm / *delta;
	if (paru == zero)
	{
		paru = dwarf / min(*delta, p1);
	}

	/* if the input par lies outside of the interval (parl, paru),
	set par to the closer endpoint.
	*/

	*par = max(*par, parl);
	*par = min(*par, paru);
	if (*par == zero)
	{
		*par = gnorm / dxnorm;
	}

	/* beginning of an iteration. */

L150:
	++iter;

	/* evaluate the function at the current value of par. */

	if (*par == zero)
	{
		/* Computing MAX */
		d__1 = dwarf, d__2 = p001 * paru;
		*par = max(d__1, d__2);
	}
	temp = sqrt(*par);
	i__1 = *n;
	for (j = 1; j <= i__1; ++j)
	{
		wa1[j] = temp * diag[j];
	}
	qrsolv_(n, &r[r_offset], ldr, &ipvt[1], &wa1[1], &qtb[1], &x[1], &sdiag[1]
	, &wa2[1]);
	i__1 = *n;
	for (j = 1; j <= i__1; ++j)
	{
		wa2[j] = diag[j] * x[j];
	}
	dxnorm = enorm_(n, &wa2[1]);
	temp = fp;
	fp = dxnorm - *delta;

	/* if the function is small enough, accept the current value
	of par. also test for the exceptional cases where parl
	is zero or the number of iterations has reached 10.
	*/

	if (fabs(fp) <= p1 * *delta || parl == zero && fp <= temp && temp < zero || iter == 10)
	{
		goto L220;
	}

	/* compute the newton correction. */

	i__1 = *n;
	for (j = 1; j <= i__1; ++j)
	{
		l = ipvt[j];
		wa1[j] = diag[l] * (wa2[l] / dxnorm);
	}
	i__1 = *n;
	for (j = 1; j <= i__1; ++j)
	{
		wa1[j] /= sdiag[j];
		temp = wa1[j];
		jp1 = j + 1;
		if (*n < jp1)
		{
			goto L200;
		}
		i__2 = *n;
		for (i = jp1; i <= i__2; ++i)
		{
			wa1[i] -= r[i + j * r_dim1] * temp;
		}
L200:
		;
	}
	temp = enorm_(n, &wa1[1]);
	parc = fp / *delta / temp / temp;

	/* depending on the sign of the function, update parl or paru. */

	if (fp > zero)
	{
		parl = max(parl, *par);
	}
	if (fp < zero)
	{
		paru = min(paru, *par);
	}

	/* compute an improved estimate for par. */

	/* Computing MAX */
	d__1 = parl, d__2 = *par + parc;
	*par = max(d__1, d__2);

	/* end of an iteration. */

	goto L150;
L220:

	/* termination. */

	if (iter == 0)
	{
		*par = zero;
	}
	return 0;

} /* lmpar_ */

long int lmder_(long int (*fcn)(long int *m, long int *n, double *x, double *fvec,
	double *fjac, long int *ldfjac, long int *iflag),
	long int *m, long int *n, double *x, double *fvec,
	double *fjac, long int *ldfjac, double *ftol, double *xtol,
	double *gtol, long int *maxfev, double *diag, long int *mode,
	double *factor, long int *nprint, long int *info, long int *nfev,
	long int *njev, long int *ipvt, double *qtf,
	double *wa1, double *wa2, double *wa3, double *wa4)
{
	/* Initialized data */
	static double one = 1.;
	static double p1 = .1;
	static double p5 = .5;
	static double p25 = .25;
	static double p75 = .75;
	static double p0001 = 1e-4;
	static double zero = 0.;

	/* System generated locals */
	long int fjac_dim1, fjac_offset, i__1, i__2;
	double d__1, d__2, d__3;

	/* Local variables */
	static long int iter;
	static double temp, temp1, temp2;
	static long int i, j, l, iflag;
	static double delta;
	static double ratio;
	static double fnorm, gnorm, pnorm, xnorm, fnorm1, actred, dirder,
		epsmch, prered;
	static double par, sum;

	/* lmder

	the purpose of lmder is to minimize the sum of the squares of
	m nonlinear functions in n variables by a modification of
	the levenberg-marquardt algorithm. the user must provide a
	function which calculates the functions and the jacobian.

	the statement is

	lmder(fcn, m, n, x, fvec, fjac, ldfjac, ftol, xtol, gtol,
	maxfev, diag, mode, factor, nprint, info, nfev,
	njev, ipvt, qtf, wa1, wa2, wa3, wa4)

	where

	fcn is the name of the user-supplied function which
	calculates the functions and the jacobian. fcn must
	be declared in an external statement in the user
	calling program, and should be written as follows.

	function fcn(m, n, x, fvec, fjac, ldfjac, iflag)
	long int m, n, ldfjac, iflag
	double precision x(n), fvec(m), fjac(ldfjac, n)
	----------
	if iflag = 1 calculate the functions at x and
	return this vector in fvec. do not alter fjac.
	if iflag = 2 calculate the jacobian at x and
	return this matrix in fjac. do not alter fvec.
	----------
	return
	end

	the value of iflag should not be changed by fcn unless
	the user wants to terminate execution of lmder.
	in this case set iflag to a negative long int.

	m is a positive long int input variable set to the number
	of functions.

	n is a positive long int input variable set to the number
	of variables. n must not exceed m.

	x is an array of length n. on input x must contain
	an initial estimate of the solution vector. on output x
	contains the final estimate of the solution vector.

	fvec is an output array of length m which contains
	the functions evaluated at the output x.

	fjac is an output m by n array. the upper n by n submatrix
	of fjac contains an upper triangular matrix r with
	diagonal elements of nonincreasing magnitude such that

	t     t           t
	p *(jac *jac)*p = r *r,

	where p is a permutation matrix and jac is the final
	calculated jacobian. column j of p is column ipvt(j)
	(see below) of the identity matrix. the lower trapezoidal
	part of fjac contains information generated during
	the computation of r.

	ldfjac is a positive long int input variable not less than m
	which specifies the leading dimension of the array fjac.

	ftol is a nonnegative input variable. termination
	occurs when both the actual and predicted relative
	reductions in the sum of squares are at most ftol.
	therefore, ftol measures the relative error desired
	in the sum of squares.

	xtol is a nonnegative input variable. termination
	occurs when the relative error between two consecutive
	iterates is at most xtol. therefore, xtol measures the
	relative error desired in the approximate solution.

	gtol is a nonnegative input variable. termination
	occurs when the cosine of the angle between fvec and
	any column of the jacobian is at most gtol in absolute
	value. therefore, gtol measures the orthogonality
	desired between the function vector and the columns
	of the jacobian.

	maxfev is a positive long int input variable. termination
	occurs when the number of calls to fcn with iflag = 1
	has reached maxfev.

	diag is an array of length n. if mode = 1 (see
	below), diag is internally set. if mode = 2, diag
	must contain positive entries that serve as
	multiplicative scale factors for the variables.

	mode is an long int input variable. if mode = 1, the
	variables will be scaled internally. if mode = 2,
	the scaling is specified by the input diag. other
	values of mode are equivalent to mode = 1.

	factor is a positive input variable used in determining the
	initial step bound. this bound is set to the product of
	factor and the euclidean norm of diag*x if nonzero, or else
	to factor itself. in most cases factor should lie in the
	interval (.1, 100.).100. is a generally recommended value.

	nprint is an long int input variable that enables controlled
	printing of iterates if it is positive. in this case,
	fcn is called with iflag = 0 at the beginning of the first
	iteration and every nprint iterations thereafter and
	immediately prior to return, with x, fvec, and fjac
	available for printing. fvec and fjac should not be
	altered. if nprint is not positive, no special calls
	of fcn with iflag = 0 are made.

	info is an long int output variable. if the user has
	terminated execution, info is set to the (negative)
	value of iflag. see description of fcn. otherwise,
	info is set as follows.

	info = 0  improper input parameters.

	info = 1  both actual and predicted relative reductions
	in the sum of squares are at most ftol.

	info = 2  relative error between two consecutive iterates
	is at most xtol.

	info = 3  conditions for info = 1 and info = 2 both hold.

	info = 4  the cosine of the angle between fvec and any
	column of the jacobian is at most gtol in
	absolute value.

	info = 5  number of calls to fcn with iflag = 1 has
	reached maxfev.

	info = 6  ftol is too small. no further reduction in
	the sum of squares is possible.

	info = 7  xtol is too small. no further improvement in
	the approximate solution x is possible.

	info = 8  gtol is too small. fvec is orthogonal to the
	columns of the jacobian to machine precision.

	nfev is an long int output variable set to the number of
	calls to fcn with iflag = 1.

	njev is an long int output variable set to the number of
	calls to fcn with iflag = 2.

	ipvt is an long int output array of length n. ipvt
	defines a permutation matrix p such that jac*p = q*r,
	where jac is the final calculated jacobian, q is
	orthogonal (not stored), and r is upper triangular
	with diagonal elements of nonincreasing magnitude.
	column j of p is column ipvt(j) of the identity matrix.

	qtf is an output array of length n which contains
	the first n elements of the vector (q transpose)*fvec.

	wa1, wa2, and wa3 are work arrays of length n.

	wa4 is a work array of length m.

	subprograms called

	user-supplied ...... fcn

	minpack-supplied ... dpmpar, enorm, lmpar, qrfac

	system-supplied ... dabs, dmax1, dmin1, dsqrt, mod

	argonne national laboratory. minpack project.

	*/

	/* Parameter adjustments */

	--wa4;
	--fvec;
	--wa3;
	--wa2;
	--wa1;
	--qtf;
	--ipvt;
	--diag;
	--x;
	fjac_dim1 = *ldfjac;
	fjac_offset = fjac_dim1 + 1;
	fjac -= fjac_offset;

	/* Function Body */

	/* epsmch is the machine precision. */
	epsmch = dpmpar_(&c__1);
	*info = 0;
	iflag = 0;
	*nfev = 0;
	*njev = 0;

	/* check the input parameters for errors. */

	if (*n <= 0 || *m < *n || *ldfjac < *m || *ftol < zero || *xtol < zero ||
		*gtol < zero || *maxfev <= 0 || *factor <= zero)
	{
		goto L300;
	}
	if (*mode != 2)
	{
		goto L20;
	}
	i__1 = *n;
	for (j = 1; j <= i__1; ++j)
	{
		if (diag[j] <= zero)
		{
			goto L300;
		}
	}
L20:

	// evaluate the function at the starting point
	// and calculate its norm.

	iflag = 1;
	(*fcn)(m, n, &x[1], &fvec[1], &fjac[fjac_offset], ldfjac, &iflag);
	*nfev = 1;
	if (iflag < 0)
	{
		goto L300;
	}
	fnorm = enorm_(m, &fvec[1]);

	// initialize levenberg-marquardt parameter and iteration counter.

	par = zero;
	iter = 1;

	// beginning of the outer loop.

L30:

	// calculate the jacobian matrix.

	iflag = 2;
	(*fcn)(m, n, &x[1], &fvec[1], &fjac[fjac_offset], ldfjac, &iflag);
	++(*njev);
	if (iflag < 0)
	{
		goto L300;
	}

	// if requested, call fcn to enable printing of iterates.

	if (*nprint <= 0)
	{
		goto L40;
	}
	iflag = 0;
	if ((iter - 1) % *nprint == 0)
	{
		(*fcn)(m, n, &x[1], &fvec[1], &fjac[fjac_offset], ldfjac, &iflag);
	}
	if (iflag < 0)
	{
		goto L300;
	}
L40:

	// compute the qr factorization of the jacobian.

	qrfac_(m, n, &fjac[fjac_offset], ldfjac, &c__1, &ipvt[1], n, &wa1[1], &
		wa2[1], &wa3[1]);

	// on the first iteration and if mode is 1, scale according
	// to the norms of the columns of the initial jacobian.

	if (iter != 1)
	{
		goto L80;
	}
	if (*mode == 2)
	{
		goto L60;
	}
	i__1 = *n;
	for (j = 1; j <= i__1; ++j)
	{
		diag[j] = wa2[j];
		if (wa2[j] == zero)
		{
			diag[j] = one;
		}
	}
L60:

	/* on the first iteration, calculate the norm of the scaled x
	and initialize the step bound delta. */

	i__1 = *n;
	for (j = 1; j <= i__1; ++j)
	{
		wa3[j] = diag[j] * x[j];
	}
	xnorm = enorm_(n, &wa3[1]);
	delta = *factor * xnorm;
	if (delta == zero)
	{
		delta = *factor;
	}
L80:

	/* form (q transpose)*fvec and store the first n components in qtf. */

	i__1 = *m;
	for (i = 1; i <= i__1; ++i)
	{
		wa4[i] = fvec[i];
	}
	i__1 = *n;
	for (j = 1; j <= i__1; ++j)
	{
		if (fjac[j + j * fjac_dim1] == zero)
		{
			goto L120;
		}
		sum = zero;
		i__2 = *m;
		for (i = j; i <= i__2; ++i)
		{
			sum += fjac[i + j * fjac_dim1] * wa4[i];
		}
		temp = -sum / fjac[j + j * fjac_dim1];
		i__2 = *m;
		for (i = j; i <= i__2; ++i)
		{
			wa4[i] += fjac[i + j * fjac_dim1] * temp;
		}
L120:
		fjac[j + j * fjac_dim1] = wa1[j];
		qtf[j] = wa4[j];
	}

	/* compute the norm of the scaled gradient. */

	gnorm = zero;
	if (fnorm == zero)
	{
		goto L170;
	}
	i__1 = *n;
	for (j = 1; j <= i__1; ++j)
	{
		l = ipvt[j];
		if (wa2[l] == zero)
		{
			goto L150;
		}
		sum = zero;
		i__2 = j;
		for (i = 1; i <= i__2; ++i)
		{
			sum += fjac[i + j * fjac_dim1] * (qtf[i] / fnorm);
		}
		/* Computing MAX */
		d__2 = gnorm, d__3 = (d__1 = sum / wa2[l], fabs(d__1));
		gnorm = max(d__2, d__3);
L150:
		;
	}
L170:

	/* test for convergence of the gradient norm. */

	if (gnorm <= *gtol)
	{
		*info = 4;
	}
	if (*info != 0)
	{
		goto L300;
	}

	/* rescale if necessary. */

	if (*mode == 2)
	{
		goto L190;
	}
	i__1 = *n;
	for (j = 1; j <= i__1; ++j)
	{
		/* Computing MAX */
		d__1 = diag[j], d__2 = wa2[j];
		diag[j] = max(d__1, d__2);
	}
L190:

	/* beginning of the inner loop. */

L200:

	/* determine the levenberg-marquardt parameter. */
	lmpar_(n, &fjac[fjac_offset], ldfjac, &ipvt[1], &diag[1], &qtf[1], &delta,
		&par, &wa1[1], &wa2[1], &wa3[1], &wa4[1]);

	/* store the direction p and x + p. calculate the norm of p. */

	i__1 = *n;
	for (j = 1; j <= i__1; ++j)
	{
		wa1[j] = -wa1[j];
		wa2[j] = x[j] + wa1[j];
		wa3[j] = diag[j] * wa1[j];
	}
	pnorm = enorm_(n, &wa3[1]);

	/* on the first iteration, adjust the initial step bound. */

	if (iter == 1)
	{
		delta = min(delta, pnorm);
	}

	/* evaluate the function at x + p and calculate its norm. */

	iflag = 1;
	(*fcn)(m, n, &wa2[1], &wa4[1], &fjac[fjac_offset], ldfjac, &iflag);
	++(*nfev);
	if (iflag < 0)
	{
		goto L300;
	}
	fnorm1 = enorm_(m, &wa4[1]);

	/* compute the scaled actual reduction. */

	actred = -one;
	if (p1 * fnorm1 < fnorm)
	{
		/* Computing 2nd power */
		d__1 = fnorm1 / fnorm;
		actred = one - d__1 * d__1;
	}

	/* compute the scaled predicted reduction and
	the scaled directional derivative.
	*/

	i__1 = *n;
	for (j = 1; j <= i__1; ++j)
	{
		wa3[j] = zero;
		l = ipvt[j];
		temp = wa1[l];
		i__2 = j;
		for (i = 1; i <= i__2; ++i)
		{
			wa3[i] += fjac[i + j * fjac_dim1] * temp;
		}
	}
	temp1 = enorm_(n, &wa3[1]) / fnorm;
	temp2 = sqrt(par) * pnorm / fnorm;
	/* Computing 2nd power */
	d__1 = temp1;
	/* Computing 2nd power */
	d__2 = temp2;
	prered = d__1 * d__1 + d__2 * d__2 / p5;
	/* Computing 2nd power */
	d__1 = temp1;
	/* Computing 2nd power */
	d__2 = temp2;
	dirder = -(d__1 * d__1 + d__2 * d__2);

	/* compute the ratio of the actual to the predicted
	reduction.
	*/

	ratio = zero;
	if (prered != zero)
	{
		ratio = actred / prered;
	}

	/* update the step bound. */

	if (ratio > p25)
	{
		goto L240;
	}
	if (actred >= zero)
	{
		temp = p5;
	}
	if (actred < zero)
	{
		temp = p5 * dirder / (dirder + p5 * actred);
	}
	if (p1 * fnorm1 >= fnorm || temp < p1)
	{
		temp = p1;
	}
	/* Computing MIN */
	d__1 = delta, d__2 = pnorm / p1;
	delta = temp * min(d__1, d__2);
	par /= temp;
	goto L260;
L240:
	if (par != zero && ratio < p75)
	{
		goto L250;
	}
	delta = pnorm / p5;
	par = p5 * par;
L250:
L260:

	/* test for successful iteration. */

	if (ratio < p0001)
	{
		goto L290;
	}

	/* successful iteration. update x, fvec, and their norms. */

	i__1 = *n;
	for (j = 1; j <= i__1; ++j)
	{
		x[j] = wa2[j];
		wa2[j] = diag[j] * x[j];
	}
	i__1 = *m;
	for (i = 1; i <= i__1; ++i)
	{
		fvec[i] = wa4[i];
	}
	xnorm = enorm_(n, &wa2[1]);
	fnorm = fnorm1;
	++iter;
L290:

	/* tests for convergence. */

	if (fabs(actred) <= *ftol && prered <= *ftol && p5 * ratio <= one)
	{
		*info = 1;
	}
	if (delta <= *xtol * xnorm)
	{
		*info = 2;
	}
	if (fabs(actred) <= *ftol && prered <= *ftol && p5 * ratio <= one && *info == 2)
	{
		*info = 3;
	}
	if (*info != 0)
	{
		goto L300;
	}

	/* tests for termination and stringent tolerances. */

	if (*nfev >= *maxfev)
	{
		*info = 5;
	}
	if (fabs(actred) <= epsmch && prered <= epsmch && p5 * ratio <= one)
	{
		*info = 6;
	}
	if (delta <= epsmch * xnorm)
	{
		*info = 7;
	}
	if (gnorm <= epsmch)
	{
		*info = 8;
	}
	if (*info != 0)
	{
		goto L300;
	}

	/*  end of the inner loop. repeat if iteration unsuccessful. */

	if (ratio < p0001)
	{
		goto L200;
	}

	/* end of the outer loop. */

	goto L30;
L300:

	/* termination, either normal or user imposed. */
	if (iflag < 0)
	{
		*info = iflag;
	}
	iflag = 0;
	if (*nprint > 0)
	{
		(*fcn)(m, n, &x[1], &fvec[1], &fjac[fjac_offset], ldfjac, &iflag);
	}
	return 0;
}

long int fcn_l1(long int *m, long int *n, double *x, double *fvec, double *fjac,
	long int *ldfjac, long int *iflag)
{
	long int k, mm = *m;
	double temp1, temp2, temp3;

	/**** P==2 in calling program ****/
	if ( p == 1.0 )
	{
		if (*iflag == 1)
		{  /* evaluate function only */
			for(k=12;k<mm;k++)
			{
				temp3=fvec[k];
			}
			for(k=0; k<mm; k++)
				fvec[k] = w[k]*(v[k] - x[0]*u[k] - x[1]);
		}

		if (*iflag == 2)
		{  /* evaluate Jacobian only */
			for(k=0; k<mm; k++)
			{
				fjac[k] = -w[k]*u[k];
				fjac[k+mm] = -w[k];
			}
		}
	}

	/**** P!=2 in calling program ****/
	if ( p !=  1.0 && p > 0.5 )
	{
		if (*iflag == 1)
		{  /* evaluate function only */
			for(k=0; k<mm; k++)
				fvec[k] = w[k]*pow(MYABS(v[k] - x[0]*u[k] - x[1]), p);
		}

		if (*iflag == 2)
		{  /* evaluate Jacobian only */
			for(k=0; k<mm; k++)
			{
				temp1 = v[k] - x[0]*u[k] - x[1];
				temp2 = p * pow(MYABS(temp1), p-1.0)*signum(temp1);
				fjac[k] = -w[k]*u[k]*temp2;
				fjac[k+mm] = -w[k]*temp2;
			}
		}
	}
	return 0L;
}

void lm_min(long int (*fcn)(long int *m, long int *n, double *x, double *fvec,
	double *fjac, long int *ldfjac, long int *iflag),
	long int m, long int n, double *x, double xtol, double *z, double *t,
	double *W, double P )
{
	double *fvec, *fjac, ftol, gtol, *diag, factor = 100.0, *qtf;
	double *wa1, *wa2, *wa3, *wa4;
	long int ldfjac = m, maxfev = 100, mode = 1, nprint = 0;
	long int info, nfev, njev, *ipvt;

	/* set global variables */
	u = z; v = t;
	w = W;
	p = P/2.0;

	/* set tolerances */
	ftol = 1.0e-8; gtol = 1.0e-8;

	/* workspace allocation */
	fjac = dvectorl(0L, m*n-1);
	fvec = dvectorl(0L, m-1);
	diag = dvectorl(0L, n-1);
	qtf  = dvectorl(0L, n-1);
	wa1  = dvectorl(0L, n-1);
	wa2  = dvectorl(0L, n-1);
	wa3  = dvectorl(0L, n-1);
	wa4  = dvectorl(0L, m-1);
	ipvt = ivectorl(0L, n-1);

	info=-1L;

	lmder_(fcn, &m, &n, x, fvec, fjac, &ldfjac, &ftol, &xtol,
		&gtol, &maxfev, diag, &mode, &factor, &nprint, &info, &nfev, &njev,
		ipvt, qtf, wa1, wa2, wa3, wa4);

	/* free workspace */
	free_dvectorl(fjac, 0L, m*n-1);
	free_dvectorl(fvec, 0L, m-1);
	free_dvectorl(diag, 0L, n-1);
	free_dvectorl(qtf, 0L, n-1);
	free_dvectorl(wa1, 0L, n-1);
	free_dvectorl(wa2, 0L, n-1);
	free_dvectorl(wa3, 0L, n-1);
	free_dvectorl(wa4, 0L, m-1);
	free_ivectorl(ipvt, 0L, n-1);
}