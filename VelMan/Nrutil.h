#ifndef _NR_UTILS_H_
#define _NR_UTILS_H_

//static float sqrarg;
//#define SQR(a) ((sqrarg=(a)) == 0.0 ? 0.0 : sqrarg*sqrarg)

static double dsqrarg;
/* constant declarations */
#define  M_PI            3.14159265358979323846

/* macro declarations */
#define DSQR(a) ((dsqrarg=(a)) == 0.0 ? 0.0 : dsqrarg*dsqrarg)

static double dmaxarg1, dmaxarg2;
#define DMAX(a, b) (dmaxarg1=(a), dmaxarg2=(b), (dmaxarg1) > (dmaxarg2) ?\
        (dmaxarg1) : (dmaxarg2))

static double dminarg1, dminarg2;
#define DMIN(a, b) (dminarg1=(a), dminarg2=(b), (dminarg1) < (dminarg2) ?\
        (dminarg1) : (dminarg2))

static float maxarg1, maxarg2;
#define FMAX(a, b) (maxarg1=(a), maxarg2=(b), (maxarg1) > (maxarg2) ?\
        (maxarg1) : (maxarg2))

static float minarg1, minarg2;
#define FMIN(a, b) (minarg1=(a), minarg2=(b), (minarg1) < (minarg2) ?\
        (minarg1) : (minarg2))

static long lmaxarg1, lmaxarg2;
#define LMAX(a, b) (lmaxarg1=(a), lmaxarg2=(b), (lmaxarg1) > (lmaxarg2) ?\
        (lmaxarg1) : (lmaxarg2))

static long lminarg1, lminarg2;
#define LMIN(a, b) (lminarg1=(a), lminarg2=(b), (lminarg1) < (lminarg2) ?\
        (lminarg1) : (lminarg2))

static int imaxarg1, imaxarg2;
#define IMAX(a, b) (imaxarg1=(a), imaxarg2=(b), (imaxarg1) > (imaxarg2) ?\
        (imaxarg1) : (imaxarg2))

static int iminarg1, iminarg2;
#define IMIN(a, b) (iminarg1=(a), iminarg2=(b), (iminarg1) < (iminarg2) ?\
        (iminarg1) : (iminarg2))

#define SIGN(a, b) ((b) >= 0.0 ? fabs(a) : -fabs(a))

/* function declarations */
void nrerror(char error_text[]);
float *vector(long nl, long nh);
int *ivector(long nl, long nh);
unsigned char *cvector(long nl, long nh);
unsigned long *lvector(long nl, long nh);
double *dvector(long nl, long nh);
float **matrix(long nrl, long nrh, long ncl, long nch);
double **dmatrix(long nrl, long nrh, long ncl, long nch);
int **imatrix(long nrl, long nrh, long ncl, long nch);
float ***f3tensor(long nrl, long nrh, long ncl, long nch, long ndl, long ndh);
void free_vector(float *v, long nl, long nh);
void free_ivector(int *v, long nl, long nh);
void free_cvector(unsigned char *v, long nl, long nh);
void free_lvector(unsigned long *v, long nl, long nh);
void free_dvector(double *v, long nl, long nh);
void free_matrix(float **m, long nrl, long nrh, long ncl, long nch);
void free_dmatrix(double **m, long nrl, long nrh, long ncl, long nch);
void free_imatrix(int **m, long nrl, long nrh, long ncl, long nch);
void free_f3tensor(float ***t, long nrl, long nrh, long ncl, long nch, long ndl, long ndh);

// New allocation/ freeing routines introduced through new evaluation code
struct point
{
    double x;
    double y;
};
typedef struct point Point;

Point *pvector(long nl, long nh);
Point **pmatrix(long nrl, long nrh, long ncl, long nch);
double ***d3tensor(long nrl, long nrh, long ncl, long nch, long ndl, long ndh);
void free_pvector(Point *v, long nl, long nh);
void free_pmatrix(Point **m, long nrl, long nrh, long ncl, long nch);
void free_d3tensor(double ***t, long nrl, long nrh, long ncl, long nch, long ndl, long ndh);

/* NR Functions used in this code */
void spline(double x[], double y[], int n, double yp1, double ypn, double y2[]);
void splint(double xa[], double ya[], double y2a[], int n, double x, double *y);
void splie2(double x2a[], double **ya, int m, int n, double **y2a);
void splin2(double x1a[], double x2a[], double **ya, double **y2a, int m, int n,
	double x1, double x2, double *y);
void lubksb(double **a, int n, int *indx, double*b);
void ludcmp(double **a, int n, int *indx, double*d);
int gaussj(double **a, int n, double **b, int m);

/* multi quadratic evaluation code*/
double multiquadric_eval(int n, double ptx, double pty, double *x, double *y, double *f);

/* these are the new fetps numerical recipes routines */
double gammln(double xx);
void gaulag(double x[], double w[], int n, double alf);

#endif /* _NR_UTILS_H_ */