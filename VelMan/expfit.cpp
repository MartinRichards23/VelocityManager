#include <math.h>
#include <stdio.h>
#include "NRUTIL.h"
#include "expfit.h"

#undef SIGN
#define SIGN(z) ( (z) > 0.0 ? 1 : -1 )

extern int dfpmin( double* p, int n, double ftol, int* iter, double* fret,
				  double (*func)( double* ), void ( *dfunc)( double*, double* ));

// global variables
int _Gm;
double *_Gt1, *_Gt2, *_Gz1, *_Gz2;
double *_Gw; // weights
double _Gpenalty;
double _Gp;

int expfit(int m, double *dt, double *z1, double *z2, double *w, double p, double *alpha)
		   // Input:
		   // dt[1..m] - time differences at top/bottom
		   // z1[1..m], z2[1..m] - depths at top/bottom
		   // w[1..m] - weights
		   // p - determines p-norm. MUST have p>1.
		   // Output:
		   // alpha[1..2]
		   //
{

	int k, iter, i;
	double fret, ftol = 1.0e-11;
	int result;
	double sum, number;

	//
	// set globals
	//_Gt1 = t1;
	_Gt2 = dt;
	_Gz1 = z1;
	_Gz2 = z2;
	_Gw  =  w;
	_Gm  =  m;

	_Gpenalty = 0.0;
	_Gp = p;

	_Gt1 = dvector(1, m);
	for(k=1; k<=m; k++)
		_Gt1[k] = 0.0;

	//
	// do a rough calculation to come up with a good starting point
	// alpha[1] = max(x)   then term exp(t/alpha1) can be expanded well and is
	//	                   more or less linear
	// alpha[2] = avg of y/x

	number=0;
	sum=0;
	alpha[1]=1.0;
	for(i=1;i<=m;i++)
	{
		if(dt[i]>0.0)
		{
			if(dt[i]>alpha[1])
				alpha[1]=dt[i];
			sum+=z2[i]/dt[i];
			number++;
		}
	}
	if(number>0)
		alpha[2]=sum/number;
	else
		alpha[2]=1000;

	result=dfpmin(alpha, 2, ftol, &iter, &fret, fp_exp, dfp_exp);

	//   TRACE("expfit: alpha[1] = %f, alpha[2] = %f \n", alpha[1], alpha[2] );

	alpha[2] = fabs(alpha[2]);
	free_dvector(_Gt1, 1, m);
	return result;
}

/*****************************************************************/

#define SQR(z) ((z)*(z))

double f_exp(double *alpha)
// alpha[1..2]
{
	int k;
	double s = 0.0, temp;

	for(k=1; k <= _Gm; k++)
	{
		if((_Gt2[k]-_Gt1[k])==0.0)
			continue;
		temp = (_Gt2[k]-_Gt1[k]) - alpha[1]*log((_Gz2[k]+fabs(alpha[2]))/(_Gz1[k]+fabs(alpha[2])));
		s += SQR(temp/(_Gt2[k]-_Gt1[k]));
	}

	return s/_Gm + _Gpenalty*SQR(alpha[2]);
}

/*****************************************************************/

void df_exp(double *alpha, double *g)
// alpha[1..2], g[1..2]
{
	int k;
	double D1 = 0.0, D2 = 0.0, temp;

	for(k=1; k <= _Gm; k++)
	{
		if((_Gt2[k]-_Gt1[k])==0.0)
			continue;
		temp = ((_Gt2[k]-_Gt1[k]) - alpha[1]*log((_Gz2[k]+fabs(alpha[2]))/(_Gz1[k]+fabs(alpha[2]))))/(_Gt2[k]-_Gt1[k]);
		D1 += temp*log((_Gz2[k]+fabs(alpha[2]))/(_Gz1[k]+fabs(alpha[2])))/(_Gt2[k]-_Gt1[k]);
		D2 += temp*(_Gz1[k] - _Gz2[k])/((_Gz2[k]+fabs(alpha[2]))*(_Gz1[k]+fabs(alpha[2])))/(_Gt2[k]-_Gt1[k]);
	}
	g[1] = -2.0*D1/_Gm;
	if (alpha[2] == 0.0)
		printf("alpha[2] = 0.0\n");
	g[2] = -2.0*alpha[1]*SIGN(alpha[2])*D2/_Gm + 2.0*_Gpenalty*alpha[2];
}

/*****************************************************************/

void setup(int m, double *t1, double *t2, double *z1, double *z2,
		   double *weight, double penalty, double p)
{
	// set globals
	_Gt1 = t1;
	_Gt2 = t2;
	_Gz1 = z1;
	_Gz2 = z2;
	_Gw  = weight;
	_Gm  =  m;

	_Gpenalty = penalty;
	_Gp = p;
}

/*****************************************************************/

double fp_exp(double *alpha)
// alpha[1..2]
{
	int k;
	double s = 0.0, temp, temp1, temp2;

	for(k=1; k <= _Gm; k++)
	{
		if((_Gt2[k]-_Gt1[k])==0.0)
			continue;
		temp = _Gt2[k] - _Gt1[k] - alpha[1]*log( (_Gz2[k]+fabs(alpha[2]))/(_Gz1[k]+fabs(alpha[2])) );
		temp1 = pow(fabs(temp/(_Gt2[k]-_Gt1[k])), _Gp);
		temp2 = fabs(temp/(_Gt2[k]-_Gt1[k]));
		s += _Gw[k]*temp1;
	}

	return pow(s, 1.0/_Gp);
}

/*****************************************************************/

void dfp_exp(double *alpha, double *g)
// alpha[1..2], g[1..2]
{
	int k;
	double D1 = 0.0, D2 = 0.0, temp1, temp2, temp3;

	for(k=1; k <= _Gm; k++)
	{
		if((_Gt2[k]-_Gt1[k])==0.0)
			continue;
		temp1 = (_Gt2[k]-_Gt1[k]) - alpha[1]*log((_Gz2[k]+fabs(alpha[2]))/(_Gz1[k]+fabs(alpha[2])));
		temp1 = temp1/(_Gt2[k]-_Gt1[k]);
		temp2 = pow(fabs(temp1), _Gp-1.0)*SIGN(temp1);
		D1 += _Gw[k]*temp2*log( ( _Gz2[k]+fabs(alpha[2]) ) / ( _Gz1[k]+fabs(alpha[2]) ) )/ ( _Gt2[k] - _Gt1[k] );
		temp3 = (_Gz1[k] - _Gz2[k])/( (_Gz1[k]+fabs(alpha[2]))*(_Gz2[k]+fabs(alpha[2])));
		D2 += _Gw[k]*temp2*alpha[1]*SIGN(alpha[2])*temp3/(_Gt2[k]-_Gt1[k]);
	}

	temp1 = pow(fp_exp(alpha), 1.0-_Gp);
	g[1] = -D1*temp1;
	g[2] = -D2*temp1;
}
