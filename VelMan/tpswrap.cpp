#include "tpswrap.h"

double **GEvaluateGridTPS(double **p, double *c, int n, Point *zz, int rs, int cs, int fast)
{
	int m = rs*cs, ierr, i, j, k;
	Point *yy = NULL;
	double **ff = NULL, **z = NULL;

	if(fast == 1)
	{
		// convert double array to point array...
		yy = ((Point *) (p[1]+1))-1;
		// Call up new TPS evaluate routine
		ff = dmatrix(1, rs, 1, cs);

		// Call fast TPS
		ierr =  fhmq2d(yy, c, n, zz, m, ff[1]);

		// Check the return value
		if(ierr != 0)
		{
			free_dmatrix(ff, 1, rs, 1, cs);
			return(NULL);
		}
		else
		{
			// add the linear terms to the result
			k=1;
			for(i=1;i<=rs;i++)
			{
				for(j=1;j<=cs;j++)
				{
					ff[i][j] += c[n+1] + c[n+2]*zz[k].x + c[n+3]*zz[k].y;
					k++;
				}
			}
			return(ff);
		}
	}
	else
	{
		// make zz into a double array...
		z = new double *[m] - 1;
		for(i=1;i<=m;i++)
			z[i] = ((double *) &(zz[i].x))-1;

		// allocate matrix for the results
		ff = new double *[rs] - 1;
		// call up old TPS evaluation routine
		ff[1] = evaluate_tps(p, c, n, z, m);

		for(i=2;i<=rs;i++)
			ff[i] = ff[i-1]+cs;

		delete (z+1);

		return(ff);
	}
}
