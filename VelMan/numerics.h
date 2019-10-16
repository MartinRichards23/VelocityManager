void lm_min(
long int (*fcn)(long int *m, long int *n, double *x, double *fvec, double *fjac, long int *ldfjac, long int *iflag),
long int m, long int  n, double *x, double xtol, double *z, double *t, double *W, double P);

void lm_min_exp(
long int (*fcn)(long int *m, long int *n, double *x, double *fvec, double *fjac, long int *ldfjac, long int *iflag),
long int m, long int  n, double *x, double xtol, double *z1, double *z, double *t, double *W, double P);

long int fcn_l1(
long int *m, long int *n, double *x, double *fvec, double *fjac, long int *ldfjac, long int *iflag);