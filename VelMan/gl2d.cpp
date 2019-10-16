#include <stdio.h>
#include <time.h>
#include <math.h>
#include <stdlib.h>

#include "nrutil.h"
#include "fetps.h"

#define PRULE  4
#define NKNOTS n
#define MEVALP mm
#define P      5
#define LEVELS 5

const double fact[]={1, 1, 2, 6, 24, 120, 720, 5040, 40320, 362880, 3628800,
	39916800.0, 479001600.0, 6227020800.0, 87178291200.0,
	1307674368000.0,  2.0922789888e+13,  3.55687428096e+14,
	6.402373705728e+15, 1.216451004088e+17, 2.432902008177e+18,
	5.109094217171e+19, 1.124000727778e+21, 2.585201673888e+22,
	6.204484017332e+23,  1.551121004333e+25, 4.032914611266e+26,
	1.088886945042e+28, 3.048883446118e+29, 8.841761993741e+30,
	2.652528598122e+32};

int fhmq2d(Point *y, double *a, long n, Point *z, long mm, double *s)
{
	long i, j, k1, k2, n1, n2, v, mom2, mom;        // general counters
	//double *x, *w, *a;                   // Gauss-Laguerre weight and knots
	double *x, *w;                   // Gauss-Laguerre weight and knots
	double sa=0, se=0;               // approximate and exact solution
	Point **c;               // knots and evaluation points
	//Point *y, *z, **c;               // knots and evaluation points
	double temp;

	double ***m, ***m2, *side, sum1, *p1, *p2, *p3;
	long l, o, bn, bnx, bny, nnb, left, right, up, down;
	unsigned long *nb;
	double s1, s2, *sqt;
	double *hermx, *hermy, pointx, pointy;

	// setup quadrature
	x=dvector(1, PRULE);
	w=dvector(1, PRULE);

	gaulag(x, w, PRULE, -.5);

	side=dvector(1, PRULE);
	sqt=dvector(1, PRULE);
	nb=lvector(1, PRULE);

	s1=s2=0;

	for (i=1; i<=PRULE; ++i)
	{
		side[i]=.6*sqrt(2/x[i]);
		//        nb[i]=(unsigned long)(1/side[i])+1;
		nb[i]=2;
		side[i]=1.0/(double)nb[i];
		sqt[i]=sqrt(x[i]);
	}

	nnb=2;                       // in fact must be max(nb)
	c=pmatrix(1, PRULE, 1, nnb*nnb);
	for (l=1; l<=PRULE; ++l)
	{
		o=1;
		for (i=0; i<nb[l]; ++i)
		{
			for (j=0; j<nb[l]; ++j)
			{
				c[l][o].x=j*side[l]+(side[l]/2);
				c[l][o].y=i*side[l]+(side[l]/2);
				o+=1;
			}
		}
	}

	m=d3tensor(1, PRULE, 1, nnb*nnb, 0, (P+1)*(P+1)-1);
	for (l=1; l<=PRULE; ++l)
	{
		for (i=0; i<=((P+1)*(P+1)-1); ++i)
		{
			for (j=1; j<=(nb[l]*nb[l]);++j)
			{
				m[l][j][i]=0;
			}
		}
	}

	m2=d3tensor(1, PRULE, 1, nnb*nnb, 0, (P+1)*(P+1)-1);
	for (l=1; l<=PRULE; ++l)
	{
		for (i=0; i<=((P+1)*(P+1)-1); ++i)
		{
			for (j=1; j<=(nb[l]*nb[l]);++j)
			{
				m2[l][j][i]=0;
			}
		}
	}

	hermx=dvector(0, 2*P);
	hermy=dvector(0, 2*P);
	p1=dvector(0, P);
	p2=dvector(0, P);
	p3=dvector(0, 2*P);

	p3[0]=1.0;
	for (k1=1; k1<=(2*P); ++k1)
	{
		p3[k1]=p3[k1-1]*(-1.0);
	}

	//t1=clock();

	// precompute moments
	for (o=1; o<=PRULE; ++o)
	{
		for (i=1; i<=NKNOTS; ++i)
		{
			bnx=(long)(y[i].x/side[o])+1;
			bny=(long)(y[i].y/side[o])+1;
			bn=(bny-1)*nb[o]+bnx;
			pointx=(y[i].x-c[o][bn].x)*sqt[o];
			pointy=(y[i].y-c[o][bn].y)*sqt[o];
			p1[0]=p2[0]=1.0;
			for (k1=1; k1<=P; ++k1)
			{
				p1[k1]=p1[k1-1]*pointx;
				p2[k1]=p2[k1-1]*pointy;
			}
			for (k1=0; k1<=P; ++k1)
			{
				for (k2=0; k2<=P; ++k2)
				{
					mom=k1*(P+1)+k2;
					m[o][bn][mom]+=(a[i]*p1[k1]*p2[k2])
						/(fact[k1]*fact[k2]);
				}
			}
		}
	}
	//t2=clock();
	//s1+=difftime(t2, t1);

	// precompute moments
	for (o=1; o<=PRULE; ++o)
	{
		for (i=1; i<=(nb[o]*nb[o]); ++i)
		{
			bnx=(long)(c[o][i].x/side[o])+1;
			left = (bnx<= LEVELS) ? 1 : (bnx-LEVELS);
			right= (bnx>=(long)(nb[o]-LEVELS)) ? nb[o] : (bnx+LEVELS);
			bny=(long)(c[o][i].y/side[o])+1;
			down = (bny<= LEVELS) ? 1 : (bny-LEVELS);
			up = (bny>=(long)(nb[o]-LEVELS)) ? nb[o] : (bny+LEVELS);
			for (l=left;l<=right;++l)
			{
				for (v=down;v<=up;++v)
				{
					bn=(v-1)*nb[o]+l;
					pointx=(c[o][i].x-c[o][bn].x)*sqt[o];
					pointy=(c[o][i].y-c[o][bn].y)*sqt[o];

					hermx[0]= exp(-DSQR(pointx));
					hermx[1]=2*pointx*exp(-DSQR(pointx));
					for (k2=2; k2<=(2*P); ++k2)
					{
						hermx[k2]=2*pointx*hermx[k2-1]-2*(k2-1)*hermx[k2-2];
					}

					hermy[0]= exp(-DSQR(pointy));
					hermy[1]=2*pointy*exp(-DSQR(pointy));
					for (k2=2; k2<=(2*P); ++k2)
					{
						hermy[k2]=2*pointy*hermy[k2-1]-2*(k2-1)*hermy[k2-2];
					}

					//t1=clock();
					for (k1=0; k1<=P; ++k1)
					{
						for (k2=0; k2<=P; ++k2)
						{
							sum1=0.0;
							for (n1=0; n1<=P; ++n1)
							{
								for (n2=0; n2<=P; ++n2)
								{
									mom=n1*(P+1)+n2;
									sum1+=m[o][bn][mom]*hermx[k1+n1]*hermy[k2+n2];
								}
							}
							mom2=k1*(P+1)+k2;
							m2[o][i][mom2]+=(p3[k1+k2]*sum1)/(fact[k1]*fact[k2]);
						}
					}
					//t2=clock();
					//s1+=difftime(t2, t1);
				}
			}
		}
	}
	// end precompute moments

	s2=0;
	for (i=1, temp=0; i<=NKNOTS; ++i)
	{
		temp+=a[i];
	}

	printf("approximate   exact        error\n");

	for (j=1; j<=MEVALP; ++j)
	{
		//t1=clock();
		sa=0;
		for (i=1; i<PRULE; ++i)
		{

			bnx=(long)(z[j].x/side[i])+1;
			bny=(long)(z[j].y/side[i])+1;
			bn=(bny-1)*nb[i]+bnx;
			sum1=0.0;
			//t1=clock();
			pointx=(z[j].x-c[i][bn].x)*sqt[i];
			pointy=(z[j].y-c[i][bn].y)*sqt[i];
			p1[0]=p2[0]=1.0;
			for (k1=1; k1<=P; ++k1)
			{
				p1[k1]=p1[k1-1]*pointx;
				p2[k1]=p2[k1-1]*pointy;
			}
			for (k1=0; k1<=P; ++k1)
			{
				for (k2=0; k2<=P; ++k2)
				{
					mom=k1*(P+1)+k2;
					sum1+=m2[i][bn][mom]*p1[k1]*p2[k2];
				}
			}
			sa+=w[i]*(temp-sum1)/x[i];
		}
		sa/=2*sqrt(M_PI);
		//t2=clock();
		//s1+=difftime(t2, t1);

		// exact solution
		//t3=clock();
		/* sk126 -- just for comparing fast -- slow
		temp1=0.0;
		for (i=1; i<=NKNOTS; ++i)
		{
		temp1+=a[i]*sqrt(DSQR(z[j].x-y[i].x)+DSQR(z[j].y-y[i].y) + 1);
		}
		sa+=temp;
		//t4=clock();
		//s2+=difftime(t4, t3);

		error1=(sa-temp1)/temp1;

		fprintf(coeff, "|%le-%le| = %le at (%lf,%lf)\n", sa, temp1, fabs(error1), z[j].x, z[j].y); */
		s[j]=sa;
	}

	//printf("fast: %5.3lf direct: %5.3lf (sec)\n", s1/CLOCKS_PER_SEC, s2/CLOCKS_PER_SEC);

	// free memory
	free_dvector(side, 1, PRULE);
	free_dvector(sqt, 1, PRULE);
	free_dvector(hermx, 0, 2*PRULE);
	free_dvector(hermy, 0, 2*PRULE);
	free_dvector(p1, 0, P);
	free_dvector(p2, 0, P);
	free_dvector(p3, 0, 2*P);
	free_lvector(nb, 1, PRULE);
	free_dvector(x, 1, PRULE);
	free_dvector(w, 1, PRULE);
	free_pmatrix(c, 1, PRULE, 1, nnb*nnb);
	free_d3tensor(m, 1, PRULE, 1, nnb*nnb, 0, (P+1)*(P+1)-1);
	free_d3tensor(m2, 1, PRULE, 1, nnb*nnb, 0, (P+1)*(P+1)-1);

	// sk126 -- just for testing purposes close output file
	// fclose(coeff);
	return(0);

}
