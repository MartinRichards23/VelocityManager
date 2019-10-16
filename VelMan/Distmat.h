/* Routines */

double phi_slow(double r);
double phi_fast(double r);
double dist(double*u, double*v, int fasttps);
double **distance_matrix(double **x, int n, int fasttps);
double *evaluate_rbf(double **x, double *c, int n, double **z, int m);
double *evaluate_tps(double **x, double *c, int n, double **z, int m);
double evaluate_point_tps(double **x, double *c, int n, double *z);
double *interpolate_rbf(double **x, int n, double *f, int fasttps);
double *interpolate_tps(double **x, int n, double *f, int fasttps);
double **tps_matrix(double **x, int n, int fasttps);
double **evaluate_grid_tps(double **x, double*c, int n, int rs, int cs,
						   double dy, double dx, double y0, double x0);
double FindAverageValue(int n, double *f);
double InterpolateExtrapolate(double x, double *xin, double *yin, int numin,
							  int method, int extrapolate_percentage, double errval);
double InterpolateExtrapolateDepthToTime(double x, double *xin, double *yin, int numin,
										 int method, int extrapolate_percentage, double errval);
